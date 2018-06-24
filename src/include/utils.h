//
// Created by 范宏昌 on 2018/5/20.
//

#ifndef CMCUP_UTILS_H
#define CMCUP_UTILS_H

#include <stm32f10x.h>
#define NULL 0

#ifdef DEBUG
#define debug(string) println(string)
#else
#define debug(string) while(0){}
#endif


void* kernel_memcpy(void* dst, void* src, int len);

void* kernel_memset(void* dst, int b, int len);

unsigned int* kernel_memset_word(unsigned int* dst, unsigned int w, int len);


/*
 * C string functions.
 *
 * kernel_strdup is like strdup, but calls kmalloc instead of malloc.
 * If out of memory, it returns NULL.
 */

int kernel_strcmp(const char* dest, const char* src);

unsigned int kernel_strlen(const char* str);

char* kernel_strchr(const char* s, int ch_arg);

char* kernel_strcpy(char* dest, const char* src);

char* kernel_strcat(char* dest, const char* src);

/*
 * Standard C string function: tokenize a string splitting based on a
 * list of separator characters. Reentrant version.
 *
 * The "context" argument should point to a "char *" that is preserved
 * between calls to strtok_r that wish to operate on same string.
 */
char* kernel_strtok_r(char* string, const char* seps, char** context);

// itoa:  convert n to characters in s.
void itoa(unsigned long num, char* dst, unsigned int maxlength);

// zitoa: use zero to fill the remaining blanks.
void zitoa(unsigned long num, char* dst, unsigned int maxlength);

void println(const char *string);

void print(const char *string);

void printhex(uint32_t num);

void printint(int num);

void System_Sleep(void);

void printstack();

void print_ready_list();


#endif //CMCUP_UTILS_H
