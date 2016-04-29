/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "RecEventTPCnv/RecoTimingObjCnv_p1.h"

#include <malloc.h>

#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>

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
      float f0(0);
      float f1(0);
      float f2(0);
      
      if ( trans->size() > 0 )
	f0=(*trans)[0];
      if ( ! trans->m_isTimings )
	{
	  if ( trans->size() > 1 )
	    f1=(*trans)[1];
	  if ( trans->size() > 2 )
	    f2=(*trans)[2];
	}
      
      if ( pers->m_isTimings )
	{
	  struct rusage r;
	  getrusage(RUSAGE_SELF, &r);
	  f0=float(r.ru_utime.tv_sec+r.ru_stime.tv_sec) + float(r.ru_utime.tv_usec+r.ru_stime.tv_usec)/1000000 - f0;
	}
      else
	{
	  if(m_pagesize==0)
	    m_pagesize=sysconf(_SC_PAGESIZE);
	  unsigned long a0(-1);
	  unsigned long a1(-1);
	  unsigned long a2(-1);
	  FILE *proc = fopen("/proc/self/statm","r");
	  if (proc) {
	    fscanf(proc, "%80lu %80lu %80lu", &a0, &a1, &a2);
            fclose(proc);
          }
	  f0=float(a0*m_pagesize)-f0;
	  f1=float(a1*m_pagesize)-f1;
	  f2=float(a2*m_pagesize)-f2;
	}
      pers->timings.push_back( f0 );
      if ( ! pers->m_isTimings )
	{
	  pers->timings.push_back( f1 );
	  pers->timings.push_back( f2 );
	}
      if (msg.level() <= MSG::DEBUG)
	msg << MSG::DEBUG << "Creating persistent state of RecoTimingObj.[end] : " << f0 << endreq;
    }
  if (msg.level() <= MSG::DEBUG)
    msg << MSG::DEBUG << "Creating persistent state of RecoTimingObj.[end]"    << endreq;
  return;
}
