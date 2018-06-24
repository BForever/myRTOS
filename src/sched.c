//
// Created by 范宏昌 on 2018/5/20.
//

#include "sched.h"
#include <stm32f10x.h>
#include <sched.h>
#include "utils.h"


// Schedule lists:
// All tasks' lists
struct list_head shed_list;
// Multilevel ready lists
struct list_head ready_list[PRIO_LEVELS];
// Sleep lists
struct list_head sleep_list;

// The current running task
TCB *current = NULL;

void idle(void *arg)
{
	println("idle start");
	enable_interrupt();
//	printstack();
	
	while (1) {
		disable_interrput();
		println((char*)arg);
		enable_interrupt();
	}
}

char idle_stack[STACK_SIZE];
char idles[]="idles";
void os_init()
{
	int i;
	// Initialize all lists
	INIT_LIST_HEAD(&shed_list);
	for (i = 0; i < PRIO_LEVELS; i++) {
		INIT_LIST_HEAD(&ready_list[i]);
	}
	INIT_LIST_HEAD(&sleep_list);
	
	// OS_Start will load the current task to work, you can replace it
	current = create_task(idle, (uint8_t *) idle_stack, PRIO_LEVELS - 1, "idle", (void*)idles);
	set_ready(current);
	
	println("os inited");
}

void set_ready(TCB* task)
{
	disable_interrput();
	list_add_tail(&task->ready,&ready_list[task->priority]);
	task->state = READY;
	enable_interrupt();
}

void set_run(TCB* task)
{
	disable_interrput();
	list_del_init(&task->ready);
	task->state = RUNNING;
	enable_interrupt();
}

// Only wrap
void os_start()
{
	current = get_ready_task();
	first_schedule_c();
}

TCB *get_ready_task()
{
	TCB *next = (TCB *) 0;
	int i;
	disable_interrput();
	for (i = 0; i < PRIO_LEVELS; i++) {
		printint(i);
		if (!list_empty(&ready_list[i])) {
			next = list_entry(ready_list[i].next, TCB, ready);
			break;
		}
	}
	enable_interrupt();
	return next;
}

TCB *create_task(task_func_ptr func, uint8_t *stack, uint32_t priority, const char *name, void *arg)
{
	// Initialization
	task_union *tasku = (task_union *) stack;
	TCB *task = (TCB *) tasku;
	task->state = CREATED;
	task->priority = priority;
	task->counter = DEFAULT_COUNTER;
	task->sp = stack + STACK_SIZE;
	// Name
	kernel_memcpy(task->name, (void *) name, TASK_NAME_LENGTH - 1);
	task->name[TASK_NAME_LENGTH - 1] = 0;
	
	disable_interrput();
	list_add_tail(&task->shed, &shed_list);
	enable_interrupt();
	
	// Context init
	hw_stack_frame_t *hf = (hw_stack_frame_t *) (task->sp - sizeof(hw_stack_frame_t));
	hf->r0 = (uint32_t) arg;
	hf->r1 = 0x11111111;
	hf->r2 = 0x22222222;
	hf->r3 = 0x33333333;
	hf->r12 = 0x12121212;
	hf->pc = (uint32_t) func;
	hf->lr = (uint32_t) del_process;
	hf->psr = 0x21000000; //default PSR value
	
	sw_stack_frame_t *sf = (sw_stack_frame_t *) (task->sp - sizeof(hw_stack_frame_t) - sizeof(sw_stack_frame_t));
	sf->r4=0x44444444;
	sf->r5=0x55555555;
	sf->r6=0x66666666;
	sf->r7=0x77777777;
	sf->r8=0x88888888;
	sf->r9=0x99999999;
	sf->r10=0x10101010;
	sf->r11=0x11011011;
	
	task->sp = (uint8_t*) sf;
	
	return task;
}

void sleep(uint32_t time)
{
	disable_interrput();
	
	list_add_tail(&current->sleep, &sleep_list);
	current->state = SLEEP;
	current->sleep_time = time;
	
	schedule();
	
	enable_interrupt();
}

void update_sleep_threads()
{
	TCB *task;
	struct list_head *pos, *n;
	disable_interrput();
	// Search the sleep list
	list_for_each_safe(pos, n, &sleep_list) {
		// Get one sleep task
		task = list_entry(pos, TCB, sleep);
		// Get current time
		task->sleep_time--;
		
		if (task->sleep_time == 0) {
			list_del_init(&task->sleep);
			set_ready(task);
		}
	}
	enable_interrupt();
}


//This is called when the task returns
void del_process(void)
{
	disable_interrput();
	// Delete it from shed list
	list_del_init(&current->shed);
	
	enable_interrupt();
	println("task stopped.");
	// Request schedule or while(1), it will never been executed again
	while (1);
}

static int32_t intstack[20];
static int32_t inttop = 0;
void enable_interrupt()
{
	int32_t tmp = intstack[--inttop];
	if(tmp==0)
		asm volatile("cpsie    i\n\t");
}

void disable_interrput()
{
	int32_t tmp;
	asm volatile(
		"mrs    %0, PRIMASK\n\t"
        :"=r"(tmp)
		);
	intstack[inttop++]=tmp;
	asm volatile("cpsid    i\n\t");
}

extern "C" {
__attribute__((naked)) void switch_context_c()
{
	TCB* prev = current;
	set_ready(current);
	
	/* Store sp */
	uint32_t tmp;
	asm volatile(
	"mrs    %0, psp\n\t"
	"stmdb  %0!,    {r4-r11}\n\t"
	"str    %0, [%1]\n\t"
	:"=r"(tmp)
	:"r"(prev)
	);
	
	TCB* next = get_ready_task();
	set_run(next);
	current = next;
	
	/* Load sp */
	asm volatile(
	"ldr    %0, [%1]\n\t"
	"ldmfd  %0!,    {r4-r11}\n\t"
	"msr    psp,    %0\n\t"
	:"=r"(tmp)
	:"r"(next)
	);
	
	/* Return from exception */
	asm volatile("mov   lr, #0xfffffffd");
	asm volatile("bx    lr");
}


__attribute__((naked)) void first_schedule_c()
{
	set_run(current);
	
	asm volatile(
	"ldr    %0, [%0]\n\t"
	"msr    PSP,    %0\n\t"
	"mrs    %0,     CONTROL\n\t"
	"orr    %0,     #2\n\t"
	"msr    CONTROL,     %0\n\t"
	"pop    {r4-r11}\n\t"
	"pop    {r0-r3,r12}\n\t"
	"ldr    lr,    [sp,#8]\n\t"
	"msr    psr,    lr\n\t"
	"pop    {lr}\n\t"
	"pop    {pc}\n\t"
	:
	:"r"(current)
	);
}

void schedule()
{
	asm volatile(
		"svc    0\n\t"
		);
}

//inline void invokePendSV()
//{
//	*(uint32_t *) (0xE000ED04) == 0x10000000;
//}

__attribute__((naked)) void TIM2_IRQHandler()
{
	TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	switch_context_c();
}
__attribute__((naked)) void SVC_Handler()
{
	switch_context_c();
}

}


