/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CLUSTERMONITORALGORITHM_H
#define CLUSTERMONITORALGORITHM_H

#include "AthenaMonitoring/AthMonitorAlgorithm.h"
#include "AthenaMonitoringKernel/Monitored.h"

#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/ReadHandleKey.h"

#include "xAODCaloEvent/CaloClusterContainer.h" 
#include "xAODCaloEvent/CaloClusterAuxContainer.h"

class ClusterMonitorAlgorithm : public AthMonitorAlgorithm {
public:
    ClusterMonitorAlgorithm( const std::string& name, ISvcLocator* pSvcLocator );
    virtual ~ClusterMonitorAlgorithm();
    virtual StatusCode initialize() override;
    virtual StatusCode fillHistograms( const EventContext& ctx ) const override;
private:
    Gaudi::Property<float> m_lowEthresh {this,"lowEthresh",10.0};
    Gaudi::Property<float> m_medEthresh {this,"medEthresh",15.0};
    Gaudi::Property<float> m_medhiEthresh {this,"medhiEthresh",25.0};
    Gaudi::Property<float> m_hiEthresh {this,"hiEthresh",50.0};

    Gaudi::Property<float> m_EMlowEthresh {this,"EMlowEthresh",4.0};
    Gaudi::Property<float> m_EMmedEthresh {this,"EMmedEthresh",10.0};
    Gaudi::Property<float> m_EMmedhiEthresh {this,"EMmedhiEthresh",15.0};
    Gaudi::Property<float> m_EMhiEthresh {this,"EMhiEthresh",25.0};

    SG::ReadHandleKey<xAOD::CaloClusterContainer> m_CaloClusterContainerKey {this,"CaloTopoClusterContainer","CaloCalTopoClusters",
        "Name of the CaloTopoClusters container"};
    SG::ReadHandleKey<xAOD::CaloClusterContainer> m_EMClusterContainerKey {this,"EMTopoClusterContainer","egammaClusters",
        "Name of the EM-only clusters container"};

};
#endif
