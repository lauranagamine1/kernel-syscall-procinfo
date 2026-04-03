# HW1: Linux Kernel – Process Control & Minimal Compilation
**CS3015 - Operating Systems**

## Overview
1. Linux kernel internals analysis (PCB/TCB, fork/clone)
2. Building and booting a minimal Linux kernel
3. Implementing a custom system call (`procinfo`)

---

## Part 1: Kernel Internals Analysis

### task_struct
The `task_struct` (defined in `include/linux/sched.h`) is the kernel's Process Control Block (PCB). Key fields analyzed:

| Field | Type | Description |
|---|---|---|
| `pid` | `pid_t` | Unique thread ID |
| `__state` | `unsigned int` | Current process state (running, sleeping, etc.) |
| `parent` | `struct task_struct *` | Pointer to parent process |
| `mm` | `struct mm_struct *` | Virtual memory descriptor |
| `flags` | `unsigned int` | Per-process behavior flags |

### PCB/TCB Layout
In Linux, `task_struct` IS the PCB. Each task has:
- A private **kernel stack** (8–16KB) referenced via `void *stack`
- A `thread_info` struct at the bottom of the kernel stack

### fork() vs clone()
Both syscalls converge internally on `kernel_clone()` → `copy_process()` in `kernel/fork.c`.

| Resource | `fork()` | `clone()` (threads) |
|---|---|---|
| Memory | Copy (COW) | Shared (`CLONE_VM`) |
| File descriptors | Copy | Shared (`CLONE_FILES`) |
| Signal handlers | Copy | Shared (`CLONE_SIGHAND`) |

---

## Part 2: Minimal Kernel Build

- **Kernel version:** 6.19.11
- **Base config:** `make defconfig` + `make localmodconfig`
- **Manual changes via `make menuconfig`:**
  - Disabled `Kernel debugging`
  - Disabled `Sound card support`
  - Disabled `DEBUG_INFO`
- **Compiled with:** `make -j$(nproc)`
- **Boot verified:** `uname -r` → `6.19.11-g455913818704`

---

## Part 3: Custom Syscall – `procinfo` (Track A)

### What it does
Given a PID, returns process information from the kernel's `task_struct`.

### ABI
- **Syscall number:** 471
- **Signature:** `sys_procinfo(pid_t pid, struct procinfo __user *info)`
- **Return:** 0 on success, `-ESRCH` if PID not found, `-EFAULT` on copy error

### Struct
```c
struct procinfo {
    pid_t    pid;
    pid_t    ppid;
    int      state;
    int      nice;
    uint64_t nvcsw;
    uint64_t nivcsw;
};
```

### Files modified
| File | Change |
|---|---|
| `kernel/procinfo.c` | New — syscall implementation |
| `include/linux/syscalls.h` | Added prototype |
| `arch/x86/entry/syscalls/syscall_64.tbl` | Registered as syscall 471 |
| `kernel/Makefile` | Added `procinfo.o` to `obj-y` |

### How to test
```bash
gcc -o test_procinfo tools/testing/procinfo_test.c
./test_procinfo 1
```

Example output:
```
PID:    1
PPID:   0
State:  1
Nice:   0
nvcsw:  2870
nivcsw: 722
```

---

## How to reproduce
1. In a VM, download kernel source:
```bash
wget https://cdn.kernel.org/pub/linux/kernel/v6.x/linux-6.19.11.tar.xz
tar xvf linux-6.19.11.tar.xz
cd linux-6.19.11
```

2. Apply the `.config` from this repo and the modified files.

3. Compile and install:
```bash
make
sudo make modules_install
sudo make install
sudo reboot
```

4. Test the syscall:
```bash
gcc -o test_procinfo tools/testing/procinfo_test.c
./test_procinfo <PID>
```
