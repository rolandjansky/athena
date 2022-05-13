/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonAmbiTrackSelectionTool.h"

#include <stdio.h>

#include <ext/functional>
#include <iostream>

#include "AthContainers/DataVector.h"
#include "TrkCompetingRIOsOnTrack/CompetingRIOsOnTrack.h"
#include "TrkEventUtils/PRDtoTrackMap.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkTrack/Track.h"

//================ Constructor =================================================

Muon::MuonAmbiTrackSelectionTool::MuonAmbiTrackSelectionTool(const std::string &t, const std::string &n, const IInterface *p) :
    AthAlgTool(t, n, p) {
    declareInterface<IAmbiTrackSelectionTool>(this);
}

//================ Initialisation =================================================

StatusCode Muon::MuonAmbiTrackSelectionTool::initialize() {
    ATH_CHECK(m_printer.retrieve());
    ATH_CHECK(m_idHelperSvc.retrieve());

    return StatusCode::SUCCESS;
}

//============================================================================================
std::tuple<Trk::Track *, bool> Muon::MuonAmbiTrackSelectionTool::getCleanedOutTrack(
    const Trk::Track *track, const Trk::TrackScore /*score*/, Trk::ClusterSplitProbabilityContainer & /*splitProbContainer*/,
    Trk::PRDtoTrackMap &prd_to_track_map, int trackId /*= -1*/, int subtrackId /*= -1*/) const {
    unsigned int numshared{0}, numhits{0}, numPhiHits{0};

    if (!track) {
        return std::make_tuple(nullptr, false);  // "reject" invalid input track
    }

    ATH_MSG_VERBOSE("New Track " << m_printer->print(*track));
    ATH_MSG_VERBOSE("trackId " << trackId << ", subtrackId " << subtrackId);

    std::map<Muon::MuonStationIndex::StIndex, int> sharedPrecisionPerLayer;
    std::map<Muon::MuonStationIndex::StIndex, int> precisionPerLayer;
    auto is_shared = [&sharedPrecisionPerLayer, &precisionPerLayer, this, &numshared, &numhits, &prd_to_track_map,
                      &numPhiHits](const Trk::RIO_OnTrack *rot) {
        if (!rot || !rot->prepRawData()) return;
        const Identifier rot_id = rot->prepRawData()->identify();
        numPhiHits += m_idHelperSvc->isMuon(rot_id) && m_idHelperSvc->measuresPhi(rot_id);
        if (!(m_idHelperSvc->isMdt(rot_id) || m_idHelperSvc->isMM(rot_id) || m_idHelperSvc->issTgc(rot_id) ||
              (m_idHelperSvc->isCsc(rot_id) && !m_idHelperSvc->measuresPhi(rot_id)))) {
            ATH_MSG_VERBOSE("Measurement " << m_printer->print(*rot) << " is not a precision one");
            return;
        }
        const Muon::MuonStationIndex::StIndex stIndex = m_idHelperSvc->stationIndex(rot_id);
        ++precisionPerLayer[stIndex];
        ++numhits;
        if (!prd_to_track_map.isUsed(*(rot->prepRawData()))) { return; }

        ATH_MSG_VERBOSE("Track overlap found! " << m_idHelperSvc->toString(rot_id));
        ++numshared;
        ++sharedPrecisionPerLayer[stIndex];
    };
    for (const Trk::TrackStateOnSurface *iTsos : *track->trackStateOnSurfaces()) {
        // get measurment from TSOS
        const Trk::MeasurementBase *meas = iTsos->measurementOnTrack();
        if (!meas) continue;

        // only look at measurements
        if (!iTsos->type(Trk::TrackStateOnSurface::Measurement)) continue;
        const Trk::RIO_OnTrack *rot = dynamic_cast<const Trk::RIO_OnTrack *>(meas);
        if (!rot) {
            // we are only interested in precision hits since only they have the resolution to distinguish between
            // close-by tracks so we only take competing ROTs if they are CSC eta hits
            const Trk::CompetingRIOsOnTrack *competingROT = dynamic_cast<const Trk::CompetingRIOsOnTrack *>(meas);
            if (competingROT) {
                const unsigned int numROTs = competingROT->numberOfContainedROTs();
                for (unsigned int i = 0; i < numROTs; ++i) {
                    const Trk::RIO_OnTrack *comp_rot = &competingROT->rioOnTrack(i);
                    is_shared(comp_rot);
                }
            }
        } else {
            is_shared(rot);
        }
    }
    if (!numhits) {
        /// Do not trigger the warning if the segment consists purely of phi hits
        if (!numPhiHits)
            ATH_MSG_WARNING("Got track without Muon hits " << m_printer->print(*track) << std::endl
                                                           << m_printer->printMeasurements(*track));
        return std::make_tuple(nullptr, false);  // reject input track
    }
    const double overlapFraction = (double)numshared / (double)numhits;

    ATH_MSG_VERBOSE("Track " << m_printer->print(*track) << " has " << numhits << " hit, shared " << numshared << " overlap fraction "
                             << overlapFraction << " layers " << precisionPerLayer.size() << " shared " << sharedPrecisionPerLayer.size());

    if (overlapFraction > m_maxOverlapFraction) {
        if (m_keepPartial) {
            if (sharedPrecisionPerLayer.empty()) {
                ATH_MSG_DEBUG("Track is not sharing precision hits, keeping ");
                return std::make_tuple(nullptr, true);  // keep input track
            }
            if (overlapFraction < 0.25 && precisionPerLayer.size() > 2 && precisionPerLayer.size() - sharedPrecisionPerLayer.size() == 1) {
                ATH_MSG_DEBUG("Three station track differing by one precision layer, keeping ");
                return std::make_tuple(nullptr, true);  // keep input track
            }
            if (overlapFraction < 0.35 && precisionPerLayer.size() > 2 && precisionPerLayer.size() - sharedPrecisionPerLayer.size() > 1 &&
                m_keepMoreThanOne) {
                ATH_MSG_DEBUG("Three station track differing by more than one precision layer, keeping ");
                return std::make_tuple(nullptr, true);  // keep input track
            }
        }
        return std::make_tuple(nullptr, false);  // reject input track
    }

    return std::make_tuple(nullptr, true);  // keep input track
}
