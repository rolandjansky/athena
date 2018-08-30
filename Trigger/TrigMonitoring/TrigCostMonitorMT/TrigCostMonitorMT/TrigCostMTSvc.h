/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGCOSTMONITORMT_TRIGCOSTMTSVC_H
#define TRIGCOSTMONITORMT_TRIGCOSTMTSVC_H

#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthService.h"
#include "ITrigCostMTSvc.h"

/////////////////////////////////////////////////////////////////////////////
class TrigCostMTSvc : public virtual AthService, public ITrigCostMTSvc {
    public:
    TrigCostMTSvc(const std::string& name, ISvcLocator* pSvcLocator);
    ~TrigCostMTSvc();

    StatusCode initialize() override;
    StatusCode finalize() override;

    StatusCode beginAlg(const std::string& caller) override; 
    StatusCode endAlg(const std::string& caller) override; 

};

#endif // TRIGCOSTMONITORMT_TRIGCOSTMTSVC_H
