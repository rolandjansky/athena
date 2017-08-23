/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

#include "xAODEventInfo/EventInfo.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "xAODCaloEvent/CaloClusterContainer.h"

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
    const static SG::AuxElement::Decorator<char> dec_isBadBatman("DFCommonJets_isBadBatman");

    const xAOD::EventInfo* ei = nullptr;
    ATH_CHECK(evtStore()->retrieve( ei, "EventInfo" ));

    const xAOD::CaloClusterContainer * clusters = nullptr;
    ATH_CHECK(evtStore()->retrieve(clusters,"CaloCalTopoClusters"));

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
