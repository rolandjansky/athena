/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonClusterSegmentFinderTool.h"

#include "EventPrimitives/EventPrimitivesHelpers.h"
#include "MuonPrepRawData/MMPrepData.h"
#include "MuonPrepRawData/MuonPrepDataCollection.h"
#include "MuonPrepRawData/sTgcPrepData.h"
#include "MuonReadoutGeometry/MuonPadDesign.h"
#include "MuonReadoutGeometry/MuonReadoutElement.h"
#include "MuonReadoutGeometry/sTgcReadoutElement.h"
#include "MuonRecToolInterfaces/IMuonTrackCleaner.h"
#include "TrkEventPrimitives/FitQuality.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkPseudoMeasurementOnTrack/PseudoMeasurementOnTrack.h"
#include "TrkTrack/Track.h"
#include "FourMomUtils/xAODP4Helpers.h"

namespace Muon {


    //============================================================================
    MuonClusterSegmentFinderTool::MuonClusterSegmentFinderTool(const std::string& type, const std::string& name, const IInterface* parent) 
    : AthAlgTool(type, name, parent) {
        declareInterface<IMuonClusterSegmentFinderTool>(this);
    }


    //============================================================================
    StatusCode MuonClusterSegmentFinderTool::initialize() {
        ATH_CHECK(m_slTrackFitter.retrieve());
        ATH_CHECK(m_printer.retrieve());
        ATH_CHECK(m_edmHelperSvc.retrieve());
        ATH_CHECK(m_ambiTool.retrieve());
        ATH_CHECK(m_trackToSegmentTool.retrieve());
        ATH_CHECK(m_idHelperSvc.retrieve());
        ATH_CHECK(m_trackCleaner.retrieve());
        ATH_CHECK(m_trackSummary.retrieve());
        ATH_MSG_DEBUG(" Max cut " << m_maxClustDist);
        return StatusCode::SUCCESS;
    }


    //============================================================================
    void MuonClusterSegmentFinderTool::find(const EventContext& ctx, 
                                            std::vector<const Muon::MuonClusterOnTrack*>& muonClusters,
                                            std::vector<std::unique_ptr<Muon::MuonSegment>>& segments, 
                                            Trk::SegmentCollection* segColl) const {

        ATH_MSG_DEBUG("Entering MuonClusterSegmentFinderTool with " << muonClusters.size() << " clusters to be fit");
        std::vector<std::unique_ptr<Muon::MuonSegment>> etaSegs;
        findPrecisionSegments(ctx, muonClusters, etaSegs);
        if (etaSegs.empty()) return;
        find3DSegments(ctx, muonClusters, etaSegs, segments, segColl);
        etaSegs.clear();
    }
    

