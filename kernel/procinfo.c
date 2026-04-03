#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/sched.h>
#include <linux/pid.h>
#include <linux/uaccess.h>

struct procinfo {
    pid_t    pid;
    pid_t    ppid;
    int      state;
    int      nice;
    uint64_t nvcsw;
    uint64_t nivcsw;
};

SYSCALL_DEFINE2(procinfo, pid_t, pid, struct procinfo __user *, info)
{
    struct task_struct *task;
    struct procinfo kinfo;

    task = find_get_task_by_vpid(pid);
    if (!task)
        return -ESRCH;

    kinfo.pid    = task_pid_vnr(task);
    kinfo.ppid   = task_ppid_nr(task);
    kinfo.state  = task->__state;
    kinfo.nice   = task_nice(task);
    kinfo.nvcsw  = task->nvcsw;
    kinfo.nivcsw = task->nivcsw;

    put_task_struct(task);

    if (copy_to_user(info, &kinfo, sizeof(kinfo)))
        return -EFAULT;

    return 0;
}
