/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EXAMPLEMONITORALGORITHM_H
#define EXAMPLEMONITORALGORITHM_H

#include "AthenaMonitoring/AthMonitorAlgorithm.h"
#include "AthenaMonitoringKernel/Monitored.h"

#include "TRandom3.h"

class ExampleMonitorAlgorithm : public AthMonitorAlgorithm {
public:
    ExampleMonitorAlgorithm( const std::string& name, ISvcLocator* pSvcLocator );
    virtual ~ExampleMonitorAlgorithm();
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