    //============================================================================
    // find the precision (eta) segments
    void MuonClusterSegmentFinderTool::findPrecisionSegments(const EventContext& ctx, 
                                                             std::vector<const Muon::MuonClusterOnTrack*>& muonClusters,
                                                             std::vector<std::unique_ptr<Muon::MuonSegment>>& etaSegs) const {

        // clean the muon clusters -- select only the eta hits
        std::vector<const Muon::MuonClusterOnTrack*> clusters = cleanClusters(muonClusters, false); // eta hits only
        ATH_MSG_VERBOSE("After hit cleaning, there are " << clusters.size() << " precision 2D clusters");

        // classify eta clusters by layer
        std::vector<std::vector<const Muon::MuonClusterOnTrack*> > orderedClusters = classifyByLayer(clusters, false);         
        if (orderedClusters.size() < 4) return; // at least four layers with eta hits (MM and sTGC)

        // create segment seeds
        std::vector<std::pair<Amg::Vector3D, Amg::Vector3D> > seeds = segmentSeed(orderedClusters, false);
        ATH_MSG_VERBOSE("Found " << seeds.size() << " 2D seeds");

        // Loop on seeds: find all clusters near the seed and try to fit
        std::vector<const Muon::MuonClusterOnTrack*> hitVec;
        std::unique_ptr<TrackCollection> segTrkColl = std::make_unique<TrackCollection>();

        for (std::pair<Amg::Vector3D, Amg::Vector3D>& seed : seeds) {

            std::vector<const Muon::MuonClusterOnTrack*> hitVecTmp;
            getClustersOnSegment(orderedClusters, seed, hitVecTmp);
            if (hitVecTmp.size() < 4) continue;

            // reduce combinatorics
            if (hitVecTmp == hitVec) {
                ATH_MSG_VERBOSE(" found same set of clusters as for the previous 2D seed");
                continue;
            }

            hitVec = std::move(hitVecTmp); 
            const unsigned int nMM = std::count_if(hitVec.begin(), hitVec.end(), [this](const Muon::MuonClusterOnTrack* hit) { return m_idHelperSvc->isMM(hit->identify()); });

            // vector of hits to be fit:
            std::vector<const Trk::MeasurementBase*> vecFitPts;
            vecFitPts.reserve(hitVec.size() + 2 + m_ipConstraint);

            // add pseudo measurement for vtx
            std::unique_ptr<Trk::PseudoMeasurementOnTrack> pseudoVtx{nullptr};
            if (m_ipConstraint) {
                double errVtx{100.};
                Amg::MatrixX covVtx(1, 1);
                covVtx(0, 0) = errVtx * errVtx;
                Amg::Vector3D vtx(0., 0., 0.);
                Trk::PerigeeSurface perVtx(vtx);
                pseudoVtx = std::make_unique<Trk::PseudoMeasurementOnTrack>(Trk::LocalParameters(Trk::DefinedParameter(0, Trk::locX)), covVtx, perVtx);
                vecFitPts.push_back(pseudoVtx.get());
            }

            // generate two pseudo phi measurements for the fit
            // one goes on the first hit surface and the other on the last hit surface.
            double errPos = (nMM) ? 1000. : 0.1;
            Amg::MatrixX cov(1, 1);
            cov(0, 0) = errPos * errPos;
            
            std::unique_ptr<Trk::PseudoMeasurementOnTrack> pseudoSegPhi1 = std::make_unique<Trk::PseudoMeasurementOnTrack>(
                Trk::LocalParameters(Trk::DefinedParameter(0, Trk::locY)), cov, hitVec.front()->associatedSurface());
            std::unique_ptr<Trk::PseudoMeasurementOnTrack> pseudoSegPhi2 = std::make_unique<Trk::PseudoMeasurementOnTrack>(
                Trk::LocalParameters(Trk::DefinedParameter(0, Trk::locY)), cov, hitVec.back()->associatedSurface());

            // add the first pseudo phi hit, the hit vector, and the second pseudo phi hit
            vecFitPts.push_back(pseudoSegPhi1.get());
            std::copy(hitVec.begin(), hitVec.end(), std::back_inserter(vecFitPts));
            vecFitPts.push_back(pseudoSegPhi2.get());

            // vector of hits ready for fit.
            ATH_MSG_VERBOSE("Fitting a 2D segment track with " << vecFitPts.size() << " hits");

            // calculate start parameters
            std::unique_ptr<Trk::TrackParameters> startpar;
            const Trk::Surface& surf = hitVec.front()->associatedSurface();
            if (surf.type() == Trk::SurfaceType::Plane) {
                const Trk::PlaneSurface& psurf = static_cast<const Trk::PlaneSurface&>(surf);
                // local position and direction of the seed on the surface of the first cluster
                Trk::Intersection intersect = psurf.straightLineIntersection(seed.first, seed.second, false, false);
                Amg::Vector2D lpos_seed;
                Trk::LocalDirection ldir_seed;
                psurf.globalToLocal(intersect.position, intersect.position, lpos_seed);
                psurf.globalToLocalDirection(seed.second, ldir_seed);
       
                // use the seed info to generate start parameters (dummy values for phi)
                Amg::Vector2D lpos(lpos_seed[Trk::locX], 0.);
                Trk::LocalDirection ldir(ldir_seed.angleXZ(), -M_PI_2);
                Amg::Vector3D gdir_seg, gpos_seg;
                psurf.localToGlobal(lpos, gpos_seg, gpos_seg);
                psurf.localToGlobalDirection(ldir, gdir_seg);

                Amg::Vector3D perpos = gpos_seg - 10*gdir_seg.unit();
                if (perpos.dot(gdir_seg) < 0) gdir_seg *= -1;
                startpar = std::make_unique<Trk::Perigee>(perpos, gdir_seg, 0, perpos);
                ATH_MSG_VERBOSE(" start parameter " << perpos << " pp " << startpar->position() << " gd " << gdir_seg.unit() << " pp " << startpar->momentum().unit());
            } else {
                // old method as fall back (should never happen)
                Trk::PerigeeSurface perpos(vecFitPts.front()->globalPosition());
                startpar = std::make_unique<Trk::Perigee>(0, 0, seed.second.phi(), seed.second.theta(), 0, perpos);
            }
            
            std::unique_ptr<Trk::Track> segtrack = fit(ctx, vecFitPts, *startpar);
            
            if (segtrack) {
                m_trackSummary->updateTrack(*segtrack);

                if (msgLvl(MSG::VERBOSE)) {
                    // Peter NEW calculate distance of segment to all muon clusters
                    Amg::Vector3D pos = segtrack->perigeeParameters()->position();
                    Amg::Vector3D dir = segtrack->perigeeParameters()->momentum().unit();
                    ATH_MSG_VERBOSE(" 2D eta segment found position " << pos << " direction " << dir << " hits " << vecFitPts.size());
                    for (unsigned int k = 0; k < muonClusters.size(); ++k) {
                        const Trk::Surface& surf = muonClusters[k]->associatedSurface();
                        Trk::Intersection intersect = surf.straightLineIntersection(pos, dir, false, false);
                        Amg::Vector2D lpos;
                        surf.globalToLocal(intersect.position, intersect.position, lpos);
                        double residual = muonClusters[k]->localParameters()[Trk::locX] - lpos[0];
                        double error    = Amg::error(muonClusters[k]->localCovariance(), Trk::locX);
                        double pull     = residual / error;
                        ATH_MSG_VERBOSE(" lay " << k << " residual " << residual << " error  " << error << " pull " << pull << "  "
                                                << m_idHelperSvc->toString(muonClusters[k]->identify()));
                    }
                }
                segTrkColl->push_back(std::move(segtrack));

            } else
                ATH_MSG_DEBUG(" 2D segment fit failed");
        }

        if (msgLvl(MSG::DEBUG)) {
            ATH_MSG_DEBUG("Tracks before ambi solving: ");
            for (const Trk::Track* trk : *segTrkColl) {
                ATH_MSG_DEBUG(m_printer->print(*trk));
                const DataVector<const Trk::MeasurementBase>* meas = trk->measurementsOnTrack();
                if (meas) ATH_MSG_DEBUG(m_printer->print(meas->stdcont()));
            }
        }

        if (segTrkColl->empty()) return;

        std::unique_ptr<const TrackCollection> resolvedTracks(m_ambiTool->process(segTrkColl.get()));
        ATH_MSG_DEBUG("Resolved track candidates: old size " << segTrkColl->size() << " new size " << resolvedTracks->size());

        // store the resolved segments
        for (const Trk::Track* trk : *resolvedTracks) {
            MuonSegment* seg = m_trackToSegmentTool->convert(ctx, *trk);
            if (seg) {
                ATH_MSG_DEBUG(" adding " << m_printer->print(*seg) << std::endl << m_printer->print(seg->containedMeasurements()));
                etaSegs.emplace_back(seg);
            } else {
                ATH_MSG_VERBOSE("Segment conversion failed, no segment created. ");
            }
        }
    }


