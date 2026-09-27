*This project has been created as part of the 42 curriculum by brportos.*

# Codexion

## Description
Codexion is a concurrency simulation in C.
Several "coders" threads compete for a limited pool of USB dongles,
and a coder takes their left and right dongles to compile.
---

## Instructions
Build with the provided Makefile. The project compiles with -Wall -Wextra -Werror -pthread.
### from project root
```bash
make
```

# run the program (example)
```bash
./codexion 4 1500 200 200 200 3 100 fifo
```

## Resources
Website
- [miltiphreading](https://qnx.com/developers/docs/7.1/com.qnx.doc.neutrino.getting_started/topic/s1_procs_thread_attr.html?utm_source=chatgpt.com)
- [pthread](https://www.youtube.com/watch?v=d9s_d28yJq0&list=PLfqABt5AS4FmuQf70psXrsMLEDQXNkLq2)
- [mutex](https://www.codequoi.com/en/threads-mutexes-and-concurrent-programming-in-c/)
- [Thread](https://www.geeksforgeeks.org/thread-functions-in-c-c/)

AI 
- AI tools were used for explanations of concepts and function specifications.
## Additional sections
All arguments are mandatory and must be positive integers except scheduler which must be fifo or edf.

number_of_coders -> number of coders
time_to_burnout (ms) -> deadline: if a coder does not start compiling before this window since their last compile or simulation start, they burn out
time_to_compile (ms)
time_to_debug (ms)
time_to_refactor (ms)
number_of_compiles_required -> simulation ends when every coder reached this compile count
dongle_cooldown (ms) -> after a dongle is released it is unavailable for this cooldown
scheduler -> fifo (First In, First Out) or edf (Earliest Deadline First)
Example:

./codexion 4 1500 200 200 200 5 100 edf
# Blocking Cases Handled

## Deadlock Prevention

A deadlock can occur only when all four Coffman conditions hold simultaneously:

| Condition | Meaning |
|---|---|
| **1. Mutual exclusion** | A dongle can be held by only one coder at a time. |
| **2. Hold and wait** | A coder holds one dongle while waiting to acquire the other. |
| **3. No preemption** | A dongle cannot be forcibly taken from the coder holding it. |
| **4. Circular wait** | A cycle exists in which each coder is waiting for a dongle held by the next coder. |

Codexion breaks condition 2 (hold-and-wait) directly: `take_dongle()` never holds one dongle indefinitely while waiting for the other. It locks the left dongle first; if the right dongle is unavailable, it immediately unlocks the left one and returns failure, so the calling coder retries from scratch with nothing held. This makes the classic circular-wait deadlock (each coder holding one dongle, waiting on its neighbor's) structurally impossible.

The scheduler additionally guards against starvation. Depending on `data->scheduler`:

- **EDF ("earliest deadline first")** — `ispriority()` compares a coder's own burnout timestamp (`get_burnout()`) against its immediate left/right neighbors; a coder only takes its turn once it is at least as "hungry" as both. The heap (`heap_ctrl`) keeps every waiting coder ordered by burnout time, so the coder closest to its `max_burnout` deadline is always favored.
- **FIFO** — coders join a strict arrival-order queue (`add_to_queue()` / `t_queue_manager`); a coder cannot proceed until it reaches the front, regardless of how urgently another coder needs a dongle.

Either policy guarantees a waiting coder is eventually served rather than being bypassed indefinitely by later arrivals.

After `release_dongles()` is called, each dongle's `cooldown` field is set to `get_simul_time(data) + data->dongle_cooldown` — a future timestamp, not a "last released at" record. `try_take_dongle()` only succeeds once the current simulation time reaches that timestamp, guaranteeing the configured cooldown period is respected before the dongle can be reacquired.

A dedicated monitor thread (`monitoring_simulation()`) watches for burnout. It repeatedly calls `check_burnout()`, which compares each coder's `time_burnout` checkpoint against the current time; if any coder exceeds `max_burnout` without checking in, the monitor calls `stop_simulation()` and logs the burnout, initiating a clean, immediate shutdown of the whole simulation.

Logging is protected by a single `mutex_print`, ensuring log lines from different coder threads and the monitor thread cannot interleave.

## Thread Synchronization Mechanisms

Each shared object is protected by its own `pthread_mutex_t`:

- `coder->mutex_burnout` protects `time_burnout` (read via `get_burnout()`, written via `set_burnout()`).
- `coder->mutex_done` protects `have_done` (read via `get_have_done()`, written via `set_done()`) — kept separate from `mutex_burnout` so a burnout check never blocks on, or is blocked by, a "finished" check, and vice versa.
- `dongle->lock` protects the dongle's `cooldown` value; it is also held for the entire duration a coder possesses that dongle (locked in `try_take_dongle()`, unlocked in `release_dongles()`), which is what enforces mutual exclusion over the resource itself.
- Three global mutexes protect simulation-wide state: `mutex_print` (logging), `mutex_simul` (the `active_simulation` flag, via `get_simulation()`/`stop_simulation()`), and `heap_ctrl.lock` / `queue_ctrl.lock` (the scheduling structure in use).

**Waiting behavior differs by scheduling mode, and neither is a pure blocking wait:**

- **EDF** (`scheduler_edf_add()`): the coder is pushed into the heap, then loops — checking `ispriority()` and attempting `take_dongle()` — with a `usleep(500)` between attempts while it does not have priority or the attempt fails. This is a throttled polling loop, not a `pthread_cond_wait` block; `heap_ctrl.cond` is broadcast on push/pop and by `release_dongles()`, but no coder actually sleeps on it.
- **FIFO** (`fifo_add_queue()`): a coder not yet at the front of the queue calls `pthread_cond_wait(&queue_ctrl.cond, &queue_ctrl.lock)` — a true blocking wait, consuming no CPU until woken. Once at the front, it switches to polling (`usleep(1000)`) while waiting only on dongle cooldown.

In both modes, `release_dongles()` broadcasts on the relevant condition variable after setting each dongle's new cooldown, so every waiting coder wakes (or, in EDF's case, simply finds the heap state changed on its next poll) and re-evaluates whether it can now proceed.

## Clean Shutdown

Shutdown reuses the same primitives, with no separate "wake everyone" helper — `stop_simulation()` does it directly and in one place:

```c
pthread_mutex_lock(&data->mutex_simul);
data->active_simulation = 0;
pthread_mutex_unlock(&data->mutex_simul);
// broadcast on queue_ctrl.cond, then on heap_ctrl.cond
```

When the monitor detects a burnout, or every coder finishes successfully, it calls `stop_simulation()`, which sets `active_simulation` to `0` under `mutex_simul` and then broadcasts on both `queue_ctrl.cond` and `heap_ctrl.cond` in turn.

Any coder blocked in `pthread_cond_wait` (FIFO, waiting for its turn) wakes immediately and re-checks its loop condition, which reads `active_simulation` and exits. Any coder in the EDF polling loop notices the flag change on its next `usleep(500)` cycle, at most 500 microseconds later. Either way, no coder is left waiting indefinitely for a dongle that will never become available.