/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////
// MuonSegmentTagTool
//  AlgTool performing MS segment tagging of ID tracks.
//  A SegmentTag is added to the InDetCandidate object.
//
//////////////////////////////////////////////////////////////////////////////

#include "MuonSegmentTagTool.h"

#include <cstdio>
#include <iomanip>
#include <vector>

#include "AthLinks/ElementLink.h"
#include "EventPrimitives/EventPrimitivesHelpers.h"
#include "MSSurfaces.h"
#include "MuonCombinedEvent/InDetCandidate.h"
#include "MuonCombinedEvent/MuonCandidate.h"
#include "MuonCombinedEvent/MuonSegmentInfo.h"
#include "MuonCombinedEvent/SegmentTag.h"
#include "MuonCombinedToolInterfaces/IMuonCombinedTagTool.h"
#include "MuonSegment/MuonSegment.h"
#include "TrkCaloExtension/CaloExtension.h"
#include "TrkEventPrimitives/PropDirection.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkTrack/Track.h"
#include "TrkTrackSummary/TrackSummary.h"
#include "xAODMuon/MuonSegment.h"
#include "xAODMuon/MuonSegmentContainer.h"
namespace MuonCombined {

    MuonSegmentTagTool::MuonSegmentTagTool(const std::string& type, const std::string& name, const IInterface* parent) :
        AthAlgTool(type, name, parent) {
        declareInterface<IMuonSegmentTagTool>(this);
    }

    StatusCode MuonSegmentTagTool::initialize() {
        ATH_CHECK(m_printer.retrieve());
        ATH_CHECK(m_MuTagMatchingTool.retrieve());
        ATH_CHECK(m_MuTagAmbiguitySolverTool.retrieve());
        ATH_CHECK(m_idHelperSvc.retrieve());
        ATH_CHECK(m_edmHelperSvc.retrieve());
        ATH_CHECK(m_segmentSelector.retrieve());
     
        ATH_MSG_INFO("Initializing MuonSegmentTagTool");

        ATH_MSG_DEBUG("Initialisation started                     ");
        ATH_MSG_DEBUG("================================           ");
        ATH_MSG_DEBUG("=Proprieties are                           ");
        ATH_MSG_DEBUG("================================           ");        
        return StatusCode::SUCCESS;
    }

    StatusCode MuonSegmentTagTool::finalize() {
        ATH_MSG_INFO("Total number of considered ID tracks         " << m_ntotTracks);
        ATH_MSG_INFO("Total number of ID tracks with angular match " << m_nangleMatch);
        ATH_MSG_INFO("Total number of preselected ID tracks        " << m_npmatch);
        ATH_MSG_INFO("Total number of ID tracks at MS entry        " << m_natMSEntrance);
        ATH_MSG_INFO("Total number of accepted ID tracks           " << m_naccepted);
        for (unsigned int i = 0; i < 12; ++i) {
            double ratio = m_extrapolated[i] == 0 ? 0 : m_goodExtrapolated[i] / (double)m_extrapolated[i];
            ATH_MSG_INFO("Layer " << i << " extrap " << m_extrapolated[i] << " good " << m_goodExtrapolated[i] << " ratio " << ratio);
        }
        return StatusCode::SUCCESS;
    }