    //============================================================================
    void MuonClusterSegmentFinderTool::find3DSegments(const EventContext& ctx, std::vector<const Muon::MuonClusterOnTrack*>& muonClusters,
                                                      std::vector<std::unique_ptr<Muon::MuonSegment>>& etaSegs, 
                                                      std::vector<std::unique_ptr<Muon::MuonSegment>>& segments,
                                                      Trk::SegmentCollection* segColl) const {
  
        std::vector<const Muon::MuonClusterOnTrack*> phiClusters = cleanClusters(muonClusters, true);  // phi clusters only
        ATH_MSG_DEBUG("After hit cleaning, there are " << phiClusters.size() << " phi clusters to be fit");
        // classify the phi clusters by layer
        std::vector<std::vector<const Muon::MuonClusterOnTrack*> > orderedWireClusters = classifyByLayer(phiClusters, true);  // wires only
        std::vector<std::vector<const Muon::MuonClusterOnTrack*> > orderedPadClusters  = classifyByLayer(phiClusters, false); // pads only
        if (orderedWireClusters.size() + orderedPadClusters.size() < 2) {
            ATH_MSG_DEBUG("Not enough phi hits present, cannot perform the 3D fit!");
            for (std::unique_ptr<Muon::MuonSegment>& etaSeg : etaSegs) {
                if (segColl) {
                    segColl->push_back(std::move(etaSeg));
                } else {
                    segments.push_back(std::move(etaSeg));
                }
            }
            return;
        }

        std::vector<const Muon::MuonClusterOnTrack*> etaClusters = cleanClusters(muonClusters, false); // eta clusters only
        std::vector<std::vector<const Muon::MuonClusterOnTrack*> > orderedEtaClusters  = classifyByLayer(etaClusters, false);

        // loop on eta segments
        bool triedWireSeed{false}; // wire seeds need to be retrieved only once, the first time they are needed
        std::vector<std::pair<Amg::Vector3D,Amg::Vector3D> > seeds_WiresSTGC;
        std::unique_ptr<TrackCollection> segTrkColl( std::make_unique<TrackCollection>(SG::OWN_ELEMENTS) );

        // Loop on eta segments
        for (std::unique_ptr<Muon::MuonSegment>& etaSeg : etaSegs) {
      
            bool is3Dseg(false);

            std::pair<Amg::Vector3D,Amg::Vector3D> seed2D(etaSeg->globalPosition(), etaSeg->globalDirection());
            std::vector<const Muon::MuonClusterOnTrack*> etaHits;
            etaHits.reserve(16);
            getClustersOnSegment(orderedEtaClusters, seed2D, etaHits); // eta clusters

            // Phi Seeding:
            // 1 - try seeding from the MM stereo strips of the eta segment
	    std::vector<Amg::Vector3D> mmStereoMeasurements = getPhiFromStereo(etaHits); 
	    std::vector<std::pair<Amg::Vector3D,Amg::Vector3D> > seeds = segmentSeedFromMM(mmStereoMeasurements);
            if (!seeds.empty()) {
                ATH_MSG_DEBUG(" Seeding from MM stereo strips");
            } else {
                // 2 - otherwise try seeding from sTGC wires
                if (std::abs(etaSeg->globalPosition().eta()) < 2.4) {
                    if (!triedWireSeed) {
                        // wire seeds need to be retrieved only once (they don't depend on the eta segment)
                        triedWireSeed   = true;
                        seeds_WiresSTGC = segmentSeed(orderedWireClusters, true);
                        segmentSeedPhiOR(seeds_WiresSTGC); // overlap removal
                    }

                    if (!seeds_WiresSTGC.empty()) {
                        seeds = seeds_WiresSTGC;
                        ATH_MSG_DEBUG(" Seeding from sTGC wires");
                    }
                }

                // 3 - last resort, try sTGC pads
                if (seeds.empty()) {
                    seeds = segmentSeedFromPads(orderedPadClusters, etaSeg.get());
                    segmentSeedPhiOR(seeds); // overlap removal
                    ATH_MSG_DEBUG(" Seeding from sTGC pads");
                }
            }

            // Loop on phi seeds
            std::vector<const Muon::MuonClusterOnTrack*> phiHitsPrevious;
            const Trk::PlaneSurface& etaSegSurf = etaSeg->associatedSurface();
            double  etaSegLocX  = etaSeg->localParameters()[Trk::locX];
            double  etaSegLocXZ = etaSeg->localDirection().angleXZ();

            for (std::pair<Amg::Vector3D, Amg::Vector3D>& seed : seeds) {

                // calculate start parameters

                // local position and direction of the seed on the surface associated to the segment.            
                Trk::Intersection intersect = etaSegSurf.straightLineIntersection(seed.first, seed.second, false, false);
                Amg::Vector2D lpos_seed;
                Trk::LocalDirection ldir_seed;
                etaSegSurf.globalToLocal(intersect.position, intersect.position, lpos_seed);
                etaSegSurf.globalToLocalDirection(seed.second, ldir_seed);

                // combine with information from the eta segment to get an estimate
                // of the global position and direction of the 3D segment on the surface.
                Amg::Vector2D lpos_seg(etaSegLocX, lpos_seed[Trk::locY]);
                Trk::LocalDirection ldir_seg(etaSegLocXZ, ldir_seed.angleYZ());

                Amg::Vector3D gpos_seg, gdir_seg;
                etaSegSurf.localToGlobal(lpos_seg, gpos_seg, gpos_seg);
                etaSegSurf.localToGlobalDirection(ldir_seg, gdir_seg);

                Amg::Vector3D perpos = gpos_seg - 10 * gdir_seg.unit();
                if (perpos.dot(gdir_seg) < 0) gdir_seg *= -1;
                std::pair<Amg::Vector3D, Amg::Vector3D> seed3D(perpos, gdir_seg);
                std::unique_ptr<Trk::TrackParameters> startpar = std::make_unique<Trk::Perigee>(perpos, gdir_seg, 0, perpos);
                ATH_MSG_VERBOSE(" start parameter " << perpos << " pp " << startpar->position() << " gd " << gdir_seg.unit() << " pp " << startpar->momentum().unit());

                // gather phi hits aligned with the segment
                std::vector<const Muon::MuonClusterOnTrack*> phiHits, phiHitsPads, phiHitsWires;
                phiHits.reserve(16);
                phiHitsWires.reserve(8);
                phiHitsPads.reserve(8);

                int nPhiHits = getClustersOnSegment(orderedPadClusters,  seed3D, phiHitsPads);  // add pad hits
                nPhiHits    += getClustersOnSegment(orderedWireClusters, seed3D, phiHitsWires); // add wire hits
		if (nPhiHits < 2) continue; // at least two phi hits

                // pad and wire hits sorted by the z-coordinate
                std::merge(phiHitsPads.begin(), phiHitsPads.end(), phiHitsWires.begin(), phiHitsWires.end(), std::back_inserter(phiHits), 
                           [](const Muon::MuonClusterOnTrack* c1, const Muon::MuonClusterOnTrack* c2) { 
                              double z1 = std::abs(c1->detectorElement()->center(c1->identify()).z());
                              double z2 = std::abs(c2->detectorElement()->center(c2->identify()).z());
                              return z1 < z2;}
                          );

                // logic to reduce combinatorics
                if (phiHits == phiHitsPrevious) {
                    ATH_MSG_VERBOSE(" same phi hits as for the previous 3D seed; skipping this seed");
                    continue;
                }
                phiHitsPrevious = phiHits;

                // calibrate the eta hits
                std::vector<const Muon::MuonClusterOnTrack*> etaHitsCalibrated = getCalibratedClusters(etaHits, seed3D); 

                ATH_MSG_VERBOSE("There are " << etaHitsCalibrated.size() << " & " << phiHits.size() << " eta and phi hits to fit");
               
                // vector of hits to be fit:
                std::vector<const Trk::MeasurementBase*> vecFitPts;
                vecFitPts.reserve(phiHits.size() + etaHitsCalibrated.size() + m_ipConstraint);

                // add a pseudo measurement for vtx
                std::unique_ptr<Trk::PseudoMeasurementOnTrack> pseudoVtx{nullptr};
                if (m_ipConstraint) {
                    ATH_MSG_DEBUG("add pseudo vertex");
                    double errVtx = 100.;
                    Amg::MatrixX covVtx(1, 1);
                    covVtx(0, 0) = errVtx * errVtx;
                    Amg::Vector3D vtx(0., 0., 0.);
                    Trk::PerigeeSurface perVtx(vtx);
                    pseudoVtx = std::make_unique<Trk::PseudoMeasurementOnTrack>(Trk::LocalParameters(Trk::DefinedParameter(0, Trk::locX)), covVtx, perVtx);
                    vecFitPts.push_back(pseudoVtx.get());
                }

                // sorted eta and sorted phi hits combined (sorted by their the z-coordinate)
                std::merge(phiHits.begin(), phiHits.end(), etaHitsCalibrated.begin(), etaHitsCalibrated.end(), std::back_inserter(vecFitPts), 
                           [](const Muon::MuonClusterOnTrack* c1, const Muon::MuonClusterOnTrack* c2) { 
                              double z1 = std::abs(c1->detectorElement()->center(c1->identify()).z());
                              double z2 = std::abs(c2->detectorElement()->center(c2->identify()).z());
                              return z1 < z2;}
                          );

                ATH_MSG_DEBUG("Fitting a 3D segment with " << phiHits.size() << " phi hits and " << vecFitPts.size() << " total hits");
                if (msgLvl(MSG::VERBOSE)) {
                    for (const Trk::MeasurementBase*& hit : vecFitPts) {
                        Identifier id = m_edmHelperSvc->getIdentifier(*hit);
                        ATH_MSG_VERBOSE(m_idHelperSvc->toString(id) << " " << hit->globalPosition().perp() << ", " << hit->globalPosition().z());
                    }
                }

                std::unique_ptr<Trk::Track> segtrack = fit(ctx, vecFitPts, *startpar);
                
                if (segtrack) {
                    m_trackSummary->updateTrack(*segtrack);
                    // store the track segment in track collection
                    is3Dseg = true;

                    // Peter NEW calculate distance of segment to all muon clusters
                    if (msgLvl(MSG::VERBOSE)) {
                        Amg::Vector3D pos = segtrack->perigeeParameters()->position();
                        Amg::Vector3D dir = segtrack->perigeeParameters()->momentum();
                        dir = dir / dir.mag();
                        ATH_MSG_VERBOSE("3D eta-phi segment found position " << pos << " direction " << dir << " hits " << vecFitPts.size());
                        for (unsigned int k = 0; k < muonClusters.size(); ++k) {
                            const Trk::Surface& surf = muonClusters[k]->associatedSurface();
                            Trk::Intersection intersect = surf.straightLineIntersection(pos, dir, false, false);
                            Amg::Vector2D lpos;
                            surf.globalToLocal(intersect.position, intersect.position, lpos);
                            double residual = muonClusters[k]->localParameters()[Trk::locX] - lpos[0];
                            double error = Amg::error(muonClusters[k]->localCovariance(), Trk::locX);
                            double pull = residual / error;
                            ATH_MSG_VERBOSE(" lay " << k << " residual " << residual << " error  " << error << " pull " << pull << "  "
                                                    << m_idHelperSvc->toString(muonClusters[k]->identify()));
                        }
		    }
		    
                    segTrkColl->push_back(std::move(segtrack));
                  
                } else {
                    ATH_MSG_DEBUG("3D segment fit failed");
                }
                
                // clear the calibrated clusters (segtrack has cloned them)
                for (const Muon::MuonClusterOnTrack* etaHitCalib : etaHitsCalibrated) 
                    delete etaHitCalib;
                etaHitsCalibrated.clear();

            }  // end loop on phi seeds

            // if we failed to combine the eta segment with phi measurements, 
            // just add the eta segment to the collection.
            if (!is3Dseg) {
                if (segColl)
                    segColl->push_back(std::move(etaSeg));
                else
                    segments.push_back(std::move(etaSeg));
            }

        }  // end loop on precision plane segments

        // create 3D segments for successful eta/phi matches
        std::unique_ptr<const TrackCollection> resolvedTracks(m_ambiTool->process(segTrkColl.get()));
        ATH_MSG_DEBUG("Resolved track candidates: old size " << segTrkColl->size() << " new size " << resolvedTracks->size());
        // store the resolved segments

        for (const Trk::Track* rtrk : *resolvedTracks) {
        
            // safety: see that the ambiguity resolver hasn't stripped the track of all its eta hits
            bool hasEta{false};
            for (const Trk::MeasurementBase* meas : *rtrk->measurementsOnTrack()) {
                Identifier id = m_edmHelperSvc->getIdentifier(*meas);
                if (id.is_valid() && !m_idHelperSvc->measuresPhi(id)) {
                    hasEta = true;
                    break;
                }
            }
            if (!hasEta) continue;

            std::unique_ptr<Muon::MuonSegment> seg{m_trackToSegmentTool->convert(ctx, *rtrk)};
            if (!seg) {
                ATH_MSG_VERBOSE("Segment conversion failed, no segment created. ");
            } else if (segColl) {
                segColl->push_back(std::move(seg));
            } else {
                segments.push_back(std::move(seg));  
            }
        }
    }


