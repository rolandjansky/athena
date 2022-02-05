/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCondTest/MDTConditionsTestAlgMT.h"

// Constructor
MDTConditionsTestAlgMT::MDTConditionsTestAlgMT(const std::string& name, ISvcLocator* pSvcLocator) : AthAlgorithm(name, pSvcLocator) {}

// Destructor
MDTConditionsTestAlgMT::~MDTConditionsTestAlgMT() = default;

// Initialize
StatusCode MDTConditionsTestAlgMT::initialize() {
    ATH_MSG_INFO("Calling initialize");
    ATH_CHECK(m_readKey.initialize());
    return StatusCode::SUCCESS;
}

// Execute
StatusCode MDTConditionsTestAlgMT::execute() {
    StatusCode sc(StatusCode::SUCCESS);

    ATH_MSG_INFO("Calling execute");
    std::stringstream ss;
    ss << "Now setting up read handle: ";
    SG::ReadCondHandle<MdtCondDbData> readHandle{m_readKey};
    const MdtCondDbData* readCdo{*readHandle};
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
    ss << "Reading Dead Tubes: ";
    int size = readCdo->getDeadTubesId().size();
    ss << "found " << size << " via Id; containing:";
    ATH_MSG_INFO(ss.str());
    for (int k = 0; k < size; ++k) { ATH_MSG_INFO("\t" << k << ": " << readCdo->getDeadTubes()[k]); }

    ss.clear();
    ss << "Reading Dead Layers: ";
    size = readCdo->getDeadLayersId().size();
    ss << "found " << size << " via Id; containing:";
    ATH_MSG_INFO(ss.str());
    for (int k = 0; k < size; ++k) { ATH_MSG_INFO("\t" << k << ": " << readCdo->getDeadLayers()[k]); }

    ss.clear();
    ss << "Reading Dead Multilayers: ";
    size = readCdo->getDeadMultilayersId().size();
    ss << "found " << size << " via Id; containing:";
    ATH_MSG_INFO(ss.str());
    for (int k = 0; k < size; ++k) { ATH_MSG_INFO("\t" << k << ": " << readCdo->getDeadMultilayers()[k]); }

    ss.clear();
    ss << "Reading Dead Stations: ";
    size = readCdo->getDeadStationsId().size();
    ss << "found " << size << " via Id; containing:";
    ATH_MSG_INFO(ss.str());
    for (int k = 0; k < size; ++k) { ATH_MSG_INFO("\t" << k << ": " << readCdo->getDeadStations()[k]); }

    ss.clear();
    ss << "Reading Chambers with Dead Channels: ";
    size = readCdo->getDeadChambersId().size();
    ss << "found " << size << " via Id; containing:";
    ATH_MSG_INFO(ss.str());
    for (int k = 0; k < size; ++k) { ATH_MSG_INFO("\t" << k << ": " << readCdo->getDeadChambersId()[k]); }

    ATH_MSG_INFO("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$");

    ATH_MSG_INFO("Now going to probe some Ids");
    ATH_MSG_INFO("ID=1699348480; isGood? " << readCdo->isGood(Identifier(1699348480)));

    ATH_MSG_INFO("MADE IT TO THE END!!");
    return sc;
}
