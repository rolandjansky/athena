/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "./AcceptWhenInputPresent.h"


AcceptWhenInputPresent::AcceptWhenInputPresent(const std::string& name, ISvcLocator* pSvcLocator) 
  : HLT::AllTEAlgo(name, pSvcLocator), 
    m_seedFromInputs(false) {

  declareProperty("requiredMultiplicity",  m_requiredMultiplicity, "For each input type the minimum multiplicity <=0 means no requreiement");
  declareProperty("seedFromInputs", m_seedFromInputs, "Use all input as seed to the output");
}


HLT::ErrorCode AcceptWhenInputPresent::hltExecute(std::vector<std::vector<HLT::TriggerElement*> >& input,
						  unsigned int output) {
  if ( input.size() != m_requiredMultiplicity.size() ) {
    ATH_MSG_ERROR("Missconfiguration, algorithm(sequence) seeded by " << input.size() 
		  << " TEs while multiplicity requirement configuration specifies thresholds for " 
		  << m_requiredMultiplicity.size() << " TEs" );
    return HLT::ErrorCode(HLT::Action::ABORT_JOB, HLT::Reason::BAD_JOB_SETUP);
  }

  // ACTUAL verification
  bool allOk = true;
  for ( size_t i = 0, max = m_requiredMultiplicity.size(); i< max; i++ ) {
    if ( int(input[i].size()) < m_requiredMultiplicity[i] ) {
      ATH_MSG_DEBUG("Missing TEs from the seed vector at position "<< i 
		    << "  " << input[i].size() <<  " < "  << m_requiredMultiplicity[i]);
      allOk=false;
    }
  }
  if ( allOk == false ) 
    return HLT::OK;
  
  std::vector<HLT::TriggerElement*> seeds;
  if ( m_seedFromInputs ) {
    for( auto& vecte:  input) 
      seeds.insert(seeds.end(), vecte.begin(), vecte.end());
  }

  HLT::TriggerElement* te = config()->getNavigation()->addNode(seeds, output);
  te->setActiveState(true);
  return HLT::OK;
}

