/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "MemoryAlg.h"
#include "RecEvent/RecoTimingObj.h"
#include "StoreGate/StoreGateSvc.h"

#include <malloc.h>
#include <stdint.h>
#include <unistd.h>

//================ Constructor =================================================

MemoryAlg::MemoryAlg(const std::string& name, ISvcLocator* pSvcLocator)
  :
  AthAlgorithm(name,pSvcLocator),
  m_memoryObjOutputName("unspecified"),
  m_pagesize(0)
{
  //  template for property declaration
  declareProperty("MemoryObjOutputName", m_memoryObjOutputName, "storegate key of output object");
}

//================ Initialisation =================================================

StatusCode MemoryAlg::initialize()
{
  ATH_MSG_DEBUG( "initialize(); will write out RecoTimingObj with key " << m_memoryObjOutputName);
  m_pagesize=sysconf(_SC_PAGESIZE);
  
  return StatusCode::SUCCESS;
}

StatusCode MemoryAlg::execute()
{
  float f0(0);
  float f1(0);
  float f2(0);
  RecoTimingObj *t(0);
  if(evtStore()->contains<RecoTimingObj>(m_memoryObjOutputName))
    {
      ATH_MSG_DEBUG("reading existing object ");
      evtStore()->retrieve(t,m_memoryObjOutputName).ignore();
      if ( t->size() > 0 )
	f0=(*t)[0];
      if ( t->size() > 1 )
	f1=(*t)[1];
      if ( t->size() > 2 )
	f2=(*t)[2];
      ATH_MSG_DEBUG("got f0 / f1 / f2" << f0 << " / " << f1 << " / " << f2);
    }
  else
    {
      ATH_MSG_DEBUG("creating new object ");
      t=new RecoTimingObj(false);
      evtStore()->record(t,m_memoryObjOutputName).ignore();
    }
  unsigned long a0(-1);
  unsigned long a1(-1);
  unsigned long a2(-1);
  FILE *proc = fopen("/proc/self/statm","r");
  if (proc) {
    (void)fscanf(proc, "%80lu %80lu %80lu", &a0, &a1, &a2);
    fclose(proc);
  }
  float f=float(a0*m_pagesize) - f0;
  t->push_back(f);
  ATH_MSG_DEBUG("Memory and delta memory for this event now is: " << f << " (" << f0 << ")" );
  f=float(a1*m_pagesize) - f1;
  t->push_back(f);
  f=float(a2*m_pagesize) - f2;
  t->push_back(f);
  
  /* Success */
  return StatusCode::SUCCESS;
}
