#ifndef __INTR_ASM_H
#define __INTR_ASM_H

#ifndef ASM

// enter points
extern void isr00(void);    // #DE, division by zero,   fault,  no code
extern void isr01(void);    // #DB, reserved
extern void isr02(void);    // NMI, Interrupt, no
extern void isr03(void);    // #BP. breakpoint,         trap,   no
extern void isr04(void);    // #OF, overflow,           trap,   no
extern void isr05(void);    // #BR, out of bounds,      fault,  no
extern void isr06(void);    // #UD, invalid opcode,     fault,  no
extern void isr07(void);    // #NM, no math coproc.,    fault,  no
extern void isr08(void);    // #DF, double fault,       abort,  0
extern void isr09(void);    // Invalid floating-point op (reserved)
extern void isr0A(void);    // #TS, invalid TSS.        fault,  yes

extern void isr0B(void);    // #NP, segment not present fault,  yes
extern void isr0C(void);    // #SS, stack-segment       fault.  yes
extern void isr0D(void);    // #GP, General protection  fault,  yes
extern void isr0E(void);    // #PF. page                fault,  yes
extern void isr0F(void);    // reserved by intel
extern void isr10(void);    // #MF, math (FPU)          fault,  no
extern void isr11(void);    // #AC, alignment check     fault,  0
extern void isr12(void);    // #MC, machine check       abort,  no
extern void isr13(void);    // #XM, SIMD fp exc         fault,  no


extern void irq00(void);
extern void irq01(void);
extern void irq02(void);
extern void irq03(void);
extern void irq04(void);
extern void irq05(void);
extern void irq06(void);
extern void irq07(void);
extern void irq08(void);
extern void irq09(void);
extern void irq0A(void);
extern void irq0B(void);
extern void irq0C(void);
extern void irq0D(void);
extern void irq0E(void);
extern void irq0F(void);

extern void syscallentry(void);     // for system call interrupt
extern void dummyentry(void);       // for all unused software interrupts
extern void isr14to1F(void);        // reserved

#include <arch/i386.h>

typedef void (*intr_entry_f)(void);

const struct {
    enum gatetype type;
    intr_entry_f entry;
} exceptions[0x14] = {
    { .type = GATE_TRAP, .entry = isr00 },
    { .type = GATE_INTR, .entry = isr01 },
    { .type = GATE_INTR, .entry = isr02 },
    { .type = GATE_CALL, .entry = isr03 },
    { .type = GATE_CALL, .entry = isr04 },

    { .type = GATE_CALL, .entry = isr05 },
    { .type = GATE_TRAP, .entry = isr06 },
    { .type = GATE_TRAP, .entry = isr07 },
    { .type = GATE_INTR, .entry = isr08 },
    { .type = GATE_INTR, .entry = isr09 },

    { .type = GATE_TRAP, .entry = isr0A },
    { .type = GATE_TRAP, .entry = isr0B },
    { .type = GATE_TRAP, .entry = isr0C },
    { .type = GATE_TRAP, .entry = isr0D },
    { .type = GATE_TRAP, .entry = isr0E },

    { .type = GATE_TRAP, .entry = isr0F },
    { .type = GATE_TRAP, .entry = isr10 },
    { .type = GATE_TRAP, .entry = isr11 },
    { .type = GATE_INTR, .entry = isr12 },
    { .type = GATE_TRAP, .entry = isr13 },
};

const intr_entry_f
interrupts[0x10] = {
    irq00, irq01, irq02, irq03, 
    irq04, irq05, irq06, irq07, 
    irq08, irq09, irq0A, irq0B, 
    irq0C, irq0D, irq0E, irq0F, 
};

#endif
#endif // __INTR_ASM_H
