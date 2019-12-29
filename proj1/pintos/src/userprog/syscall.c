#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include <string.h>
#include <devices/input.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/vaddr.h"
#include "devices/shutdown.h"
#include "userprog/process.h"

#define ptr(a) *(uint32_t *)(f->esp+a) 

static void syscall_handler (struct intr_frame *);

void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

static void
syscall_handler (struct intr_frame *f UNUSED) 
{
  //printf ("system call!\n");
  //printf("%d\n",ptr(0));
  //printf("%x\n",f->esp);
  //hex_dump(f->esp,f->esp,100,1);
  /* is_user_vaddr()
	check that given virtual address is an user virtual address
  */ 
  if(!is_user_vaddr((const void *)ptr(0))) exit(-1);
  if(ptr(0)==SYS_HALT)
	halt();
  else if(ptr(0)==SYS_EXIT){
	if(is_user_vaddr((const void *)ptr(4))) exit(ptr(4));
	else exit(-1);
  }
  else if(ptr(0)==SYS_EXEC){
	if(is_user_vaddr((const void *)ptr(4))||(const void *)ptr(4)<PHYS_BASE) f->eax=exec((const char*)ptr(4));
	else exit(-1);
  }
  else if(ptr(0)==SYS_WAIT){
	if(is_user_vaddr((const void *)ptr(4))) f->eax=wait(ptr(4));
	else exit(-1);
  }
  else if(ptr(0)==SYS_WRITE){
    if(is_user_vaddr((const void *)ptr(4))&&is_user_vaddr((const void *)ptr(8))&&is_user_vaddr((const void *)ptr(12)))
		f->eax=write((int)ptr(4),(void *)ptr(8),(unsigned)ptr(12));
	else exit(-1);
  }
  else if(ptr(0)==SYS_READ){
    if(is_user_vaddr((const void *)ptr(4))&&is_user_vaddr((const void *)ptr(8))&&is_user_vaddr((const void *)ptr(12)))
	  f->eax=read((int)ptr(4),(void*)ptr(8),(unsigned)ptr(12));
	else exit(-1);
  }
  else if(ptr(0)==SYS_SOFI){
    if(is_user_vaddr((const void *)ptr(4))&&is_user_vaddr((const void *)ptr(8))&&is_user_vaddr((const void *)ptr(12))&&is_user_vaddr((const void *)ptr(16)))
	  f->eax=sum_of_four_integers((int)ptr(4),(int)ptr(8),(int)ptr(12),(int)ptr(16));
	else exit(-1);
  }
  else if(ptr(0)==SYS_FIBO){
	if(is_user_vaddr((const void *)ptr(4)))
	  f->eax=fibonacci((int)ptr(4));
	else exit(-1);
  }
  else{
     puts("!@#\n");
  }
  //thread_exit ();
}
void exit(int status)
{
  printf("%s: exit(%d)\n",thread_name(),status);
  thread_current()->exit_status=status;
  thread_exit();
}
void halt(void)
{
  shutdown_power_off();
}
/*Runs the executable whose name is given in cmd_line,passing any given arguments, and return the new process's
program id(pid). Must return pid-1 which otherwwise should not be a valid pid, if the program cannot load or run
for any reason, thus parrent process cannot return form the exec unil it knows whether the child process successfully
loaded its executable you must use appropriate synchronization to esure this*/
pid_t exec(const char * cmd_line)
{
  return process_execute(cmd_line);
}
/*Waits for a child process pid and retrieves the child’s exit status.
If pid is still alive, waits until it terminates. Then, returns the status that pid passed
to exit. If pid did not call exit(), but was terminated by the kernel (e.g. killed due
to an exception), wait(pid) must return -1. It is perfectly legal for a parent process
to wait for child processes that have already terminated by the time the parent calls
wait, but the kernel must still allow the parent to retrieve its child’s exit status, or
learn that the child was terminated by the kernel.
wait must fail and return -1 immediately if any of the following conditions is true:
• pid does not refer to a direct child of the calling process. pid is a direct child
of the calling process if and only if the calling process received pid as a return
value from a successful call to exec.
Note that children are not inherited: if A spawns child B and B spawns child
process C, then A cannot wait for C, even if B is dead. A call to wait(C) by
process A must fail. Similarly, orphaned processes are not assigned to a new
parent if their parent process exits before they do.
• The process that calls wait has already called wait on pid. That is, a process
may wait for any given child at most once.
Processes may spawn any number of children, wait for them in any order, and may
even exit without having waited for some or all of their children. Your design should
consider all the ways in which waits can occur. All of a process’s resources, including
its struct thread, must be freed whether its parent ever waits for it or not, and
regardless of whether the child exits before or after its parent.
You must ensure that Pintos does not terminate until the initial process exits.
The supplied Pintos code tries to do this by calling process_wait() (in
‘userprog/process.c’) from main() (in ‘threads/init.c’). We suggest that you
implement process_wait() according to the comment at the top of the function
and then implement the wait system call in terms of process_wait().
Implementing this system call requires considerably more work than any of the rest.*/
int wait(pid_t pid)
{
  return process_wait(pid);
}
/*Writes size bytes from buffer to the open file fd. Returns the number of bytes actually
written, which may be less than size if some bytes could not be written.
Writing past end-of-file would normally extend the file, but file growth is not implemented by the basic file system. The expected behavior is to write as many bytes as
possible up to end-of-file and return the actual number written, or 0 if no bytes could
be written at all.
Fd 1 writes to the console. Your code to write to the console should write all of buffer
in one call to putbuf(), at least as long as size is not bigger than a few hundred
bytes. (It is reasonable to break up larger buffers.) Otherwise, lines of text output
by different processes may end up interleaved on the console, confusing both human
readers and our grading scripts.*/
int write(int fd,void * buffer, unsigned size)
{
  if(fd==0||buffer>=PHYS_BASE){
	exit(-1);
    return -1;
  }
  putbuf((char*)buffer,size);	
  return strlen(buffer);
}
/*Reads size bytes from the file open as fd into buffer. Return the number of bytes actually read(0 at end of file),
or -l if the file could not be read(due to a condition other than end of file).Fd 0 reads form the keyboard using
input_getc().*/
int read(int fd,void * buffer,unsigned size)
{
  unsigned i;
  if(buffer >= PHYS_BASE || fd==1){
        exit(-1);
        return -1;
  }
  for(i=0;i<size;i++) ((uint8_t *)(buffer))[i]= input_getc();  //((uint8_t*)buffer)[i]
  //pintos/src/devices/input.c : unit8_t input_getc(void)
  return strlen(buffer);
}
int sum_of_four_integers(int a,int b,int c,int d)
{
  return a+b+c+d;
}
int fibonacci(int n)
{
  int n1=1,n2=1,n0=1,i=0;
  if(n==1||n==2) return 1;
  for(i=0;i<=n-3;i++){
        n0=n1+n2;
        n2=n1;
    n1=n0;
  }
  return n0;
}


