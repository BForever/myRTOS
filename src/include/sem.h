#ifndef CMCUP_SEM_H
#define CMCUP_SEM_H

#include "utils.h"

struct mutex{
	int32_t count;
	TCB* user;
	struct list_head wait_list;
};

void init_mutex(struct mutex *mtx);

void release_mutex(struct mutex *mtx);

void wait_mutex(struct mutex *mtx);

void signal_mutex(struct mutex *mtx);

struct semaphore_bin{
	int32_t count;
	uint32_t count_max;
	struct list_head wait_list;
};

void init_semaphore_bin(struct semaphore_bin *sem,int32_t count_init, uint32_t count_max);

void release_semaphore_bin(struct semaphore_bin * sem);

void wait_semaphore_bin(struct semaphore_bin * sem);

void signal_semaphore_bin(struct semaphore_bin * sem);

struct semaphore_cnt{
	int32_t count;
	struct list_head wait_list;
};

void init_semaphore_cnt(struct semaphore_cnt *sem,int32_t count_init);

void release_semaphore_cnt(struct semaphore_cnt * sem);

void wait_semaphore_cnt(struct semaphore_cnt * sem);

void signal_semaphore_cnt(struct semaphore_cnt * sem);

#endif //CMCUP_SEM_H
