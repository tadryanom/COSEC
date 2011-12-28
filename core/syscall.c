#include <syscall.h>
#include <arch/i386.h>
#include <fs/fs_syscalls.h>

#include <log.h>

int sys_print(const char **fmt);

typedef int (*syscall_handler)();

const syscall_handler syscalls[] = {
    [SYS_MOUNT] = sys_mount,

    [SYS_MKDIR] = sys_mkdir,
    [SYS_LSDIR] = sys_lsdir,

    [N_SYSCALLS - 1] = sys_print,
};

void int_syscall() {
    uint * stack = (uint *)intr_context_esp() + CONTEXT_SIZE/sizeof(uint);
    uint intr_num = *(stack - 1);
    uint arg1 = *(stack - 2);
    uint arg2 = *(stack - 3);
    uint arg3 = *(stack - 4);

    logf("\n#syscall(%d, 0x%x, 0x%x, 0x%x)\n", 
            intr_num, arg1, arg2, arg3);

    assertvf( intr_num >= N_SYSCALLS,
            "#SYS: invalid syscall 0x%x\n", intr_num);

    const syscall_handler callee = syscalls[intr_num];
    assertvf(callee, "#SYS: invalid handler for syscall[0x%x]\n", intr_num);

    callee(arg1, arg2, arg3);
}

int sys_print(const char **fmt) {
    print(*fmt);
    return 0;
}