    //============================================================================
    std::vector<const Muon::MuonClusterOnTrack*> MuonClusterSegmentFinderTool::cleanClusters(
        std::vector<const Muon::MuonClusterOnTrack*>& muonClusters, bool selectPhiHits) const {
        
        // Keep only eta or only phi clusters
        std::vector<const Muon::MuonClusterOnTrack*> clusters;
        clusters.reserve(muonClusters.size());
        for (const Muon::MuonClusterOnTrack* cluster : muonClusters) {
            if (cluster && selectPhiHits == m_idHelperSvc->measuresPhi(cluster->identify())) {
                clusters.push_back(cluster);
            }
        }
        return clusters;
    }

   
    //============================================================================
    std::vector<std::vector<const Muon::MuonClusterOnTrack*> > MuonClusterSegmentFinderTool::classifyByLayer(
        std::vector<const Muon::MuonClusterOnTrack*>& clusters, bool useWires) const {

        // Classifies clusters by layer, starting from the layer closest to the IP and moving outwards.
        // "clusters" is expected to contain only eta (MM+sTGC strip) or only phi hits (sTGC pads XOR wires).
        // The returned vector contains only layers that have hits.
        
        // Internal logic. Initialize with 16 layers:
        // [0-3]   for the four sTGC IP layers
        // [4-11]  for the eight MM IP+HO layers (empty when phi hits are requested)
        // [12-15] for the four sTGC HO layers
        std::vector<std::vector<const Muon::MuonClusterOnTrack*> > orderedClusters(16);
        int nBad{0};
        bool hasEta{false}, hasPhi{false};

        for (const Muon::MuonClusterOnTrack* hit : clusters) {
            if (!hit) continue;
            int iorder{0};

            Identifier id = hit->identify();
            if (m_idHelperSvc->isMM(id)) {
                int     ml = m_idHelperSvc->mmIdHelper().multilayer(id);
                int  layer = m_idHelperSvc->mmIdHelper().gasGap(id);
                iorder = 4 + 4*(ml - 1) + (layer - 1); // 4 - 11
                hasEta = true;
            } else if (m_idHelperSvc->issTgc(id)) {
                (m_idHelperSvc->stgcIdHelper().measuresPhi(id) ? hasPhi : hasEta) = true;
	        int channelType = m_idHelperSvc->stgcIdHelper().channelType(id);
	        //skip sTGC pads if using wires, or skip wires if using pads
	        if ( useWires && channelType == sTgcIdHelper::Pad) continue;
	        if (!useWires && channelType == sTgcIdHelper::Wire) continue;
                int     ml = m_idHelperSvc->stgcIdHelper().multilayer(id);
                int  layer = m_idHelperSvc->stgcIdHelper().gasGap(id);
                iorder = 12*(ml - 1) + (layer - 1); // 0 - 3 (IP) and 12 - 15 (HO)    
            } else {
                ++nBad;
                continue;
            }
            orderedClusters[iorder].push_back(hit);
        }

        if (hasEta && hasPhi) ATH_MSG_WARNING("Classification by layer requested for mixed eta and phi hits!");
        if (nBad) ATH_MSG_WARNING("Unable to classify " << nBad << "clusters by their layer since they are neither MM nor sTGC");

        // Erase layers without hits
        orderedClusters.erase(
            std::remove_if(orderedClusters.begin(), orderedClusters.end(), [](std::vector<const Muon::MuonClusterOnTrack*>& vec) { return vec.empty(); }),
            orderedClusters.end()
        );
        
        return orderedClusters;          
    }


