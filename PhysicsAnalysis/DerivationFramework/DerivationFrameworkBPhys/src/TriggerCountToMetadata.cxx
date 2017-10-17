/*
Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
//============================================================================
// 
// Author : Matteo Bedognetti <matteo.bedognetti@cern.ch.>
// Changes:
//
// Store trigger counts for specific chains in the DAOD's MetaData.
// This allows it to store information about triggers upon which events are NOT selected during the derivation
//
// Job options:
// - TriggerList   -- a vector containing all triggers to store as strings
// - FolderName -- Is supposed to be the derivation name (some convention I guess)
// - TrigDecisionTool -- if one wants to pass this a specific TrigDecisionTool
//                           
//============================================================================
//

#include "DerivationFrameworkBPhys/TriggerCountToMetadata.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "TrigDecisionTool/TrigDecisionTool.h"

#include <memory>

namespace DerivationFramework {

  //--------------------------------------------------------------------------
  TriggerCountToMetadata::TriggerCountToMetadata(const std::string& t,
				       const std::string& n,
				       const IInterface*  p)
    : CfAthAlgTool(t,n,p),   m_trigDecisionTool( "Trig::TrigDecisionTool/TrigDecisionTool" ), m_outputMetaStore("StoreGateSvc/MetaDataStore", n)
 {
    declareInterface<DerivationFramework::IAugmentationTool>(this);
    
    declareProperty("TrigDecisionTool", m_trigDecisionTool );
    declareProperty("FolderName",       m_folderName = "DerivationLevel");
    declareProperty("TriggerList",      m_triggerList);

  }
  //--------------------------------------------------------------------------
  StatusCode TriggerCountToMetadata::initialize() {
  

    return StatusCode::SUCCESS;
  }  
  //--------------------------------------------------------------------------
  StatusCode TriggerCountToMetadata::finalize() {

    ATH_MSG_DEBUG("TriggerCountToMetadata::finalize()");

    return StatusCode::SUCCESS;
  }
  //--------------------------------------------------------------------------
  StatusCode TriggerCountToMetadata::addBranches() const {

	  ATH_MSG_DEBUG("Inside TriggerCountToMetadata::addBranches()");

	  // W.w. method
	  addEvent("AllEvents");

	  for( unsigned int i=0; i<m_triggerList.size(); i++){
	  	addEvent(m_triggerList[i] , m_trigDecisionTool->isPassed(m_triggerList[i]) );
	  }

	  return StatusCode::SUCCESS;
  }  

}  // End of namespace DerivationFramework
