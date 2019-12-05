/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AFPHITSMONITORALGORITHM_H
#define AFPHITSMONITORALGORITHM_H

#include "AthenaMonitoring/AthMonitorAlgorithm.h"
#include "AthenaMonitoringKernel/Monitored.h"

#include "TRandom3.h"

class AFPHitsMonitorAlgorithm : public AthMonitorAlgorithm {
public:
    AFPHitsMonitorAlgorithm( const std::string& name, ISvcLocator* pSvcLocator );
    virtual ~AFPHitsMonitorAlgorithm();
    virtual StatusCode initialize() override;
    virtual StatusCode fillHistograms( const EventContext& ctx ) const override;
private:
    Gaudi::Property<bool> m_doRandom {this,"RandomHist",false};
    std::vector<int> m_abGroups1;
    std::vector<std::vector<int>> m_abGroups2;
    std::map<std::string,int> m_cGroups1;
    std::map<std::string,std::map<std::string,int>> m_cGroups2;
};
#endif