    //============================================================================
    std::vector<std::pair<Amg::Vector3D, Amg::Vector3D> > MuonClusterSegmentFinderTool::segmentSeed(
        std::vector<std::vector<const Muon::MuonClusterOnTrack*> >& orderedClusters, bool usePhi) const {
        std::vector<std::pair<Amg::Vector3D, Amg::Vector3D> > seeds;

        // oderedClusters should contain either eta clusters (MM and sTGC) 
        // or sTGC phi hits. For MM phi, use the dedicated function.

        if (orderedClusters.size() < 4) return seeds;

        // Create seeds using each pair of hits on the two most distant layers (that containing hits).
        // m_nOfSeedLayers (default = 1) dictates whether we want to also use hits from inner layers.
        
        // Loop on layers to get the first seed point
        bool hasPureMMseeds{false};
        int  seedingLayersL{0};
        for (unsigned int ilayerL{0}; (ilayerL < orderedClusters.size() && seedingLayersL < m_nOfSeedLayers); ++ilayerL) {

            bool usedLayerL{false};
            for (const Muon::MuonClusterOnTrack* hitL : orderedClusters[ilayerL]) {

                if (usePhi != m_idHelperSvc->measuresPhi(hitL->identify())) continue;
                bool isMML  = !usePhi && m_idHelperSvc->isMM(hitL->identify());
                usedLayerL  = true;
                const Amg::Vector3D& gpL = hitL->prepRawData()->globalPosition();

                // For the second point, loop on layers in reverse to be as far as possible from the first. 
                int seedingLayersR{0};
                for (unsigned int ilayerR = orderedClusters.size() - 1; (ilayerR > ilayerL && seedingLayersR < m_nOfSeedLayers); --ilayerR) {

                    bool usedLayerR{false};        
                    for (const Muon::MuonClusterOnTrack* hitR : orderedClusters[ilayerR]) {

                        if (usePhi != m_idHelperSvc->measuresPhi(hitR->identify())) continue;
                        bool isMMR  = !usePhi && m_idHelperSvc->isMM(hitR->identify());
                        usedLayerR  = true;
                        const Amg::Vector3D& gpR = hitR->prepRawData()->globalPosition();

                        if (isMML && isMMR) hasPureMMseeds = true;

                        // If the two hits are in distant panels (in z), define the seed direction 
                        // from their difference, otherwise define it from the IP and the hit mean position. 
                        if (std::abs(gpR.z() - gpL.z()) >= 200.) {
                          seeds.emplace_back(gpL, gpR - gpL);
                        } else {
                          seeds.emplace_back(gpL, 0.5*(gpR + gpL));
                        }
                    }
                    if (usedLayerR) ++seedingLayersR;
                }
            }
            if (usedLayerL) ++seedingLayersL;
        }

        // For the case of phi seeds, there is nothing more to do.
        // For the case of eta seeds, collect seeds from the outmost MM layers
        // in addition to the above seeds (in the unlikely case such seeds
        // are already included, just exit). 

        if (usePhi || hasPureMMseeds) return seeds;

        // use the innermost and outermost MM hits to form the seeds
        unsigned int mmLayerL{999}, mmLayerR{999};
        for (unsigned int il = 0; il < orderedClusters.size(); ++il) {
            if (orderedClusters[il].empty()) continue;
            if (!m_idHelperSvc->isMM(orderedClusters[il].front()->identify())) continue;
            mmLayerL = il;
            break;
        }

        for (unsigned int il = orderedClusters.size() - 1; il > mmLayerL; --il) {
            if (orderedClusters[il].empty()) continue;
            if (!m_idHelperSvc->isMM(orderedClusters[il].front()->identify())) continue;
            mmLayerR = il;
            break;
        }

        if (mmLayerL == 999 || mmLayerR == 999) return seeds;

        // create seeds from the selected layers
        for (const Muon::MuonClusterOnTrack* hitL : orderedClusters[mmLayerL]) {
            const Amg::Vector3D& gpL = hitL->prepRawData()->globalPosition();
            for (const Muon::MuonClusterOnTrack* hitR : orderedClusters[mmLayerR]) {
                const Amg::Vector3D& gpR = hitR->prepRawData()->globalPosition();
       
                if (std::abs(gpR.z() - gpL.z()) >= 200.) {
                    seeds.emplace_back(gpL, gpR - gpL);
                } else {
                    seeds.emplace_back(gpL, 0.5*(gpR + gpL));
                }
            }
        }

        return seeds;
    }


    //============================================================================
    int MuonClusterSegmentFinderTool::getClustersOnSegment(std::vector<std::vector<const Muon::MuonClusterOnTrack*> >& orderedclusters, 
                                                           std::pair<Amg::Vector3D, Amg::Vector3D>& seed,
                                                           std::vector<const Muon::MuonClusterOnTrack*>& clusOnSeg) const {

        ATH_MSG_VERBOSE(" getClustersOnSegment: layers " << orderedclusters.size());
        int layer{0};
        int nHitsAdded{0};

        for (std::vector<const Muon::MuonClusterOnTrack*>& surfHits : orderedclusters) {
            const  Muon::MuonClusterOnTrack* layerBestHit{nullptr};
            double minDist{9999.};

            // get the best hit candidate on this layer
            for (const Muon::MuonClusterOnTrack* hit : surfHits) {

                double dist  = clusterDistanceToSeed(hit, seed);
                if (dist < 0) continue;

                double error = Amg::error(hit->localCovariance(), Trk::locX);
                if (m_idHelperSvc->isMM(hit->identify()) && m_idHelperSvc->mmIdHelper().isStereo(hit->identify())) error = 15;

                ATH_MSG_VERBOSE(" lay " << layer << " dist " << dist << " pull " << dist / error << " cut " << m_maxClustDist << "  " << m_idHelperSvc->toString(hit->identify()));
                if (dist < minDist && std::abs(dist / error) < m_maxClustDist) {
                    minDist = dist;
                    layerBestHit = hit;
                }
            }

            if (layerBestHit) {
                ATH_MSG_VERBOSE(" adding  " << minDist << "  " << m_idHelperSvc->toString(layerBestHit->identify()));
                clusOnSeg.push_back(layerBestHit);
                ++nHitsAdded;
            }
            ++layer;
        }
        ATH_MSG_VERBOSE(" getClustersOnSegment: returning "<<nHitsAdded<<" hits ");
        return nHitsAdded;
    }

