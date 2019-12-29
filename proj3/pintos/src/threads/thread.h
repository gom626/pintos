
#ifndef THREADS_THREAD_H
#define THREADS_THREAD_H

#include <debug.h>
#include <list.h>
#include <stdint.h>
#include "synch.h"

#ifndef USERPROG
/*Project #3. */
extern bool thread_prior_aging;
#endif
/* States in a thread's life cycle. */
enum thread_status
  {
    THREAD_RUNNING,     /* Running thread. */
    THREAD_READY,       /* Not running but ready to run. */
    THREAD_BLOCKED,     /* Waiting for an event to trigger. */
    THREAD_DYING        /* About to be destroyed. */
  };

/* Thread identifier type.
   You can redefine this to whatever type you like. */
typedef int tid_t;
#define TID_ERROR ((tid_t) -1)          /* Error value for tid_t. */

/* Thread priorities. */
#define PRI_MIN 0                       /* Lowest priority. */
#define PRI_DEFAULT 31                  /* Default priority. */
#define PRI_MAX 63                      /* Highest priority. */
#define MAX_FILE_SIZE 135				/* Maximum file size */

#define INT_MUL_FLOAT(a,b)  (a*b)		/*00*/
#define INT_SUB_FLOAT(a,b)  (a*(0x4000)-b)
#define FLOAT_ADD_FLOAT(a,b) (a+b)
#define FLOAT_SUB_FLOAT(a,b) (a-b)	/*00*/
#define FLOAT_DIV_FLOAT_(a,b,c) {c=a;c=c*(0x4000)/b;}
#define FLOAT_MUL_FLOAT_(a,b,c) {c=a;c=c*b/(0x4000);}
#define FLOAT_DIV_INT(a,b) (a/b)			/*00*/
#define FLOAT_ADD_INT(a,b) (a+b*(0x4000)) /*00*/

/* A kernel thread or user process.

   Each thread structure is stored in its own 4 kB page.  The
   thread structure itself sits at the very bottom of the page
   (at offset 0).  The rest of the page is reserved for the
   thread's kernel stack, which grows downward from the top of
   the page (at offset 4 kB).  Here's an illustration:

        4 kB +---------------------------------+
             |          kernel stack           |
             |                |                |
             |                |                |
             |                V                |
             |         grows downward          |
             |                                 |
             |                                 |
             |                                 |
             |                                 |
             |                                 |
             |                                 |
             |                                 |
             |                                 |
             +---------------------------------+
             |              magic              |
             |                :                |
             |                :                |
             |               name              |
             |              status             |
        0 kB +---------------------------------+

   The upshot of this is twofold:

      1. First, `struct thread' must not be allowed to grow too
         big.  If it does, then there will not be enough room for
         the kernel stack.  Our base `struct thread' is only a
         few bytes in size.  It probably should stay well under 1
         kB.

      2. Second, kernel stacks must not be allowed to grow too
         large.  If a stack overflows, it will corrupt the thread
         state.  Thus, kernel functions should not allocate large
         structures or arrays as non-static local variables.  Use
         dynamic allocation with malloc() or palloc_get_page()
         instead.

   The first symptom of either of these problems will probably be
   an assertion failure in thread_current(), which checks that
   the `magic' member of the running thread's `struct thread' is
   set to THREAD_MAGIC.  Stack overflow will normally change this
   value, triggering the assertion. */
/* The `elem' member has a dual purpose.  It can be an element in
   the run queue (thread.c), or it can be an element in a
   semaphore wait list (synch.c).  It can be used these two ways
   only because they are mutually exclusive: only a thread in the
   ready state is on the run queue, whereas only a thread in the
   blocked state is on a semaphore wait list. */
struct thread
  {
    /* Owned by thread.c. */
    tid_t tid;                          /* Thread identifier. */
    enum thread_status status;          /* Thread state. */
    char name[16];                      /* Name (for debugging purposes). */
    uint8_t *stack;                     /* Saved stack pointer. */
    int priority;                       /* Priority. */
    struct list_elem allelem;           /* List element for all threads list. */

    /* Shared between thread.c and synch.c. */
    struct list_elem elem;              /* List element. */

#ifdef USERPROG
    /* Owned by userprog/process.c. */
    uint32_t *pagedir;                  /* Page directory. */
    struct semaphore can_die;			/*up when thread allowed to die*/
	struct semaphore support_die;		/*memorize element*/
	struct semaphore load_sema;			/*parents wait when? child load*/
	struct semaphore waitg;				/*씨바 multi-oom*/
	struct semaphore waitc;				/*씨바 2 multi-oon*/
	int exit_status;					/*save current statement*/
    int waith;
	struct list children;				/*List of child threads*/
	struct list_elem children_elem;		/*Element of 'children' list*/
	struct thread * parent;
	int check;
						     	/*이거 하라는데 ....*/
#endif
    int thread_cnt;                             /*이거 하라는데 ....*/
	//File Descriptor 테이블 추가
	struct file * file_descriptors[MAX_FILE_SIZE];  /*파일 객체 포인터의 배열*/
	int fd;											/*현재 테이블에 존재하는 fd값의 최댓값+1*/
    //현재 실행중인 파일의 정보를 수정 할 수 있도록 thread 구조체에 추가
	struct file * deny_file;						/*현재 실행중인 파일 추가*/
	
    int64_t wakeTime;
	/* Owned by thread.c. */
    unsigned magic;									/* Detects stack overflow. */
	int time1;
	int time2;
  };

/* If false (default), use round-robin scheduler.
   If true, use multi-level feedback queue scheduler.
   Controlled by kernel command-line option "-o mlfqs". */
extern bool thread_mlfqs;

void thread_init (void);
void thread_start (void);

void thread_tick (void);
void thread_print_stats (void);

typedef void thread_func (void *aux);
tid_t thread_create (const char *name, int priority, thread_func *, void *);

void thread_block (void);
void thread_unblock (struct thread *);

struct thread *thread_current (void);
tid_t thread_tid (void);
const char *thread_name (void);

void thread_exit (void) NO_RETURN;
void thread_yield (void);

/* Performs some operation on thread t, given auxiliary data AUX. */
typedef void thread_action_func (struct thread *t, void *aux);
void thread_foreach (thread_action_func *, void *);

int thread_get_priority (void);
void thread_set_priority (int);

int thread_get_nice (void);
void thread_set_nice (int);
int thread_get_recent_cpu (void);
int thread_get_load_avg (void);
bool cmp_priority(const struct list_elem *a, const struct list_elem *b,void *aux UNUSED);    /*인자로 주어진 >스레드들의 우선순의 비교- hanyang pintos p194*/
void thread_aging(void);
bool cmp_priority(const struct list_elem *a, const struct list_elem *b,void *aux UNUSED);    /*인자로 주어진 >스레드들의 우선순의 비교- hanyang pintos p194*/
int test_max_priority(void);/*현재 수행중인 스레드와가장 높은 우선순위의 스레드의 우선순위를 비교하여 스케줄링*/
void calculate_time(void);
void priority_yield(void);
#endif /* threads/thread.h */
