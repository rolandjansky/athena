/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"
#include "ViewAlgs/TrigCompositeTraversal.h"
#include "TrigConfHLTData/HLTUtils.h"

#include "./DumpDecisions.h"
DumpDecisions::DumpDecisions(const std::string& name, ISvcLocator* pSvcLocator) 
  : AthAlgorithm(name, pSvcLocator), 
    m_decisions("OutputDecisions") {
  declareProperty("OutputDecisions", m_decisions, "Name of the decisions object which needs to be dumped");
}



StatusCode DumpDecisions::initialize() {
  return StatusCode::SUCCESS;
}


StatusCode DumpDecisions::execute() {
  if ( not m_decisions.isValid() ) {
    ATH_MSG_WARNING("No decisions object");
    return StatusCode::RECOVERABLE;
  }

  const std::string decSource("Passed");
  for (const auto decision: *m_decisions.cptr() ) {
    
    if ( decision->hasDetail<std::vector<int>>(decSource) ) {
        std::vector<int> passedIdentifiers;

	decision->getDetail(decSource, passedIdentifiers);	
	std::vector<std::string> passedIdentifiersNames( passedIdentifiers.size() );
        std::vector<TrigConf::HLTHash> passedIdentifiersHash( passedIdentifiers.size() );
	std::transform(passedIdentifiers.begin(), passedIdentifiers.end(), passedIdentifiersNames.begin(), 
		       [](int id) { return TrigConf::HLTUtils::hash2string(id, "TE"); } );

	std::transform(passedIdentifiers.begin(), passedIdentifiers.end(), passedIdentifiersHash.begin(), 
		       [](int id) { return TrigConf::HLTHash(id); } );	

	ATH_MSG_DEBUG("Passed IDs   : " << passedIdentifiersHash);
	ATH_MSG_DEBUG("Passed Names : " << passedIdentifiersNames);
      } else {
	ATH_MSG_WARNING("Decision object has no information about \""+decSource+"\" identifiers");
      }
    // try to dive deeper for the RoI objects 
    auto withRoI = ViewAlgs::find( decision,  ViewAlgs::HasObject("initialRoI") );
    if ( withRoI ) {
      auto roi = withRoI->objectLink<TrigRoiDescriptorCollection>("initialRoI");      
      ATH_MSG_DEBUG("RoI Eta: " << (*roi)->eta() << " Phi: " << (*roi)->phi() << " RoIWord: " << (*roi)->roiWord());
    }
  }   
  return StatusCode::SUCCESS;
}