    //============================================================================
    double MuonClusterSegmentFinderTool::clusterDistanceToSeed(const Muon::MuonClusterOnTrack* clus, std::pair<Amg::Vector3D, Amg::Vector3D>& seed) const {

        // Get the seed intersection with the surface in local plane coordiantes
        const Trk::Surface&    surf = clus->associatedSurface();
        Trk::Intersection intersect = surf.straightLineIntersection(seed.first, seed.second, false, false);

        Amg::Vector2D lpos_seed;
        surf.globalToLocal(intersect.position, intersect.position, lpos_seed);

        // case it's an sTgc pad: just check that the seed is on the hit pad
        Identifier hitID = clus->identify();
        if (m_idHelperSvc->issTgc(hitID) && m_idHelperSvc->stgcIdHelper().channelType(hitID) == sTgcIdHelper::Pad) {
            const sTgcPrepData* prd = dynamic_cast<const sTgcPrepData*>(clus->prepRawData());
            if (!prd) {
                ATH_MSG_DEBUG(" clusterDistanceToSeed: no sTgcPrepData found ");
                return -1;
            }

            const MuonGM::MuonPadDesign* design = prd->detectorElement()->getPadDesign(hitID);
            if (!design) {
                ATH_MSG_WARNING("MuonPadDesign not found for " << m_idHelperSvc->toString(hitID));
                return -1;
            }

            double halfPadWidthX = 0.5*design->channelWidth(prd->localPosition(), true);  // X = phi direction for pads
            double halfPadWidthY = 0.5*design->channelWidth(prd->localPosition(), false); // Y = eta direction for pads
            const Amg::Vector2D diff = prd->localPosition() - lpos_seed;

            if (std::abs(diff.x()) < halfPadWidthX && std::abs(diff.y()) < halfPadWidthY) return 0;

            return -1;
        }
            
        return std::abs(clus->localParameters()[Trk::locX] - lpos_seed[Trk::locX]);
    }


    //============================================================================
    std::unique_ptr<Trk::Track> MuonClusterSegmentFinderTool::fit(const EventContext& ctx, 
                                                                  const std::vector<const Trk::MeasurementBase*>& vecFitPts,
                                                                  const Trk::TrackParameters& startpar) const {

        std::unique_ptr<Trk::Track> segtrack = m_slTrackFitter->fit(ctx, vecFitPts, startpar, false, Trk::nonInteracting);
        if (segtrack) {
            ATH_MSG_VERBOSE("segment fit succeeded");
            std::unique_ptr<Trk::Track> cleanedTrack = m_trackCleaner->clean(*segtrack, ctx);
            if (cleanedTrack && cleanedTrack->perigeeParameters() != segtrack->perigeeParameters()) {
                segtrack.swap(cleanedTrack);
            }

            if (!m_edmHelperSvc->goodTrack(*segtrack, 10)) {
                if (segtrack->fitQuality()) {
                    ATH_MSG_DEBUG("segment fit with chi^2/nDoF = " << segtrack->fitQuality()->chiSquared() << "/"
                                                                   << segtrack->fitQuality()->numberDoF());
                }
                return nullptr;

            } else {
                m_trackSummary->updateTrack(*segtrack);
            }
        }
        return segtrack;
    }


    //============================================================================
    std::vector<std::pair<Amg::Vector3D, Amg::Vector3D> > MuonClusterSegmentFinderTool::segmentSeedFromPads(
                                                     std::vector<std::vector<const Muon::MuonClusterOnTrack*> >& orderedClusters, 
                                                     const Muon::MuonSegment* etaSeg) const {

        std::vector<std::pair<Amg::Vector3D, Amg::Vector3D> > seeds;
        std::vector<std::vector<const Muon::MuonClusterOnTrack*> > sTgcIP(4); // IP: layers nearest to the IP will be added first
        std::vector<std::vector<const Muon::MuonClusterOnTrack*> > sTgcHO(4); // HO: layers furthest from the IP will be added first
 
        // Process clusters separately for each multilayer
        for (int iml{1}; iml <= 2; ++iml) {
            
            int il     = (iml == 1) ? 0 : orderedClusters.size()-1;
            int iend   = (iml == 1) ? orderedClusters.size() : -1;
            int idir   = (iml == 1) ? 1 : -1;
            int nLayersWithHitMatch{0};

            // Loop on layers (reverse loop for HO)
            for (; il != iend; il += idir) {

                double lastDistance{1000.};
                std::vector<const Muon::MuonClusterOnTrack*>& matchedHits = (iml == 1) ? sTgcIP[nLayersWithHitMatch] : sTgcHO[nLayersWithHitMatch];
                                      
                // Loop on the hits on this layer. Find the one closest (in eta) to the segment intersection.
                for (const Muon::MuonClusterOnTrack* padHit : orderedClusters[il]) {

                    // check the multilayer the hit is on
                    if (m_idHelperSvc->stgcIdHelper().multilayer(padHit->identify()) != iml) continue;

                    const sTgcPrepData* prd = dynamic_cast<const sTgcPrepData*>(padHit->prepRawData());
                    if (!prd) continue;

                    const MuonGM::MuonPadDesign* design = prd->detectorElement()->getPadDesign(padHit->identify());
                    if (!design) continue;
                
                    // local position of the segment intersection with the plane
                    const Trk::Surface& surf = padHit->associatedSurface();
                    Trk::Intersection intersect = surf.straightLineIntersection(etaSeg->globalPosition(), etaSeg->globalDirection(), false, false);
                    Amg::Vector2D segLocPosOnSurf;
                    surf.globalToLocal(intersect.position, intersect.position, segLocPosOnSurf);
                                              
                    // eta distance between the hit and the segment intersection with the plane
                    // check that it's no more than half of the pad eta-pitch.
                    double chWidth = design->channelWidth(prd->localPosition(), false);
                    double etaDistance = std::abs(prd->localPosition().y() - segLocPosOnSurf[1]);
                    if (etaDistance > 0.5*chWidth) continue;
                    ATH_MSG_DEBUG(" etaDistance "<<etaDistance<<" between pad center and position on the pad.");
                    
                    if (matchedHits.empty()) {
                        // first hit
                        matchedHits.push_back(padHit);
                        ATH_MSG_DEBUG(" best etaDistance: " << etaDistance);
                    } else if (std::abs(etaDistance - lastDistance) < 0.001) {
                        // competing hit pad, keep both (all hit pads of the same eta row will be candidates)
                        matchedHits.push_back(padHit);
                        ATH_MSG_DEBUG(" added etaDistance: " << etaDistance << " size " << matchedHits.size());
                    } else if (etaDistance < lastDistance) {
                        // found a better hit; clear the old ones (possible only for clustered pad hits)
                        matchedHits.clear();
                        matchedHits.push_back(padHit);
                        ATH_MSG_DEBUG(" replacing best etaDistance with: " << etaDistance);                    
                    } else {
                        continue;
                    }

                    lastDistance = etaDistance;
                } // end of loop on hits

                if (!matchedHits.empty()) ++nLayersWithHitMatch;

            } // end of loop on layers
            
            // need at least one hit in each multilayer to create a seed
            if (!nLayersWithHitMatch) return seeds;

        } // end of loop on multilayers

        // get refined phi ranges on each ml, by taking into account pad staggering
        std::vector<std::pair<double, double> > sTgcIP_phiRanges = getPadPhiOverlap(sTgcIP);
        std::vector<std::pair<double, double> > sTgcHO_phiRanges = getPadPhiOverlap(sTgcHO);

        // reference prds on the outermost hit surfaces
        const sTgcPrepData* prdL1 = dynamic_cast<const sTgcPrepData*>(sTgcIP.front().front()->prepRawData());
        const sTgcPrepData* prdL2 = dynamic_cast<const sTgcPrepData*>(sTgcHO.front().front()->prepRawData());

        // create a seed for each combination of IP and HO points
        std::vector<Amg::Vector3D> seedPointIP, seedPointHO;
        for (std::pair<double, double>& range1 : sTgcIP_phiRanges) {
            double midPhi1 = 0.5 * (range1.first + range1.second);
            Amg::Vector2D lp1(midPhi1, prdL1->localPosition().y());
            Amg::Vector3D gpL1;
            prdL1->detectorElement()->surface().localToGlobal(lp1, gpL1, gpL1);

            for (std::pair<double, double>& range2 : sTgcHO_phiRanges) {
                double midPhi2 = 0.5 * (range2.first + range2.second);
                Amg::Vector2D lp2(midPhi2, prdL2->localPosition().y());
                Amg::Vector3D gpL2;
                prdL2->detectorElement()->surface().localToGlobal(lp2, gpL2, gpL2);

                // create the seed taking the average position (w.r.t. IP)
                // as global direction (as for an infinite momentum track).
                Amg::Vector3D gDir = (gpL2 + gpL1).unit();
                seeds.emplace_back(gpL1, gDir);
            }
        }

        ATH_MSG_DEBUG(" segmentSeedFromPads: seeds.size() " << seeds.size());
        return seeds;
    }


