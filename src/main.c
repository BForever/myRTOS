#include <sched.h>
#include "init.h"
#include "sched.h"
#include "utils.h"
#include "sem.h"

void task_println_normal(void *string)
{
	println("normal task start");
	while (1) {
		int i;
		for (i = 4; i >= 0; i--) {
			print((char *) string);
			print("  :  ");
			printint(i);
			println(".");
		}
	}
}
void task_println_sched(void *string)
{
	println("shed task start");
	while (1) {
		int i;
		for (i = 4; i >= 0; i--) {
			print((char *) string);
			print("  :  ");
			printint(i);
			println(".");
		}
        schedule();
	}
}
void task_println_sleep(void *string)
{
	println("sleep task start");
	while (1) {
		int i;
		for (i = 4; i >= 0; i--) {
			print((char *) string);
			print("  :  ");
			printint(i);
			println(".");
		}
		sleep(6);
	}
}

struct mutex mtx;
void task_println_mtx1(void *arg)
{
	println("mtx1 task start");
	
	while (1) {
		println("task1 signal.");
		signal_mutex(&mtx);
		println("task1 wait.");
		wait_mutex(&mtx);
	}
}
void task_println_mtx2(void *arg)
{
	println("mtx2 task start");
	
	while (1) {
		println("task2 signal.");
		signal_mutex(&mtx);
		println("task2 wait.");
		wait_mutex(&mtx);
	}
}

char s1[] = "this is task 1";
char s2[] = "this is task 2";
char s3[] = "this is task 3";
char stack1[STACK_SIZE];
char stack2[STACK_SIZE];
char stack3[STACK_SIZE];
char stack4[STACK_SIZE];
char stack5[STACK_SIZE];
char stack6[STACK_SIZE];

extern TCB *current;

int main()
{
	os_init();
	
//	set_ready_tail(create_task(task_println_normal, (uint8_t *) stack1, 0, "task1", (void *) s1));
//	set_ready_tail(create_task(task_println_sched, (uint8_t *) stack2, 0, "task2", (void *) s2));
//	set_ready_tail(create_task(task_println_sleep, (uint8_t *) stack3, 0, "task3", (void *) s3));
	
	init_mutex(&mtx);
	set_ready_tail(create_task(task_println_mtx1, (uint8_t *) stack4, 0, "task4",NULL));
	set_ready_tail(create_task(task_println_mtx2, (uint8_t *) stack5, 0, "task5",NULL));
	
	os_start();
}



