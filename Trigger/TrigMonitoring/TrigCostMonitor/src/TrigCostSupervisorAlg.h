/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGCOSTMONITOR_TRIGCOSTSUPERVISORALG_H
#define TRIGCOSTMONITOR_TRIGCOSTSUPERVISORALG_H 1

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "TrigCostMonitor/ITrigCostSvc.h"


/**
 * @class TrigCostSupervisorAlg
 * @brief Simple class to start the Cost Monitoring at the begining of HLTBeginSeq
 **/
class TrigCostSupervisorAlg : public AthReentrantAlgorithm
{ 
 public: 

    TrigCostSupervisorAlg( const std::string& name, ISvcLocator* pSvcLocator );

    virtual StatusCode  initialize() override;
    virtual StatusCode  execute( const EventContext& context ) const override;

  private:
    ServiceHandle<ITrigCostSvc> m_trigCostSvcHandle {
        this, "TrigCostSvc", "TrigCostSvc", "The trigger cost service" };
};

#endif //> !TRIGCOSTMONITOR_TRIGCOSTSUPERVISORALG_H

