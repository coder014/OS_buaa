#include <env.h>
#include <pmap.h>
#include <printk.h>
#include <trap.h>

extern void handle_int(void);
extern void handle_tlb(void);
extern void handle_sys(void);
extern void handle_ov(void);
extern void handle_mod(void);
extern void handle_reserved(void);

void (*exception_handlers[32])(void) = {
    [0 ... 31] = handle_reserved,
    [0] = handle_int,
    [2 ... 3] = handle_tlb,
    [12] = handle_ov,
#if !defined(LAB) || LAB >= 4
    [1] = handle_mod,
    [8] = handle_sys,
#endif
};

/* Overview:
 *   The fallback handler when an unknown exception code is encountered.
 *   'genex.S' wraps this function in 'handle_reserved'.
 */
void do_reserved(struct Trapframe *tf) {
	print_tf(tf);
	panic("Unknown ExcCode %2d", (tf->cp0_cause >> 2) & 0x1f);
}

void do_ov(struct Trapframe *tf) {
	u_long epc = tf->cp0_epc;
	curenv->env_ov_cnt++;
	struct Page *pp = page_lookup(curenv->env_pgdir, epc, NULL);
	epc = page2kva(pp) | (epc & 0xFFF);
	u_long instr = *(u_long*)epc;
	u_int instype, special, imm, rs, rt;
	instype = instr >> 26;
	special = instr & 0x3F;
	rs = (instr >> 21) & 0x1F;
	rt = (instr >> 16) & 0x1F;
	imm = instr & 0xFFFF;
	if(instype==0 && special==0x20) { // add
		printk("add ov handled\n");
		instr |= 1;
	} else if(instype==0 && special==0x22) { // sub
		printk("sub ov handled\n");
		instr |= 1;
	} else if(instype==0x8) { // addi
		printk("addi ov handled\n");
		tf->cp0_epc += 4;
		tf->regs[rt] = (tf->regs[rs] >> 1) + (imm >> 1);
	}
	*(u_long*)epc = instr;
}
