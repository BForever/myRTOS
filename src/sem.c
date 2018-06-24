//
// Created by 范宏昌 on 2018/6/24.
//

#include <sched.h>
#include "sem.h"
#include "list.h"
#include "sched.h"

extern TCB *current;

void init_mutex(struct mutex *mtx)
{
	mtx->count = 1;
	INIT_LIST_HEAD(&mtx->wait_list);
}

void release_mutex(struct mutex *mtx)
{
	TCB *task;
	struct list_head *pos, *n;
	
	disable_interrput();
	// Search the wait list
	list_for_each_safe(pos, n, &mtx->wait_list) {
		// Get one wait task
		task = list_entry(pos, TCB, sleep);
		list_del_init(&task->sleep);
		set_ready_tail(task);
	}
	enable_interrupt();
}

void wait_mutex(struct mutex *mtx)
{
	disable_interrput();
	
	// in use
	if (mtx->count <= 0) {
		if (mtx->user == current) {
			// current user
			// update depth
			mtx->count--;
//			println("wait():mtx in use, current user wait, update depth");
		} else {
			// another user
			// wait
//			println("wait():mtx in use, another user wait, sleep");
			list_add_tail(&current->sleep, &mtx->wait_list);
			current->state = SLEEP;
			enable_interrupt();
			schedule();
			goto end_not_enable;
		}
	} else {
		// not in use
		// update count, change user
//		println("wait():mtx not in use, login new user.");
		mtx->count--;
		mtx->user = current;
	}
	end_enable:
	enable_interrupt();
	end_not_enable:
	return;
}

void signal_mutex(struct mutex *mtx)
{
	TCB *task;
	disable_interrput();
	
	// in use
	if (mtx->count < 1) {
		if (mtx->user == current) {
			// current user
			// update depth
//			println("signal():mtx in use, current user signal.");
			mtx->count++;
			// user release
			// search other waiting tasks
			if (mtx->count == 1) {
				// get one wait task
//				println("signal():mtx now free.");
				if(!list_empty(&mtx->wait_list)){
//					println("signal():mtx wakeup next user.");
					task = list_entry(mtx->wait_list.next, TCB, sleep);
					list_del_init(&task->sleep);
					set_ready_head(task);
					mtx->count--;
					mtx->user=task;
					enable_interrupt();
					schedule();
					goto end_not_enable;
				}
			}
		} else {
			// another user
//			println("signal():mtx in use, another user signal, ignore.");
		}
	} else {
		// not in use
//		println("signal():mtx not in use, ignore.");
	}
	
	end_enable:
	enable_interrupt();
	end_not_enable:
	return;
}


void init_semaphore(struct semaphore *sem, int32_t count_init, uint32_t count_max)
{
	sem->count = count_init;
	sem->count_max = count_max;
	INIT_LIST_HEAD(&sem->wait_list);
}

void release_semaphore(struct semaphore *sem)
{
	TCB *task;
	struct list_head *pos, *n;
	
	disable_interrput();
	// Search the wait list
	list_for_each_safe(pos, n, &sem->wait_list) {
		// Get one wait task
		task = list_entry(pos, TCB, sleep);
		list_del_init(&task->sleep);
		set_ready_tail(task);
	}
	enable_interrupt();
}

void wait(struct semaphore *sem)
{
	disable_interrput();
	
	sem->count--;
	if (sem->count < 0) {
		list_add_tail(&current->sleep, &sem->wait_list);
		current->state = SLEEP;
		enable_interrupt();
		schedule();
	} else {
		enable_interrupt();
	}
}

void signal(struct semaphore *sem)
{
	TCB *task;
	disable_interrput();
	
	sem->count++;
	if (sem->count <= 0 && !list_empty(&sem->wait_list)) {
		task = list_entry(sem->wait_list.next, TCB, sleep);
		list_del_init(&task->sleep);
		set_ready_head(task);
		enable_interrupt();
	} else if (sem->count == sem->count_max) {
		list_add_tail(&current->sleep, &sem->wait_list);
		current->state = SLEEP;
		enable_interrupt();
		schedule();
	}
	
}