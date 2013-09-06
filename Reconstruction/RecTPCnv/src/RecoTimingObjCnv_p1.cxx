/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "RecTPCnv/RecoTimingObjCnv_p1.h"

#include <malloc.h>

#include <sys/time.h>
#include <sys/resource.h>

void RecoTimingObjCnv_p1::persToTrans( const RecoTimingObj_p1* pers,
				       RecoTimingObj* trans, 
				       MsgStream& msg ) 
{
  if (msg.level() <= MSG::DEBUG)
    msg << MSG::DEBUG << "Loading RecoTimingObj from persistent state.[start]"  << endreq;
  trans->m_readFromFile=true;
  trans->m_isTimings=pers->m_isTimings;

  trans->clear();
  trans->reserve(pers->timings.size());
  for( unsigned int i(0); i<pers->timings.size(); ++i )
    trans->push_back( pers->timings[i] );
  if (msg.level() <= MSG::DEBUG)
    msg << MSG::DEBUG << "Loading RecoTimingObj from persistent state.[end]"  << endreq;
  return;
}

void RecoTimingObjCnv_p1::transToPers( const RecoTimingObj* trans, 
				       RecoTimingObj_p1* pers, 
				       MsgStream& msg ) 
{
  if (msg.level() <= MSG::DEBUG)
    msg << MSG::DEBUG << "Creating persistent state of RecoTimingObj.[start]"    << endreq;
  if ( trans->m_readFromFile )
    pers->timings.reserve(trans->size());
  else
    {
      if ( trans->m_isTimings )
	pers->timings.reserve(trans->size()+1);
      else
	pers->timings.reserve(trans->size()+2);
    }
  pers->m_isTimings=trans->m_isTimings;

  for( unsigned int i(0); i<trans->size(); ++i )
    {
      pers->timings.push_back( (*trans)[i] );
      if (msg.level() <= MSG::DEBUG)
	msg << MSG::DEBUG << "Creating persistent state of RecoTimingObj.[" << i << "] : " << (*trans)[i] << endreq;
    }
  if ( ! trans->m_readFromFile && trans->size() > 0 )
    {
      float fv(0);
      float f0(0);
      float f1(0);
      uintptr_t mem(0);
      
      if ( trans->size() > 0 )
	f0=(*trans)[0];
      if ( ! trans->m_isTimings )
	if ( trans->size() > 1 )
	  f1=(*trans)[1];
      
      if ( pers->m_isTimings )
	{
	  struct rusage r;
	  getrusage(RUSAGE_SELF, &r);
	  fv=float(r.ru_utime.tv_sec+r.ru_stime.tv_sec) + float(r.ru_utime.tv_usec+r.ru_stime.tv_usec)/1000000;
	}
      else
	{
	  int a(0);
	  FILE *proc = fopen("/proc/self/statm","r");
	  if (proc) {
	    fscanf(proc, "%d", &a);
            fclose(proc);
          }
	  fv=float(a);
	  
	  char buffer[2048] = {0};
	  int out_pipe[2];
	  int saved_stderr = dup (STDERR_FILENO);
	  pipe (out_pipe);
	  dup2 (out_pipe[1], STDERR_FILENO);
	  close (out_pipe[1]);
	  malloc_stats ();
	  int n = read (out_pipe[0], buffer, 2047);
          // Make coverity happy...
          if (n < 0) n = 0;
          if (n >= 2048) n = 2047;
          buffer[n] = 0;
	  // close (STDERR_FILENO);
          if (saved_stderr >= 0) {
            dup2 (saved_stderr, STDERR_FILENO);
            close (saved_stderr);
          }
	  close (out_pipe[0]);
	  
	  // ATH_MSG_INFO("ROLF " << buffer);  
	  unsigned int start(0);
	  while ( start < strlen(buffer) )
	    {
	      int pos(0);
	      uintptr_t memtmp(0);
	      //int ret;
	      if ( trans->m_usetcmalloc )
		{
#if __WORDSIZE == 64
		  sscanf(&buffer[start], "MALLOC:   %lu Bytes in use by application%n", &memtmp, &pos);
#else
		  sscanf(&buffer[start], "MALLOC:   %u Bytes in use by application%n", &memtmp, &pos);
#endif
		  if ( pos )
		    mem=memtmp;
		}
	      else
		{
#if __WORDSIZE == 64
		  sscanf(&buffer[start], "in use bytes     = %lu%n", &memtmp, &pos);
#else
		  sscanf(&buffer[start], "in use bytes     = %u%n", &memtmp, &pos);
#endif
		  if ( pos )
		    mem=memtmp;
		}
	      // ATH_MSG_INFO("ROLF rr " << start << " " << ret << " " << memtmp << " " << mem << " len: " << strlen(buffer) << " pos : " << pos );  
	      while (buffer[start] != '\n' && start<strlen(buffer))
		start += 1;
	      start += 1;
	    }
	}
      pers->timings.push_back( fv - f0 );
      if ( ! pers->m_isTimings )
	pers->timings.push_back( float(mem)/1024 - f1 );
      
      if (msg.level() <= MSG::DEBUG)
	msg << MSG::DEBUG << "Creating persistent state of RecoTimingObj.[end] : " << fv - f0 << endreq;
    }
  if (msg.level() <= MSG::DEBUG)
    msg << MSG::DEBUG << "Creating persistent state of RecoTimingObj.[end]"    << endreq;
  return;
}
