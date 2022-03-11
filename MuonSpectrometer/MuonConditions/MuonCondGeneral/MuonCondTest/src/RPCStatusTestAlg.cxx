/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "RPCStatusTestAlg.h"

#include "MuonCondSvc/MuonHierarchy.h"

// Gaudi includes
#include "GaudiKernel/StatusCode.h"

// Athena includes
#include "Identifier/Identifier.h"

RPCStatusTestAlg::RPCStatusTestAlg(const std::string& name, ISvcLocator* pSvcLocator) :
    AthAlgorithm(name, pSvcLocator), m_pSummarySvc("RPCCondSummarySvc", name) {}

RPCStatusTestAlg::~RPCStatusTestAlg() = default;

// Initialize
StatusCode RPCStatusTestAlg::initialize() {
    StatusCode sc(StatusCode::SUCCESS);
    msg(MSG::INFO) << "Calling initialize" << endmsg;
    sc = m_pSummarySvc.retrieve();
    if (StatusCode::SUCCESS not_eq sc) { msg(MSG::ERROR) << "Could not retrieve the summary service" << endmsg; }
    return sc;
}

// Execute
StatusCode RPCStatusTestAlg::execute() {
    msg(MSG::INFO) << "Calling execute" << endmsg;
    msg(MSG::INFO) << "Dummy call for the RPC STATUS" << endmsg;
    msg(MSG::INFO) << "THE Panels efficiencies ARE: " << endmsg;
    m_pSummarySvc->EffPanelId();
    unsigned int size = m_pSummarySvc->EffPanelId().size();
    msg(MSG::VERBOSE) << "writing from Algo SERVICE \n" << size << endmsg;
    for (unsigned int k = 0; k < size; k++) {
        Identifier panel = (m_pSummarySvc->EffPanelId()[k]);
        msg(MSG::VERBOSE) << "writing from Algo SERVICE Panel \n" << panel << endmsg;
    }

    StatusCode sc(StatusCode::SUCCESS);

    return sc;
}