    //============================================================================
    std::vector<std::pair<Amg::Vector3D, Amg::Vector3D> >
    MuonClusterSegmentFinderTool::segmentSeedFromMM( std::vector< Amg::Vector3D >& phiStereo ) const {
      
      std::vector<std::pair<Amg::Vector3D,Amg::Vector3D> > seeds;
      if (phiStereo.size() < 2) return seeds;
      
      double dz = 0;
      double dz_tmp = 0;
      
      std::vector<std::pair<unsigned int, unsigned int> > good_indices;
      for( unsigned int ipoint = 0; ipoint < phiStereo.size(); ipoint++ ){
        for( unsigned int jpoint = ipoint+1; jpoint < phiStereo.size(); jpoint++ ){
	  dz_tmp = std::abs(phiStereo[jpoint].z()-phiStereo[ipoint].z());
	  if ( dz_tmp > dz ) {
	    dz = dz_tmp;
            good_indices.clear();
            good_indices.emplace_back(ipoint, jpoint);
          } else if ( dz_tmp == dz ) {
	    good_indices.emplace_back(ipoint, jpoint);
          }
        }//end jpoint                                                                                                                   
      }//end ipoint

      for ( unsigned int i = 0; i < good_indices.size(); i++ ) {
	const Amg::Vector3D segdir = phiStereo[good_indices[i].second] - phiStereo[good_indices[i].first];
	seeds.emplace_back(phiStereo[good_indices[i].first], segdir);
      }

      return seeds;
    }


    //============================================================================
    void MuonClusterSegmentFinderTool::segmentSeedPhiOR(std::vector<std::pair<Amg::Vector3D, Amg::Vector3D> >& seeds) const {
      
        std::vector<std::pair<Amg::Vector3D, Amg::Vector3D> > uniqueSeeds;
        
        for (auto it = seeds.begin(); it != seeds.end(); ++it) {
            double phi1 = it->first.phi();
	    double phi2 = (it->first + it->second).phi();
                               
            if (std::find_if( std::next(it), seeds.end(), 
                [&phi1, &phi2](std::pair<Amg::Vector3D, Amg::Vector3D>& s) {
                    double dphi1 = std::abs(xAOD::P4Helpers::deltaPhi(phi1, s.first.phi()));
                    double dphi2 = std::abs(xAOD::P4Helpers::deltaPhi(phi2, (s.first + s.second).phi()));
                    return ( dphi1 < 0.05 && dphi2 < 0.05);}) == seeds.end()) {
      
                uniqueSeeds.push_back(*it);
            }  
        }

        if (uniqueSeeds.size() != seeds.size()) seeds.assign(uniqueSeeds.begin(), uniqueSeeds.end());                
    }       

    

    //============================================================================
    std::vector<std::pair<double, double> > MuonClusterSegmentFinderTool::getPadPhiOverlap(
        std::vector<std::vector<const Muon::MuonClusterOnTrack*> >& pads) const {

        // 'pads' contains segment hit candidates, classified in four layers (IP or HO). 
        // Layers are ordered; for IP, the layer with hits that is nearest to 
        // the IP is first, while for HO, the one furthest from the IP is first.

        std::vector<std::vector<double> > padsPhiL, padsPhiR;
        std::vector<double> padsPhiC;

        // Loop on layers
        for (std::vector<const Muon::MuonClusterOnTrack*>& surfHits : pads) {

            // Loop on layer hits
            std::vector<double> surfPadsPhiL, surfPadsPhiR;
            for (const Muon::MuonClusterOnTrack* hit : surfHits) {

                Identifier id = hit->identify();

                const sTgcPrepData* prd = dynamic_cast<const sTgcPrepData*>(hit->prepRawData());
                if (!prd) {
                    ATH_MSG_WARNING("No prd available for " << m_idHelperSvc->toString(id));
                    continue;
                }

                const MuonGM::MuonPadDesign* design = prd->detectorElement()->getPadDesign(id);
                if (!design) {
                    ATH_MSG_WARNING("No design available for " << m_idHelperSvc->toString(id));
                    continue;
                }

                // Phi boundaries of this pad in local coordinates
                double halfWidthX = 0.5 * design->channelWidth(prd->localPosition(), true);
                double hitPadX    = prd->localPosition().x(); // x is in the phi direction

                // Reject hit candidates on pads too close (in phi) to any pad kept so far 
                // (pad fuzziness) to constrain the number of combinations.
                bool samePhi{false};
                for (double& prevPadPhi : padsPhiC) {              
                    if (std::abs( hitPadX - prevPadPhi ) < 0.9*halfWidthX ) {
                        samePhi = true;
                        break;
                    }
                }
                if (samePhi) continue;

                // Store the new pad candidate
                surfPadsPhiL.push_back(hitPadX - halfWidthX);
                surfPadsPhiR.push_back(hitPadX + halfWidthX);
                padsPhiC.push_back(hitPadX);
                ATH_MSG_DEBUG(" keep pad id " << m_idHelperSvc->toString(id) << " local x: " << hitPadX << " width: " << halfWidthX);
            }

            padsPhiL.push_back(surfPadsPhiL);
            padsPhiR.push_back(surfPadsPhiR);
        }

        unsigned int nSurf = padsPhiR.size();
        
        // number of combinations we can make out of pads in different layers
        // we want to keep combinations of overlapping pads.
        unsigned int nCombos{1};
        std::for_each(padsPhiR.begin(), padsPhiR.end(), 
                      [&nCombos](std::vector<double>& surfPadsPhiR) { 
                          if (!surfPadsPhiR.empty()) nCombos *= surfPadsPhiR.size(); 
                      });
     
        std::vector<std::pair<double, double> > phiOverlap;
        phiOverlap.reserve(nCombos);
     
        if (nCombos <= 100) {
            unsigned int N{nCombos};
            for (unsigned int isurf{0}; isurf < nSurf; ++isurf) {
                if (padsPhiR[isurf].empty()) continue;
                unsigned int nSurfHits = padsPhiR[isurf].size();
                N /= nSurfHits;

                for (unsigned int icombo{0}; icombo < nCombos; ++icombo) {
                    // index of the pad that corresponds to this combination
                    unsigned int padIdx = (icombo / N) % nSurfHits; 
                    if (isurf == 0) { 
                        // first surface: just add the range of each hit pad
                        phiOverlap.emplace_back(padsPhiL[isurf][padIdx], padsPhiR[isurf][padIdx]);
                    } else {
                        // subsequent surfaces: use staggering to narrow the phi ranges
                        if (padsPhiL[isurf][padIdx] > phiOverlap[icombo].first)  phiOverlap[icombo].first  = padsPhiL[isurf][padIdx];
                        if (padsPhiR[isurf][padIdx] < phiOverlap[icombo].second) phiOverlap[icombo].second = padsPhiR[isurf][padIdx];
                    }
                }
            }
                
            // delete bad combinations with xmin > xmax (indicates non overlapping pads) 
            phiOverlap.erase(
                std::remove_if(phiOverlap.begin(), phiOverlap.end(), [](std::pair<double, double>& range) { return range.first >= range.second; }),
                phiOverlap.end()
            );
            ATH_MSG_DEBUG("Pad seeding - #combinations initial: "<<nCombos<<", after cleaning for non overlapping pads: "<<phiOverlap.size());

        } else {
            // in case combinations are too many, store the phi ranges of individual pads
            for (unsigned int isurf{0}; isurf < nSurf; ++isurf) {
                unsigned int nSurfHits = padsPhiR[isurf].size();
                for (unsigned int ihit{0}; ihit < nSurfHits; ++ihit) {
                    phiOverlap.emplace_back(padsPhiL[isurf][ihit], padsPhiR[isurf][ihit]);
                }
            }
            ATH_MSG_DEBUG("Pad seeding - #combinations: "<< nCombos << " is too large. Seeding from" << phiOverlap.size() << " individual pads.");
        }

        return phiOverlap;
    }


