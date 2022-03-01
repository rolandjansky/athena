/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
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
#include <optional>
#include "AthLinks/ElementLink.h"
#include "EventPrimitives/EventPrimitivesHelpers.h"
#include "MuonCombinedEvent/InDetCandidate.h"
#include "MuonCombinedEvent/MuonCandidate.h"
#include "MuonCombinedEvent/MuonSegmentInfo.h"
#include "MuonCombinedEvent/SegmentTag.h"
#include "MuonCombinedToolInterfaces/IMuonCombinedTagTool.h"
#include "MuonSegment/MuonSegment.h"
#include "TrkCaloExtension/CaloExtension.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkTrack/Track.h"
#include "TrkTrackSummary/TrackSummary.h"
#include "xAODMuon/MuonSegment.h"
#include "xAODMuon/MuonSegmentContainer.h"
namespace {
    using Intersection = Muon::MuonSystemExtension::Intersection;
    using LayerIndex = Muon::MuonStationIndex::LayerIndex;
    using StationIndex = Muon::MuonStationIndex::StIndex;

    /// Helper struct to cache extrapolations to the same
    /// SegmentSurface from the same tracking parameters
    struct SegmentExtrCache {
        SegmentExtrCache(const std::shared_ptr<const Trk::AtaPlane>& extp,
                        const Trk::Surface& surf):
             segment_surface{&surf},
             segment_pars{extp}{}
        const Trk::Surface* segment_surface{nullptr};
        std::shared_ptr<const Trk::AtaPlane> segment_pars{nullptr};
    };
}
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
        for (unsigned int i = 0; i < SurfDef::NumSurf; ++i) {
            const double recyc_extr = m_recycledIntersect[i] + m_extrapolated[i];
            auto calc_ratio= [&recyc_extr](unsigned int N)->double{return recyc_extr > 0. ? 100.*N /recyc_extr : 0; };
            ATH_MSG_INFO("Layer " << m_surfaces.stationType(i) 
                                  << " processed  " << int(recyc_extr) << " extrapolated " <<m_extrapolated[i]<< " ("<<calc_ratio(m_extrapolated[i])<<" %) "
                                  << " recycled " << m_recycledIntersect[i] <<" ("<<calc_ratio(m_recycledIntersect[i])<<"%) " 
                                  << " good " <<m_goodExtrapolated[i]<<" ("<<calc_ratio(m_goodExtrapolated[i])<<"%)");
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
                segmentToxAODSegmentMap[mseg] = std::move(link);
            }
        }
        tag(ctx, inDetCandidates, segments, &segmentToxAODSegmentMap, tagMap);
    }
    // todo: fix segmentToxAODSegmentMap
    void MuonSegmentTagTool::tag(const EventContext& ctx, const InDetCandidateCollection& inDetCandidates, const std::vector<const Muon::MuonSegment*>& segments,
                                 SegmentMap* segmentToxAODSegmentMap, InDetCandidateToTagMap* tagMap) const {

        if (inDetCandidates.empty()) return;
        
        std::vector<const Muon::MuonSegment*> FilteredSegmentCollection = getCandidateSegments(segments);
       
        ATH_MSG_DEBUG("Filtered segments... in: " << segments.size() << ", out: " << FilteredSegmentCollection.size());

        if (FilteredSegmentCollection.empty()) return;

        unsigned int trackCount{0};
        ATH_MSG_DEBUG("performing tag of " << inDetCandidates.size() << " tracks with " << FilteredSegmentCollection.size()
                                           << " segments. ");
        // Checking which surfaces have segments, to avoid useless extrapolations
        
        const SortedSegmentMap populStat = findPopulatedStations(FilteredSegmentCollection);
      
        std::vector<MuonCombined::MuonSegmentInfo> segmentsInfoSelected;
        segmentsInfoSelected.reserve(FilteredSegmentCollection.size());
        for (const MuonCombined::InDetCandidate* idTP : inDetCandidates) {
            // ensure that the id trackparticle has a track           
            // Ignore if this is a siAssociated disk (and property set).
            if (m_ignoreSiAssocated && idTP->isSiliconAssociated()) continue;

            ++trackCount;

            bool matchedSegment {false};
            const xAOD::TrackParticle& track = idTP->indetTrackParticle();
          
            //
            // Remove low p and low Field region defined by p < 6 and eta between 1.4-1.7
            //
            if (m_removeLowPLowFieldRegion) {
                const double eta = std::abs(track.eta());
                const double p = std::abs(1.0 / track.qOverP());
                if (eta > 1.4 && eta < 1.7 && p < 6000.) continue;
            }
            ATH_MSG_DEBUG("Treating track " << trackCount<<" "<<idTP->toString());           

            uint8_t numSCTHits{0},  numPixelHits{0};
            if (!track.summaryValue(numSCTHits, xAOD::numberOfSCTHits))
                ATH_MSG_DEBUG("TrackParticle missing numberOfSCTHits");
            if (!track.summaryValue(numPixelHits, xAOD::numberOfPixelHits))
                ATH_MSG_DEBUG("TrackParticle missing numberOfPixelHits");

            unsigned int nrIDetaHits = numSCTHits + numPixelHits;
            const bool trkEtaInfo = nrIDetaHits >= 5;
            if (!trkEtaInfo) ATH_MSG_DEBUG("Track has no ID eta information! (" << nrIDetaHits << " etaHits)");
            
            ++m_ntotTracks;
            
            /// Associate the segments to the Track
            const SortedSegmentMap assocStat  = m_doBidirectional ? populStat : findCandidateStations(*idTP, populStat); 
            if (assocStat.empty()) continue;
            
            const unsigned int num_assoc_segs = std::accumulate(populStat.begin(), populStat.end(), 0, 
                []( unsigned int N,const std::pair<int, std::vector<const Muon::MuonSegment*>>& seg_stat){
                    return N + seg_stat.second.size();
            });                    

            std::vector<MuonCombined::MuonSegmentInfo> segmentsInfo;
            segmentsInfo.reserve(num_assoc_segs);
            const int multiply = 1 + m_doBidirectional;

            std::optional<std::vector<std::string>> didExtrapolate = m_doTable ? std::make_optional<std::vector<std::string>>(SurfDef::NumSurf * multiply, "o") : std::nullopt;
            std::optional<std::vector<std::string>> segStation = m_doTable ? std::make_optional<std::vector<std::string>>(num_assoc_segs, "XXX") : std::nullopt;
            std::optional<std::vector<std::vector<std::string>>> trkToSegment = 
                                m_doTable ? std::make_optional<std::vector<std::vector<std::string>>>(SurfDef::NumSurf * multiply, *segStation) : std::nullopt;

            
            const int numberOfExtrapolations = 1 + m_doBidirectional;
            std::array<std::shared_ptr<const Trk::TrackParameters>, 2> trackAtMSEntrance;

            /// Start the extrapolations through the spectrometer
            for (int i_extrapolations = 0; i_extrapolations < numberOfExtrapolations; ++i_extrapolations) {              

                // in case of along momentum extrapolation, use pre-existing extrapolation if available
                const Trk::CaloExtension* extension = idTP->getCaloExtension();
                const  Muon::MuonSystemExtension* msExtension = idTP->getExtension();
                std::vector<Intersection> intersections;
                if (i_extrapolations == 0) {
                    /// update the extensions
                    if (msExtension) intersections = msExtension->layerIntersections();                    
                    if (extension && extension->muonEntryLayerIntersection()) {
                        const Trk::TrackParameters& pars = *extension->muonEntryLayerIntersection();
                        trackAtMSEntrance[i_extrapolations] = pars.uniqueClone();
                        ATH_MSG_DEBUG("Got MS entry pos: r " << pars.position().perp() << " z " << pars.position().z() << " momentum "
                                                             << pars.momentum().perp() << " cov " << pars.covariance());
                    }
                /// Do not use the intersections in the back extrapolations 
                } else intersections.clear();
                /// Actually this should never happen for the forward extrapolation
                if (!trackAtMSEntrance[i_extrapolations]) {
                    trackAtMSEntrance[i_extrapolations] = m_MuTagMatchingTool->ExtrapolateTrktoMSEntrance(ctx, *track.track(), Trk::alongMomentum);
                }

                if (!trackAtMSEntrance[i_extrapolations]) {
                    ATH_MSG_DEBUG("there is no track at MS entrance... not tagging for direction " << i_extrapolations);
                    continue;
                }
                ++m_natMSEntrance;

                std::shared_ptr<const Trk::TrackParameters> atSurface{trackAtMSEntrance[i_extrapolations]}, nextSurface{trackAtMSEntrance[i_extrapolations]};
                bool barrel_extp{false}, endcap_extp{false};
                
               
                ATH_MSG_DEBUG("Start extrapolation "<<i_extrapolations<<" found in total "<<num_assoc_segs<<" associated segments.");
           
                for (auto& stat_pair : assocStat) {
                    const int surface_counter = stat_pair.first;
                    const int extrapolation_counter = i_extrapolations* SurfDef::NumSurf + surface_counter;
                    const std::vector<const Muon::MuonSegment*>& targets = stat_pair.second;                    
                    
                    ATH_MSG_DEBUG("Surface " <<m_surfaces.stationType(surface_counter));                  
                    /// Segments found here
                    if (m_doTable) (*didExtrapolate)[extrapolation_counter] = "X";

                    std::optional<std::vector<std::string>> segVsSurf = m_doTable ? std::make_optional<std::vector<std::string>>(num_assoc_segs, "xxx") : std::nullopt;
                    const Trk::Surface* surface = m_surfaces.getSurface(surface_counter);
                    
                    const bool is_barrel = surface_counter < SurfDef::EIA;
                    /// Recylce the intersections to get as close as possible
                    /// to the target surfaces. In this case switch off the extrapolations 
                    bool extrapolate{true};
                    if (!intersections.empty()) {
                        const bool is_aside = surface_counter < SurfDef::EIC;                        
                        /// Find the intersection matching at best to the target surface                       
                        Intersection best_intersec;
                        for (const Intersection& intersect : intersections) {
                            if (m_surfaces.stIdxToSurfDef(intersect.layerSurface.stIndex(), is_aside) != surface_counter) continue;
                            if (!best_intersec.trackParameters || 
                                std::abs(getPropDistance(*intersect.trackParameters, *surface)) <
                                std::abs(getPropDistance(*best_intersec.trackParameters, *surface))) best_intersec = intersect;                          
                        }                       
                        if(best_intersec.trackParameters) {
                            atSurface = best_intersec.trackParameters;
                            extrapolate = false;
                            barrel_extp |= is_barrel;
                            endcap_extp |= !is_barrel;
                            ++m_recycledIntersect[surface_counter];
                        }
                    } 
                   
                   
                    if (extrapolate) {
                        if (m_doTable) (*didExtrapolate)[extrapolation_counter] = "V";
                        /// There exist tracks in the transition region between the endcaps & the barrel which have associated 
                        /// segment candidates with Barrel & Endcap like surfaces. However, if the previous surface was on a 
                        /// barrel and we start to extrapolate from the Barrel to an endcap like surface. It is very likely to fail
                        /// hence use as a fall back the MS entrance again
                        if (!is_barrel && barrel_extp && !endcap_extp) {
                            atSurface = trackAtMSEntrance[i_extrapolations];
                            endcap_extp = true;
                        }
                        const Trk::PropDirection surf_dir = getPropDistance(*atSurface, *surface) > 0 ?
                                                         Trk::alongMomentum : Trk::oppositeMomentum;                            
                        ATH_MSG_DEBUG("Extrapolate "<<m_surfaces.stationType(surface_counter)<<" from ("<<atSurface->position().perp()<<", "<<atSurface->position().z()<<"), to  surface "<<
                                                        "("<<surface->globalReferencePoint().perp()<<","<<surface->globalReferencePoint().z()<< ") "
                                                    << (surf_dir == Trk::alongMomentum ? "along" : "opposite")<<" momentum");       
                           
                        nextSurface = m_MuTagMatchingTool->ExtrapolateTrktoMSSurface(ctx, *surface, *atSurface, surf_dir);
                        /// Next surface succesfully reached
                        ++m_extrapolated[surface_counter];
                        if (nextSurface) {
                            atSurface = nextSurface;
                            barrel_extp |= is_barrel;
                            endcap_extp |= !is_barrel;   
                            /// Some debugging statements
                            if (m_doTable) (*trkToSegment)[extrapolation_counter] = (*segVsSurf);
                        } else {
                            continue;
                        }                        
                    } else if (m_doTable) (*didExtrapolate)[extrapolation_counter] = "S";                    
                    
                    ++m_goodExtrapolated[surface_counter]; 
                    
                    std::vector<SegmentExtrCache> seg_extps{};
                    seg_extps.reserve(targets.size());
                    ATH_MSG_VERBOSE("Perform segment matching");
                    int segmentCount{-1}; // Variable used for debugging
                    for (const Muon::MuonSegment* seg_ptr : targets) {
                        ++segmentCount;
                        if (m_doTable) { (*segStation)[segmentCount] = "   "; }

                        ////// Per Abstract Layer, loop over all segments, and find out wether the segment is close to the
                        ////// extrapolated track to the abstract layer.
                        bool isMatched{false};
                        if (!m_doTable) {
                            isMatched = m_MuTagMatchingTool->match(*atSurface, *seg_ptr, surface_counter);
                        } else {
                            if (!m_MuTagMatchingTool->surfaceMatch(*seg_ptr, surface_counter)) {
                                (*segVsSurf)[segmentCount] = "surface";
                                continue;
                            }
                            ATH_MSG_VERBOSE("treating track " << trackCount << " (extrapolation = " << i_extrapolations << ") and Segment "
                                                              << (*segStation)[segmentCount] << " (segment " << segmentCount << ")");

                            if (!m_MuTagMatchingTool->phiMatch(*atSurface, *seg_ptr)) {
                                (*segVsSurf)[segmentCount] = "RghPhi";
                                continue;
                            }
                            if (trkEtaInfo) {
                                if (is_barrel) {
                                    if (!m_MuTagMatchingTool->thetaMatch(*atSurface, *seg_ptr)) {
                                        (*segVsSurf)[segmentCount] = "RghTheta";
                                        continue;
                                    }
                                } else {
                                    if (!m_MuTagMatchingTool->rMatch(*atSurface, *seg_ptr)) {
                                        (*segVsSurf)[segmentCount] = "RghR";
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
                                    (*segVsSurf)[segmentCount] = "posPhi";
                                else
                                    (*segVsSurf)[segmentCount] = "pos";
                            }
                            continue;
                        }
                        ////// after a rough match (on station name, second coordinate and finally precision coordinate)
                        ////// extrapolate the track to the plane-surface associated to the matching segment.

                        std::shared_ptr<const Trk::AtaPlane> atSegSurface;
                        /// Check if there exists already a segment on the same surface. No need to redo the extrapolations twice
                        std::vector<SegmentExtrCache>::const_iterator cache_itr = std::find_if(seg_extps.begin(), seg_extps.end(), [seg_ptr](const SegmentExtrCache& cache){
                                return cache.segment_surface == &(seg_ptr->associatedSurface());
                            });
                        if (cache_itr != seg_extps.end()) {
                            atSegSurface = cache_itr->segment_pars;
                            ATH_MSG_DEBUG("Recycle extrapolation");
                        } else {
                            const Trk::PropDirection seg_dir = getPropDistance(*atSurface, seg_ptr->associatedSurface()) > 0 ?
                                                         Trk::alongMomentum : Trk::oppositeMomentum;
                            ATH_MSG_DEBUG("Extrapolate "<<m_surfaces.stationType(surface_counter)<<" from ("<<atSurface->position().perp()<<", "<<atSurface->position().z()<<"), to  segment "<<
                                                        "("<<seg_ptr->globalPosition().perp()<<","<<seg_ptr->globalPosition().z()<< ") "
                                                        << (seg_dir == Trk::alongMomentum ? "along" : "opposite")<<" momentum");       
                            atSegSurface = m_MuTagMatchingTool->ExtrapolateTrktoSegmentSurface(ctx, *seg_ptr, *atSurface, seg_dir);
                            seg_extps.emplace_back(atSegSurface, seg_ptr->associatedSurface());
                        }
                        if (!atSegSurface || !atSegSurface->covariance() ||
                            !Amg::saneCovarianceDiagonal(*atSegSurface->covariance()))
                          continue;
                        const AmgSymMatrix(5) invCov =
                          atSegSurface->covariance()->inverse();
                        if (!Amg::saneCovarianceDiagonal(invCov))
                          continue;

                        MuonCombined::MuonSegmentInfo info = m_MuTagMatchingTool->muTagSegmentInfo(ctx, track.track(), *seg_ptr, atSegSurface);
                        if (segmentToxAODSegmentMap) info.link = (*segmentToxAODSegmentMap)[seg_ptr];
                        isMatched = m_MuTagMatchingTool->matchSegmentPosition(info, trkEtaInfo);
                        if (!isMatched) {
                            if (m_doTable) {
                                if (!trkEtaInfo)
                                    (*segVsSurf)[segmentCount] = "posPhi";
                                else
                                    (*segVsSurf)[segmentCount] = "pos";
                            }
                            continue;
                        }
                        isMatched = m_MuTagMatchingTool->matchSegmentDirection(info, trkEtaInfo);

                        if (!isMatched) {
                            if (m_doTable) {
                                if (!trkEtaInfo)
                                    (*segVsSurf)[segmentCount] = "dirPhi";
                                else
                                    (*segVsSurf)[segmentCount] = "dir";
                            }
                            continue;
                        }

                        isMatched = m_MuTagMatchingTool->matchDistance(info);

                        if (!isMatched) {
                            if (m_doTable) { (*segVsSurf)[segmentCount] = "dist"; }
                            continue;
                        }

                        isMatched = m_MuTagMatchingTool->matchCombinedPull(info);
                        if (!isMatched) {
                            if (m_doTable) { (*segVsSurf)[segmentCount] = "cpull"; }
                            continue;
                        }

                        if (m_doPtDependentPullCut) {
                            isMatched = m_MuTagMatchingTool->matchPtDependentPull(info, *track.track());
                            if (!isMatched) {
                                if (m_doTable) { (*segVsSurf)[segmentCount] = "ptpull"; }
                                continue;
                            }
                        }
                        if (m_doTable) (*segVsSurf)[segmentCount] = "TAG";
                        matchedSegment = true;
                        ATH_MSG_DEBUG("Tagged the track with Segment " << segmentCount);

                        segmentsInfo.push_back(std::move(info));
                       
                    }  // end loop over segments

                    if (m_doTable) (*trkToSegment)[extrapolation_counter] = (*segVsSurf);
                }  // end loop over MS surfaces

            }  // end loop over nr Directions of extrapolation.

            if (m_doTable) {               
                printTable(*didExtrapolate, *segStation, *trkToSegment);
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
                ATH_MSG_DEBUG("number of accepted tracks " << m_naccepted << " segmentsInfo size " << segmentsInfo.size());

                std::vector<MuonCombined::MuonSegmentInfo> segmentsInfoSolved =
                    m_MuTagAmbiguitySolverTool->selectBestMuTaggedSegments(ctx, segmentsInfo);
                segmentsInfoSelected.insert(segmentsInfoSelected.end(), 
                                            std::make_move_iterator(segmentsInfoSolved.begin()),
                                            std::make_move_iterator(segmentsInfoSolved.end()));
                ATH_MSG_DEBUG("segmentsInfoSelected size " << segmentsInfoSelected.size());
            }

        }  // end loop over tracks

        ATH_MSG_DEBUG("segmentsInfoSelected size after track loop " << segmentsInfoSelected.size());
        std::vector<MuonCombined::MuonSegmentInfo> segmentsInfoFinal = m_MuTagAmbiguitySolverTool->solveAmbiguities(ctx, segmentsInfoSelected);
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
            std::vector<MuonCombined::MuonSegmentInfo> segmentsInfoTag;
            segmentsInfoTag.reserve(segmentsInfoFinal.size());
            
            for (unsigned int ns1 = 0; ns1 < segmentsInfoFinal.size(); ++ns1) {
                if (segmentsInfoFinal[ns1].track == track && segmentsInfoFinal[ns1].nsegments > 0) {
                    segmentsInfoTag.push_back(segmentsInfoFinal[ns1]);                                  
                }                
            }
            if (segmentsInfoTag.empty()) continue;
            ATH_MSG_DEBUG("make Segment Tag object for " << m_printer->print(*track) << " nr segments " << segmentsInfoTag.size());
            SegmentTag* tag = new SegmentTag(segmentsInfoTag);
            tagMap->addEntry(idTP, tag);
            
        }
    }

    void MuonSegmentTagTool::printTable(const std::vector<std::string>& didEx, const std::vector<std::string>& segStation,
                                        const std::vector<std::vector<std::string>>& segToSurf) const {
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

        for (int counter = 0; counter < SurfDef::NumSurf; ++counter) {
            if (msgLvl(MSG::DEBUG))
                msg(MSG::DEBUG) << std::setw(5) << m_surfaces.stationType(counter) << signstr << std::setw(3) << didEx[extrapolation_counter];
            for (unsigned int segment_counter = 0; segment_counter < segStation.size(); ++segment_counter) {
                if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << std::setw(10) << (segToSurf[extrapolation_counter])[segment_counter];
            }  // end loop segments
            if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << endmsg;
            ++extrapolation_counter;
        }  // end loop surfaces

        if (m_doBidirectional) {
            signstr = "-";
            for (unsigned int counter = 0; counter < SurfDef::NumSurf; ++counter) {
                if (msgLvl(MSG::DEBUG))
                    msg(MSG::DEBUG) << std::setw(5) << m_surfaces.stationType(counter) << signstr << std::setw(3)
                                    << didEx[extrapolation_counter];
                for (unsigned int segment_counter = 0; segment_counter < segStation.size(); ++segment_counter) {
                    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << std::setw(10) << (segToSurf[extrapolation_counter])[segment_counter];
                }  // end loop segments
                if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << endmsg;
                ++extrapolation_counter;
            }  // end loop surfaces
        }        
    }
    std::vector<const Muon::MuonSegment*> MuonSegmentTagTool::getCandidateSegments(const std::vector<const Muon::MuonSegment*> & segments) const{
         if (!m_doSegmentsFilter){
            ATH_MSG_VERBOSE("No segment filtering required. ");
            return segments;
         }
        std::vector<const Muon::MuonSegment*> FilteredSegmentCollection;
        FilteredSegmentCollection.reserve(segments.size());
         
        for (const Muon::MuonSegment* itSeg : segments) {
            int quality = m_segmentSelector->quality(*itSeg);
            ATH_MSG_DEBUG(" Segment quality " << quality);
            if (quality < m_segmentQualityCut) continue;
            Muon::IMuonSegmentHitSummaryTool::HitCounts hitCounts = m_hitSummaryTool->getHitCounts(*itSeg);
            if (hitCounts.ncscHitsPhi + hitCounts.ncscHitsEta > 0) ATH_MSG_DEBUG(" CSC segment ");
            else {
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
                StationIndex stIndex = m_idHelperSvc->stationIndex(chId);
                if (!m_triggerHitCut && stIndex == Muon::MuonStationIndex::EM) {
                    // don't apply the TGC requirement for the first station as it sometimes has not trigger hits due to TGC acceptance
                    int stationEta = m_idHelperSvc->stationEta(chId);
                    if (std::abs(stationEta) != 1) {
                        // remove EM segments without trigger hits
                        if (hitCounts.nphiTrigHitLayers == 0 && hitCounts.netaTrigHitLayers == 0) continue;
                    }
                }
                if (stIndex == Muon::MuonStationIndex::EI) {
                    // remove CSC segment with hits in only one projection
                    /// Update ME: Need to think about cuts for the NSW!
                    if (hitCounts.nmdtHits() == 0 && (!hitCounts.ncscHitsEta  | !hitCounts.ncscHitsPhi))
                        continue;
                }
            }
            if (hitCounts.ncscHitsPhi + hitCounts.ncscHitsEta > 0) ATH_MSG_DEBUG(" CSC segment passed");
            FilteredSegmentCollection.emplace_back(itSeg);
        }
        return FilteredSegmentCollection;
    }
    MuonSegmentTagTool::SortedSegmentMap MuonSegmentTagTool::findPopulatedStations(const std::vector<const Muon::MuonSegment*>& filteredSegments) const {
        SortedSegmentMap sortedSegments{};
        for (const Muon::MuonSegment* itSeg : filteredSegments) {
            const Identifier chId = m_edmHelperSvc->chamberId(*itSeg);
            const int surf_def =  MuonCombined::MuonSegmentTagSurfaces::stIdxToSurfDef(m_idHelperSvc->stationIndex(chId),  m_idHelperSvc->stationEta(chId) > 0);
            std::vector<const Muon::MuonSegment*>& surf_vec = sortedSegments[surf_def];
            if (surf_vec.empty()) surf_vec.reserve(filteredSegments.size());
            surf_vec.push_back(itSeg);
        }
        return sortedSegments;
    }
    MuonSegmentTagTool::SortedSegmentMap MuonSegmentTagTool::findCandidateStations(const MuonCombined::InDetCandidate& id_cand, const MuonSegmentTagTool::SortedSegmentMap& filteredSegments) const {
        SortedSegmentMap sortedSegments; 
              
        bool hasAngleMatch {false}, hasMatch{false};
        const Trk::Track* track = id_cand.indetTrackParticle().track();
        const Amg::Vector3D id_mom = track->perigeeParameters()->momentum();
        
        const double qID = track->perigeeParameters()->charge();
        const double pID = id_mom.mag();
     
        if (pID <= 20000. * (qID *  id_mom.eta() - 2)) {
            return sortedSegments;
        } 

        const Trk::CaloExtension* extension = id_cand.getCaloExtension();
        const Trk::TrackParameters* ms_entry = extension ? extension->muonEntryLayerIntersection() : nullptr;
        const double ThetaID =  (ms_entry ? ms_entry->position() : id_mom).theta();
       
        std::vector<int> id_sectors;
        if (ms_entry) m_sectorMapping.getSectors(ms_entry->position().phi(), id_sectors);       
        auto sector_match = [&, this](const Amg::Vector3D& seg_pos) -> bool {
            if (!ms_entry) return std::abs(qID * seg_pos.deltaPhi(id_mom) - qID / pID) < 0.6;
            std::vector<int> seg_sectors;
            m_sectorMapping.getSectors(seg_pos.phi(), seg_sectors);            
            return std::find_if( id_sectors.begin(), id_sectors.end(), [&seg_sectors](const int id_sec) {
                return std::find(seg_sectors.begin(), seg_sectors.end(), id_sec) != seg_sectors.end();
            }) != id_sectors.end();
        };

        for (const auto& key_pair : filteredSegments) {
            const int surf_def = key_pair.first;
            const std::vector<const Muon::MuonSegment*>& source_vec = key_pair.second;
            std::vector<const Muon::MuonSegment*> target_vec;
            target_vec.reserve(source_vec.size());
            for (const Muon::MuonSegment* itSeg : source_vec) {
                const Amg::Vector3D& pos = itSeg->globalPosition();
                const double dTheta = pos.theta() - ThetaID; 
                const bool theta_match = ms_entry ? std::abs(dTheta) < 0.2 : (qID * dTheta < 0.2 && qID * dTheta > -0.6);
                if (!theta_match) {
                    ATH_MSG_VERBOSE("dTheta cut failed");
                    continue;
                }
                 if (!sector_match(pos)) {
                    ATH_MSG_VERBOSE("dPhi cut failed");
                    continue;
                }   
                hasAngleMatch = true;               
                const Identifier chId = m_edmHelperSvc->chamberId(*itSeg);
                if (!m_idHelperSvc->isCsc(chId)) hasMatch = true;
                target_vec.push_back(itSeg);
            }
            if (target_vec.empty()) continue;
            sortedSegments[surf_def] = std::move(target_vec);
        }        
        if (hasAngleMatch) ++m_nangleMatch;
        if (hasMatch) ++m_npmatch;
        else sortedSegments.clear();     
        return sortedSegments;
    }
    double MuonSegmentTagTool::getPropDistance(const Trk::TrackParameters& start_pars, const Trk::Surface& target_surf) {
        Trk::DistanceSolution distSol = target_surf.straightLineDistanceEstimate(start_pars.position(), start_pars.momentum().normalized());
        if (distSol.numberOfSolutions() > 0) return distSol.first();
        return distSol.toPointOfClosestApproach();  
    }      
}  // namespace MuonCombined
