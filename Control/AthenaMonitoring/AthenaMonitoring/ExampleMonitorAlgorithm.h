/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EXAMPLEMONITORALGORITHM_H
#define EXAMPLEMONITORALGORITHM_H

#include "AthenaMonitoring/AthMonitorAlgorithm.h"

#include "AthenaMonitoring/MonitoredScope.h"
#include "AthenaMonitoring/MonitoredScalar.h"
#include "AthenaMonitoring/MonitoredCollection.h"

#include "TRandom3.h"

class ExampleMonitorAlgorithm : public AthMonitorAlgorithm {
public:
    ExampleMonitorAlgorithm( const std::string& name, ISvcLocator* pSvcLocator );
    virtual ~ExampleMonitorAlgorithm();
    StatusCode initialize();
    virtual StatusCode fillHistograms( const EventContext& ctx ) const override;
private:
	Gaudi::Property<bool> m_doRandom {this,"RandomHist",false};
};
#endif