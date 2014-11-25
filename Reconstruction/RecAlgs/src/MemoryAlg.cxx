/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "MemoryAlg.h"
#include "RecEvent/RecoTimingObj.h"
#include "StoreGate/StoreGateSvc.h"

#include <malloc.h>
#include <stdint.h>

//================ Constructor =================================================

MemoryAlg::MemoryAlg(const std::string& name, ISvcLocator* pSvcLocator)
  :
  AthAlgorithm(name,pSvcLocator),
  m_memoryObjOutputName("unspecified"),
  m_usetcmalloc(true)
{
  //  template for property declaration
  declareProperty("MemoryObjOutputName", m_memoryObjOutputName, "storegate key of output object");
}

//================ Initialisation =================================================

StatusCode MemoryAlg::initialize()
{
  ATH_MSG_DEBUG( "initialize(); will write out RecoTimingObj with key " << m_memoryObjOutputName);
  char *ch = getenv("USETCMALLOC");
  if ( ch && strncmp(ch,"0",1) == 0 )
    m_usetcmalloc=false;
  
  return StatusCode::SUCCESS;
}

StatusCode MemoryAlg::execute()
{
  float f0(0);
  float f1(0);
  RecoTimingObj *t(0);
  if(evtStore()->contains<RecoTimingObj>(m_memoryObjOutputName))
    {
      ATH_MSG_DEBUG("reading existing object ");
      evtStore()->retrieve(t,m_memoryObjOutputName).ignore();
      if ( t->size() )
	f0=(*t)[0];
      if ( t->size() > 0 )
	f1=(*t)[1];
      ATH_MSG_DEBUG("got f0 / f1 " << f0 << " / " << f1);
    }
  else
    {
      ATH_MSG_DEBUG("creating new object ");
      t=new RecoTimingObj(false);
      evtStore()->record(t,m_memoryObjOutputName).ignore();
    }
  int a(-1);
  FILE *proc = fopen("/proc/self/statm","r");
  if (proc) {
    (void)fscanf(proc, "%d", &a);
    fclose(proc);
  }
  float f=float(a) - f0;
  t->push_back(f);
  ATH_MSG_DEBUG("Memory and delta memory for this event now is: " << f << " (" << f0 << ")" );
  
  uintptr_t mem(0);
  
  char buffer[2048] = {0};
  int out_pipe[2];
  int saved_stderr;
  saved_stderr = dup (STDERR_FILENO);
  pipe (out_pipe);
  dup2 (out_pipe[1], STDERR_FILENO);
  close (out_pipe[1]);
  malloc_stats ();
  // when running under valgrind malloc_stats is a no-op, so reading from the pipe will stall
  fprintf(stderr,"\n");
  int len = read (out_pipe[0], buffer, 2047);
  if (len >= 0)
    buffer[len] = '\0'; // Redundant, but keeps coverity happy.
  // close (STDERR_FILENO);
  if (saved_stderr >= 0)
    dup2 (saved_stderr, STDERR_FILENO);
  close (out_pipe[0]);
  close (saved_stderr);
  
  ATH_MSG_DEBUG("ROLF " << buffer);  
  
  unsigned int start(0);
  while ( start < strlen(buffer) )
    {
      int pos(0);
      uintptr_t memtmp(0);
      int ret;
      if ( m_usetcmalloc )
	{
#if __WORDSIZE == 64
	  ret = sscanf(&buffer[start], "MALLOC:   %lu Bytes in use by application%n", &memtmp, &pos);
#else
	  ret = sscanf(&buffer[start], "MALLOC:   %u Bytes in use by application%n", &memtmp, &pos);
#endif
	  if ( pos )
	    mem=memtmp;
	}
      else
	{
#if __WORDSIZE == 64
	  ret = sscanf(&buffer[start], "in use bytes     = %lu%n", &memtmp, &pos);
#else
	  ret = sscanf(&buffer[start], "in use bytes     = %u%n", &memtmp, &pos);
#endif
	  if ( pos )
	    mem=memtmp;
	}
      ATH_MSG_VERBOSE("ROLF rr " << start << " " << ret << " " << memtmp << " " << mem << " len: " << strlen(buffer) << " pos : " << pos );  
      while (buffer[start] != '\n' && start<strlen(buffer))
	start += 1;
      start += 1;
    }
  
  
  t->push_back(float(mem)/1024 - f1);
  
  /* Success */
  return StatusCode::SUCCESS;
}
