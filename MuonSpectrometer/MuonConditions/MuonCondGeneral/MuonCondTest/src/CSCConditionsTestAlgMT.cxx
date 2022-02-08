/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCondTest/CSCConditionsTestAlgMT.h"

// Constructor
CSCConditionsTestAlgMT::CSCConditionsTestAlgMT(const std::string& name, ISvcLocator* pSvcLocator) : AthAlgorithm(name, pSvcLocator) {}

// Destructor
CSCConditionsTestAlgMT::~CSCConditionsTestAlgMT() = default;

// Initialize
StatusCode CSCConditionsTestAlgMT::initialize() {
    ATH_MSG_INFO("Calling initialize");
    ATH_CHECK(m_readKey.initialize());
    return StatusCode::SUCCESS;
}

// Execute
StatusCode CSCConditionsTestAlgMT::execute() {
    StatusCode sc(StatusCode::SUCCESS);

    ATH_MSG_INFO("Calling execute");
    std::stringstream ss;
    ss << "Now setting up read handle: ";
    SG::ReadCondHandle<CscCondDbData> readHandle{m_readKey};
    const CscCondDbData* readCdo{*readHandle};
    if (readCdo == nullptr) {
        ss << "DID NOT WORK!";
        ATH_MSG_INFO(ss.str());
        ATH_MSG_ERROR("Null pointer to the read conditions object");
        return StatusCode::FAILURE;
    } else {
        ss << "WORKED!";
        ATH_MSG_INFO(ss.str());
    }

    ss.clear();

    unsigned int hash = 24903;
    ATH_MSG_INFO("Focussing on channel with hash " << hash);
    ATH_MSG_INFO("Reading F001 =====> " << readCdo->getChannelF001(hash));
    ATH_MSG_INFO("Reading Noise ====> " << readCdo->getChannelNoise(hash));
    ATH_MSG_INFO("Reading Pedestal => " << readCdo->getChannelPed(hash));
    ATH_MSG_INFO("Reading PSlope ===> " << readCdo->getChannelPSlope(hash));
    ATH_MSG_INFO("Reading RMS ======> " << readCdo->getChannelRMS(hash));
    ATH_MSG_INFO("Reading Status ===> " << readCdo->getChannelStatus(hash));
    ATH_MSG_INFO("Reading T0Base ===> " << readCdo->getChannelT0Base(hash));
    ATH_MSG_INFO("Reading T0Phase ==> " << readCdo->getChannelT0Phase(hash));
    ATH_MSG_INFO("Is it Good? ======> " << readCdo->isGoodChannelHash(hash));

    ATH_MSG_INFO("MADE IT TO THE END!!");
    return sc;
}
