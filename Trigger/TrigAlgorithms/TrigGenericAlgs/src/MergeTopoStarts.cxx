/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MergeTopoStarts.h"


MergeTopoStarts::MergeTopoStarts(const std::string& name, ISvcLocator* pSvcLocator)
  : AllTEAlgo(name, pSvcLocator) 
{
  declareMonitoredVariable("reductionWrt1", m_reductionWrt1);
  declareMonitoredVariable("reductionWrt2", m_reductionWrt2);

}



HLT::ErrorCode MergeTopoStarts::hltExecute(std::vector<std::vector<HLT::TriggerElement*> >& tes_in, 
					   unsigned int type_out){
  float merged = 0;
  m_reductionWrt1 = m_reductionWrt2 = -1;
  if ( tes_in.size() != 2 ) {
    ATH_MSG_ERROR("This alg should be seeded by exactly two TEs");
    return HLT::ErrorCode(HLT::Action::ABORT_JOB, HLT::Reason::BAD_JOB_SETUP);
  }
  for ( auto te1Ptr: tes_in[0]) {
    for ( auto te2Ptr: tes_in[1] ) {
      if ( te1Ptr->getId() == te2Ptr->getId() ) {
	ATH_MSG_ERROR("This alg *should not* be seeded by the same TEs");
	return HLT::ErrorCode(HLT::Action::ABORT_JOB, HLT::Reason::BAD_JOB_SETUP);
      }
      const std::vector<HLT::TriggerElement*>& te1Seeds =  HLT::Navigation::getDirectPredecessors(te1Ptr);
      const std::vector<HLT::TriggerElement*>& te2Seeds =  HLT::Navigation::getDirectPredecessors(te2Ptr);
      if ( std::set<HLT::TriggerElement*>(te1Seeds.begin(), te1Seeds.end()) 
	   == std::set<HLT::TriggerElement*>(te2Seeds.begin(), te2Seeds.end()) )  {
	merged += 1;
	std::vector<HLT::TriggerElement*> seeds(te1Seeds.begin(), te1Seeds.end());
	config()->getNavigation()->addNode(seeds, type_out);
      }	
    }
  }
  if ( not tes_in[0].empty())
    m_reductionWrt1 = merged/tes_in[0].size();

  if ( not tes_in[1].empty())
    m_reductionWrt2 = merged/tes_in[1].size();

  return HLT::OK;
}

