#include <sched.h>
#include "init.h"
#include "sched.h"
#include "utils.h"
#include "sem.h"

void task_normal(void *string)
{
	while (1) {
		int i;
		for (i = 3; i > 0; i--) {
			disable_interrput();
			print((char *) string);
			print("  :  ");
			printint(i);
			println(".");
			enable_interrupt();
		}
	}
}

void task_sched(void *string)
{
	while (1) {
		int i;
		for (i = 3; i > 0; i--) {
			disable_interrput();
			print((char *) string);
			print("  :  ");
			printint(i);
			println(".");
			enable_interrupt();
		}
		schedule();
	}
}

void task_sleep(void *string)
{
	while (1) {
		int i;
		for (i = 3; i > 0; i--) {
			disable_interrput();
			print((char *) string);
			print("  :  ");
			printint(i);
			println(".");
			enable_interrupt();
		}
		sleep(4);
	}
}

struct mutex mtx;

void task_mutex(void *arg)
{
	while (1) {
		signal_mutex(&mtx);
		wait_mutex(&mtx);
	}
}

struct semaphore_bin sembin;

void task_sembin_signal(void *arg)
{
	while (1) {
		signal_semaphore_bin(&sembin);
	}
}

void task_sembin_wait(void *arg)
{
	while (1) {
		wait_semaphore_bin(&sembin);
	}
}

struct semaphore_cnt semcnt;

void task_semcnt(void *arg)
{
	while (1) {
		wait_semaphore_cnt(&semcnt);
		for (int i = 6; i > 0; i--) {
			disable_interrput();
			print("task \'");
			printtaskname();
			print("\' is working  ");
			printint(i);
			println(".");
			enable_interrupt();
		}
		signal_semaphore_cnt(&semcnt);
	}
}


char s1[] = "this is task 1";
char s2[] = "this is task 2";
char s3[] = "this is task 3";
char stack1[STACK_SIZE];
char stack2[STACK_SIZE];
char stack3[STACK_SIZE];

extern TCB *current;

#define TEST_SHED 0
#define TEST_MUTEX 0
#define TEST_SEMBIN 0
#define TEST_SEMCOU 1

int main()
{
	os_init();

#if TEST_SHED
	set_ready_tail(create_task(task_normal, (uint8_t *) stack1, 0, "task_normal", (void *) s1));
	set_ready_tail(create_task(task_sched, (uint8_t *) stack2, 0, "task_sched", (void *) s2));
	set_ready_tail(create_task(task_sleep, (uint8_t *) stack3, 0, "task_sleep", (void *) s3));
#endif

#if TEST_MUTEX
	init_mutex(&mtx);
	set_ready_tail(create_task(task_mutex, (uint8_t *) stack1, 0, "task_1", NULL));
	set_ready_tail(create_task(task_mutex, (uint8_t *) stack2, 0, "task_2", NULL));
	set_ready_tail(create_task(task_mutex, (uint8_t *) stack3, 0, "task_3", NULL));
#endif

#if TEST_SEMBIN
	init_semaphore_bin(&sembin, 0, 2);
	set_ready_tail(create_task(task_sembin_signal, (uint8_t *) stack1, 0, "task_signal", NULL));
	set_ready_tail(create_task(task_sembin_wait, (uint8_t *) stack2, 0, "task_wait_1", NULL));
	set_ready_tail(create_task(task_sembin_wait, (uint8_t *) stack3, 0, "task_wait_2", NULL));
#endif

#if TEST_SEMCOU
	init_semaphore_cnt(&semcnt, 2);
	set_ready_tail(create_task(task_semcnt, (uint8_t *) stack1, 0, "task_1", NULL));
	set_ready_tail(create_task(task_semcnt, (uint8_t *) stack2, 0, "task_2", NULL));
	set_ready_tail(create_task(task_semcnt, (uint8_t *) stack3, 0, "task_3", NULL));
#endif
	
	os_start();
}



