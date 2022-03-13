/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonStationIntersectCondAlg.h"

MuonStationIntersectCondAlg::MuonStationIntersectCondAlg(const std::string& name, ISvcLocator* pSvcLocator) :
    AthAlgorithm(name, pSvcLocator) {}
StatusCode MuonStationIntersectCondAlg::initialize() {
    ATH_CHECK(m_idHelperSvc.retrieve());
    ATH_CHECK(m_writeKey.initialize());
    ATH_CHECK(m_condKey.initialize(!m_condKey.empty()));
    ATH_CHECK(m_DetectorManagerKey.initialize());
    return StatusCode::SUCCESS;
}
StatusCode MuonStationIntersectCondAlg::execute() {
    const EventContext& ctx = Gaudi::Hive::currentContext();
    SG::ReadCondHandle<MuonGM::MuonDetectorManager> det_mgr{m_DetectorManagerKey, ctx};
    if (!det_mgr.isValid()) {
        ATH_MSG_FATAL("Failed to retrieve Muon detector manager " << m_DetectorManagerKey.fullKey());
        return StatusCode::FAILURE;
    }

    EventIDRange rangeGeo;
    if (!det_mgr.range(rangeGeo)) {
        ATH_MSG_ERROR("Failed to retrieve validity range for " << m_DetectorManagerKey.key());
        return StatusCode::FAILURE;
    }
    EventIDRange rangeIntersection = rangeGeo;

    const MdtCondDbData* condData{nullptr};
    if (!m_condKey.empty()) {
        SG::ReadCondHandle<MdtCondDbData> condDbHandle{m_condKey, ctx};
        if (!condDbHandle.isValid()) {
            ATH_MSG_FATAL("Failed to retrieve conditions data " << m_condKey.fullKey());
            return StatusCode::FAILURE;
        }

        EventIDRange rangeCondDb;
        if (!condDbHandle.range(rangeCondDb)) {
            ATH_MSG_ERROR("Failed to retrieve validity range for " << m_DetectorManagerKey.key());
            return StatusCode::FAILURE;
        }
        // Create an intersection of input IOVs
        rangeIntersection = EventIDRange::intersect(rangeGeo, rangeCondDb);
    }

    SG::WriteCondHandle<Muon::MuonIntersectGeoData> writeHandle{m_writeKey, ctx};
    if (writeHandle.isValid()) {
        ATH_MSG_DEBUG("CondHandle " << writeHandle.fullKey() << " is already valid."
                                    << ". In theory this should not be called, but may happen"
                                    << " if multiple concurrent events are being processed out of order.");
        return StatusCode::SUCCESS;
    }

    std::unique_ptr<Muon::MuonIntersectGeoData> geo_data =
        std::make_unique<Muon::MuonIntersectGeoData>(msgStream(), det_mgr.cptr(), m_idHelperSvc.get(), condData);

    if (writeHandle.record(rangeIntersection, std::move(geo_data)).isFailure()) {
        ATH_MSG_FATAL("Could not record MuonMDT_CablingMap " << writeHandle.key() << " with EventRange " << rangeIntersection
                                                             << " into Conditions Store");
        return StatusCode::FAILURE;
    }

    return StatusCode::SUCCESS;
}
