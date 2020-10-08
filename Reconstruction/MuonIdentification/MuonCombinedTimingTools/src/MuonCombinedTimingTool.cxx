/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCombinedTimingTool.h"

#include "TrkSpaceTimePoint/SpaceTimePoint.h"

//================ Constructor =================================================

Rec::MuonCombinedTimingTool::MuonCombinedTimingTool(const std::string& t, const std::string& n, const IInterface* p)
    : AthAlgTool(t, n, p)
{
    declareInterface<IMuonCombinedTimingTool>(this);
}

//================ Initialisation =================================================

StatusCode
Rec::MuonCombinedTimingTool::initialize()
{

    StatusCode sc = AlgTool::initialize();
    if (sc.isFailure()) return sc;

    if (!m_trtTimingTool.empty()) {
        ATH_CHECK(m_trtTimingTool.retrieve());
        ATH_MSG_DEBUG("Retrieved " << m_trtTimingTool.name());
    }
    if (!m_caloTimingTool.empty()) {
        ATH_CHECK(m_caloTimingTool.retrieve());
        ATH_MSG_DEBUG("Retrieved " << m_caloTimingTool.name());
    }
    if (!m_mdtSegmentTimingTool.empty()) {
        ATH_CHECK(m_mdtSegmentTimingTool.retrieve());
        ATH_MSG_DEBUG("Retrieved " << m_mdtSegmentTimingTool.name());
    }
    if (!m_rpcTimingTool.empty()) {
        ATH_CHECK(m_rpcTimingTool.retrieve());
        ATH_MSG_DEBUG("Retrieved " << m_rpcTimingTool.name());
    }

    ATH_MSG_DEBUG("initialize() successful in " << name());
    return StatusCode::SUCCESS;
}

//============================================================================================
std::vector<Trk::SpaceTimePoint*>
Rec::MuonCombinedTimingTool::timeMeasurements(const xAOD::Muon&) const
{
    std::vector<Trk::SpaceTimePoint*> spaceTimePoints;  // empty vector
    ATH_MSG_WARNING(
        "This tool is not migrated to xAOD yet, please volunteer to update it if you need the information filled");

    return spaceTimePoints;
}


// Helper function to figure out which is the appropriate segment container to retrieve
// Based on the muon's author
const Trk::SegmentCollection*
Rec::MuonCombinedTimingTool::getSegmentContainer(const xAOD::Muon&) const
{
    return nullptr;
}
