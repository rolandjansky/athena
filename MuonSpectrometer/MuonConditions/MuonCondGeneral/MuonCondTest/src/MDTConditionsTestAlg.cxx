/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "MDTConditionsTestAlg.h"

#include "MuonCondSvc/MuonHierarchy.h"

// Gaudi includes
#include "GaudiKernel/StatusCode.h"

// Athena includes
#include "Identifier/Identifier.h"

MDTConditionsTestAlg::MDTConditionsTestAlg(const std::string& name, ISvcLocator* pSvcLocator) :
    AthAlgorithm(name, pSvcLocator), m_pSummarySvc("MDTCondSummarySvc", name) {}

MDTConditionsTestAlg::~MDTConditionsTestAlg() = default;
// Initialize
StatusCode MDTConditionsTestAlg::initialize() {
    StatusCode sc(StatusCode::SUCCESS);
    msg(MSG::INFO) << "Calling initialize" << endmsg;
    sc = m_pSummarySvc.retrieve();

    if (StatusCode::SUCCESS not_eq sc) { msg(MSG::ERROR) << "Could not retrieve the summary service" << endmsg; }

    return sc;
}

// Execute
StatusCode MDTConditionsTestAlg::execute() {
    StatusCode sc(StatusCode::SUCCESS);

    msg(MSG::INFO) << "Calling execute" << endmsg;
    msg(MSG::INFO) << "Dummy call for the MDT STATUS" << endmsg;
    msg(MSG::INFO) << "THE CHAMBER SWITCHED OFF ARE: " << endmsg;
    m_pSummarySvc->deadStations();
    int size = m_pSummarySvc->deadStations().size();
    msg(MSG::INFO) << "writing from Algo SERVICE \n" << size << endmsg;
    for (int k = 0; k < size; k++) {
        std::string chamber = (m_pSummarySvc->deadStations()[k]);
        msg(MSG::INFO) << "writing from Algo SERVICE CHAMBER \n" << chamber << endmsg;
    }

    std::cout << "$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$4" << std::endl;
    if (m_pSummarySvc->isGoodChamber(Identifier(1699348480)))
        msg(MSG::VERBOSE) << "The chamber is good\n" << endmsg;
    else
        msg(MSG::INFO) << "The chamber is not good\n" << endmsg;
    int size_id = m_pSummarySvc->deadStationsId().size();
    msg(MSG::INFO) << "writing from Algo SERVICE Identifier\n" << size_id << endmsg;
    for (int k = 0; k < size_id; k++) {
        Identifier Id = (m_pSummarySvc->deadStationsId()[k]);
        msg(MSG::INFO) << "writing from Algo SERVICE CHAMBER \n" << Id << endmsg;
    }
    std::cout << "$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$4" << std::endl;

    int size_id2 = m_pSummarySvc->deadMultiLayersId().size();
    msg(MSG::INFO) << "writing from Algo SERVICE Identifier\n" << size_id2 << endmsg;
    for (int k = 0; k < size_id2; k++) {
        Identifier Id = (m_pSummarySvc->deadMultiLayersId()[k]);
        msg(MSG::INFO) << "writing from Algo SERVICE MULTILAYERS \n" << Id << endmsg;
    }

    std::cout << "$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$4" << std::endl;
    m_pSummarySvc->deadTubes();
    int size2 = m_pSummarySvc->deadTubes().size();
    msg(MSG::INFO) << "writing from Algo SERVICE TUBES\n" << size2 << endmsg;
    std::cout << "$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$4" << std::endl;

    m_pSummarySvc->deadTubesId();
    int size2id = m_pSummarySvc->deadTubesId().size();
    msg(MSG::INFO) << "writing from Algo SERVICE TUBES\n" << size2id << endmsg;
    std::cout << "$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$4" << std::endl;
    return sc;
}
