/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**************************************************************************
 **
 **   File:         RegSelTimer.h  
 **
 **   Description:  small timer wrapper  
 **                   
 **                   
 ** 
 **   Author:       M.Sutton  
 **
 **   Created:      Fri Jun 29 18:59:09 BST 2001
 **   Modified:     
 **                   
 **                   
 **
 **************************************************************************/ 


#ifndef __REGSELTIMER_H
#define __REGSELTIMER_H

#include <time.h>
#include <sys/time.h>

#define GTT_TIMER_START  0
#define GTT_TIMER_STOP   1
#define GTT_TIMER_SINCE -1

#ifdef __cplusplus
//extern "C" {
#endif

double gtttimer(int   );
void   gtttimer_initkey(void);
struct timeval gtttimer_start(void);
double         gtttimer_stop(struct timeval );

#ifdef __cplusplus
//}
#endif

#endif /* __REGSELTIMER_H */










