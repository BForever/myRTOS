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
	print("Task named \'");print(current->name);print("\' wait(): ");
	// in use
	if (mtx->count <= 0) {
		if (mtx->user == current) {
			// current user
			// update depth
			mtx->count--;
			print("mutex in use, current user wait, current depth: ");printint(-mtx->count);println(".");
		} else {
			// another user
			// wait
			println("mutex in use, another user wait, sleep");
			list_add_tail(&current->sleep, &mtx->wait_list);
			current->state = SLEEP;
			enable_interrupt();
			schedule();
			goto end_not_enable;
		}
	} else {
		// not in use
		// update count, change user
		print("mutex not in use, login new user: \'");print(current->name);println("\'.");
		mtx->count--;
		mtx->user = current;
	}
	
	enable_interrupt();
	end_not_enable:
	return;
}

void signal_mutex(struct mutex *mtx)
{
	TCB *task;
	disable_interrput();
	print("Task named \'");print(current->name);print("\' signal(): ");
	
	// in use
	if (mtx->count < 1) {
		if (mtx->user == current) {
			// current user
			// update depth
			println("mutex in use, current user signal.");
			mtx->count++;
			// user release
			// search other waiting tasks
			if (mtx->count == 1) {
				// get one wait task
				println("mutex now free.");
				if(!list_empty(&mtx->wait_list)){
					print("wakeup next user: \'");
					task = list_entry(mtx->wait_list.next, TCB, sleep);
					list_del_init(&task->sleep);
					set_ready_head(task);
					mtx->count--;
					mtx->user=task;
					print(task->name);println("\'.");
					enable_interrupt();
					schedule();
					goto end_not_enable;
				}
			}
		} else {
			// another user
			println("mutex in use, another user signal, ignore.");
		}
	} else {
		// not in use
		println("mutex not in use, ignore.");
	}
	
	enable_interrupt();
	end_not_enable:
	return;
}


void init_semaphore_bin(struct semaphore_bin *sem, int32_t count_init, uint32_t count_max)
{
	sem->count = count_init;
	sem->count_max = count_max;
	INIT_LIST_HEAD(&sem->wait_list);
}

void release_semaphore_bin(struct semaphore_bin *sem)
{
	TCB *task;
	struct list_head *pos, *n;
	
	disable_interrput();
	// Search the wait list
	list_for_each_safe(pos, n, &sem->wait_list) {
		// get one wait task
		task = list_entry(pos, TCB, sleep);
		list_del_init(&task->sleep);
		set_ready_tail(task);
	}
	enable_interrupt();
}

void wait_semaphore_bin(struct semaphore_bin *sem)
{
	TCB *task;
	disable_interrput();
	print("Task named \'");print(current->name);print("\' wait(): ");
	
	if(sem->count==sem->count_max&&!list_empty(&sem->wait_list)){
		// release one blocked signal task, but not preempt
		println("sem full and has blocked signal, release but not preempt.");
		task = list_entry(sem->wait_list.next, TCB, sleep);
		list_del_init(&task->sleep);
		set_ready_head(task);
		
	}else if (sem->count <= 0) {
		print("sem empty, block to wait, current count: -");printint(1-sem->count);println(".");
		sem->count--;
		list_add_tail(&current->sleep, &sem->wait_list);
		current->state = SLEEP;
		enable_interrupt();
		schedule();
		goto end_not_enable;
	} else {
		print("sem available, success, current count: ");printint(sem->count-1);println(".");
		sem->count--;
	}
	
	enable_interrupt();
	end_not_enable:
	return;
}

void signal_semaphore_bin(struct semaphore_bin *sem)
{
	TCB *task;
	disable_interrput();
	print("Task named \'");print(current->name);print("\' signal(): ");
	
	if (sem->count < 0  ) {
		// unlock one waiting task
		print("sem count: -");printint(-sem->count);println(", unlock one waiting task.");
		sem->count++;
		if(!list_empty(&sem->wait_list)){
			print("task named \'");
			task = list_entry(sem->wait_list.next, TCB, sleep);
			print(task->name);print("\' unlocked, current count:-");printint(-sem->count);println(".");
			list_del_init(&task->sleep);
			set_ready_head(task);
			enable_interrupt();
			schedule();
			goto end_not_enable;
		}
		
	} else if (sem->count == sem->count_max) {
		// over signal, block it
		println("over signal, block.");
		list_add_tail(&current->sleep, &sem->wait_list);
		current->state = SLEEP;
		enable_interrupt();
		schedule();
		goto end_not_enable;
	} else{
		print("normal signal, current count: ");
		printint(sem->count+1);println(".");
		sem->count++;
	}
	
	enable_interrupt();
	end_not_enable:
	return;
}

void init_semaphore_cnt(struct semaphore_cnt *sem,int32_t count_init)
{
	sem->count = count_init;
	INIT_LIST_HEAD(&sem->wait_list);
}

void release_semaphore_cnt(struct semaphore_cnt * sem)
{
	TCB *task;
	struct list_head *pos, *n;
	
	disable_interrput();
	// Search the wait list
	list_for_each_safe(pos, n, &sem->wait_list) {
		// get one wait task
		task = list_entry(pos, TCB, sleep);
		list_del_init(&task->sleep);
		set_ready_tail(task);
	}
	enable_interrupt();
}

void wait_semaphore_cnt(struct semaphore_cnt * sem)
{
	TCB *task;
	disable_interrput();
	print("Task named \'");print(current->name);print("\' wait(): ");
	
	if (sem->count <= 0) {
		print("sem empty, block to wait, current count: -");printint(1-sem->count);println(".");
		sem->count--;
		list_add_tail(&current->sleep, &sem->wait_list);
		current->state = SLEEP;
		enable_interrupt();
		schedule();
		goto end_not_enable;
	} else {
		print("sem available, success, current count: ");printint(sem->count-1);println(".");
		sem->count--;
	}
	
	enable_interrupt();
	end_not_enable:
	return;
}


void signal_semaphore_cnt(struct semaphore_cnt * sem)
{
	TCB *task;
	disable_interrput();
	print("Task named \'");print(current->name);print("\' signal(): ");
	
	if (sem->count < 0  ) {
		// unlock one waiting task
		print("sem count: -");printint(-sem->count);println(", unlock one waiting task.");
		sem->count++;
		if(!list_empty(&sem->wait_list)){
			print("task named \'");
			task = list_entry(sem->wait_list.next, TCB, sleep);
			print(task->name);print("\' unlocked, current count:-");printint(-sem->count);println(".");
			list_del_init(&task->sleep);
			set_ready_head(task);
			enable_interrupt();
			schedule();
			goto end_not_enable;
		}
		
	} else{
		print("normal signal, current count: ");
		printint(sem->count+1);println(".");
		sem->count++;
	}
	
	enable_interrupt();
	end_not_enable:
	return;
}
