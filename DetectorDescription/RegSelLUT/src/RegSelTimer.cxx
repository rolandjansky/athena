/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**************************************************************************
 **
 **   File:         RegSelTimer.cxx        
 **
 **   Description:    
 **                   
 **                   
 ** 
 **   Author:       M.Sutton    
 **
 **   Created:      Fri Jun 29 18:59:11 BST 2001
 **   Modified:     
 **                   
 **                   
 **
 **************************************************************************/ 

#include <stdlib.h>
#include <pthread.h>
#include "RegSelLUT/RegSelTimer.h"


// pthread_mutex_t time_lock = PTHREAD_MUTEX_INITIALIZER;  
// static pthread_key_t   time_key;


typedef struct {
  struct timeval start_time;
  struct timeval stop_time;
  struct timeval since_time;  
  struct timeval diff_time;
} __gtttimer;



#define _timersub Timersub


int Timersub(struct timeval* , struct timeval* , struct timeval* );



/** destructor **/
void __free_timer(void* t) {  if ( t!=NULL ) free((__gtttimer*)t); }

/** initialiser **/
// void gtttimer_initkey(void) {  pthread_key_create(&time_key, (void*)__free_timer); }
// void gtttimer_initkey(void) {  pthread_key_create(&time_key, __free_timer); }
// void gtttimer_initkey(void) { pthread_key_create(&time_key, __free_timer); }


/** initialise the gtttimer thread specific key **/
// __gtttimer* init_gtttimer(void) 
// {
//  __gtttimer* t = (__gtttimer*) malloc(1*sizeof(__gtttimer));
//  pthread_setspecific(time_key, (void*)t);
//  return t;
// }



/**  get the time, (probably needlessly) mutex protected 
 **  call to gettimeofday
 **/

void __gtt_gettime(struct timeval* t)
{
  //  pthread_mutex_lock(&time_lock);
  gettimeofday (t, NULL);            
  //  pthread_mutex_unlock(&time_lock);
}


#if 0

/**  store the times in thread specific keys to avoid conflicts 
 **  of the timer routines from different threads 
 **/

double gtttimer(int flag)
{
  double      time = 0;
  __gtttimer* t = (__gtttimer*)pthread_getspecific(time_key);
  
  if (flag==GTT_TIMER_START) {
    if (t==NULL) t = init_gtttimer();
    __gtt_gettime (&(t->start_time));
    t->since_time = t->start_time;
  }
  if (flag==GTT_TIMER_STOP)  {
    if (t==NULL) return -1;  /** if the timer wasn't initialised ... **/
    __gtt_gettime (&(t->stop_time));
    t->since_time = t->stop_time;
    _timersub( &(t->stop_time), &(t->start_time), &(t->diff_time) );  
    time = (t->diff_time.tv_sec*1000.0) + (t->diff_time.tv_usec/1000.0);
  }
  if (flag==GTT_TIMER_SINCE)  {
    if (t==NULL) return -1;  /** if the timer wasn't initialised ... **/
    __gtt_gettime (&(t->stop_time));
    _timersub( &(t->stop_time), &(t->since_time), &(t->diff_time) );  
    t->since_time = t->stop_time;
    time = (t->diff_time.tv_sec*1000.0) + (t->diff_time.tv_usec/1000.0);
  }
  
  return time;
}

#endif

/**  significantly (0.02ms) faster and simpler timer start/stop 
 **  functions 
 **/

struct timeval gtttimer_start(void)
{
  struct timeval start_time;
  __gtt_gettime (&start_time);
  return start_time;
}


double gttd(struct timeval time) { return (time.tv_sec*1000.0) + (time.tv_usec/1000.0); }

double gtttimer_stop(struct timeval start_time)
{
  double time = 0;
  struct timeval stop_time;
  struct timeval diff_time;
  //  double diff;

  __gtt_gettime (&stop_time);
  //  pthread_mutex_lock(&time_lock);
  _timersub( &stop_time, &start_time, &diff_time );
  //  printf("diff: %d\n", _timersub( &stop_time, &start_time, &diff_time ) );
  //  diff = gttd(stop_time)-gttd(start_time);
  //  printf("timer:   %12.5f %12.5f  %12.5f   %12.5f\n", gttd(stop_time), gttd(start_time), gttd(diff_time), diff ); 
  //  pthread_mutex_unlock(&time_lock);  
  time = (diff_time.tv_sec*1000.0) + (diff_time.tv_usec/1000.0);
  return time;
}


int Timersub(struct timeval* stop_time, struct timeval* start_time, struct timeval* diff_time)
{  
  diff_time->tv_sec  = stop_time->tv_sec  - start_time->tv_sec;
  diff_time->tv_usec = stop_time->tv_usec - start_time->tv_usec;
  return 0;
}









