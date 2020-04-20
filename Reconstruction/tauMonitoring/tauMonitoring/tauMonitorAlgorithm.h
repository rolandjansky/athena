/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUMONITORALGORITHM_H
#define TAUMONITORALGORITHM_H

#include "AthenaMonitoring/AthMonitorAlgorithm.h"
#include "AthenaMonitoringKernel/Monitored.h"

#include "TRandom3.h"

#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/ReadHandleKey.h"

#include "xAODTau/TauJetContainer.h" 


class tauMonitorAlgorithm : public AthMonitorAlgorithm {
public:
    tauMonitorAlgorithm( const std::string& name, ISvcLocator* pSvcLocator );
    virtual ~tauMonitorAlgorithm();
    virtual StatusCode initialize() override;
    virtual StatusCode fillHistograms( const EventContext& ctx ) const override;
private:
    Gaudi::Property<bool> m_doRandom {this,"RandomHist",false};
    std::vector<int> m_abGroups1;
    std::vector<std::vector<int>> m_abGroups2;
    std::map<std::string,int> m_cGroups1;
    std::map<std::string,std::map<std::string,int>> m_cGroups2;

    SG::ReadHandleKey<xAOD::TauJetContainer> m_TauContainerKey {this, "TauRecContainer", "TauJets"};

    Gaudi::Property<float> m_etaMin {this, "etaMin", -1.};
    Gaudi::Property<float> m_etaMax {this, "etaMax", 3.0};

    Gaudi::Property<std::string> m_kinGroupName {this, "kinGroupName", "tauMonKinGroupBA"};

};
#endif
