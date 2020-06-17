/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// BatBatmanAugmentationTool.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Author: Chris Young (christopher.young@cern.ch)
///////////////////////////////////////////////////////////////////
// This code is designed to augment EventInfo with a flag which
// labels events which are likely to suffer from excess noise
// in the EMEC-IW induced by saturation between trains. This is
// based on counting the number of clusters in this |eta| region
// of significant pT and bad LAr Quality.
///////////////////////////////////////////////////////////////////
#include "BadBatmanAugmentationTool.h"

#include "StoreGate/WriteDecorHandle.h"

namespace DerivationFramework {

  BadBatmanAugmentationTool::BadBatmanAugmentationTool(const std::string& t,
					       const std::string& n,
					       const IInterface* p) : 
    AthAlgTool(t,n,p)
  {
    declareInterface<DerivationFramework::IAugmentationTool>(this);
  }

  StatusCode BadBatmanAugmentationTool::initialize()
  {

    ATH_MSG_INFO("Init BadBatmanAugmentationTool");

    ATH_CHECK(m_eventInfo_key.initialize());
    ATH_CHECK(m_clusterContainer_key.initialize());
    ATH_CHECK(m_isBadBatmanKey.initialize());

    
    return StatusCode::SUCCESS;
  }

  StatusCode BadBatmanAugmentationTool::finalize()
  {

    ATH_MSG_INFO("Finalize BadBatmanAugmentationTool");

    return StatusCode::SUCCESS;
  }

  StatusCode BadBatmanAugmentationTool::addBranches() const
  {
    //Running BadBatmanAugmentationTool

    //Set the name of the variable to augment

    SG::WriteDecorHandle<xAOD::EventInfo,char> dec_isBadBatman(m_isBadBatmanKey);

    auto eventInfo = SG::makeHandle (m_eventInfo_key);
    if (!eventInfo.isValid()){
      ATH_MSG_WARNING("Invalid  xAOD::EventInfo datahandle"
		      << m_eventInfo_key.key()); 
      return StatusCode::FAILURE;
    }
    auto ei = eventInfo.cptr();

    auto clusterContainer = SG::makeHandle (m_clusterContainer_key);
    if(!clusterContainer.isValid()){
      ATH_MSG_WARNING("Invalid  xAOD::CaloClusterContainer datahandle"
		      << m_clusterContainer_key.key());
      return StatusCode::FAILURE;
    }
    auto clusters = clusterContainer.cptr();

    //We will now loop over the cluster container counting the number of clusters which pass the criteria
    bool isBatman=false;
    size_t nBatman=0;
    const static SG::AuxElement::ConstAccessor<float>  acc_AVGLARQ("AVG_LAR_Q");
    for ( auto ipart : *clusters ) {
      if (std::fabs(ipart->rawEta())<=2.5) continue;
      if (std::fabs(ipart->rawEta())>=3.2) continue;
      if (ipart->rawE()/cosh(ipart->rawEta())<500.) continue;
      if (acc_AVGLARQ(*ipart)/65535.<=0.2) continue;
      nBatman++;
    }
    //If we found more than 20 clusters then mark the event as bad
    if (nBatman>20) isBatman=true;
    dec_isBadBatman(*ei) = isBatman;

    return StatusCode::SUCCESS;
  }
}
