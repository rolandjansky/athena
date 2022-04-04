/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGLONGLIVEDPARTICLESHYPO_TRIGMUONHYPOTOOL_H
#define TRIGLONGLIVEDPARTICLESHYPO_TRIGMUONHYPOTOOL_H

#include <string>
#include <vector>
#include <set>
#include <map>

#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/StatusCode.h"

#include "Gaudi/Property.h"

#include "AthenaBaseComps/AthAlgTool.h"
#include "AthenaMonitoringKernel/GenericMonitoringTool.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"

#include "xAODTrigger/TrigComposite.h"
#include "xAODTrigger/TrigCompositeContainer.h"
#include "xAODTrigger/TrigCompositeAuxContainer.h"

#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"

class MuonClusterHypoTool : public AthAlgTool
{

public:

    MuonClusterHypoTool( const std::string& type, const std::string& name, const IInterface* parent );
    ~MuonClusterHypoTool();
    StatusCode initialize();

    struct DecisionInfo {
        TrigCompositeUtils::Decision* decision;
        const xAOD::TrigCompositeContainer* Composites;
        const TrigCompositeUtils::DecisionIDContainer activeChainIDs;
    };

    StatusCode decide( DecisionInfo& input )  const;

private:

    HLT::Identifier m_decisionId;
    ToolHandle<GenericMonitoringTool> m_monTool{ this, "MonTool", "", "Monitoring tool" };
    Gaudi::Property<bool> m_acceptAll {this, "acceptAll", false, "Whether we are to accept all MuonRoIClusters or only those that pass selection"};
    Gaudi::Property<int>  m_nRoIEndCap {this, "nRoIEndCap", 4, "Number of RoI required to reconstruct a cluster in the Endcap"};
    Gaudi::Property<int>  m_nRoIBarrel {this, "nRoIBarrel", 3, "Number of RoI required to reconstruct a cluster in the Barrel"};
    Gaudi::Property<float>  m_etaMax   {this, "maxEta", 2.5, "Maximum eta value below which muonRoIClusters can reliably be reconstructed"};
    Gaudi::Property<float>  m_etaMid   {this, "midEta", 1.0, "Eta value used to define the boundary between the Barrel and Endcap"};

};

#endif //> !TRIGLONGLIVEDPARTICLESHYPO_TRIGMUONHYPOTOOL_H
