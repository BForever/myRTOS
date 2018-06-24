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

struct semaphore{
	int32_t count;
	uint32_t count_max;
	struct list_head wait_list;
};

void init_semaphore(struct semaphore *sem,int32_t count_init, uint32_t count_max);

void release_semaphore(struct semaphore * sem);

void wait(struct semaphore * sem);

void signal(struct semaphore * sem);


#endif //CMCUP_SEM_H
