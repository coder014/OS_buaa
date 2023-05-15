// User-level IPC library routines

#include <env.h>
#include <lib.h>
#include <mmu.h>
#include <drivers/dev_rtc.h>

u_int get_time(u_int *us) {
	uint32_t temp;
	syscall_read_dev(&temp, DEV_RTC_ADDRESS|DEV_RTC_TRIGGER_READ, sizeof(uint32_t));
	syscall_read_dev(&temp, DEV_RTC_ADDRESS|DEV_RTC_SEC, sizeof(uint32_t));
	syscall_read_dev(us, DEV_RTC_ADDRESS|DEV_RTC_USEC, sizeof(u_int));
	return temp;
}
void usleep(u_int us) {
	u_int in_s, in_us;
	u_int time_s, time_us;
	in_s = get_time(&in_us);
	while (1) {
		time_s = get_time(&time_us);
		u_int t_s = in_s + us/1000000 + (us%1000000 + in_us)/1000000;
		u_int t_us = (us%1000000 + in_us)%1000000;
		if (time_s > t_s || (time_s == t_s && time_us >= t_us)) {
			return;
		} else {
			syscall_yield();
		}
	}
}

// Send val to whom.  This function keeps trying until
// it succeeds.  It should panic() on any error other than
// -E_IPC_NOT_RECV.
//
// Hint: use syscall_yield() to be CPU-friendly.
void ipc_send(u_int whom, u_int val, const void *srcva, u_int perm) {
	int r;
	while ((r = syscall_ipc_try_send(whom, val, srcva, perm)) == -E_IPC_NOT_RECV) {
		syscall_yield();
	}
	user_assert(r == 0);
}

// Receive a value.  Return the value and store the caller's envid
// in *whom.
//
// Hint: use env to discover the value and who sent it.
u_int ipc_recv(u_int *whom, void *dstva, u_int *perm) {
	int r = syscall_ipc_recv(dstva);
	if (r != 0) {
		user_panic("syscall_ipc_recv err: %d", r);
	}

	if (whom) {
		*whom = env->env_ipc_from;
	}

	if (perm) {
		*perm = env->env_ipc_perm;
	}

	return env->env_ipc_value;
}
