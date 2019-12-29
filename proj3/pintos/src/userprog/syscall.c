#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include <string.h>
#include <devices/input.h>
#include <filesys/file.h>
#include <filesys/filesys.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/vaddr.h"
#include "devices/shutdown.h"
#include "userprog/process.h"

static void syscall_handler (struct intr_frame *);

void
syscall_init (void) 
{
  //Syscall_init() 함수에서 filesys_lock초기화 코드 추가
  //userprof/syscall.c에 구현	
  lock_init(&mutex);	// lock_init(struct lock*)인터페이스 사용
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
	if(is_user_vaddr((const void *)ptr(4))||(const void *)ptr(4)<PHYS_BASE) 
	  f->eax=exec((const char*)ptr(4));
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
  else if(ptr(0)==SYS_CREATE){

	if(is_user_vaddr((const void*)ptr(4))&&is_user_vaddr((const void*)ptr(8))&&(const char *)ptr(4)!=NULL)
	  f->eax=create((const char*)ptr(4),(unsigned)ptr(8));
	else exit(-1);
  }
  else if(ptr(0)==SYS_REMOVE){
	if(is_user_vaddr((const void*)ptr(4))&&(const char *)ptr(4)!=NULL)
	  f->eax=remove((const char*)ptr(4));
	else exit(-1);
  }
  else if(ptr(0)==SYS_OPEN){
	if(is_user_vaddr((const void*)ptr(4))&&(const char *)ptr(4)!=NULL)
	  f->eax=open((const void*)ptr(4));
	else exit(-1);
  }
  else if(ptr(0)==SYS_FILESIZE){
	if(is_user_vaddr((const void*)ptr(4)))
	  f->eax=filesize((int)ptr(4));
	else exit(-1);
  }
  else if(ptr(0)==SYS_SEEK){
	if(is_user_vaddr((const void*)ptr(4))&&is_user_vaddr((const void*)ptr(8)))
	  seek((int)ptr(4),(unsigned)ptr(8));
	else exit(-1);
  }
  else if(ptr(0)==SYS_TELL){
	if(is_user_vaddr((const void*)ptr(4)))
      f->eax=tell((int)ptr(4));
	else exit(-1);
  }
  else if(ptr(0)==SYS_CLOSE){
    if(is_user_vaddr((const void*)ptr(4)))
      close((int)ptr(4));
    else exit(-1);
  }
  else{
     puts("!@#\n");
  }
  //thread_exit ();
}
void exit(int status)
{
  int i=2;
  printf("%s: exit(%d)\n",thread_name(),status);
  thread_current()->exit_status=status;
  while(i<MAX_FILE_SIZE){
	if(fptr(i)!=NULL) {
	  close(i);
	  fptr(i)=NULL;
    }
	i++;
  }
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
/*
int write(int fd,void * buffer, unsigned size)
{
  if(fd==0||buffer>=PHYS_BASE){
	exit(-1);
    return -1;
  }
  putbuf((char*)buffer,size);	
  return strlen(buffer);
}*/
/*Reads size bytes from the file open as fd into buffer. Return the number of bytes actually read(0 at end of file),
or -l if the file could not be read(due to a condition other than end of file).Fd 0 reads form the keyboard using
input_getc().*/
/*
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
*/
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
//Project 2
/*bool create (const char *file, unsigned initial_size) [System Call]
Creates a new file called file initially initial size bytes in size. Returns true if successful, false otherwise. Creating a new file does not open it: opening the new file is
a separate operation which would require a open system call.*/
int create(const char *file,unsigned initial_size)
{
  return filesys_create(file,initial_size);//파일 이름과 파일 사이즈를 인자 값으로 받아 파일을 생성하는 함수
}
/*bool remove (const char *file) [System Call]
Deletes the file called file. Returns true if successful, false otherwise. A file may be
removed regardless of whether it is open or closed, and removing an open file does
not close it. See [Removing an Open File], page 35, for details.*/
int remove (const char *file)
{
  return filesys_remove(file);//파일 이름에 해당하는 파일을 제거하는 함수
}
/*int open (const char *file) [System Call]
Opens the file called file. Returns a nonnegative integer handle called a “file descriptor” (fd), or -1 if the file could not be opened.
File descriptors numbered 0 and 1 are reserved for the console: fd 0 (STDIN_FILENO) is
standard input, fd 1 (STDOUT_FILENO) is standard output. The open system call will
never return either of these file descriptors, which are valid as system call arguments
only as explicitly described below.
Each process has an independent set of file descriptors. File descriptors are not
inherited by child processes.
When a single file is opened more than once, whether by a single process or different
processes, each open returns a new file descriptor. Different file descriptors for a single
file are closed independently in separate calls to close and they do not share a file
position.*/
int open (const char *file)
{
  //파일을 open
  //해당 파일 객체에 파일 디스크립터 부여
  //파일 디스크럽터 리턴
  //해당 파일이 존재하지 않으면 -1 리턴
  int i=3;  //0은 키보드 입력 1은 버퍼에 저장된 값을 화면에 출력
  struct file * new = NULL;
  lock_acquire(&mutex);
  new=filesys_open(file);
  if(new==NULL){ 
	lock_release(&mutex);
	return -1;
  }
  else{
	while(i<MAX_FILE_SIZE){
	  if(fptr(i)==NULL){
	    //fptr(i)=new;
		if(!strcmp(thread_current()->name,file)) file_deny_write(new);
		fptr(i)=new;
		lock_release(&mutex);
		return i;
	  }
	  i++;
	}
  }
  lock_release(&mutex);
  return -1;
}
/*int filesize (int fd) [System Call]
Returns the size, in bytes, of the file open as fd.*/
int filesize (int fd)
{
  //파일 이름 필요~
  //파일 디그크립터를 이용하여 파일 객체 검색
  //해당 파일의 길이를 리턴
  //해당 파일이 존재하지 않음면 -1 리턴
  if(fptr(fd)==NULL) return -1;
  else return file_length(fptr(fd));//파일 크기를 알려주는 함수
}
/*int read (int fd, void *buffer, unsigned size) [System Call]
Reads size bytes from the file open as fd into buffer. Returns the number of bytes
actually read (0 at end of file), or -1 if the file could not be read (due to a condition
other than end of file). Fd 0 reads from the keyboard using input_getc().*/
int read (int fd, void *buffer, unsigned size)
{
  //파일에 동시 접근이 일어날 수 있으므로 LOCK이용
  //파일 디스크립터를 이용하여 파일 객체 검색
  //파일 디스크립터가 0일 경우 키보드에 입력을 버퍼에 저장 후 버퍼의 저장한 크기를 리턴(input_getc() 이용)
  //파일 디스크립터가 0이 아닐 경우 파일의 데이터를 크기 만큼 저장 후 읽은 바이트 수를 리턴
  char ch;
  unsigned i=0;
  lock_acquire(&mutex);
  if(buffer >= PHYS_BASE || fd==1 || fd==2 ){//fd=1 화면에 파일 쓰기 fd==err
	exit(-1);
	lock_release(&mutex);
    return -1;
  }
  if(fd==0){
	//lock_acquire(&mutex);	
	for(i=0;i<size&&(ch=input_getc())!='\0';i++) (((uint8_t *)(buffer))[i]=ch);
	lock_release(&mutex);
	return i;  
  }
  else{
	//lock_acquire(&mutex);
	i=file_read(fptr(fd),buffer,size);
	lock_release(&mutex);
	return i;
  }
}
/*int write (int fd, const void *buffer, unsigned size) [System Call]
Writes size bytes from buffer to the open file fd. Returns the number of bytes actually
written, which may be less than size if some bytes could not be written.
Writing past end-of-file would normally extend the file, but file growth is not implemented by the basic file system. The expected behavior is to write as many bytes as
possible up to end-of-file and return the actual number written, or 0 if no bytes could
be written at all.
Fd 1 writes to the console. Your code to write to the console should write all of buffer
in one call to putbuf(), at least as long as size is not bigger than a few hundred
bytes. (It is reasonable to break up larger buffers.) Otherwise, lines of text output
by different processes may end up interleaved on the console, confusing both human
readers and our grading scripts.*/
int write (int fd, const void *buffer, unsigned size)
{
  //파일에 동시 접근이 일어날 수 있으므로 LOCk 사용
  //파일 디스크립터를 이용하여 파일 객체 검색
  //파일 디스크립터가 1일 경우 버퍼에 저장된 값을 화면에 출력후 버퍼의 크기 리턴(putbuf())이용
  //파일 디스크립터가 1이 아닐 경우 버퍼에 저장된 데이터를 크기 만큼 파일에 기록후 기록한 바이트 수를 리턴
  unsigned value=0;
  lock_acquire(&mutex); 
  if(fd==0||buffer>=PHYS_BASE||fd==2){
    exit(-1);
	lock_release(&mutex);
    return -1;
  }
  if(fd==1){
	//lock_acquire(&mutex);
	putbuf((char*)buffer,size);
	lock_release(&mutex);
	return size;
  }
  else{
    //lock_acquire(&mutex);
	if((fptr(fd)->deny_write)==1)
	  file_deny_write(fptr(fd));
	value=file_write(fptr(fd),buffer,size);
	lock_release(&mutex);
	return value;
  }
}
/*void seek (int fd, unsigned position) [System Call]
Changes the next byte to be read or written in open file fd to position, expressed in
bytes from the beginning of the file. (Thus, a position of 0 is the file’s start.)
A seek past the current end of a file is not an error. A later read obtains 0 bytes,
indicating end of file. A later write extends the file, filling any unwritten gap with
zeros. (However, in Pintos files have a fixed length until project 4 is complete, so
writes past end of file will return an error.) These semantics are implemented in the
file system and do not require any special effort in system call implementation.*/
void seek (int fd, unsigned position)
{
  //파일디스크립텅를 이용하여 파일 객체 검색
  //해당 열린 파일의 위치(offset)를 position만큼 이동
  file_seek(fptr(fd),position);
}
/*unsigned tell (int fd) [System Call]
Returns the position of the next byte to be read or written in open file fd, expressed
in bytes from the beginning of the file.*/
unsigned tell (int fd)
{
  //파일 디스크립터를 이용하여 파일 객체 검색
  //해당 열린 파일의 위치를 반환
  return file_tell(fptr(fd));

}
/*void close (int fd) [System Call]
Closes file descriptor fd. Exiting or terminating a process implicitly closes all its open
file descriptors, as if by calling this function for each one.*/
void close (int fd)
{
  //해당 파일 디스크립터에 해당하는 파일을 닫음
  //파일 디스크립터 엔트리 초기화
  file_close(fptr(fd));
  fptr(fd)=NULL;
}
