/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


#include "TimingAlg.h"
#include "RecEvent/RecoTimingObj.h"
#include "ParseCpuInfo.h"

unsigned int TimingAlg::m_CPUID=0;

//================ Constructor =================================================

TimingAlg::TimingAlg(const std::string& name, ISvcLocator* pSvcLocator)
  :
  AthAlgorithm(name,pSvcLocator),
  m_timingObjOutputName("unspecified"),
  m_determineCPUID(true),
  m_CPUID_a(0),
  m_CPUID_b(0)
{
  //  template for property declaration
  declareProperty("TimingObjOutputName", m_timingObjOutputName, "storegate key of output object");
  declareProperty("DetermineCPUID", m_determineCPUID,"flag to decide if CPUID should be determined");
}

//================ Initialisation =================================================

StatusCode TimingAlg::initialize()
{
  ATH_MSG_DEBUG( "initialize(); will write out RecoTimingObj with key " << m_timingObjOutputName);
  
  if ( m_determineCPUID && ! m_CPUID ){
    std::optional<CpuInfo> cpuInfo;
    try{
      cpuInfo = ParseCpuInfoFromFile("/proc/cpuinfo");
    } catch (std::runtime_error & e){
      ATH_MSG_ERROR(e.what());
      return StatusCode::FAILURE;
    }
    if (not cpuInfo.has_value()){
      ATH_MSG_ERROR("cpuinfo could not be parsed");
      return StatusCode::FAILURE;
    }
    m_CPUID = cpuInfo->cpuId;
    m_CPUID_a = cpuInfo->cpuId_a;
    m_CPUID_b = cpuInfo->cpuId_b;
    ATH_MSG_INFO(cpuInfo->msg);
  }
  return StatusCode::SUCCESS;  
}

StatusCode TimingAlg::execute()
{
  float f0(0);
  RecoTimingObj *t(0);
  if(evtStore()->contains<RecoTimingObj>(m_timingObjOutputName))
    {
      ATH_MSG_DEBUG("reading existing object ");
      evtStore()->retrieve(t,m_timingObjOutputName).ignore();
      if ( t->size() )
	f0=(*t)[0];
      ATH_MSG_DEBUG("got f0 " << f0 );
      m_CPUID=0;
    }
  else
    {
      ATH_MSG_DEBUG("creating new object ");
      t=new RecoTimingObj();
      evtStore()->record(t,m_timingObjOutputName).ignore();
    }
  
  struct rusage r;
  getrusage(RUSAGE_SELF, &r);
  float f=float(r.ru_utime.tv_sec+r.ru_stime.tv_sec) + float(r.ru_utime.tv_usec+r.ru_stime.tv_usec)*1e-6-f0;
  t->push_back(f);
  ATH_MSG_DEBUG(" CPU time for this event until now is: " << f);
  if ( m_determineCPUID && m_CPUID )
    {
      union mytypes_t
      {
	unsigned int ui;
	float f;
      } mytypes;
      // mytypes.ui=m_CPUID;     t->push_back(mytypes.f);
      
      mytypes.ui=0xF0F0F0F0;   t->push_back(mytypes.f);
      mytypes.ui=m_CPUID_a;    t->push_back(mytypes.f);
      mytypes.ui=m_CPUID_b;    t->push_back(mytypes.f);
      mytypes.ui=0x0F0F0F0F;   t->push_back(mytypes.f);
      
      // don't write out again
      m_CPUID=0;
      m_determineCPUID=false;
    }
  return StatusCode::SUCCESS;
}
