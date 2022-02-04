/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RPCStatusTestAlg_H
#define RPCStatusTestAlg_H
// STL
#include <string>

// Gaudi
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"

// Athena

//#include "MuonCondInterface/IMuonConditionsSvc.h"
#include "MuonCondInterface/IRPCConditionsSvc.h"

// Forward declarations
class ISvcLocator;
class StatusCode;

/// Example class to show calling the RPC_ConditionsSummarySvc
class RPCStatusTestAlg : public AthAlgorithm {
public:
    RPCStatusTestAlg(const std::string &name, ISvcLocator *pSvcLocator);
    ~RPCStatusTestAlg();

    StatusCode initialize() override;
    StatusCode execute() override;

private:
    ServiceHandle<IRPCConditionsSvc> m_pSummarySvc;
};  // end of class

#endif
