/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonAmbiTrackSelectionTool/MuonAmbiTrackSelectionTool.h"

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

Muon::MuonAmbiTrackSelectionTool::MuonAmbiTrackSelectionTool(const std::string &t, const std::string &n,
                                                             const IInterface *p)
    : AthAlgTool(t, n, p)
{
    declareInterface<IAmbiTrackSelectionTool>(this);

    declareProperty("MaxOverlapFraction", m_maxOverlapFraction = 0.1);
    declareProperty("KeepPartialOverlaps", m_keepPartial = true);
    declareProperty("KeepMoreThanOneShare", m_keepMoreThanOne = true);
}

//================ Initialisation =================================================

StatusCode
Muon::MuonAmbiTrackSelectionTool::initialize()
{
    ATH_CHECK(m_printer.retrieve());
    ATH_CHECK(m_idHelperSvc.retrieve());

    return StatusCode::SUCCESS;
}

//============================================================================================
std::tuple<Trk::Track *, bool>
Muon::MuonAmbiTrackSelectionTool::getCleanedOutTrack(const Trk::Track *track, const Trk::TrackScore score,
                                                     [[maybe_unused]] Trk::ClusterSplitProbabilityContainer &splitProbContainer,
                                                     Trk::PRDtoTrackMap &prd_to_track_map) const
{
    (void)score;  //@TODO unused ?

    unsigned int numshared = 0;
    unsigned int numhits   = 0;

    if (!track) {
        return std::make_tuple(static_cast<Trk::Track *>(nullptr), false);  // "reject" invalid input track
    }

    // get all TSOS
    const DataVector<const Trk::TrackStateOnSurface> *tsos = track->trackStateOnSurfaces();


    ATH_MSG_VERBOSE("New Track " << m_printer->print(*track));

    std::map<Muon::MuonStationIndex::StIndex, int> sharedPrecisionPerLayer;
    std::map<Muon::MuonStationIndex::StIndex, int> precisionPerLayer;

    // loop over TSOS
    DataVector<const Trk::TrackStateOnSurface>::const_iterator iTsos    = tsos->begin();
    DataVector<const Trk::TrackStateOnSurface>::const_iterator iTsosEnd = tsos->end();
    for (; iTsos != iTsosEnd; ++iTsos) {

        // get measurment from TSOS
        const Trk::MeasurementBase *meas = (*iTsos)->measurementOnTrack();
        if (!meas) continue;

        // only look at measurements
        if (!(*iTsos)->type(Trk::TrackStateOnSurface::Measurement)) continue;


        const Trk::RIO_OnTrack *rot = dynamic_cast<const Trk::RIO_OnTrack *>(meas);
        if (!rot) {
            // we are only interested in precision hits since only they have the resolution to distinguish between
            // close-by tracks so we only take competing ROTs if they are CSC eta hits
            const Trk::CompetingRIOsOnTrack *competingROT = dynamic_cast<const Trk::CompetingRIOsOnTrack *>(meas);
            if (competingROT) {
                const unsigned int numROTs = competingROT->numberOfContainedROTs();
                for (unsigned int i = 0; i < numROTs; ++i) {
                    const Trk::RIO_OnTrack *rot = &competingROT->rioOnTrack(i);
                    if (!rot || !rot->prepRawData() || !m_idHelperSvc->isMuon(rot->identify())) continue;
                    // only use precision hits for muon track overlap
                    if (!m_idHelperSvc->isMdt(rot->identify())
                        && !(m_idHelperSvc->isCsc(rot->identify()) && !m_idHelperSvc->measuresPhi(rot->identify()))
                        && !m_idHelperSvc->isMM(rot->identify()) && !m_idHelperSvc->issTgc(rot->identify()))
                        continue;
                    Muon::MuonStationIndex::StIndex stIndex = m_idHelperSvc->stationIndex(rot->identify());
                    ++precisionPerLayer[stIndex];
                    if (prd_to_track_map.isUsed(*(rot->prepRawData()))) {
                        ATH_MSG_VERBOSE("Track overlap found! " << m_idHelperSvc->toString(rot->identify()));
                        ++numshared;
                        ++sharedPrecisionPerLayer[stIndex];
                    }
                }
            }
        } else {

            if (!m_idHelperSvc->isMuon(rot->identify())) continue;
            if (!m_idHelperSvc->isMdt(rot->identify())
                && !(m_idHelperSvc->isCsc(rot->identify()) && !m_idHelperSvc->measuresPhi(rot->identify()))
                && !m_idHelperSvc->isMM(rot->identify()) && !m_idHelperSvc->issTgc(rot->identify()))
                continue;  // only precision hits used for overlap

            ++numhits;


            Muon::MuonStationIndex::StIndex stIndex = m_idHelperSvc->stationIndex(rot->identify());
            ++precisionPerLayer[stIndex];
            // allow no overlap
            if (prd_to_track_map.isUsed(*(rot->prepRawData()))) {
                ATH_MSG_VERBOSE("Track overlap found! " << m_idHelperSvc->toString(rot->identify()));
                ++numshared;
                ++sharedPrecisionPerLayer[stIndex];
            }
        }
    }
    if (numhits == 0) {
        ATH_MSG_WARNING("Got track without Muon hits " << m_printer->print(*track));
        return std::make_tuple(static_cast<Trk::Track *>(nullptr), false);  // reject input track
    }
    double overlapFraction = (double)numshared / (double)numhits;

    ATH_MSG_DEBUG("Track " << track << " has " << numhits << " hit, shared " << numshared << " overlap fraction "
                           << overlapFraction << " layers " << precisionPerLayer.size() << " shared "
                           << sharedPrecisionPerLayer.size());

    if (overlapFraction > m_maxOverlapFraction) {
        if (m_keepPartial) {
            if (sharedPrecisionPerLayer.empty()) {
                ATH_MSG_DEBUG("Track is not sharing precision hits, keeping ");
                return std::make_tuple(static_cast<Trk::Track *>(nullptr), true);  // keep input track
            }
            if (overlapFraction < 0.25 && precisionPerLayer.size() > 2
                && precisionPerLayer.size() - sharedPrecisionPerLayer.size() == 1)
            {
                ATH_MSG_DEBUG("Three station track differing by one precision layer, keeping ");
                return std::make_tuple(static_cast<Trk::Track *>(nullptr), true);  // keep input track
            }
            if (overlapFraction < 0.35 && precisionPerLayer.size() > 2
                && precisionPerLayer.size() - sharedPrecisionPerLayer.size() > 1 && m_keepMoreThanOne)
            {
                ATH_MSG_DEBUG("Three station track differing by more than one precision layer, keeping ");
                return std::make_tuple(static_cast<Trk::Track *>(nullptr), true);  // keep input track
            }
        }
        return std::make_tuple(static_cast<Trk::Track *>(nullptr), false);  // reject input track
    }

    return std::make_tuple(static_cast<Trk::Track *>(nullptr), true);  // keep input track
}
