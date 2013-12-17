/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/



#define private public
#define protected public
#include "TrigMonitoringEvent/TrigConfChain.h"
#undef private
#undef protected

#include "TrigCostBase/UtilHack.h"

using namespace std;

//-----------------------------------------------------------------------------
bool Anp::HasExpressStream(const TrigConfChain &chain)
{
  //
  // Chain has express stream tag?
  //
  const vector<string> &str = chain.getStream();

  for(unsigned i = 0; i < str.size(); ++i) {
    if(str.at(i) == "express") {	
      return true;
    }
  }
  
  return false;
}

//-----------------------------------------------------------------------------
double Anp::GetExpressPS(const TrigConfChain &chain)
{
  //
  // Hack to get around missing public get function to extract stream PS 
  //
  const vector<string> &str = chain.getStream();
  const vector<float>  &psv = chain.m_stream_prescale;
  
  if(str.size() == psv.size()) {
    for(unsigned i = 0; i < str.size(); ++i) {
      if(str.at(i) == "express") {	
	return psv.at(i);
      }
    }
  }

  return 0.0;
}

//-----------------------------------------------------------------------------
void Anp::SetExpressPS(TrigConfChain &chain, float ps)
{
  //
  // Hack to get around missing public get function to extract stream PS 
  //
  const vector<string> &str = chain.getStream();
  vector<float>        &psv = chain.m_stream_prescale;
  
  if(str.size() == psv.size()) {
    for(unsigned i = 0; i < str.size(); ++i) {
      if(str.at(i) == "express") {	
	psv.at(i) = ps;
	return;
      }
    }
  }
}


//-----------------------------------------------------------------------------
void Anp::RemoveChainGroups(TrigConfChain &chain)
{
  chain.m_group.clear();
}