    //============================================================================
    std::vector<const Muon::MuonClusterOnTrack*> MuonClusterSegmentFinderTool::getCalibratedClusters(
        std::vector<const Muon::MuonClusterOnTrack*>& clusters, std::pair<Amg::Vector3D, Amg::Vector3D>& seed) const {
        std::vector<const Muon::MuonClusterOnTrack*> calibratedClusters;

        // loop on the segment clusters and use the phi of the seed to correct them
        for (const Muon::MuonClusterOnTrack* clus : clusters) {

            const Muon::MuonClusterOnTrack* newClus{nullptr};

            // get the intercept of the seed direction with the cluster surface
            Identifier hitID = clus->identify();
            const Trk::Surface& surf = clus->associatedSurface();
            Trk::Intersection intersect = surf.straightLineIntersection(seed.first, seed.second, false, false);

            if (m_idHelperSvc->isMM(hitID)) {
                // build a new MMClusterOnTrack with correct position
                newClus = m_mmClusterCreator->calibratedCluster(*(clus->prepRawData()), intersect.position);

                if (msgLvl(MSG::VERBOSE)) {
                    const Amg::Vector3D& gpos_hit = clus->globalPosition();
                    const Amg::Vector3D& gpos_new = newClus->globalPosition();
                    ATH_MSG_VERBOSE("Position before correction: " << gpos_hit.x() << " " << gpos_hit.y() << " " << gpos_hit.z());
                    ATH_MSG_VERBOSE("Position after correction: "  << gpos_new.x() << " " << gpos_new.y() << " " << gpos_new.z());
                }

            } else if (m_idHelperSvc->issTgc(clus->identify())) {
                // calibration to be added for sTGCs
                newClus = m_stgcClusterCreator->createRIO_OnTrack(*(clus->prepRawData()), intersect.position);
            }

            calibratedClusters.push_back(newClus);
        }

        return calibratedClusters;
    }


    //============================================================================
    std::vector<Amg::Vector3D>
    MuonClusterSegmentFinderTool::getPhiFromStereo( std::vector<const Muon::MuonClusterOnTrack*>& clusters  ) const
    {
  
      //vector of 3D points of all phi obtained from the stereo-stereo combination                                                                              
      std::vector<Amg::Vector3D> phiStereo;
      
      /// loop on the clusters and look for the stereo                                                                                                       
      for ( unsigned int iclu1 = 0; iclu1 < clusters.size(); ++iclu1 ) {
        Identifier id1 = clusters[iclu1]->identify();
        if ( !m_idHelperSvc->isMM(id1) ) continue;
        if ( !m_idHelperSvc->mmIdHelper().isStereo(id1) ) continue;
	int lay1 = m_idHelperSvc->mmIdHelper().gasGap(id1);
	int ml1  = m_idHelperSvc->mmIdHelper().multilayer(id1);

	const MMPrepData* prd1 = dynamic_cast<const MMPrepData*>(clusters[iclu1]->prepRawData());
	Trk::LocalParameters loc1(prd1->localPosition());
  	Amg::Vector3D gPos1 = clusters[iclu1]->associatedSurface().localToGlobal(loc1);
  	  	  
  	// get the intersections of the stereo clusters with the other layers of the same multilayer                                                       
	for ( unsigned int iclu2 = iclu1+1; iclu2 < clusters.size(); ++iclu2 ) {
  	  Identifier id2 = clusters[iclu2]->identify();
	  if ( !m_idHelperSvc->isMM(id2) ) continue;
	  if ( !m_idHelperSvc->mmIdHelper().isStereo(id2) ) continue; //remove this if you want to use eta+stereo or stereo+stereo. Now use only stereo+stereo
	  int lay2 = m_idHelperSvc->mmIdHelper().gasGap(id2);
	  int ml2  = m_idHelperSvc->mmIdHelper().multilayer(id2);

	  /// skip clusters if not in the same multilayer and if on the same layer                                                                         
	  if ( ml1 != ml2 || lay1 == lay2 ) continue;

	  const MMPrepData* prd2 = dynamic_cast<const MMPrepData*>(clusters[iclu2]->prepRawData());
  	  Trk::LocalParameters loc2(prd2->localPosition());
  	  Amg::Vector3D gPos2 = clusters[iclu2]->associatedSurface().localToGlobal(loc2);
  
  	  Amg::Vector3D gPos;
	  bool spacePoint = prd1->detectorElement()->spacePointPosition(id1,id2,gPos1,gPos2,gPos);
  	  if ( spacePoint ) phiStereo.push_back(gPos);
  	  else ATH_MSG_VERBOSE("Could not build phi measurement from MM stereo");
  	}
      }
  
      return phiStereo;

    }
  
}  // namespace Muon
