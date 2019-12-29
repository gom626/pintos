#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H
#include "filesys/off_t.h"
#include "threads/synch.h"

#define ptr(a) *(uint32_t *)(f->esp+a)
#define fptr(a) thread_current()->file_descriptors[a]

struct file 
{
  struct inode *inode;        /* File's inode. */
  off_t pos;                  /* Current position. */
  int deny_write;            /* Has file_deny_write() been called? */
};

/*
struct lock
{
	struct thread * holder;
	struct semaphore semaphore;
}

*/
//struct lock filesys_lock 추가
//userprog/syscall.h 헤더파일에 전역변수로 추기
//read(),write() 시스템 콜에서 파일 접근하기 전에 lock을 획득하도록 구현
//파일에 대한 접근이 끝난 뒤 Lock 해제
struct lock mutex;

void syscall_init (void);

typedef int pid_t;

void syscall_init (void);
void exit(int); //exit
void halt (void);       //exec
pid_t exec(const char *);       //wait
//int read(int ,void *, unsigned );//read
//int write(int,void * , unsigned);//write
int wait(pid_t); //wait
//2 new system calls
int fibonacci(int n); //fibonacci
int sum_of_four_integers(int a, int b , int c, int d); //sum_of_four_int

int create(const char *,unsigned);
int remove (const char *file);
int open (const char *file);
int filesize (int fd);
int read (int fd, void *buffer, unsigned size);
int write (int fd, const void *buffer, unsigned size);
void seek (int fd, unsigned position);
unsigned tell (int fd);
void close (int fd);
#endif /* userprog/syscall.h */

