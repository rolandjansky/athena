/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCandidateTrackBuilderTool.h"

#include "MuonLayerEvent/MuonCandidate.h"
#include "MuonSegment/MuonSegment.h"
#include "TrkCompetingRIOsOnTrack/CompetingRIOsOnTrack.h"
#include "TrkEventPrimitives/FitQuality.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkTrack/Track.h"
namespace Muon {

    MuonCandidateTrackBuilderTool::MuonCandidateTrackBuilderTool(const std::string& type, const std::string& name,
                                                                 const IInterface* parent) :
        AthAlgTool(type, name, parent) {
        declareInterface<IMuonCandidateTrackBuilderTool>(this);
    }

    StatusCode MuonCandidateTrackBuilderTool::initialize() {
        ATH_CHECK(m_printer.retrieve());
        ATH_CHECK(m_idHelperSvc.retrieve());
        ATH_CHECK(m_edmHelperSvc.retrieve());
        ATH_CHECK(m_trackFitter.retrieve());
        return StatusCode::SUCCESS;
    }

    std::unique_ptr<Trk::Track> MuonCandidateTrackBuilderTool::buildCombinedTrack(const EventContext& ctx, const Trk::Track& idTrack,
                                                                                  const MuonCandidate& candidate) const {
        ATH_MSG_DEBUG("Building track from candidate with " << candidate.layerIntersections.size() << " layers ");
        // copy and sort layerIntersections according to their distance to the IP
        std::vector<MuonLayerIntersection> layerIntersections = candidate.layerIntersections;
        std::stable_sort(layerIntersections.begin(), layerIntersections.end(),
                         [](const MuonLayerIntersection& lay1, const MuonLayerIntersection& lay2) {
                             auto getDistance = [](const MuonLayerIntersection& layerIntersection) {
                                 if (layerIntersection.intersection.trackParameters.get() == nullptr) return 1e9;
                                 return layerIntersection.intersection.trackParameters->position().mag();
                             };
                             return getDistance(lay1) < getDistance(lay2);
                         });

        // loop over sorted layers and extract measurements
        std::vector<const Trk::MeasurementBase*> measurements;
        int intersec = 0;
        bool isEndcap{false}, isBarrel{false}, isSmall{false}, isLarge{false};
        for (const MuonLayerIntersection& layerIntersection : layerIntersections) {
            intersec++;
            ATH_MSG_VERBOSE(" layerIntersection " << intersec << " perp "
                                                  << layerIntersection.intersection.trackParameters->position().perp() << " z "
                                                  << layerIntersection.intersection.trackParameters->position().z() << " distance to IP "
                                                  << layerIntersection.intersection.trackParameters->position().mag());
            ATH_MSG_VERBOSE(" segment surface center perp " << layerIntersection.segment->associatedSurface().center().perp() << " z "
                                                            << layerIntersection.segment->associatedSurface().center().z() << " nr of msts "
                                                            << layerIntersection.segment->containedMeasurements().size());
            ATH_MSG_VERBOSE(m_printer->print(*(layerIntersection.segment)));

            // Fix problem with segments where measurements are not ordered wrt IP

            // first check whether it is a Barrel or Endcap segment

            std::vector<const Trk::MeasurementBase*> containedMeasurements = layerIntersection.segment->containedMeasurements();
            for (const Trk::MeasurementBase* mit : containedMeasurements) {
                // get Identifier
                Identifier id;
                const Trk::RIO_OnTrack* rio = dynamic_cast<const Trk::RIO_OnTrack*>(mit);
                if (rio)
                    id = rio->identify();
                else {
                    const Trk::CompetingRIOsOnTrack* crio = dynamic_cast<const Trk::CompetingRIOsOnTrack*>(mit);
                    if (crio)
                        id = crio->rioOnTrack(0).identify();
                    else
                        continue;
                }
                // check if valid ID
                if (!id.is_valid()) continue;
                // check if muon
                if (!m_idHelperSvc->isMuon(id)) continue;

                if (m_idHelperSvc->isEndcap(id)) {
                    isEndcap = true;
                } else {
                    isBarrel = true;
                }
                if (m_idHelperSvc->isTrigger(id)) continue;

                if (m_idHelperSvc->isSmallChamber(id)) {
                    isSmall = true;
                } else {
                    isLarge = true;
                }
            }

            if (m_reOrderMeasurements) {
                // reorder measurements using SortMeas (defined in header file)
                std::stable_sort(containedMeasurements.begin(), containedMeasurements.end(),
                                 SortMeas(&*m_edmHelperSvc, &*m_idHelperSvc, isEndcap));
            }
            // insert in measurements list
            measurements.insert(measurements.end(), containedMeasurements.begin(), containedMeasurements.end());
        }

        // check for rare case in which 2 layers are large/small and one segment contains the other only with the opposite assignment of
        // radius signs
        if (candidate.layerIntersections.size() == 2 && isSmall && isLarge) {
            if (m_idHelperSvc->stationIndex(m_edmHelperSvc->chamberId(*candidate.layerIntersections.at(0).segment)) ==
                m_idHelperSvc->stationIndex(m_edmHelperSvc->chamberId(*candidate.layerIntersections.at(1).segment))) {
                const Muon::MuonSegment* seg1 = candidate.layerIntersections.at(0).segment.get();
                const Muon::MuonSegment* seg2 = candidate.layerIntersections.at(1).segment.get();
                if (seg1->containedMeasurements().size() > seg2->containedMeasurements().size()) {
                    seg2 = candidate.layerIntersections.at(0).segment.get();
                    seg1 = candidate.layerIntersections.at(1).segment.get();
                }
                bool found = false;
                for (const auto& meas1 : seg1->containedMeasurements()) {
                    found = false;
                    for (const auto& meas2 : seg2->containedMeasurements()) {
                        if (m_edmHelperSvc->getIdentifier(*meas1) == m_edmHelperSvc->getIdentifier(*meas2)) {
                            found = true;
                            break;
                        }
                    }
                    // if any hit isn't found we're off the hook
                    if (!found) break;
                }
                if (found) {
                    ATH_MSG_DEBUG("S/L overlap where one segment contains the other, don't use");
                    return std::unique_ptr<Trk::Track>();
                }
            }
        }

        // reorder in case of Small Large overlaps in Barrel or Endcap ONLY

        bool reorderAllMeasurements = false;
        if (isSmall && isLarge) reorderAllMeasurements = true;
        if (isEndcap && isBarrel) reorderAllMeasurements = false;

        if (m_reOrderMeasurements && reorderAllMeasurements) {
            // reorder measurements using SortMeas (defined in header file)
            ATH_MSG_VERBOSE(" reorder all measurements before " << std::endl << m_printer->print(measurements));
            std::stable_sort(measurements.begin(), measurements.end(), SortMeas(&*m_edmHelperSvc, &*m_idHelperSvc, isEndcap));
        }

        ATH_MSG_VERBOSE("final measurement list: " << std::endl << m_printer->print(measurements));

        ATH_MSG_DEBUG("Extracted hits from candidate: " << measurements.size());
        std::unique_ptr<Trk::Track> refittedTrack{m_trackFitter->indetExtension(ctx, idTrack, measurements)};
        if (refittedTrack) {
            ATH_MSG_DEBUG("got Track: " << m_printer->print(*refittedTrack) << std::endl << m_printer->printStations(*refittedTrack));
        }
        return refittedTrack;
    }
}  // namespace Muon