    void MuonSegmentTagTool::tag(const EventContext& ctx, const InDetCandidateCollection& inDetCandidates, const xAOD::MuonSegmentContainer& xaodSegments,
                                 InDetCandidateToTagMap* tagMap) const {
        // loop over segments are extract MuonSegments + create links between segments and xAOD segments
        std::map<const Muon::MuonSegment*, ElementLink<xAOD::MuonSegmentContainer>> segmentToxAODSegmentMap;
        std::vector<const Muon::MuonSegment*> segments;
        segments.reserve(xaodSegments.size());
        unsigned int index = 0;
        for (auto it = xaodSegments.begin(); it != xaodSegments.end(); ++it, ++index) {
            if (!(*it)->muonSegment().isValid()) continue;
            const Muon::MuonSegment* mseg = dynamic_cast<const Muon::MuonSegment*>(*(*it)->muonSegment());
            ElementLink<xAOD::MuonSegmentContainer> link(xaodSegments, index);
            link.toPersistent();
            if (mseg) {
                segments.push_back(mseg);
                segmentToxAODSegmentMap[mseg] = link;
            }
        }
        tag(ctx, inDetCandidates, segments, &segmentToxAODSegmentMap, tagMap);
    }
    // todo: fix segmentToxAODSegmentMap
    void MuonSegmentTagTool::tag(const EventContext& ctx, const InDetCandidateCollection& inDetCandidates, const std::vector<const Muon::MuonSegment*>& segments,
                                 SegmentMap* segmentToxAODSegmentMap, InDetCandidateToTagMap* tagMap) const {
        MSSurfaces surfaces{};

        std::vector<const Muon::MuonSegment*> FilteredSegmentCollection;
        if (m_doSegmentsFilter) {
            for (const Muon::MuonSegment* itSeg : segments) {
                int quality = m_segmentSelector->quality(*itSeg);
                ATH_MSG_DEBUG(" Segment quality " << quality);
                if (quality < m_segmentQualityCut) continue;
                Muon::IMuonSegmentHitSummaryTool::HitCounts hitCounts = m_hitSummaryTool->getHitCounts(*itSeg);
                if (hitCounts.ncscHitsPhi + hitCounts.ncscHitsEta > 0) ATH_MSG_DEBUG(" CSC segment ");

                if (hitCounts.ncscHitsPhi + hitCounts.ncscHitsEta == 0) {
                    if (hitCounts.nmdtHoles >= m_nmdtHoles) continue;
                    if (hitCounts.nmdtHitsMl1 + hitCounts.nmdtHitsMl2 < m_nmdtHits) continue;  // precision hits
                    if (hitCounts.nmdtHitsMl1 > 0 && hitCounts.nmdtHitsMl1 < m_nmdtHitsML) continue;
                    if (hitCounts.nmdtHitsMl2 > 0 && hitCounts.nmdtHitsMl2 < m_nmdtHitsML) continue;
                    // If Trigger hits are expected one should at least find one
                    ATH_MSG_DEBUG(" expected TrigHits " << hitCounts.nexpectedTrigHitLayers << " TrigHits found "
                                                        << hitCounts.nphiTrigHitLayers + hitCounts.netaTrigHitLayers);
                    if (m_triggerHitCut && hitCounts.nexpectedTrigHitLayers > 1 && hitCounts.nphiTrigHitLayers == 0 &&
                        hitCounts.netaTrigHitLayers == 0)
                        continue;
                }
                if (m_segmentQualityCut > 0) {
                    Identifier chId = m_edmHelperSvc->chamberId(*itSeg);
                    Muon::MuonStationIndex::StIndex stIndex = m_idHelperSvc->stationIndex(chId);
                    if (!m_triggerHitCut && stIndex == Muon::MuonStationIndex::EM) {
                        // don't apply the TGC requirement for the first station as it sometimes has not trigger hits due to TGC acceptance
                        int stationEta = m_idHelperSvc->stationEta(chId);
                        if (abs(stationEta) != 1) {
                            // remove EM segments without trigger hits
                            if (hitCounts.nphiTrigHitLayers == 0 && hitCounts.netaTrigHitLayers == 0) continue;
                        }
                    }
                    if (stIndex == Muon::MuonStationIndex::EI) {
                        // remove CSC segment with hits in only one projection
                        if (hitCounts.nmdtHits() == 0 && ((hitCounts.ncscHitsPhi > 0 && hitCounts.ncscHitsEta == 0) ||
                                                          (hitCounts.ncscHitsPhi == 0 && hitCounts.ncscHitsEta > 0)))
                            continue;
                    }
                }
                if (hitCounts.ncscHitsPhi + hitCounts.ncscHitsEta > 0) ATH_MSG_DEBUG(" CSC segment passed");
                FilteredSegmentCollection.emplace_back(itSeg);
            }
        } else {
            ATH_MSG_VERBOSE("No segment filtering required. ");
            FilteredSegmentCollection.insert(FilteredSegmentCollection.end(), segments.begin(), segments.end());
        }
        ATH_MSG_DEBUG("Filtered segments... in: " << segments.size() << ", out: " << FilteredSegmentCollection.size());

        if ((FilteredSegmentCollection.empty()) || (inDetCandidates.empty())) return;

        bool matchedSegment(false);
        unsigned int trackCount(0);

        ATH_MSG_DEBUG("performing tag of " << inDetCandidates.size() << " tracks with " << FilteredSegmentCollection.size()
                                           << " segments. ");
        // Checking which surfaces have segments, to avoid useless extrapolations
        std::array<bool,12> hasSeg{false};
        if (m_doBidirectional) {
            for (const Muon::MuonSegment* itSeg : FilteredSegmentCollection) {
                Identifier chId = m_edmHelperSvc->chamberId(*itSeg);
                Muon::MuonStationIndex::StIndex stIndex = m_idHelperSvc->stationIndex(chId);
                if (stIndex == Muon::MuonStationIndex::BI || stIndex == Muon::MuonStationIndex::BE)
                    hasSeg[0] = true;
                else if (stIndex == Muon::MuonStationIndex::BM)
                    hasSeg[1] = true;
                else if (stIndex == Muon::MuonStationIndex::BO)
                    hasSeg[2] = true;
                else {
                    int stationEta = m_idHelperSvc->stationEta(chId);
                    if (stationEta > 0) {
                        if (stIndex == Muon::MuonStationIndex::EI) hasSeg[4] = true;
                        if (stIndex == Muon::MuonStationIndex::EM) hasSeg[5] = true;
                        if (stIndex == Muon::MuonStationIndex::EO) hasSeg[6] = true;
                        if (stIndex == Muon::MuonStationIndex::EE) hasSeg[7] = true;
                    } else {
                        if (stIndex == Muon::MuonStationIndex::EI) hasSeg[8] = true;
                        if (stIndex == Muon::MuonStationIndex::EM) hasSeg[9] = true;
                        if (stIndex == Muon::MuonStationIndex::EO) hasSeg[10] = true;
                        if (stIndex == Muon::MuonStationIndex::EE) hasSeg[11] = true;
                    }
                }
            }
            ATH_MSG_DEBUG("Surfaces with segments: " << hasSeg);
        }

        std::vector<MuonCombined::MuonSegmentInfo> segmentsInfoSelected;
        for (const MuonCombined::InDetCandidate* idTP : inDetCandidates) {
            // ensure that the id trackparticle has a track           
            // Ignore if this is a siAssociated disk (and property set).
            if (m_ignoreSiAssocated && idTP->isSiliconAssociated()) continue;

            ++trackCount;

            matchedSegment = false;
            const Trk::Track* track = idTP->indetTrackParticle().track();

            if (!track) continue;
            if (!track->perigeeParameters()) continue;

            //
            // Remove low p and low Field region defined by p < 6 and eta between 1.4-1.7
            //
            const Trk::Perigee* aMeasPer = track->perigeeParameters();
            if (m_removeLowPLowFieldRegion) {
                double eta = -std::log(std::tan(aMeasPer->parameters()[Trk::theta] / 2.));
                double p = std::abs(1.0 / (aMeasPer->parameters()[Trk::qOverP]));
                if (std::abs(eta) > 1.4 && std::abs(eta) < 1.7 && p < 6000.) continue;
            }
            ATH_MSG_DEBUG("Treating track " << trackCount);
            ATH_MSG_VERBOSE("========================== dumping the full track =========================");
            ATH_MSG_VERBOSE(*track);

            bool trkEtaInfo(false);
            uint8_t numSCTHits = 0;
            uint8_t numPixelHits = 0;
            if (!idTP->indetTrackParticle().summaryValue(numSCTHits, xAOD::numberOfSCTHits))
                ATH_MSG_DEBUG("TrackParticle missing numberOfSCTHits");
            if (!idTP->indetTrackParticle().summaryValue(numPixelHits, xAOD::numberOfPixelHits))
                ATH_MSG_DEBUG("TrackParticle missing numberOfPixelHits");

            unsigned int nrIDetaHits = numSCTHits + numPixelHits;
            if (nrIDetaHits >= 5) trkEtaInfo = true;
            if (!trkEtaInfo) ATH_MSG_DEBUG("Track has no ID eta information! (" << nrIDetaHits << " etaHits)");

            ++m_ntotTracks;
            bool hasMatch = m_doBidirectional;
            if (!m_doBidirectional) {
                hasSeg.fill(false);
                bool hasAngleMatch = false;
                const Amg::Vector3D id_mom = track->perigeeParameters()->momentum();
                const double qID = track->perigeeParameters()->charge();
                const double pID = id_mom.mag();
                const double EtaID = id_mom.eta();
                for (const Muon::MuonSegment* itSeg : FilteredSegmentCollection) {
                    const Amg::Vector3D& pos = itSeg->globalPosition();
                    const double thetaSeg = pos.theta();
                    const double dPhi = qID * pos.deltaPhi(id_mom) - qID / pID;
                    const double dTheta = thetaSeg - id_mom.theta();
                    if (std::abs(dPhi) < 0.6 && qID * dTheta < 0.2 && qID * dTheta > -0.6) {
                        hasAngleMatch = true;
                        if (pID > 20000 * (qID * EtaID - 2)) {
                            Identifier chId = m_edmHelperSvc->chamberId(*itSeg);
                            if (!m_idHelperSvc->isCsc(chId)) hasMatch = true;
                            Muon::MuonStationIndex::StIndex stIndex = m_idHelperSvc->stationIndex(chId);
                            if (stIndex == Muon::MuonStationIndex::BI || stIndex == Muon::MuonStationIndex::BE)
                                hasSeg[0] = true;
                            else if (stIndex == Muon::MuonStationIndex::BM)
                                hasSeg[1] = true;
                            else if (stIndex == Muon::MuonStationIndex::BO)
                                hasSeg[2] = true;
                            else {
                                int stationEta = m_idHelperSvc->stationEta(chId);
                                if (stationEta > 0) {
                                    if (stIndex == Muon::MuonStationIndex::EI) hasSeg[4] = true;
                                    if (stIndex == Muon::MuonStationIndex::EM) hasSeg[5] = true;
                                    if (stIndex == Muon::MuonStationIndex::EO) hasSeg[6] = true;
                                    if (stIndex == Muon::MuonStationIndex::EE) hasSeg[7] = true;
                                } else {
                                    if (stIndex == Muon::MuonStationIndex::EI) hasSeg[8] = true;
                                    if (stIndex == Muon::MuonStationIndex::EM) hasSeg[9] = true;
                                    if (stIndex == Muon::MuonStationIndex::EO) hasSeg[10] = true;
                                    if (stIndex == Muon::MuonStationIndex::EE) hasSeg[11] = true;
                                }
                            }
                        }
                    }
                }
                if (hasAngleMatch) ++m_nangleMatch;
                if (m_useSegmentPreselection && !hasMatch) continue;
                if (hasMatch) ++m_npmatch;
            }

            std::vector<MuonCombined::MuonSegmentInfo> segmentsInfo;
            const int multiply = 1 + m_doBidirectional;
            std::vector<std::string> didExtrapolate(12 * multiply, "o");
            std::vector<std::string> segStation(FilteredSegmentCollection.size(), "XXX");
            std::vector<std::vector<std::string>> trkToSegment(12 * multiply, segStation);

            unsigned int extrapolation_counter(0);
            const int numberOfExtrapolations = 1 + m_doBidirectional;
            std::array<std::unique_ptr<const Trk::TrackParameters>, 2> trackAtMSEntrance;

            for (int i_extrapolations = 0; i_extrapolations < numberOfExtrapolations; ++i_extrapolations) {
                ATH_MSG_DEBUG("Extrapolation " << i_extrapolations);

                Trk::PropDirection direction(Trk::alongMomentum);
                if (i_extrapolations == 1) direction = Trk::oppositeMomentum;

                // in case of along momentum extrapolation, use pre-existing extrapolation if available
                const Trk::CaloExtension* extension = idTP->getCaloExtension();
                if (direction == Trk::alongMomentum) {
                    if (extension && extension->muonEntryLayerIntersection()) {
                        const Trk::TrackParameters& pars = *extension->muonEntryLayerIntersection();
                        trackAtMSEntrance[i_extrapolations] = pars.uniqueClone();
                        ATH_MSG_DEBUG("Got MS entry pos: r " << pars.position().perp() << " z " << pars.position().z() << " momentum "
                                                             << pars.momentum().perp() << " cov " << pars.covariance());
                    }
                } else {
                    trackAtMSEntrance[i_extrapolations] = m_MuTagMatchingTool->ExtrapolateTrktoMSEntrance(ctx, track, direction);
                }

                if (!trackAtMSEntrance[i_extrapolations]) {
                    ATH_MSG_DEBUG("there is no track at MS entrance... not tagging for direction " << i_extrapolations);
                    continue;
                }
                ++m_natMSEntrance;

                std::unique_ptr<const Trk::TrackParameters> atSurface;
                std::unique_ptr<const Trk::TrackParameters> nextSurface;
                std::array<bool,12> hasSurf{false};

                for (unsigned int surface_counter = 0; surface_counter < 12; ++surface_counter, ++extrapolation_counter) {
                    ATH_MSG_DEBUG("Surface " << surface_counter);
                    if (surface_counter == 3) continue;
                    if (!hasSeg[surface_counter]) continue;
                    if (m_doTable) didExtrapolate[extrapolation_counter] = "X";

                    std::vector<std::string> segVsSurf(FilteredSegmentCollection.size(), "xxx");
                    const Trk::Surface* surface = surfaces.getSurface(surface_counter);

                    switch (surface_counter) {
                        case 0:  // BI + BEE
                            atSurface = m_MuTagMatchingTool->ExtrapolateTrktoMSSurface(
                                ctx, surface, trackAtMSEntrance[i_extrapolations].get(), direction);
                            if (atSurface) hasSurf[surface_counter] = true;
                            break;

                        case 1:  // BM
                            if (hasSurf[0] && atSurface) {
                                nextSurface = m_MuTagMatchingTool->ExtrapolateTrktoMSSurface(ctx, surface, atSurface.get(), direction);
                                if (nextSurface) hasSurf[surface_counter] = true;
                                atSurface.swap(nextSurface);
                            } else {
                                atSurface = m_MuTagMatchingTool->ExtrapolateTrktoMSSurface(
                                    ctx, surface, trackAtMSEntrance[i_extrapolations].get(), direction);
                                if (atSurface) hasSurf[surface_counter] = true;
                                if ((atSurface) && (hasSeg[0])) ATH_MSG_DEBUG("Extrap to BI failed but to BM successful: why?");
                            }
                            break;

                        case 2:  // BO
                            if ((hasSurf[0] || hasSurf[1]) && atSurface) {
                                nextSurface = m_MuTagMatchingTool->ExtrapolateTrktoMSSurface(ctx, surface, atSurface.get(), direction);
                                if (nextSurface) hasSurf[surface_counter] = true;
                                atSurface.swap(nextSurface);
                            } else {
                                atSurface = m_MuTagMatchingTool->ExtrapolateTrktoMSSurface(
                                    ctx, surface, trackAtMSEntrance[i_extrapolations].get(), direction);
                                if (atSurface) hasSurf[surface_counter] = true;
                                if ((atSurface) && (hasSeg[0] || hasSeg[1]))
                                    ATH_MSG_DEBUG("Extrap to BI/BM failed but to BO successful: why?");
                            }
                            break;

                            //	  case 3: // BE -> merged with BI

                        case 4:  // EIA
                            atSurface = m_MuTagMatchingTool->ExtrapolateTrktoMSSurface(
                                ctx, surface, trackAtMSEntrance[i_extrapolations].get(), direction);
                            if (atSurface) hasSurf[surface_counter] = true;
                            break;

                        case 5:  // EMA
                            if ((hasSurf[4]) && (atSurface)) {
                                nextSurface = m_MuTagMatchingTool->ExtrapolateTrktoMSSurface(ctx, surface, atSurface.get(), direction);
                                if (nextSurface) hasSurf[surface_counter] = true;
                                atSurface.swap(nextSurface);
                            } else {
                                atSurface = m_MuTagMatchingTool->ExtrapolateTrktoMSSurface(
                                    ctx, surface, trackAtMSEntrance[i_extrapolations].get(), direction);
                                if (atSurface) hasSurf[surface_counter] = true;
                                if ((atSurface) && (hasSeg[4])) ATH_MSG_DEBUG("Extrap to EIA failed but to EMA successful: why?");
                            }
                            break;

                        case 6:  // EOA
                            if ((hasSurf[4] || hasSurf[5]) && (atSurface)) {
                                nextSurface = m_MuTagMatchingTool->ExtrapolateTrktoMSSurface(ctx, surface, atSurface.get(), direction);
                                if (nextSurface) hasSurf[surface_counter] = true;
                                atSurface.swap(nextSurface);
                            } else {
                                atSurface = m_MuTagMatchingTool->ExtrapolateTrktoMSSurface(
                                    ctx, surface, trackAtMSEntrance[i_extrapolations].get(), direction);
                                if (atSurface) hasSurf[surface_counter] = true;
                                if ((atSurface) && (hasSeg[4] || hasSeg[5]))
                                    ATH_MSG_DEBUG("Extrap to EMA failed but to EOA successful: why?");
                            }
                            break;

                        case 7:  // EEA
                            atSurface = m_MuTagMatchingTool->ExtrapolateTrktoMSSurface(
                                ctx, surface, trackAtMSEntrance[i_extrapolations].get(), direction);
                            break;

                        case 8:  // EIC
                            atSurface = m_MuTagMatchingTool->ExtrapolateTrktoMSSurface(
                                ctx, surface, trackAtMSEntrance[i_extrapolations].get(), direction);
                            if (atSurface) hasSurf[surface_counter] = true;
                            break;

                        case 9:  // EMC
                            if ((hasSurf[8]) && (atSurface)) {
                                nextSurface = m_MuTagMatchingTool->ExtrapolateTrktoMSSurface(ctx, surface, atSurface.get(), direction);
                                if (nextSurface) hasSurf[surface_counter] = true;
                                atSurface.swap(nextSurface);
                            } else {
                                atSurface = m_MuTagMatchingTool->ExtrapolateTrktoMSSurface(
                                    ctx, surface, trackAtMSEntrance[i_extrapolations].get(), direction);
                                if (atSurface) hasSurf[surface_counter] = true;
                                if ((atSurface) && (hasSeg[8])) ATH_MSG_DEBUG("Extrap to EIC failed but to EMC successful: why?");
                            }
                            break;

                        case 10:  // EOC
                            if ((hasSurf[8] || hasSurf[9]) && (atSurface)) {
                                nextSurface = m_MuTagMatchingTool->ExtrapolateTrktoMSSurface(ctx, surface, atSurface.get(), direction);
                                if (nextSurface) hasSurf[surface_counter] = true;
                                atSurface.swap(nextSurface);
                            } else {
                                atSurface = m_MuTagMatchingTool->ExtrapolateTrktoMSSurface(
                                    ctx, surface, trackAtMSEntrance[i_extrapolations].get(), direction);
                                if (atSurface) hasSurf[surface_counter] = true;
                                if ((atSurface) && (hasSeg[8] || hasSeg[9]))
                                    ATH_MSG_DEBUG("Extrap to EMC failed but to EOC successful: why?");
                            }
                            break;

                        case 11:  // EEC

                            atSurface = m_MuTagMatchingTool->ExtrapolateTrktoMSSurface(
                                ctx, surface, trackAtMSEntrance[i_extrapolations].get(), direction);
                            break;
                    }

                    ++m_extrapolated[surface_counter];

                    if (!atSurface) {
                        if (m_doTable) trkToSegment[extrapolation_counter] = segVsSurf;
                        continue;
                    }
                    if (m_doTable) didExtrapolate[extrapolation_counter] = "V";
                    ++m_goodExtrapolated[surface_counter];

                    //	m_MuTagMatchingTool->testExtrapolation( surface, track );

                    unsigned int segmentCount(0);

                    for (std::vector<const Muon::MuonSegment*>::iterator itSeg = FilteredSegmentCollection.begin();
                         itSeg != FilteredSegmentCollection.end(); ++itSeg, ++segmentCount) {
                        
                        const Muon::MuonSegment* seg_ptr = (*itSeg);
                        if (std::find_if(segmentsInfo.begin(),segmentsInfo.end(),[seg_ptr](const MuonCombined::MuonSegmentInfo& info){
                            return info.segment == seg_ptr;
                        }) != segmentsInfo.end()) continue;
                        
                        if (m_doTable) { segStation[segmentCount] = "   "; }

                        ////// Per Abstract Layer, loop over all segments, and find out wether the segment is close to the
                        ////// extrapolated track to the abstract layer.
                        bool isMatched(false);
                        if (!m_doTable) {
                            isMatched = m_MuTagMatchingTool->match(atSurface.get(), seg_ptr, surfaces.stationType(surface_counter));
                        } else {
                            if (!m_MuTagMatchingTool->surfaceMatch(atSurface.get(), seg_ptr, surfaces.stationType(surface_counter))) {
                                segVsSurf[segmentCount] = "surface";
                                continue;
                            }

                            ATH_MSG_VERBOSE("treating track " << trackCount << " (extrapolation = " << direction << ") and Segment "
                                                              << segStation[segmentCount] << " (segment " << segmentCount << ")");

                            if (!m_MuTagMatchingTool->phiMatch(atSurface.get(), seg_ptr, surfaces.stationType(surface_counter))) {
                                segVsSurf[segmentCount] = "RghPhi";
                                continue;
                            }
                            if (trkEtaInfo) {
                                if ((surfaces.stationType(surface_counter)).find('B') != std::string::npos) {
                                    if (!m_MuTagMatchingTool->thetaMatch(atSurface.get(), seg_ptr)) {
                                        segVsSurf[segmentCount] = "RghTheta";
                                        continue;
                                    }
                                } else {
                                    if (!m_MuTagMatchingTool->rMatch(atSurface.get(), seg_ptr)) {
                                        segVsSurf[segmentCount] = "RghR";
                                        continue;
                                    }
                                }
                                isMatched = true;
                            } else {  // trk has no eta info
                                ATH_MSG_DEBUG("bypassed roughEta/roughR cut since ID has no eta info!");
                                isMatched = true;
                            }
                        }  //! m_dotable

                        if (!isMatched) {
                            if (m_doTable) {
                                ATH_MSG_DEBUG("isMatched is " << isMatched << " at this point, incoherent!");
                                if (!trkEtaInfo)
                                    segVsSurf[segmentCount] = "posPhi";
                                else
                                    segVsSurf[segmentCount] = "pos";
                            }
                            continue;
                        }
                        ////// after a rough match (on station name, second coordinate and finally precision coordinate)
                        ////// extrapolate the track to the plane-surface associated to the matching segment.
                        std::unique_ptr<const Trk::AtaPlane> atSegSurface{m_MuTagMatchingTool->ExtrapolateTrktoSegmentSurface(
                            ctx, seg_ptr, trackAtMSEntrance[i_extrapolations].get(), direction)};
                        if (!atSegSurface || !atSegSurface->covariance() ||
                            !Amg::saneCovarianceDiagonal(*atSegSurface->covariance()))
                          continue;
                        const AmgSymMatrix(5) invCov =
                          atSegSurface->covariance()->inverse();
                        if (!Amg::saneCovarianceDiagonal(invCov))
                          continue;

                        MuonCombined::MuonSegmentInfo info = m_MuTagMatchingTool->muTagSegmentInfo(track, seg_ptr, atSegSurface.get());
                        if (segmentToxAODSegmentMap) info.link = (*segmentToxAODSegmentMap)[seg_ptr];

                        isMatched = m_MuTagMatchingTool->matchSegmentPosition(&info, trkEtaInfo);

                        if (!isMatched) {
                            if (m_doTable) {
                                if (!trkEtaInfo)
                                    segVsSurf[segmentCount] = "posPhi";
                                else
                                    segVsSurf[segmentCount] = "pos";
                            }
                            continue;
                        }
                        isMatched = m_MuTagMatchingTool->matchSegmentDirection(&info, trkEtaInfo);

                        if (!isMatched) {
                            if (m_doTable) {
                                if (!trkEtaInfo)
                                    segVsSurf[segmentCount] = "dirPhi";
                                else
                                    segVsSurf[segmentCount] = "dir";
                            }
                            continue;
                        }

                        isMatched = m_MuTagMatchingTool->matchDistance(&info);

                        if (!isMatched) {
                            if (m_doTable) { segVsSurf[segmentCount] = "dist"; }
                            continue;
                        }

                        isMatched = m_MuTagMatchingTool->matchCombinedPull(&info);
                        if (!isMatched) {
                            if (m_doTable) { segVsSurf[segmentCount] = "cpull"; }
                            continue;
                        }

                        if (m_doPtDependentPullCut) {
                            isMatched = m_MuTagMatchingTool->matchPtDependentPull(&info, track);
                            if (!isMatched) {
                                if (m_doTable) { segVsSurf[segmentCount] = "ptpull"; }
                                continue;
                            }
                        }
                        if (m_doTable) segVsSurf[segmentCount] = "TAG";
                        matchedSegment = true;
                        ATH_MSG_DEBUG("Tagged the track with Segment " << segmentCount);

                        segmentsInfo.push_back(info);

                    }  // end loop over segments

                    if (m_doTable) trkToSegment[extrapolation_counter] = segVsSurf;
                }  // end loop over MS surfaces

            }  // end loop over nr Directions of extrapolation.

            if (m_doTable) {
                const Trk::Perigee* Perigee = track->perigeeParameters();
                ATH_MSG_DEBUG("Dump Table for TP " << std::setw(5) << trackCount);
                ATH_MSG_DEBUG("storing track to ntuple : " << Perigee->parameters()[Trk::d0] << ", " << Perigee->parameters()[Trk::z0]
                                                           << Perigee->parameters()[Trk::phi] << ", " << Perigee->parameters()[Trk::theta]
                                                           << ", " << Perigee->parameters()[Trk::qOverP]);
                printTable(didExtrapolate, segStation, trkToSegment, surfaces);
            }

            if (!matchedSegment) {
                ATH_MSG_DEBUG("ID " << std::setw(3) << trackCount << " could not be matched with any segment on any abstract surface");
                continue;
            }

            // The loop over SM surfaces yielded matches for the extrapolated track.
            // Per match, a MuTagObject is created and added to the MuTagObjectContainer
            // A Track which has at least one match with the extrapolated track may be called a muon
            //----------------------------------------------------------------
            //----------------------------------------------------------------
            if (!segmentsInfo.empty()) {
                ++m_naccepted;

                if (!m_doBidirectional && !hasMatch) { ATH_MSG_WARNING("MuTagIMO Muon rejected by matching cut "); }

                ATH_MSG_DEBUG("number of accepted tracks " << m_naccepted << " segmentsInfo size " << segmentsInfo.size());

                std::vector<MuonCombined::MuonSegmentInfo> segmentsInfoSolved =
                    m_MuTagAmbiguitySolverTool->selectBestMuTaggedSegments(segmentsInfo);
                segmentsInfoSelected.reserve(segmentsInfoSolved.size());
                for (const auto& x : segmentsInfoSolved) segmentsInfoSelected.push_back(x);
                ATH_MSG_DEBUG("segmentsInfoSelected size " << segmentsInfoSelected.size());
            }

        }  // end loop over tracks

        ATH_MSG_DEBUG("segmentsInfoSelected size after track loop " << segmentsInfoSelected.size());
        std::vector<MuonCombined::MuonSegmentInfo> segmentsInfoFinal = m_MuTagAmbiguitySolverTool->solveAmbiguities(segmentsInfoSelected);
        ATH_MSG_DEBUG("segmentsInfoFinal size " << segmentsInfoFinal.size());

        if (msgLevel(MSG::DEBUG)) {
            for (unsigned int ns1 = 0; ns1 < segmentsInfoFinal.size(); ++ns1) {
                if (ns1 == 0)
                    ATH_MSG_DEBUG(m_printer->print(*segmentsInfoFinal[ns1].track));
                else if (segmentsInfoFinal[ns1].track != segmentsInfoFinal[ns1 - 1].track)
                    ATH_MSG_DEBUG(m_printer->print(*segmentsInfoFinal[ns1].track));
                ATH_MSG_DEBUG(m_printer->print(*segmentsInfoFinal[ns1].segment));
            }
        }
        
        for (const InDetCandidate* idTP : inDetCandidates) {
            const Trk::Track* track = idTP->indetTrackParticle().track();
            matchedSegment = false;
            std::vector<MuonCombined::MuonSegmentInfo> segmentsInfoTag;
            segmentsInfoTag.reserve(segmentsInfoFinal.size());
            bool match = false;
            for (unsigned int ns1 = 0; ns1 < segmentsInfoFinal.size(); ns1++) {
                if (segmentsInfoFinal[ns1].track == track && segmentsInfoFinal[ns1].nsegments > 0) {
                    segmentsInfoTag.push_back(segmentsInfoFinal[ns1]);
                    match = true;                      
                }                
            }
            if (match) {
                ATH_MSG_DEBUG("make Segment Tag object for " << m_printer->print(*track) << " nr segments " << segmentsInfoTag.size());
                SegmentTag* tag = new SegmentTag(segmentsInfoTag);
                tagMap->addEntry(idTP, tag);
            }
        }
    }

