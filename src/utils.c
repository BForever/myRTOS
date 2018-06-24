//
// Created by 范宏昌 on 2018/5/20.
//

#include <sched.h>
#include "utils.h"


/*
 * C memory functions.
 */

void* kernel_memcpy(void* dst, void* src, int len)
{
    char* deststr = (char*)dst;
    char* srcstr = (char*)src;
    while (len--) {
        *deststr = *srcstr;
        deststr++;
        srcstr++;
    }
    return dst;
}

#pragma GCC push_options
#pragma GCC optimize("O2")
void* kernel_memset(void* dst, int b, int len)
{
    char content = b ? -1 : 0;
    char* deststr = (char*)dst;
    while (len--) {
        *deststr = content;
        deststr++;
    }
    return dst;
}
#pragma GCC pop_options

unsigned int* kernel_memset_word(unsigned int* dst, unsigned int w, int len)
{
    while (len--)
        *dst++ = w;
    
    return dst;
}

int kernel_strcmp(const char* dest, const char* src)
{
    while ((*dest == *src) && (*dest != 0)) {
        dest++;
        src++;
    }
    return *dest - *src;
}

unsigned int kernel_strlen(const char* str)
{
    unsigned int len = 0;
    while (str[len])
        ++len;
    return len;
}

char* kernel_strchr(const char* s, int ch_arg)
{
    // Avoid sign-extension problems.
    const char ch = ch_arg;
    
    // Scan from left to right.
    while (*s) {
        // If we hit it, return it.
        if (*s == ch) {
            return (char*)s;
        }
        s++;
    }
    
    // If we were looking for the 0, return that.
    if (*s == ch) {
        return (char*)s;
    }
    
    // Didn't find it.
    return 0;
}

char* kernel_strcpy(char* dest, const char* src)
{
    while ((*dest++ = *src++))
        ;
    return dest;
}

char* kernel_strcat(char* dest, const char* src)
{
    unsigned int offset = kernel_strlen(dest);
    
    kernel_strcpy(dest + offset, src);
    
    return dest;
}

/*
 * Standard C string function: tokenize a string splitting based on a
 * list of separator characters. Reentrant version.
 *
 * The "context" argument should point to a "char *" that is preserved
 * between calls to strtok_r that wish to operate on same string.
 */
char* kernel_strtok_r(char* string, const char* seps, char** context)
{
    char* head; /* start of word */
    char* tail; /* end of word */
    
    // If we're starting up, initialize context.
    if (string) {
        *context = string;
    }
    
    // Get potential start of this next word.
    head = *context;
    if (head == 0) {
        return 0;
    }
    
    // Skip any leading separators.
    while (*head && kernel_strchr(seps, *head)) {
        head++;
    }
    
    // Did we hit the end?
    if (*head == 0) {
        // Nothing left.
        *context = 0;
        return 0;
    }
    
    // Skip over word.
    tail = head;
    while (*tail && !kernel_strchr(seps, *tail)) {
        tail++;
    }
    
    // Save head for next time in context.
    if (*tail == 0) {
        *context = 0;
    } else {
        *tail = 0;
        tail++;
        *context = tail;
    }
    
    // Return current word.
    return head;
}

// itoa:  convert n to characters in s.
void itoa(unsigned long num, char* dst, unsigned int maxlength)
{
    if (!num) {
        dst[maxlength - 1] = '0';
    } else {
        while (num) {
            dst[maxlength - 1] = (num % 10) + '0';
            num /= 10;
            maxlength--;
        }
    }
}

// zitoa: use zero to fill the remaining blanks.
void zitoa(unsigned long num, char* dst, unsigned int maxlength)
{
    int i;
    for (i = maxlength - 1; i >= 0; i--) {
        dst[i] = (num % 10) + '0';
        num /= 10;
    }
}


void println(const char *string)
{
    uint16_t da = 0xa;
    uint16_t dd = 0xd;
    while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
    while (*string) {
        USART_SendData(USART1, (uint8_t) (*string));
        string++;
        while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
    }
    USART_SendData(USART1, da);
    while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
    USART_SendData(USART1, dd);
    while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
}

void print(const char *string)
{
    while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
    while (*string) {
        USART_SendData(USART1, (uint8_t) (*string));
        string++;
        while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
    }
}

void printint(int num)
{
	char dec[] = "0123456789";
	if (num > 9) {
		printint(num / 10);
		printint(num % 10);
	} else {
		dec[num + 1] = 0;
		print(dec + num);
	}
}

void printhex(uint32_t num)
{
	print("0x");
	for (int i = 7; i >= 0; i--) {
		printint((num >> (i * 4)) % 16);
	}
}

void System_Sleep(void)
{
	SCB->SCR |= 0X00;
#if defined ( __CC_ARM   )
	__force_stores();
#endif
	/* Request Wait For Interrupt */
	__WFI();
}

#define STACK_INSIGHT_SIZE 16

void printstack()
{
	uint32_t stack[STACK_INSIGHT_SIZE];
	uint32_t psp,msp;
	println("read psp");
	
	asm volatile(
	"mrs %0, psp\n\t"
	:"=r"(psp)
	);
	asm volatile(
	"mrs %0, msp\n\t"
	:"=r"(msp)
	);
	
	print("read stack: psp ");
	printhex(psp);
	print("msp ");
	printhex(msp);
	println(".");
	
	for (int j = 0; j < STACK_INSIGHT_SIZE; ++j) {
		asm volatile(
		"LDR %0, [%1]\n\t"
		:"=r"(stack[j])
		:"r"(psp)
		);
		psp+=4;
	}
	println("print stack");
	for (int i = STACK_INSIGHT_SIZE - 1; i >= 0; i--) {
		print("psp+");
		printint(i*4);
		print(": ");
		printhex(stack[i]);
		println(".");
	}
}

#include "list.h"
#include "sched.h"

extern struct list_head ready_list[PRIO_LEVELS];
void print_ready_list()
{
	println("Ready list:--------------");
	
	for(int i=0;i<PRIO_LEVELS;i++){
		print("level ");printint(i);println(":");
		if(!list_empty(&ready_list[i])){
			struct list_head *pos;
			TCB* curr;
			list_for_each(pos,&ready_list[i]){
				curr = list_entry(pos,TCB,ready);
				println(curr->name);
			}
		}
	}
	
	println("Ready list ended.--------");
}



