/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "TimingAlg.h"
#include "RecEvent/RecoTimingObj.h"
#include "StoreGate/StoreGateSvc.h"

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
  
  // retrieve the StoreGate Service (delete if not needed)
  if (!evtStore().retrieve().isSuccess()) {
    ATH_MSG_ERROR("Could not retrieve StoreGateSvc!");
    return StatusCode::FAILURE;
  }
  
  if ( m_determineCPUID && ! m_CPUID )
    {
      std::string vendor("none");
      std::string stepping("none");
      std::string cpuFamily("none");
      std::string model("none");
      
      FILE* file;
      unsigned int numProcessors(0);
      char line[1024];
      
      file = fopen("/proc/cpuinfo", "r");
      numProcessors = 0;
      while(fgets(line, 1024, file) != NULL){
	if (strncmp(line, "processor",   9) == 0) numProcessors++;
	if (strncmp(line, "vendor_id",   9) == 0) vendor=line;
	if (strncmp(line, "stepping",    8) == 0) stepping=line;
	if (strncmp(line, "cpu family", 10) == 0) cpuFamily=line;
	if (strncmp(line, "model\t"   ,  6) == 0) model=line;
      }
      fclose(file);
      
      size_t found=vendor.find(':');
      if (found!=std::string::npos)
	vendor.erase(0,found+2);
      
      vendor.erase(vendor.size()-1);
      stepping.erase(stepping.size()-1);
      cpuFamily.erase(cpuFamily.size()-1);
      model.erase(model.size()-1);
      
      ATH_MSG_INFO( "number of CPUs found: " << numProcessors << " of type " << vendor << " "
		    << cpuFamily << " " << model << " " << stepping );
      
      //compress all strings/numbers into 2 uints
      m_CPUID_a = 0xFFFFFFFF;
      if ( vendor.length() == 12 )
	{
	  m_CPUID_a =
	    0x01000000 * vendor[0] +
	    0x00010000 * vendor[1] +
	    0x00000100 * vendor[10] +
	    0x00000001 * vendor[11]; 
	  
	  ATH_MSG_DEBUG( "CPU : vendor " << vendor[0] << vendor[1] << " * " << vendor[10] << vendor[11] );
	}
      
      m_CPUID_b = 0;
      int s(0);
      s=0xFF;
      if (stepping.compare("none") != 0)
	sscanf(stepping.c_str(),"stepping\t: %80d",&s);
      ATH_MSG_VERBOSE( "stepping  : " << s );
      if ( s > 255 )
	s = 255;
      m_CPUID_b += 0x00000100 * s;
      
      s=0xFF;
      if (cpuFamily.compare("none") != 0)
	sscanf(cpuFamily.c_str(),"cpu family\t: %80d",&s);
      ATH_MSG_VERBOSE( "cpuFamily : " << s );
      if ( s > 255 )
	s = 255;
      m_CPUID_b += 0x01000000 * s;
      
      s=0xFF;
      if (model.compare("none") != 0)
	sscanf(model.c_str(),"model\t: %80d",&s);
      ATH_MSG_VERBOSE( "model     : " << s );
      if ( s > 255 )
	s = 255;
      m_CPUID_b += 0x00010000 * s;
      
      s = numProcessors;
      if ( s > 255 )
	s = 255;
      m_CPUID_b += s;
      
      ATH_MSG_VERBOSE( "CPU : compresses to " << std::hex << m_CPUID_a << " " << m_CPUID_b << std::dec );
      
      m_CPUID=1;
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
