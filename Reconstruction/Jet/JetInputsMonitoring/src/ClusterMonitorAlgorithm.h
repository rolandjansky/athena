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
    Gaudi::Property<float> m_lowEthresh {this,"lowEthresh",5.0};
    Gaudi::Property<float> m_medEthresh {this,"medEthresh",10.0};
    Gaudi::Property<float> m_medhiEthresh {this,"medhiEthresh",15.0};
    Gaudi::Property<float> m_hiEthresh {this,"hiEthresh",20.0};

    SG::ReadHandleKey<xAOD::CaloClusterContainer> m_CaloClusterContainerKey {this, "ClusterKey", "CaloCalTopoClusters",
	"StoreGate key of cluster collection to be monitored"};
};
#endif
