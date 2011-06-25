%:include <multiboot.h>
%:include <mboot.h>
%:include <asm.h>

%:include <mm/gdt.h>
%:include <mm/pmem.h>

%:include <dev/intrs.h>
%:include <dev/kbd.h>
%:include <dev/timer.h>

%:include <fs/fs.h>

%:include <console.h>


void kmain(uint32_t magic, struct multiboot_info *mbi)
<%
    if (magic != MULTIBOOT_BOOTLOADER_MAGIC) <%
        k_printf("invalid boot");
        return;
    %>
    print_welcome();

    mboot_info_parse(mbi);

    /* general setup */
    gdt_setup();
    pmem_setup();
    intrs_setup();
    fs_setup();

    /* devices setup */
    timer_setup();
    kbd_setup();

    intrs_enable();

    /* do something useful */
    console_run();
%>

