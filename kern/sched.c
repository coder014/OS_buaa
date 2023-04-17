#include <env.h>
#include <pmap.h>
#include <printk.h>

/* Overview:
 *   Implement a round-robin scheduling to select a runnable env and schedule it using 'env_run'.
 *
 * Post-Condition:
 *   If 'yield' is set (non-zero), 'curenv' should not be scheduled again unless it is the only
 *   runnable env.
 *
 * Hints:
 *   1. The variable 'count' used for counting slices should be defined as 'static'.
 *   2. Use variable 'env_sched_list', which contains and only contains all runnable envs.
 *   3. You shouldn't use any 'return' statement because this function is 'noreturn'.
 */
void schedule(int yield) {
	static int count = 0; // remaining time slices of current env
	struct Env *e = curenv;
	static int user_time[5] = {0};
	int users[5] = {0};

	struct Env *_e;
	TAILQ_FOREACH(_e, &env_sched_list, env_sched_link) {
		users[_e->env_user]++;
	}

	if(yield || count==0 || e==NULL || e->env_status!=ENV_RUNNABLE) {
		if(e && e->env_status==ENV_RUNNABLE) {
			TAILQ_REMOVE(&env_sched_list, e, env_sched_link);
			TAILQ_INSERT_TAIL(&env_sched_list, e, env_sched_link);
			user_time[e->env_user]+=e->env_pri;
		}
		if(TAILQ_EMPTY(&env_sched_list))
			panic("No runnable env to schedule!");
		int u, maxt=0x7FFFFFFF;
		for(int i=0; i<5; i++) {
			if(users[i] && user_time[i]<maxt) {
				maxt=user_time[i];
				u=i;
			}
		}
		TAILQ_FOREACH(_e, &env_sched_list, env_sched_link) {
			if(_e->env_user == u) {
				e = _e;
				break;
			}
		}
		//e=TAILQ_FIRST(&env_sched_list);
		count=e->env_pri;
	}
	--count;
	env_run(e);
}
