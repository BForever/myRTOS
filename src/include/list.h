//
// Created by 范宏昌 on 2018/5/20.
//

#ifndef CMCUP_LIST_H
#define CMCUP_LIST_H

#ifndef _XSU_LIST_H
#define _XSU_LIST_H

struct list_head {
    struct list_head *prev;
    struct list_head *next;
};

#define LIST_POISON1 (list_head *)0x10101010
#define LIST_POISON2 (list_head *)0x20202020

#define container_of(ptr, type, member) ((type*)((char*)ptr - (char*)&(((type*)0)->member)))

#define LIST_HEAD_INIT(name) \
    { &(name), &(name) }

#define LIST_HEAD(name) struct list_head name = LIST_HEAD_INIT(name)

static inline void INIT_LIST_HEAD(struct list_head *list) {
    list->prev = list;
    list->next = list;
}

static inline void __list_add(struct list_head *newnode, struct list_head *prev, struct list_head *next) {
    newnode->next = next;
    newnode->prev = prev;
    prev->next = newnode;
    next->prev = newnode;
}

static inline void list_add(struct list_head *newnode, struct list_head *head) { __list_add(newnode, head, head->next); }

static inline void list_add_tail(struct list_head *newnode, struct list_head *head) { __list_add(newnode, head->prev, head); }

static inline void __list_del(struct list_head *prev, struct list_head *next) {
    prev->next = next;
    next->prev = prev;
}

static inline void list_del(struct list_head *entry) {
    __list_del(entry->prev, entry->next);
    entry->prev = LIST_POISON1;
    entry->next = LIST_POISON2;
}

static inline void list_del_init(struct list_head *entry) {
    __list_del(entry->prev, entry->next);
    INIT_LIST_HEAD(entry);
}

static inline void list_move(struct list_head *entry, struct list_head *head) {
    __list_del(entry->prev, entry->next);
    list_add(entry, head);
}

static inline void list_move_tail(struct list_head *entry, struct list_head *head) {
    __list_del(entry->prev, entry->next);
    list_add_tail(entry, head);
}

static inline unsigned int list_empty(struct list_head *head) { return head->next == head; }

#define list_entry(ptr, type, member) container_of(ptr, type, member)

#define list_for_each(pos, head) for (pos = (head)->next; pos != (head); pos = pos->next)

#define list_for_each_safe(pos, n, head) for (pos = (head)->next, n = pos->next; pos != (head); pos = n, n = pos->next)

#endif


#endif //CMCUP_LIST_H