#include <sched.h>
#include "init.h"
#include "sched.h"
#include "utils.h"

void task_println(void *string)
{
    disable_interrput();
    println("task start");
    enable_interrupt();
    while (1) {
        int i;
        for(i=4;i>=0;i--){
            disable_interrput();
            print((char *) string);
            print("  :  ");printint(i);println(".");
            enable_interrupt();
        }
//        schedule();
    }
}
char s1[]="this is task 1";
char s2[]="this is task 2";
char stack1[STACK_SIZE];
char stack2[STACK_SIZE];

extern TCB* current;
int main()
{
    RCC_Configuration();
    GPIO_Configuration();
    USART1_configuration();// now println is available
    TIM_Configuration();
//    EXIT_Configuration();
    NVIC_Configuration();
    
    disable_interrput();
    
    println("hello!");
    os_init();
    current = create_task(task_println,(uint8_t *)stack1,0,"task1",(void*)s1);
	set_ready(current);
    set_ready(create_task(task_println,(uint8_t*)stack2,0,"task2",(void*)s2));
//    print_ready_list();
//
//    TCB* next;
//    print("next ready: ");
//    next = get_ready_task();
//    println(next->name);
//    set_run(next);
//    print_ready_list();
//
//    set_ready(next);
//    print("next ready: ");
//    next = get_ready_task();
//    println(next->name);
//    set_run(next);
//    print_ready_list();
//
//    while (1);
    
    os_start();
    
    /* Wouldn't get here */
    while(1);
}



