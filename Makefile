export top_dir      := .
export include_dir  := $(top_dir)/include
export src_dir      := $(top_dir)
export build        := $(top_dir)/build

host_os := $(shell uname)

ifeq ($(host_os),Darwin)
gcc_home := /usr/local/gcc-4.5.2-for-linux32
crosscompile := $(gcc_home)/bin/i586-pc-linux-
endif

cc  ?=  $(crosscompile)gcc
as  ?=  $(crosscompile)gcc
ld  ?=  $(crosscompile)ld

nm  :=  $(crosscompile)nm
objdump :=  $(crosscompile)objdump

lds := vmcosec.lds

cc_flags    := -ffreestanding -nostdinc -nostdlib -Wall -Wextra -Winline -Wno-unused -O2 -MD 
as_flags    := -Wall -MD $(addprefix -I, $(include_dir))
ld_flags    := -static -nostdlib -T$(build)/$(lds)

cc_includes := $(addprefix -I, $(include_dir)) -include globl.h 

### for 64bit host
cc_flags  += -m32
as_flags  += -m32
ld_flags  += -melf_i386

objs    := $(src_dir)/arch/boot.S
objs    += $(wildcard $(src_dir)/arch/[^b]*.S) # exclude boot.S
objs    += $(wildcard $(src_dir)/[^u]*/*.c) # exclude usr/ folder
objs    += $(wildcard $(src_dir)/*.c)

objs    := $(patsubst $(src_dir)/%.S, $(build)/%.o, $(objs))
objs    := $(patsubst $(src_dir)/%.c, $(build)/%.o, $(objs))
objs 	+= $(build)/core/repl.o

libinit   := $(build)/usr/init.a
kernel    := kernel
initfs    := res/initfs

mnt_img     := bootfd
image       := cosec.img

fuse    := $(shell which ext2fuse)

### Use native mount/umount for a GRUB installation, fuseext2 fails at this
ifeq ($(strip $(fuse)),)
    do_mount	:= sudo mount -o loop 
    do_umount	:= sudo umount -l
    do_install	:= sudo cp 
else
    do_mount	:= $(fuse) -o rw+,uid=`id -u`,gid=`id -g` 
    do_umount	:= fusermount -u 
    do_install	:= cp
endif

log_name      := fail.log
objdfile      := $(build)/$(kernel).objd
pipe_file     := pipe

vbox_name   := COSEC
qemu_flags  := -fda $(image) -boot a -m 64 -net nic,model=rtl8139

.PHONY: run install mount umount clean
.PHONY: qemu vbox bochs

run:	install
	@echo "\n#### Running..." && \
	if [ $$DISPLAY ] ; then	\
		make vbox || make qemu || make bochs || \
			echo "@@@@ Error: VirtualBox, qemu or Bochs must be installed";	\
	else qemu $(qemu_flags) -curses;	fi

krun: $(kernel)
	qemu -kernel $(build)/$(kernel) -fda $(image)

qemu:	install 
	@if [ -S $(pipe_file) ]; 							\
	then qemu $(qemu_flags) -serial unix:$(pipe_file) ;	\
	else qemu $(qemu_flags) -serial stdio ;							\
	fi 

vbox:	install
	VBoxManage startvm $(vbox_name)

bochs:	install
	bochs


install:  $(kernel) $(initfs)
	@make mount \
		&& $(do_install) $(build)/$(kernel) $(mnt_img) 	\
		&& echo "\n## Kernel installed";				\
	$(do_install) $(initfs) $(mnt_img) 					\
		&& echo "## Initfs installed"; 					\
	make umount

$(initfs):
	@res/mkinitfs $(initfs) && echo "## initfs created"

$(mnt_img):	
	@mkdir $(mnt_img) 

mount:	$(image) $(mnt_img)
	@$(do_mount) $(image) $(mnt_img) \
	&& echo "## Image mounted"

umount:	
	@$(do_umount) $(mnt_img) &&	echo "## Image unmounted" || true; \
	rmdir $(mnt_img)

$(image):	 
	@echo "\n#### Checking image"
	@if [ ! -e $(image) ]; then	\
        echo -en "\n## Generating image...\t\t"; \
		cp res/fd.img.bz2 .;	\
		bunzip2 fd.img.bz2;	\
		mv fd.img $(image);	\
		make mount \
			&& mkdir -p $(mnt_img)/boot/grub/ \
			&& $(do_install) res/menu.lst $(mnt_img)/boot/grub \
			&& make umount;	\
		echo -e "## ...generated\n";	\
	fi

$(kernel): $(build) $(objs) $(libinit) $(build)/$(lds)
	@echo "\n#### Linking..."
	@echo -n "LD: "
	$(ld) -o $(build)/$(kernel)	$(objs) $(libinit) $(ld_flags) && echo "## ...linked"
	@[ `which $(objdump) 2>/dev/null` ] && $(objdump) -d $(build)/$(kernel) > $(kernel).objd || true
	@[ `which $(nm) 2>/dev/null` ] && $(nm) $(build)/$(kernel) | sort > $(build)/$(kernel).nm || true
	@[ `which ctags 2>/dev/null ` ] && ctags -R * || true
	
$(build):
	@echo "\n#### Compiling"
	@mkdir -p $(build)
	@for d in * ; do		\
		[ -d $$d ] && mkdir $(build)/$$d || true;	\
	done

$(build)/$(lds):    $(lds).S
	@echo "CPP: "
	$(cc) -E $< -o $@ -P -DNOT_CC $(cc_includes)

$(build)/%.o: $(src_dir)/%.secd
	@echo -n "LD: "
	ld -melf_i386 -r -b binary -o $@ $<
	
$(build)/%.o : $(src_dir)/%.c
	@echo -n "CC: "
	$(cc) -c $< -o $@ $(cc_includes) $(cc_flags) -MT $(subst .d,.c,$@) 

$(build)/%.o : $(src_dir)/%.S
	@echo -n "AS: "
	$(as) -c $< -o $@ $(as_flags) -MT $(subst .d,.c,$@)

$(libinit):
	@cd usr && make

$(pipe_file):
	mkfifo $(pipe_file)

clean:
	rm -rf $(build)

help:
	@echo "USAGE:"; \
	echo "	[run] - (default action) compile sources, link, generate image if needed, install kernel, search a virtual machine, run"; \
	echo "	qemu | vbox | bochs - make all the things needed for run and run in the specified emulator"; \
	echo "	install - check kernel and image and install former to latter";	\
	echo "	kernel - compile and link kernel"; \
	echo "	mount/umount - mount/umount image (root privileges are required unless using FUSE)";	\
	echo "  You may wish to install fuseext2 tools to work without root privileges, but it is still recommended"; \
	echo "  to make image with native sudo. In order to do this, use"; \
	echo "	make fuse=";	echo

include $(wildcard $(addprefix /*.d, $(build)))
$(build)/core/repl.o: $(src_dir)/core/repl.secd
