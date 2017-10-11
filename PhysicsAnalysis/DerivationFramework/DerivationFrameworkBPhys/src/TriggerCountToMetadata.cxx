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
#include "TH1D.h"
#include "xAODMetaData/FileMetaData.h"
#include "xAODCore/AuxInfoBase.h"

#include <memory>

namespace DerivationFramework {

  //--------------------------------------------------------------------------
  TriggerCountToMetadata::TriggerCountToMetadata(const std::string& t,
				       const std::string& n,
				       const IInterface*  p)
    : AthAlgTool(t,n,p),   m_trigDecisionTool( "Trig::TrigDecisionTool/TrigDecisionTool" ), m_outputMetaStore("StoreGateSvc/MetaDataStore", n)
 {
    declareInterface<DerivationFramework::IAugmentationTool>(this);
    
    declareProperty("TrigDecisionTool", m_trigDecisionTool );
    declareProperty("FolderName",       m_folderName = "DerivationLevel");
    declareProperty("TriggerList",      m_triggerList);

  }
  //--------------------------------------------------------------------------
  StatusCode TriggerCountToMetadata::initialize() {
  
	  ATH_MSG_INFO("Pre-derivation trigger counts will be stored inside: "<< m_folderName<<"_MetaData" );

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

	  std::string mdFolderKey= m_folderName + "_MetaData";

	  if(! m_outputMetaStore->contains< xAOD::FileMetaData >( mdFolderKey ) ){
		  //Produce the output folder if it doesn't exist already
		  std::unique_ptr<xAOD::FileMetaData> fm(new xAOD::FileMetaData());
		  std::unique_ptr<xAOD::AuxInfoBase> fmAux(new xAOD::AuxInfoBase());

		  fm->setStore( fmAux.get() );

		  //Replace with 2 vectors!!!
		  std::vector<std::string> v_name = m_triggerList;
		  v_name.push_back("All Events"); //Add an entry which counts all events

		  //Store directly a vector of zeros
		  fm->auxdata< std::vector<uint64_t> >("TriggerCounts") = std::vector<uint64_t>(m_triggerList.size()+1, 0);
		  fm->auxdata< std::vector<std::string> >("TriggerNames") = v_name;
		  ATH_CHECK( m_outputMetaStore->record( std::move(fm), mdFolderKey ) );
		  ATH_CHECK( m_outputMetaStore->record( std::move(fmAux), mdFolderKey+"Aux." ) );

	  }

	  xAOD::FileMetaData* fm = m_outputMetaStore->retrieve<xAOD::FileMetaData>(mdFolderKey);
	  if(!fm || ! fm->isAvailableWritable< std::vector<uint64_t> >("TriggerCounts") ){ATH_MSG_ERROR("Logic mismatch in TriggerCountToMetadata"); return StatusCode::FAILURE;}
	  if(!fm || ! fm->isAvailableWritable< std::vector<std::string> >("TriggerNames") ){ATH_MSG_ERROR("Logic mismatch in TriggerCountToMetadata"); return StatusCode::FAILURE;}



	  std::vector<uint64_t>& v_count = fm->auxdata< std::vector<uint64_t> >("TriggerCounts");

	  for(unsigned int i=0; i< m_triggerList.size(); i++) { //Do not get to the last!!!
		  if (m_trigDecisionTool->isPassed(m_triggerList[i]))v_count[i]++;
	  }

	  v_count[m_triggerList.size()]++;; //Keep track of the total number of events as well

	  return StatusCode::SUCCESS;
  }  

}  // End of namespace DerivationFramework