    void MuonSegmentTagTool::printTable(const std::vector<std::string>& didEx, const std::vector<std::string>& segStation,
                                        const std::vector<std::vector<std::string>>& segToSurf, MSSurfaces& surfaces) const {
        ATH_MSG_DEBUG(std::setw(6) << ""
                                   << "EX? (o: no extrap, X: extrap failed, V: extrap OK)");

        if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Surface" << std::setw(2) << "";
        for (unsigned int segment_counter = 0; segment_counter < segStation.size(); ++segment_counter) {
            if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << std::setw(10) << segStation[segment_counter];
        }
        if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << endmsg;

        unsigned int extrapolation_counter(0);
        std::string signstr("");
        if (m_doBidirectional) signstr = "+";

        for (unsigned int counter = 0; counter < 12; ++counter) {
            if (msgLvl(MSG::DEBUG))
                msg(MSG::DEBUG) << std::setw(5) << surfaces.stationType(counter) << signstr << std::setw(3) << didEx[extrapolation_counter];
            for (unsigned int segment_counter = 0; segment_counter < segStation.size(); ++segment_counter) {
                if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << std::setw(10) << (segToSurf[extrapolation_counter])[segment_counter];
            }  // end loop segments
            if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << endmsg;
            ++extrapolation_counter;
        }  // end loop surfaces

        if (m_doBidirectional) {
            signstr = "-";
            for (unsigned int counter = 0; counter < 12; ++counter) {
                if (msgLvl(MSG::DEBUG))
                    msg(MSG::DEBUG) << std::setw(5) << surfaces.stationType(counter) << signstr << std::setw(3)
                                    << didEx[extrapolation_counter];
                for (unsigned int segment_counter = 0; segment_counter < segStation.size(); ++segment_counter) {
                    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << std::setw(10) << (segToSurf[extrapolation_counter])[segment_counter];
                }  // end loop segments
                if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << endmsg;
                ++extrapolation_counter;
            }  // end loop surfaces
        }        
    }

}  // namespace MuonCombined
