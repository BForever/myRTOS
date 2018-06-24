//
// Created by 范宏昌 on 2018/5/20.
//

#ifndef CMCUP_SCHED_H
#define CMCUP_SCHED_H

#include <stm32f10x.h>
#include "list.h"
#include "init.h"

#define STACK_SIZE 2048
#define CONTEXT_SIZE 128 //TODO:don't known yet
#define PRIO_LEVELS 2
#define TASK_NAME_LENGTH 20
#define DEFAULT_COUNTER 22

extern "C"{


typedef void (*task_func_ptr)(void *);

enum TASK_STATE{RUNNING,READY,SLEEP,CREATED};

typedef struct {
    // stack
    uint8_t * sp;
    
    // schedule
    struct list_head shed;
    struct list_head ready;
    struct list_head sleep;
    unsigned int state;
    unsigned int priority;
    unsigned int counter;
    
    // Sleep time /us
    unsigned int sleep_time;
    
    // info
    char name[TASK_NAME_LENGTH];
    
} TCB;
typedef union {
    TCB task;
    unsigned char stack[STACK_SIZE];
} task_union;

//This defines the stack frame that is saved  by the hardware
typedef struct {
    uint32_t r0;
    uint32_t r1;
    uint32_t r2;
    uint32_t r3;
    uint32_t r12;
    uint32_t lr;
    uint32_t pc;
    uint32_t psr;
} hw_stack_frame_t;

//This defines the stack frame that must be saved by the software
typedef struct {
    uint32_t r4;
    uint32_t r5;
    uint32_t r6;
    uint32_t r7;
    uint32_t r8;
    uint32_t r9;
    uint32_t r10;
    uint32_t r11;
} sw_stack_frame_t;

#define MAIN_RETURN 0xFFFFFFF9  //Tells the handler to return using the MSP
#define THREAD_RETURN 0xFFFFFFFD //Tells the handler to return using the PSP


void os_init();

void os_start();

TCB* create_task(task_func_ptr func,uint8_t * stack,uint32_t priority,const char* name, void* arg);

void sleep(uint32_t time);

void schedule();

void enable_interrupt();

void disable_interrput();

void del_process(void);

void printstack();

void TIM2_IRQHandler(void);
void SVC_Handler(void);

void first_schedule_c();

void set_ready_tail(TCB *task);
void set_ready_head(TCB *task);
void set_run(TCB* task);

TCB *get_ready_task();

}

#endif //CMCUP_SCHED_H
