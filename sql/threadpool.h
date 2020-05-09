/* Copyright (C) 2012 Monty Program Ab

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; version 2 of the License.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301,  USA */
#ifndef THREADPOOL_INCLUDED
#define THREADPOOL_INCLUDED

#include "conn_handler/connection_handler_manager.h"
//#include <my_counter.h>
#include "sql_ib_ut0counter.h"
#include <mysql/plugin.h>
#include <sql_plist.h>

#define DEFAULT_THREADPOOL_STALL_LIMIT 500U
#define MAX_THREAD_GROUPS 128
extern uint threadpool_max_size;


#if defined(HAVE_SCHED_GETCPU)
#define TP_INDEXER get_sched_indexer_t
#else
#define TP_INDEXER thread_id_indexer_t
#endif

//MY_ALIGNED(CPU_LEVEL1_DCACHE_LINESIZE) Atomic_counter<unsigned long long> tp_waits[THD_WAIT_LAST];
//ib_counter_t<ulonglong, 64, TP_INDEXER> tp_waits[THD_WAIT_LAST];

enum tp_high_prio_mode_t {
  TP_HIGH_PRIO_MODE_TRANSACTIONS,
  TP_HIGH_PRIO_MODE_STATEMENTS,
  TP_HIGH_PRIO_MODE_NONE
};

typedef struct st_mysql_show_var SHOW_VAR;

/* Threadpool parameters */
extern uint threadpool_min_threads;  /* Minimum threads in pool */
extern uint threadpool_idle_timeout; /* Shutdown idle worker threads  after this timeout */
extern uint threadpool_size; /* Number of parallel executing threads */
extern uint threadpool_stall_limit;  /* time interval in 10 ms units for stall checks*/
extern uint threadpool_max_threads;  /* Maximum threads in pool */
extern uint threadpool_oversubscribe;  /* Maximum active threads in group */
extern uint threadpool_prio_kickup_timer;  /* Time before low prio item gets prio boost */
extern my_bool threadpool_exact_stats; /* Better queueing time stats for information_schema, at small performance cost */
extern my_bool threadpool_dedicated_listener; /* Listener thread does not pick up work items. */

/* Possible values for thread_pool_high_prio_mode */
extern const char *threadpool_high_prio_mode_names[];

/* Common thread pool routines, suitable for different implementations */
extern void threadpool_remove_connection(THD *thd);
extern int  threadpool_process_request(THD *thd);
extern int  threadpool_add_connection(THD *thd);

/*
  Functions used by scheduler. 
  OS-specific implementations are in
  threadpool_unix.cc or threadpool_win.cc
*/
extern bool tp_init();
extern void tp_wait_begin(THD *, int);
extern void tp_wait_end(THD*);
extern void tp_post_kill_notification(THD *thd);
extern void tp_end(void);

extern THD_event_functions tp_event_functions;

/* Used in SHOW for threadpool_idle_thread_count */
extern int  tp_get_idle_thread_count();

/*
  Threadpool statistics
*/
struct TP_STATISTICS
{
  /* Current number of worker thread. */
  volatile int32 num_worker_threads;
};

extern TP_STATISTICS tp_stats;


/* Functions to set threadpool parameters */
extern void tp_set_min_threads(uint val);
extern void tp_set_max_threads(uint val);
extern void tp_set_threadpool_size(uint val);
extern void tp_set_threadpool_stall_limit(uint val);


/** Maximum number of native events a listener can read in one go */
#define MAX_EVENTS 1024

#define  INVALID_HANDLE_VALUE -1

struct thread_group_t;

/* Per-thread structure for workers */
struct worker_thread_t
{
  ulonglong  event_count; /* number of request handled by this thread */
  thread_group_t* thread_group;
  worker_thread_t *next_in_list;
  worker_thread_t **prev_in_list;

  mysql_cond_t  cond;
  bool          woken;
};

typedef I_P_List<worker_thread_t, I_P_List_adapter<worker_thread_t,
                 &worker_thread_t::next_in_list,
                 &worker_thread_t::prev_in_list>,
                 I_P_List_counter
                 >
worker_list_t;

struct connection_t
{
  THD *thd;
  thread_group_t *thread_group;
  connection_t *next_in_queue;
  connection_t **prev_in_queue;
  ulonglong abs_wait_timeout;
  ulonglong enqueue_time;
  ulonglong enqueue_htime;  
  bool logged_in;
  bool bound_to_poll_descriptor;
  bool waiting;
  ulong kickup;
  ulong tickets;
};

typedef I_P_List<connection_t,
                     I_P_List_adapter<connection_t,
                                      &connection_t::next_in_queue,
                                      &connection_t::prev_in_queue>,
                     I_P_List_counter,
                     I_P_List_fast_push_back<connection_t> >
connection_queue_t;


struct thread_group_counters_t
{
  ulonglong thread_creations;
  ulonglong thread_creations_due_to_stall;
  ulonglong wakes;
  ulonglong wakes_due_to_stall;
  ulonglong throttles;
  ulonglong stalls;
  ulonglong dequeues_by_worker;
  ulonglong dequeues_by_listener;
  ulonglong polls_by_listener;
  ulonglong polls_by_worker;
  ulonglong kickups;
};

//struct MY_ALIGNED(CPU_LEVEL1_DCACHE_LINESIZE) thread_group_t
struct MY_ALIGNED(512) thread_group_t
{
  mysql_mutex_t mutex;
  connection_queue_t queue;
  connection_queue_t high_prio_queue;
  worker_list_t waiting_threads;
  worker_thread_t *listener;
  pthread_attr_t *pthread_attr;
  int  pollfd;
  int  thread_count;
  int  active_thread_count;
  int  connection_count;
  int  waiting_thread_count;
  /* Stats for the deadlock detection timer routine.*/
  int io_event_count;
  int queue_event_count;
  ulonglong last_thread_creation_time;
  int  shutdown_pipe[2];
  bool shutdown;
  bool stalled;
  int id;
  thread_group_counters_t counters;
  };
  
#define TP_INCREMENT_GROUP_COUNTER(group,var) group->counters.var++;

extern thread_group_t all_groups[MAX_THREAD_GROUPS];
//static thread_group_t all_groups[MAX_THREAD_GROUPS];
//static uint group_count;


#endif /* THREADPOOL_INCLUDED */
