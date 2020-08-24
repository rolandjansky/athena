/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// BatBatmanAugmentationTool.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Author: Chris Young (christopher.young@cern.ch)
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_BADBATMANAUGMENTATIONTOOL_H
#define DERIVATIONFRAMEWORK_BADBATMANAUGMENTATIONTOOL_H

#include <string>
#include <vector>

#include "xAODEventInfo/EventInfo.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "xAODCaloEvent/CaloClusterContainer.h"

#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/ReadDecorHandleKey.h"
#include "StoreGate/WriteDecorHandleKey.h"

namespace DerivationFramework {

  class BadBatmanAugmentationTool : public AthAlgTool, public IAugmentationTool {
  public: 
    BadBatmanAugmentationTool(const std::string& t, const std::string& n, const IInterface* p);

    StatusCode initialize();
    StatusCode finalize();
    virtual StatusCode addBranches() const;

  private:

    SG::ReadHandleKey<xAOD::EventInfo> m_eventInfo_key{this, "EventInfo", "EventInfo", "Input event information"};
    SG::ReadHandleKey<xAOD::CaloClusterContainer> m_clusterContainer_key{this, "CaloCalTopoClusters", "CaloCalTopoClusters", "Input cluster container"};

    SG::WriteDecorHandleKey<xAOD::EventInfo> m_isBadBatmanKey {this
	,"IsBadBatmanKey"
	,"EventInfo.DFCommonJets_isBadBatman"
	,"Decoration for isBadBatman flag"};

  }; 
}

#endif // DERIVATIONFRAMEWORK_PFLOWAUGMENTATIONTOOL_H
