/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// Algorithm producing truth info for PrepRawData, keeping all MC particles contributed to a PRD.
// A. Gaponenko, 2006

#include "MuonTruthAlgs/MuonDetailedTrackTruthMaker.h"

#include <iterator>
#include <vector>

//================================================================
MuonDetailedTrackTruthMaker::MuonDetailedTrackTruthMaker(const std::string& name, ISvcLocator* pSvcLocator) :
    AthReentrantAlgorithm(name, pSvcLocator) {}

// Initialize method
// -----------------------------------------------------------------------------------------------------
StatusCode MuonDetailedTrackTruthMaker::initialize() {
    ATH_MSG_DEBUG("MuonDetailedTrackTruthMaker::initialize()");

    if (m_PRD_TruthNames.empty()) {
        ATH_MSG_FATAL("No PRD truth collections have been configured for processing");
        return StatusCode::FAILURE;
    }
    //----------------
    ATH_CHECK(m_truthTool.retrieve());
    ATH_MSG_DEBUG("Retrieved tool " << m_truthTool);

    m_detailedTrackTruthNames.reserve(m_trackCollectionNames.size());
    for (unsigned int i = 0; i < m_trackCollectionNames.size(); i++) {
        m_detailedTrackTruthNames.emplace_back(m_trackCollectionNames.at(i).key() + "DetailedTruth");
        ATH_MSG_INFO("process " << m_trackCollectionNames.at(i).key() << " for detailed truth collection "
                                << m_detailedTrackTruthNames.at(i).key());
    }

    ATH_CHECK(m_trackCollectionNames.initialize());
    ATH_CHECK(m_PRD_TruthNames.initialize());
    ATH_CHECK(m_detailedTrackTruthNames.initialize());

    //----------------
    return StatusCode::SUCCESS;
}

// -----------------------------------------------------------------------------------------------------
StatusCode MuonDetailedTrackTruthMaker::execute(const EventContext& ctx) const {
    ATH_MSG_DEBUG("MuonDetailedTrackTruthMaker::execute()");

    //----------------------------------------------------------------
    // Retrieve prep raw data truth
    std::vector<const PRD_MultiTruthCollection*> prdCollectionVector;
    for (SG::ReadHandle<PRD_MultiTruthCollection>& col : m_PRD_TruthNames.makeHandles(ctx)) {
        if (!col.isPresent()) continue;
        if (!col.isValid()) {
            ATH_MSG_WARNING("invalid PRD_MultiTruthCollection " << col.name());
            return StatusCode::FAILURE;
        }
        prdCollectionVector.push_back(col.cptr());
    }

    //----------------------------------------------------------------
    // Retrieve track collections

    int i = 0;
    for (SG::ReadHandle<TrackCollection>& tcol : m_trackCollectionNames.makeHandles(ctx)) {
        if (!tcol.isValid()) {
            ATH_MSG_WARNING("invalid TrackCollection " << tcol.name());
            return StatusCode::FAILURE;
        }
        if (!tcol.isPresent()) continue;

        //----------------------------------------------------------------
        // Produce and store the output.

        SG::WriteHandle<DetailedTrackTruthCollection> dttc(m_detailedTrackTruthNames.at(i), ctx);
        ATH_CHECK(dttc.record(std::make_unique<DetailedTrackTruthCollection>()));
        dttc->setTrackCollection(tcol.cptr());
        m_truthTool->buildDetailedTrackTruth(dttc.ptr(), *(tcol.cptr()), prdCollectionVector);
        i++;
    }
    return StatusCode::SUCCESS;
}