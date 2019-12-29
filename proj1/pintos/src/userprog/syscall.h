#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H

void syscall_init (void);

typedef int pid_t;

void syscall_init (void);
void exit(int); //exit
void halt (void);       //exec
pid_t exec(const char *);       //wait
int read(int ,void *, unsigned );//read
int write(int,void * , unsigned);//write
int wait(pid_t); //wait
//2 new system calls
int fibonacci(int n); //fibonacci
int sum_of_four_integers(int a, int b , int c, int d); //sum_of_four_int
#endif /* userprog/syscall.h */

