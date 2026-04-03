#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <stdint.h>

#define SYS_PROCINFO 471

struct procinfo {
    int      pid;
    int      ppid;
    int      state;
    int      nice;
    uint64_t nvcsw;
    uint64_t nivcsw;
};

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Uso: %s <PID>\n", argv[0]);
        return 1;
    }

    int pid = atoi(argv[1]);
    struct procinfo info;

    long ret = syscall(SYS_PROCINFO, pid, &info);
    if (ret < 0) {
        perror("syscall procinfo failed");
        return 1;
    }

    printf("PID:    %d\n", info.pid);
    printf("PPID:   %d\n", info.ppid);
    printf("State:  %d\n", info.state);
    printf("Nice:   %d\n", info.nice);
    printf("nvcsw:  %lu\n", info.nvcsw);
    printf("nivcsw: %lu\n", info.nivcsw);

    return 0;
}
