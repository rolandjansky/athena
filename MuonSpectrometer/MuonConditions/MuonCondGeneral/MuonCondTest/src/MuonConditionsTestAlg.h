/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonConditionsTestAlg_H
#define MuonConditionsTestAlg_H
// STL
#include <string>

// Gaudi
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"

// Athena

#include "MuonCondInterface/ICSCConditionsSvc.h"
#include "MuonCondInterface/IMDTConditionsSvc.h"
#include "MuonCondInterface/IRPCConditionsSvc.h"
#include "MuonCondInterface/ITGCConditionsSvc.h"

// Forward declarations
class ISvcLocator;
class StatusCode;

class MuonConditionsTestAlg : public AthAlgorithm {
public:
    MuonConditionsTestAlg(const std::string &name, ISvcLocator *pSvcLocator);
    ~MuonConditionsTestAlg();

    StatusCode initialize() override;
    StatusCode execute() override;

private:
    ServiceHandle<IMDTConditionsSvc> m_pSummarySvc;
    ServiceHandle<IRPCConditionsSvc> m_rSummarySvc;
    ServiceHandle<ITGCConditionsSvc> m_tSummarySvc;
    ServiceHandle<ICSCConditionsSvc> m_vSummarySvc;
};  // end of class

#endif
