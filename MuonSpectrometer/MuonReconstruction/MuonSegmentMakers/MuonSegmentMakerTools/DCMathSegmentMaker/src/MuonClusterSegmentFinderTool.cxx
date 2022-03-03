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

    MuonClusterSegmentFinderTool::MuonClusterSegmentFinderTool(const std::string& type, const std::string& name, const IInterface* parent) :
        AthAlgTool(type, name, parent) {
        declareInterface<IMuonClusterSegmentFinderTool>(this);
        //
    }

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

    MuonClusterSegmentFinderTool::~MuonClusterSegmentFinderTool() = default;

    void MuonClusterSegmentFinderTool::find(const EventContext& ctx, std::vector<const Muon::MuonClusterOnTrack*>& muonClusters,
                                            std::vector<std::unique_ptr<Muon::MuonSegment>>& segments, Trk::SegmentCollection* segColl) const {
        ATH_MSG_DEBUG("Entering MuonClusterSegmentFinderTool with " << muonClusters.size() << " clusters to be fit");
        if (belowThreshold(muonClusters, 4)) return;
        std::vector<std::unique_ptr<Muon::MuonSegment>> etaSegs;
        findPrecisionSegments(ctx, muonClusters, etaSegs);
        if (etaSegs.empty()) { return; }
        find3DSegments(ctx, muonClusters, etaSegs, segments, segColl);
    }
    // find the precision (eta) segments
    void MuonClusterSegmentFinderTool::findPrecisionSegments(const EventContext& ctx, std::vector<const Muon::MuonClusterOnTrack*>& muonClusters,
                                                             std::vector<std::unique_ptr<Muon::MuonSegment>>& etaSegs) const {
        // clean the muon clusters -- select only the eta hits
        bool selectPhiHits(false);
        std::vector<const Muon::MuonClusterOnTrack*> clusters = cleanClusters(muonClusters, selectPhiHits);
        ATH_MSG_VERBOSE("After hit cleaning, there are " << clusters.size() << " 2D clusters to be fit");
        if (belowThreshold(clusters, 4)) { return; }

        std::unique_ptr<TrackCollection> segTrkColl = std::make_unique<TrackCollection>();
        // std::vector< const Muon::MuonClusterOnTrack* > clusters = muonClusters;
        // order the muon clusters by layer
        std::vector<std::vector<const Muon::MuonClusterOnTrack*> > orderedClusters = orderByLayer(clusters, false);

        // create segment seeds
        std::vector<std::pair<Amg::Vector3D, Amg::Vector3D> > seeds = segmentSeed(orderedClusters, false);
        ATH_MSG_VERBOSE("Found " << seeds.size() << " 2D seeds to be fit");

        std::vector<const Muon::MuonClusterOnTrack*> rioVecPrevious;
        // find all clusters near the seed and try to fit
        for (unsigned int i = 0; i < seeds.size(); ++i) {
            std::vector<const Muon::MuonClusterOnTrack*> rioVec = getClustersOnSegment(orderedClusters, seeds[i]);
            //  make consistent cut
            if (belowThreshold(rioVec, 4)) continue;
            // logic to reduce combinatorics
            if (rioVec.size() == rioVecPrevious.size()) {
                bool sameContent = true;
                for (unsigned int k = 0; k < rioVec.size(); ++k) {
                    if (rioVec[k] != rioVecPrevious[k]) sameContent = false;
                }
                if (sameContent) {
                    ATH_MSG_VERBOSE(" same clusters found for 2D seed " << i);
                    continue;
                }
            }
            ATH_MSG_VERBOSE(" new clusters for 2D seed " << i);
            rioVecPrevious.clear();
            int nMM = 0;
            for (unsigned int k = 0; k < rioVec.size(); ++k) {
                rioVecPrevious.push_back(rioVec[k]);
                if (m_idHelperSvc->isMM(rioVec[k]->identify())) nMM++;
            }

            // fit the segment!!
            std::vector<const Trk::MeasurementBase*> vec2;
            if (m_ipConstraint)
                vec2.reserve(rioVec.size() + 3);
            else
                vec2.reserve(rioVec.size() + 2);

            // pseudo measurement for vtx
            std::unique_ptr<Trk::PseudoMeasurementOnTrack> pseudoVtx;
            if (m_ipConstraint) {
                double errVtx = 100.;
                Amg::MatrixX covVtx(1, 1);
                covVtx(0, 0) = errVtx * errVtx;
                Amg::Vector3D vtx(0., 0., 0.);
                Trk::PerigeeSurface perVtx(vtx);
                pseudoVtx = std::make_unique<Trk::PseudoMeasurementOnTrack>(Trk::LocalParameters(Trk::DefinedParameter(0, Trk::locX)), covVtx, perVtx);
                vec2.push_back(pseudoVtx.get());
            }

            // create a pseudo phi hit
            double errPos = 0.1;
            if (nMM > 0) errPos = 1000.;
            Amg::MatrixX cov(1, 1);
            cov(0, 0) = errPos * errPos;
            std::unique_ptr<Trk::PseudoMeasurementOnTrack> pseudoSegPhi1 = std::make_unique<Trk::PseudoMeasurementOnTrack>(
                Trk::LocalParameters(Trk::DefinedParameter(0, Trk::locY)), cov, rioVec.front()->associatedSurface());
            std::unique_ptr<Trk::PseudoMeasurementOnTrack> pseudoSegPhi2 = std::make_unique<Trk::PseudoMeasurementOnTrack>(
                Trk::LocalParameters(Trk::DefinedParameter(0, Trk::locY)), cov, rioVec.back()->associatedSurface());
            vec2.push_back(pseudoSegPhi1.get());
            std::copy(rioVec.begin(), rioVec.end(), std::back_inserter(vec2));
            vec2.push_back(pseudoSegPhi2.get());
            ATH_MSG_VERBOSE("fitting 2D segment track with " << vec2.size() << " hits");
            for (unsigned int k = 0; k < rioVec.size(); ++k) {
                ATH_MSG_VERBOSE(m_idHelperSvc->toString(rioVec[k]->identify())
                                << " " << rioVec[k]->globalPosition().perp() << ", " << rioVec[k]->globalPosition().z() << "  "
                                << m_idHelperSvc->measuresPhi(rioVec[k]->identify()));
            }

            std::unique_ptr<Trk::TrackParameters> startpar;

            const Trk::PlaneSurface* surf = dynamic_cast<const Trk::PlaneSurface*>(&rioVec.front()->associatedSurface());
            if (surf) {
                Amg::Vector3D posOnSurf = intersectPlane(*surf, seeds[i].first, seeds[i].second);
                Amg::Vector2D lpos;
                surf->globalToLocal(posOnSurf, posOnSurf, lpos);
                Trk::LocalDirection ldir;
                surf->globalToLocalDirection(seeds[i].second, ldir);
                Amg::Vector2D lp(lpos[Trk::locX], 0.);
                Trk::LocalDirection ld(ldir.angleXZ(), std::asin(1.));
                Amg::Vector3D gd;
                surf->localToGlobalDirection(ld, gd);
                Amg::Vector3D gp;
                surf->localToGlobal(lp, gp, gp);

                Amg::Vector3D perpos = gp + -10 * gd.unit();
                if (perpos.dot(gd) < 0) gd = -1 * gd;
                startpar = std::make_unique<Trk::Perigee>(perpos, gd, 0, perpos);
                ATH_MSG_VERBOSE(" start parameter " << perpos << " pp " << startpar->position() << " gd " << gd.unit() << " pp "
                                                    << startpar->momentum().unit());
            }
            // old method as fall back
            if (!startpar) {
                double phi = seeds[i].second.phi();
                double theta = seeds[i].second.theta();
                double qoverp = 0;
                Trk::PerigeeSurface persurf(vec2.front()->globalPosition());
                startpar = std::make_unique<Trk::Perigee>(0, 0, phi, theta, qoverp, persurf);
            }
            std::unique_ptr<Trk::Track> segtrack = fit(ctx, vec2, *startpar);
            if (segtrack) {
                m_trackSummary->updateTrack(*segtrack);
                // Peter NEW calculate distance of segment to all muon clusters
                if (msgLvl(MSG::VERBOSE)) {
                    Amg::Vector3D pos = segtrack->perigeeParameters()->position();
                    Amg::Vector3D dir = segtrack->perigeeParameters()->momentum();
                    dir = dir / dir.mag();
                    ATH_MSG_VERBOSE(" 2D eta segment found position " << pos << " direction " << dir << " hits " << vec2.size());
                    for (unsigned int k = 0; k < muonClusters.size(); ++k) {
                        const Trk::PlaneSurface* surf = dynamic_cast<const Trk::PlaneSurface*>(&(muonClusters[k]->associatedSurface()));
                        if (surf) {
                            Amg::Vector3D posOnSurf = intersectPlane(*surf, pos, dir);
                            Amg::Vector2D lpos;
                            surf->globalToLocal(posOnSurf, posOnSurf, lpos);
                            double residual = muonClusters[k]->localParameters()[Trk::locX] - lpos[0];
                            double error = Amg::error(muonClusters[k]->localCovariance(), Trk::locX);
                            double pull = residual / error;
                            ATH_MSG_VERBOSE(" lay " << k << " residual " << residual << " error  " << error << " pull " << pull << "  "
                                                    << m_idHelperSvc->toString(muonClusters[k]->identify()));
                        }
                    }
                }
                segTrkColl->push_back(std::move(segtrack));


            } else
                ATH_MSG_DEBUG(" 2D segment fit failed");
            // delete the pseudo hits
        }

        if (msgLvl(MSG::DEBUG)) {
            ATH_MSG_DEBUG("Tracks before ambi solving ");

            for (const Trk::Track* trk : *segTrkColl) {
                ATH_MSG_DEBUG(m_printer->print(*trk));
                const DataVector<const Trk::MeasurementBase>* measu = trk->measurementsOnTrack();
                if (measu) ATH_MSG_DEBUG(m_printer->print(measu->stdcont()));
            }
        }

        if (segTrkColl->empty()) { return; }

        std::unique_ptr<const TrackCollection> resolvedTracks(m_ambiTool->process(segTrkColl.get()));
        ATH_MSG_DEBUG("Resolved track candidates: old size " << segTrkColl->size() << " new size " << resolvedTracks->size());
        // store the resolved segments

        for (const Trk::Track* trk : *resolvedTracks) {
            MuonSegment* seg = m_trackToSegmentTool->convert(ctx, *trk);
            if (!seg) {
                ATH_MSG_VERBOSE("Segment conversion failed, no segment created. ");
            } else {
                ATH_MSG_DEBUG(" adding " << m_printer->print(*seg) << std::endl << m_printer->print(seg->containedMeasurements()));
                etaSegs.emplace_back(seg);
            }
        }
    }

    void MuonClusterSegmentFinderTool::find3DSegments(const EventContext& ctx, std::vector<const Muon::MuonClusterOnTrack*>& muonClusters,
                                                      std::vector<std::unique_ptr<Muon::MuonSegment>>& etaSegs, std::vector<std::unique_ptr<Muon::MuonSegment>>& segments,
                                                      Trk::SegmentCollection* segColl) const {
        bool selectPhiHits(true);
        std::vector<const Muon::MuonClusterOnTrack*> clusters = cleanClusters(muonClusters, selectPhiHits);
        std::vector<const Muon::MuonClusterOnTrack*> etaClusters = cleanClusters(muonClusters, false);
        ATH_MSG_DEBUG("After hit cleaning, there are " << clusters.size() << " 3D clusters to be fit");
        if (belowThreshold(clusters, 4)) {
            ATH_MSG_DEBUG("Not enough phi hits present, cannot perform the fit!");

            for (std::unique_ptr<Muon::MuonSegment>& vsit : etaSegs) {
                if (segColl) {
                    segColl->push_back(std::move(vsit));
                } else {
                    segments.push_back(std::move(vsit));
                }
            }

            etaSegs.clear();
            return;
        }
        std::unique_ptr<TrackCollection> segTrkColl = std::make_unique<TrackCollection>(SG::OWN_ELEMENTS);

	//definition of the vector of seeds
	std::vector<std::pair<Amg::Vector3D,Amg::Vector3D> > seeds_preOR;
	std::vector<std::pair<Amg::Vector3D,Amg::Vector3D> > seeds;
	std::vector<std::pair<Amg::Vector3D,Amg::Vector3D> > seedsMM;
	std::vector<std::pair<Amg::Vector3D,Amg::Vector3D> > seedsWires;
	std::vector<std::pair<Amg::Vector3D,Amg::Vector3D> > seedsPads;

	std::vector<Amg::Vector3D> mmStereoMeasurements = getPhiFromStereo(etaClusters);
	seedsMM = segmentSeedFromMM(mmStereoMeasurements);

	if(seedsMM.empty()) ATH_MSG_DEBUG( "No MicroMegas seeds found" );

        // order the clusters by layer
        bool useWires(true);
        std::vector<std::vector<const Muon::MuonClusterOnTrack*> > orderedClusters = orderByLayer(clusters, useWires);
        std::vector<std::vector<const Muon::MuonClusterOnTrack*> > orderedEtaClusters = orderByLayer(etaClusters, false);
	seedsWires = segmentSeed(orderedClusters,useWires);

        // loop on the seeds and combine with the eta segments
        for (std::unique_ptr<Muon::MuonSegment> & sit :  etaSegs) {
            bool is3Dseg(false);

	    seeds_preOR.clear();

	    //If too many MM seeds are created, we dont use any of them to avoid memory problems. 
	    //This case can occur when there are N clusters on the first stereo layer and 
	    //M clusters on the second stereo layer, resulting in NxM combinations (i.e. NxM seeds).
	    //the cut at 100 seeds needs to be adjusted
	    if (seedsMM.size() < 100)
	      seeds_preOR.insert(seeds_preOR.end(), seedsMM.begin(), seedsMM.end());
	    else 
	      ATH_MSG_DEBUG("Too many seeds created from the MM stereo layers. seedsMM size is " << seedsMM.size() << ". Not using MM seeds");

	    if (seeds_preOR.empty() && std::abs((*sit).globalPosition().eta()) < 2.4)
	      seeds_preOR.insert(seeds_preOR.end(), seedsWires.begin(), seedsWires.end());

	    orderedClusters = orderByLayer(clusters,false); //clusters with the pads
	    if (seeds_preOR.empty()) {
	      seedsPads = segmentSeedFromPads(orderedClusters, sit.get());
	      seeds_preOR.insert(seeds_preOR.end(), seedsPads.begin(), seedsPads.end());
	    }

	    //OR between same seeds
	    seeds.clear();
	    bool is_first_equal = false;
	    bool is_second_equal = false;
	    double phi1 = -999; //phi of the first hit used as seed
	    double phi2 = -999; //phi of the second hit used as seed
	    for (unsigned int s1 = 1; s1 < seeds_preOR.size(); ++s1) {
	      is_first_equal = false;
	      is_second_equal = false;
	      phi1 = seeds_preOR[s1].first.phi();
	      phi2 = (seeds_preOR[s1].first+seeds_preOR[s1].second).phi();
	      for (unsigned int s2 = 0; s2 < s1; ++s2) {
		if ( std::abs( phi1 - seeds_preOR[s2].first.phi() ) < 0.05 ) is_first_equal = true;
		if ( std::abs( phi2 - (seeds_preOR[s2].first+seeds_preOR[s2].second).phi() ) < 0.05 ) is_second_equal = true;
	      }
	      if (!(is_first_equal && is_second_equal)) seeds.emplace_back(seeds_preOR[s1]);
	    }
	    
            std::vector<const Muon::MuonClusterOnTrack*> phiHitsPrevious;
            for (unsigned int i = 0; i < seeds.size(); ++i) {

                std::pair<Amg::Vector3D, Amg::Vector3D> seed3D;
                std::unique_ptr<Trk::TrackParameters> startpar;
                // calculate start parameters
                const Trk::PlaneSurface& surf = sit->associatedSurface();
                Amg::Vector3D posOnSeg = intersectPlane(surf, seeds[i].first, seeds[i].second);
                Amg::Vector2D lpos;
                surf.globalToLocal(posOnSeg, posOnSeg, lpos);
                Trk::LocalDirection ldir;
                surf.globalToLocalDirection(seeds[i].second, ldir);

                Amg::Vector2D lp(sit->localParameters()[Trk::locX], lpos[Trk::locY]);
                Trk::LocalDirection ld(sit->localDirection().angleXZ(), ldir.angleYZ());
                Amg::Vector3D gd;
                surf.localToGlobalDirection(ld, gd);
                Amg::Vector3D gp;
                surf.localToGlobal(lp, gp, gp);
                Amg::Vector3D perpos = gp + -10 * (gd.unit());
                if (perpos.dot(gd) < 0) gd = -1 * gd;
                startpar = std::make_unique<Trk::Perigee>(perpos, gd, 0, perpos);
                seed3D = std::pair<Amg::Vector3D, Amg::Vector3D>(perpos, gd);
                ATH_MSG_VERBOSE(" start parameter " << perpos << " pp " << startpar->position() << " gd " << gd.unit() << " pp "
                                                    << startpar->momentum().unit());

                // old method as fall back
                if (!startpar) {
                    double phi = seeds[i].second.phi();
                    double theta = sit->globalDirection().theta();
                    double qoverp = 0;
                    Trk::PerigeeSurface persurf(sit->rioOnTrack(0)->globalPosition());
                    startpar = std::make_unique<Trk::Perigee>(0, 0, phi, theta, qoverp, persurf);
                    seed3D = seeds[i];
                }

                std::vector<const Muon::MuonClusterOnTrack*> phiHits = getClustersOnSegment(orderedClusters, seed3D);
                std::vector<const Muon::MuonClusterOnTrack*> etaHits = getClustersOnSegment(orderedEtaClusters, seed3D);
		if (phiHits.size() < 2) {
		  continue;
                }
                // logic to reduce combinatorics
                if (phiHits.size() == phiHitsPrevious.size()) {
                    bool sameContent = true;
                    for (unsigned int k = 0; k < phiHits.size(); ++k) {
                        if (phiHits[k] != phiHitsPrevious[k]) sameContent = false;
                    }
                    if (sameContent) {
                        ATH_MSG_VERBOSE(" same clusters found for 3D seed " << i);
                        continue;
                    }
                }
                phiHitsPrevious = phiHits;
                ATH_MSG_VERBOSE(" new clusters for 3D seed " << i);

                // interleave the phi hits
                std::vector<const Trk::MeasurementBase*> vec2;


                /// here get the new corrected set of eta hits
                std::vector<const Muon::MuonClusterOnTrack*> etaHitsCalibrated;
                etaHitsCalibrated = getCalibratedClusters(etaHits, seed3D);

                unsigned int netas = etaHitsCalibrated.size();
                ATH_MSG_DEBUG("got " << netas << " eta hits ");

                if (m_ipConstraint)
                    vec2.reserve(phiHits.size() + netas + 1);
                else
                    vec2.reserve(phiHits.size() + netas);

                // pseudo measurement for vtx
                std::unique_ptr<Trk::PseudoMeasurementOnTrack> pseudoVtx;
                if (m_ipConstraint) {
                    ATH_MSG_DEBUG("add pseudo vertex");
                    double errVtx = 100.;
                    Amg::MatrixX covVtx(1, 1);
                    covVtx(0, 0) = errVtx * errVtx;
                    Amg::Vector3D vtx(0., 0., 0.);
                    Trk::PerigeeSurface perVtx(vtx);
                    pseudoVtx =
                        std::make_unique<Trk::PseudoMeasurementOnTrack>(Trk::LocalParameters(Trk::DefinedParameter(0, Trk::locX)), covVtx, perVtx);
                    vec2.push_back(pseudoVtx.get());
                }
                unsigned int iEta(0), iPhi(0);
                ATH_MSG_VERBOSE("There are " << etaHitsCalibrated.size() << " & " << phiHits.size() << " eta and phi hits");

                while (true) {
                    float phiZ(999999.), etaZ(999999.);
                    if (iPhi < phiHits.size()) phiZ = std::abs(phiHits[iPhi]->globalPosition().z());
                    if (iEta < etaHitsCalibrated.size()) etaZ = std::abs(etaHitsCalibrated[iEta]->globalPosition().z());
                    if (phiZ < etaZ) {
                        vec2.push_back(phiHits[iPhi]);
                        iPhi++;
                    } else {
                        vec2.push_back(etaHitsCalibrated[iEta]);
                        iEta++;
                    }
                    if (iEta >= netas && iPhi >= phiHits.size()) break;
                }

                ATH_MSG_DEBUG("Fitting a 3D segment with " << phiHits.size() << " phi hits and " << vec2.size() << " total hits");

                for (unsigned int k = 0; k < vec2.size(); ++k) {
                    Identifier id = m_edmHelperSvc->getIdentifier(*vec2[k]);
                    ATH_MSG_VERBOSE(m_idHelperSvc->toString(id)
                                    << " " << vec2[k]->globalPosition().perp() << ", " << vec2[k]->globalPosition().z());
                }
                std::unique_ptr<Trk::Track> segtrack {fit(ctx, vec2, *startpar)};
                if (segtrack) {
                    m_trackSummary->updateTrack(*segtrack);
                    // store the track segment in track collection
                    is3Dseg = true;

                    // Peter NEW calculate distance of segment to all muon clusters
                    if (msgLvl(MSG::VERBOSE)) {
                        Amg::Vector3D pos = segtrack->perigeeParameters()->position();
                        Amg::Vector3D dir = segtrack->perigeeParameters()->momentum();
                        dir = dir / dir.mag();
                        ATH_MSG_VERBOSE("3D eta-phi segment found position " << pos << " direction " << dir << " hits " << vec2.size());
                        for (unsigned int k = 0; k < muonClusters.size(); ++k) {
                            const Trk::PlaneSurface* surf = dynamic_cast<const Trk::PlaneSurface*>(&(muonClusters[k]->associatedSurface()));
                            if (surf) {
                                Amg::Vector3D posOnSurf = intersectPlane(*surf, pos, dir);
                                Amg::Vector2D lpos;
                                surf->globalToLocal(posOnSurf, posOnSurf, lpos);
                                double residual = muonClusters[k]->localParameters()[Trk::locX] - lpos[0];
                                double error = Amg::error(muonClusters[k]->localCovariance(), Trk::locX);
                                double pull = residual / error;
                                ATH_MSG_VERBOSE(" lay " << k << " residual " << residual << " error  " << error << " pull " << pull << "  "
                                                        << m_idHelperSvc->toString(muonClusters[k]->identify()));
                            }
                        }
		    }
                    segTrkColl->push_back(std::move(segtrack));
                  
                } else {
                    ATH_MSG_DEBUG("3D segment fit failed");
                }
            }  // end loop on phi seeds
            if (!is3Dseg) {
                if (!segColl)
                    segments.emplace_back(sit->clone());
                else
                    segColl->push_back(sit->clone());
            }
        }  // end loop on precision plane segments

        std::unique_ptr<const TrackCollection> resolvedTracks(m_ambiTool->process(segTrkColl.get()));
        ATH_MSG_DEBUG("Resolved track candidates: old size " << segTrkColl->size() << " new size " << resolvedTracks->size());
        // store the resolved segments
        for (const Trk::Track* rtrk : *resolvedTracks) {
            MuonSegment* seg = m_trackToSegmentTool->convert(ctx, *rtrk);
            if (!seg) {
                ATH_MSG_VERBOSE("Segment conversion failed, no segment created. ");
            } else {
                if (!segColl)
                    segments.emplace_back(seg);
                else
                    segColl->push_back(seg);
            }
        }
    }

    std::vector<const Muon::MuonClusterOnTrack*> MuonClusterSegmentFinderTool::cleanClusters(
        std::vector<const Muon::MuonClusterOnTrack*>& muonClusters, bool selectPhiHits) const {
        std::vector<const Muon::MuonClusterOnTrack*> clusters;
        // remove the phi hits
        for (std::vector<const Muon::MuonClusterOnTrack*>::const_iterator cit = muonClusters.begin(); cit != muonClusters.end(); ++cit) {
            if (!(*cit)) continue;
            bool measPhi = m_idHelperSvc->measuresPhi((*cit)->identify());
            if (measPhi != selectPhiHits) continue;
            clusters.push_back(*cit);
        }
        return clusters;
    }

    std::vector<std::vector<const Muon::MuonClusterOnTrack*> > MuonClusterSegmentFinderTool::orderByLayer(
        std::vector<const Muon::MuonClusterOnTrack*>& clusters, bool useWires) const {
        //start by sorting input clusters by global z
        std::stable_sort(clusters.begin(),clusters.end(),[](const Muon::MuonClusterOnTrack* c1,const Muon::MuonClusterOnTrack* c2){return std::abs(c1->prepRawData()->globalPosition().z())<std::abs(c2->prepRawData()->globalPosition().z());});
        //now loop through clusters and add them to vectors, with all clusters with 0.1 in z getting their own vector
        std::vector<std::vector<const Muon::MuonClusterOnTrack*> > orderedClusters;
	std::vector<const Muon::MuonClusterOnTrack*> tmpclusters;
	float currentz=0;
        for (std::vector<const Muon::MuonClusterOnTrack*>::const_iterator cit = clusters.begin(); cit != clusters.end(); ++cit) {
	    //first, skip sTGC pads (if using wires) or wires (if using pads)
	    if (useWires && m_idHelperSvc->issTgc((*cit)->identify()) &&
		m_idHelperSvc->stgcIdHelper().channelType((*cit)->identify()) == 0) {
	        continue;
	    } 
	    if (!useWires && m_idHelperSvc->issTgc((*cit)->identify()) &&
		m_idHelperSvc->stgcIdHelper().channelType((*cit)->identify()) == 2) {
	        continue;
	    }
	    if(std::abs(std::abs((*cit)->prepRawData()->globalPosition().z())-currentz)<0.1){
	        tmpclusters.push_back(*cit);
		continue;
	    }
	    if(!tmpclusters.empty()){
	        orderedClusters.push_back(tmpclusters);
		tmpclusters.clear();
	    }
	    tmpclusters.push_back(*cit);
	    currentz=std::abs((*cit)->prepRawData()->globalPosition().z());
	}
	if(!tmpclusters.empty()) orderedClusters.push_back(tmpclusters);
        return orderedClusters;
    }

    std::vector<std::pair<Amg::Vector3D, Amg::Vector3D> > MuonClusterSegmentFinderTool::segmentSeed(
        std::vector<std::vector<const Muon::MuonClusterOnTrack*> >& orderedClusters, bool usePhi) const {
        std::vector<std::pair<Amg::Vector3D, Amg::Vector3D> > seeds;
        if (orderedClusters.size() < 4) return seeds;

        // calculate the straight line between the two furthest points
        int seedlayers1 = 0;
        for (unsigned int i = 0; (i < orderedClusters.size() && seedlayers1 < m_nOfSeedLayers); ++i) {
            bool usedLayer1 = false;
            for (std::vector<const Muon::MuonClusterOnTrack*>::const_iterator cit = orderedClusters[i].begin();
                 cit != orderedClusters[i].end(); ++cit) {
                if (usePhi != m_idHelperSvc->measuresPhi((*cit)->identify())) continue;
                usedLayer1 = true;
                const Amg::Vector3D& gp1 = (*cit)->prepRawData()->globalPosition();

                int seedlayers2 = 0;
                for (unsigned int k = orderedClusters.size() - 1; (k > i && seedlayers2 < m_nOfSeedLayers); --k) {
                    bool usedLayer2 = false;
                    for (std::vector<const Muon::MuonClusterOnTrack*>::const_iterator cit2 = orderedClusters[k].begin();
                         cit2 != orderedClusters[k].end(); ++cit2) {
                        if (usePhi != m_idHelperSvc->measuresPhi((*cit2)->identify())) continue;

                        usedLayer2 = true;
                        const Amg::Vector3D& gp2 = (*cit2)->prepRawData()->globalPosition();
                        double dx = (gp2.x() - gp1.x());
                        double dy = (gp2.y() - gp1.y());
                        double dz = (gp2.z() - gp1.z());
                        if (std::abs(dz) < 200.) {
                            dx = (gp2.x() + gp1.x()) / 2.;
                            dy = (gp2.y() + gp1.y()) / 2.;
                            dz = (gp2.z() + gp1.z()) / 2.;
                        }
                        Amg::Vector3D segdir(dx, dy, dz);
                        seeds.push_back(std::pair<Amg::Vector3D, Amg::Vector3D>(gp1, segdir));
                    }
                    if (usedLayer2) ++seedlayers2;
                }
            }
            if (usedLayer1) ++seedlayers1;
        }
        if (usePhi) return seeds;

        // use the innermost and outermost MM hits to form the seeds
        unsigned int innerMM(999), outerMM(999);
        for (unsigned int i = 0; i < orderedClusters.size(); ++i) {
            if (m_idHelperSvc->isMM(orderedClusters[i].front()->identify())) {
                innerMM = i;
                break;
            }
        }
        for (unsigned int i = orderedClusters.size() - 1; i > innerMM; --i) {
            if (m_idHelperSvc->isMM(orderedClusters[i].front()->identify())) {
                outerMM = i;
                break;
            }
        }
        if (innerMM == 999 || outerMM == 999) return seeds;
        // create the seeds
        for (std::vector<const Muon::MuonClusterOnTrack*>::const_iterator cit = orderedClusters[innerMM].begin();
             cit != orderedClusters[innerMM].end(); ++cit) {
            const Amg::Vector3D& gp1 = (*cit)->prepRawData()->globalPosition();
            for (std::vector<const Muon::MuonClusterOnTrack*>::const_iterator cit2 = orderedClusters[outerMM].begin();
                 cit2 != orderedClusters[outerMM].end(); ++cit2) {
                const Amg::Vector3D& gp2 = (*cit2)->prepRawData()->globalPosition();
                double dx = (gp2.x() - gp1.x());
                double dy = (gp2.y() - gp1.y());
                double dz = (gp2.z() - gp1.z());
                if (std::abs(dz) < 200.) {
                    dx = (gp2.x() + gp1.x()) / 2.;
                    dy = (gp2.y() + gp1.y()) / 2.;
                    dz = (gp2.z() + gp1.z()) / 2.;
                }
                Amg::Vector3D segdir(dx, dy, dz);
                seeds.push_back(std::pair<Amg::Vector3D, Amg::Vector3D>(gp1, segdir));
            }
        }

        return seeds;
    }

    std::vector<const Muon::MuonClusterOnTrack*> MuonClusterSegmentFinderTool::getClustersOnSegment(
        std::vector<std::vector<const Muon::MuonClusterOnTrack*> >& clusters, std::pair<Amg::Vector3D, Amg::Vector3D>& seed) const {
        ATH_MSG_VERBOSE(" getClustersOnSegment: layers " << clusters.size());
        std::vector<const Muon::MuonClusterOnTrack*> rios;
        int layer = 0;
        for (std::vector<std::vector<const Muon::MuonClusterOnTrack*> >::const_iterator cvecIt = clusters.begin(); cvecIt != clusters.end();
             ++cvecIt) {
            const Muon::MuonClusterOnTrack* rio = nullptr;
            double bestDist(9999.);

            for (std::vector<const Muon::MuonClusterOnTrack*>::const_iterator cit = (*cvecIt).begin(); cit != (*cvecIt).end(); ++cit) {
                double dist = clusterDistanceToSeed(*cit, seed);
                double error = Amg::error((*cit)->localCovariance(), Trk::locX);
                if (m_idHelperSvc->isMM((*cit)->identify()) && m_idHelperSvc->mmIdHelper().isStereo((*cit)->identify())) error = 15;

                ATH_MSG_VERBOSE(" lay " << layer << " dist " << dist << " pull " << dist / error << " cut " << m_maxClustDist << "  "
                                        << m_idHelperSvc->toString((*cit)->identify()));
                if (std::abs(dist / error) < m_maxClustDist) {
                    if (std::abs(dist) < bestDist) {
                        bestDist = std::abs(dist);
                        rio = (*cit);
                    }
                }
            }
            if (rio) {
                ATH_MSG_VERBOSE(" adding  " << bestDist << "  " << m_idHelperSvc->toString(rio->identify()));
                rios.push_back(rio);
            }
            ++layer;
        }
        ATH_MSG_VERBOSE(" getClustersOnSegment: returning hits " << rios.size());
        return rios;
    }

    Amg::Vector3D MuonClusterSegmentFinderTool::intersectPlane(const Trk::PlaneSurface& surf, const Amg::Vector3D& pos,
                                                               const Amg::Vector3D& dir) const {
        const Amg::Vector3D& planepostion = surf.center();
        const Amg::Vector3D& planenormal = surf.normal();
        double denom = dir.dot(planenormal);
        double u = planenormal.dot(planepostion - pos) / denom;
        return pos + u * dir;
    }

    double MuonClusterSegmentFinderTool::clusterDistanceToSeed(const Muon::MuonClusterOnTrack* clus,
                                                               std::pair<Amg::Vector3D, Amg::Vector3D>& seed) const {
        const Trk::PlaneSurface* surf = dynamic_cast<const Trk::PlaneSurface*>(&clus->associatedSurface());
        double dist(-99999);
        if (!surf) {
            ATH_MSG_DEBUG(" clusterDistanceToSeed: no Trk::PlaneSurface found on cluster ");
            return dist;
        }
        Amg::Vector3D piOnPlane = intersectPlane(*surf, seed.first, seed.second);
        // transform to local plane coordiantes
        Amg::Vector2D lpos;
        surf->globalToLocal(piOnPlane, piOnPlane, lpos);
        // check if the MuonCluster is pad hit
        if (m_idHelperSvc->issTgc(clus->identify()) && m_idHelperSvc->stgcIdHelper().channelType(clus->identify()) == 0) {
            const sTgcPrepData* prd = dynamic_cast<const sTgcPrepData*>(clus->prepRawData());
            if (!prd) {
                ATH_MSG_DEBUG(" clusterDistanceToSeed: no sTgcPrepData found ");
                return dist;
            }
            const MuonGM::MuonPadDesign* design = prd->detectorElement()->getPadDesign(clus->identify());
            if (!design) {
                ATH_MSG_WARNING("MuonPadDesign not found for " << m_idHelperSvc->toString(clus->identify()));
                return dist;
            }
            // check the alignment in locX
            double chWidth1 = 0.5 * design->channelWidth(prd->localPosition(), false);
            double chWidth2 = 0.5 * design->channelWidth(prd->localPosition(), true);
            chWidth1 = std::abs(chWidth1);
            chWidth2 = std::abs(chWidth2);
            Amg::Vector2D lp1(prd->localPosition().x() - chWidth2, prd->localPosition().y() - chWidth1);
            Amg::Vector2D lp2(prd->localPosition().x() + chWidth2, prd->localPosition().y() + chWidth1);
            ATH_MSG_DEBUG(" chWidth2 locX " << chWidth2 << " chWidth1 locY" << chWidth1 << " design->sPadWidth " << design->sPadWidth
                                            << " design->lPadWidth " << design->lPadWidth);
            ATH_MSG_DEBUG(" cluster locX " << prd->localPosition().x() << " lpos.x() " << lpos.x() << " cluster locY "
                                           << prd->localPosition().y() << " lpos.y() " << lpos.y());
            if (lp1.y() < lpos.y() && lp2.y() > lpos.y() && lp1.x() < lpos.x() && lp2.x() > lpos.x()) { dist = 0; }
        } else {
            dist = clus->localParameters()[Trk::locX] - lpos[Trk::locX];
            ATH_MSG_DEBUG(" clus [locX] " << clus->localParameters()[Trk::locX] << " clus [LocY] " << clus->localParameters()[Trk::locY]
                                          << " lpos[locX] " << lpos[Trk::locX] << " lpos[locY] " << lpos[Trk::locY]);
        }

        return dist;
    }

    std::unique_ptr<Trk::Track> MuonClusterSegmentFinderTool::fit(const EventContext& ctx, const std::vector<const Trk::MeasurementBase*>& vec2,
                                                  const Trk::TrackParameters& startpar) const {
        std::unique_ptr<Trk::Track> segtrack = m_slTrackFitter->fit(ctx, vec2, startpar, false, Trk::nonInteracting);
        if (segtrack) {
            ATH_MSG_VERBOSE("segment fit succeeded");
            std::unique_ptr<Trk::Track> cleanedTrack = m_trackCleaner->clean(*segtrack, ctx);
            if (cleanedTrack && !(cleanedTrack->perigeeParameters() == segtrack->perigeeParameters())) {
                // using release until the entire code can be migrated to use smart pointers
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

    std::vector<std::pair<Amg::Vector3D, Amg::Vector3D> > MuonClusterSegmentFinderTool::segmentSeedFromPads(
        std::vector<std::vector<const Muon::MuonClusterOnTrack*> >& orderedClusters, const Muon::MuonSegment* etaSeg) const {
        std::vector<std::pair<Amg::Vector3D, Amg::Vector3D> > seeds;

        std::vector<std::vector<const Muon::MuonClusterOnTrack*> > sTgc1, sTgc2;

        for (int iml = 1; iml < 3; ++iml) {
            // multilayer loop
            unsigned int istart = 0;
            unsigned int idir = 1;
            if (iml == 2) istart = orderedClusters.size() - 1;
            if (iml == 2) idir = -1;
            for (unsigned int i = istart; i < orderedClusters.size(); i = i + idir) {
                if (m_idHelperSvc->stgcIdHelper().multilayer(orderedClusters[i].front()->identify()) != iml) continue;
                std::vector<const Muon::MuonClusterOnTrack*> hits;
                double distance = 1000.;
                for (std::vector<const Muon::MuonClusterOnTrack*>::const_iterator cit = orderedClusters[i].begin();
                     cit != orderedClusters[i].end(); ++cit) {
                    // check the hit is aligned with the segment
                    // segment position on the surface
                    const Trk::PlaneSurface* surf = dynamic_cast<const Trk::PlaneSurface*>(&(*cit)->associatedSurface());
                    if (!surf) continue;
                    Amg::Vector3D piOnPlane = intersectPlane(*surf, etaSeg->globalPosition(), etaSeg->globalDirection());
                    // transform to local plane coordiantes
                    Amg::Vector2D lpos;
                    surf->globalToLocal(piOnPlane, piOnPlane, lpos);
                    // pad width
                    const sTgcPrepData* prd = dynamic_cast<const sTgcPrepData*>((*cit)->prepRawData());
                    if (!prd) continue;
                    const MuonGM::MuonPadDesign* design = prd->detectorElement()->getPadDesign((*cit)->identify());
                    if (!design) continue;
                    double chWidth1 = 0.5 * design->channelWidth(prd->localPosition(), false);
                    Amg::Vector2D lp1(prd->localPosition().x(), prd->localPosition().y() - chWidth1);
                    Amg::Vector2D lp2(prd->localPosition().x(), prd->localPosition().y() + chWidth1);
                    double etaDistance = prd->localPosition().y() - lpos[1];
                    ATH_MSG_DEBUG(" etaDistance " << etaDistance);
                    if (std::abs(etaDistance) < distance + 0.001) {
                        double lastDistance = distance;
                        distance = std::abs(etaDistance);
                        if (distance < 100.) {
                            if (lp1.y() < lpos.y() && lp2.y() > lpos.y()) {
                                if (hits.size() == 0) {
                                    ATH_MSG_DEBUG(" start best etaDistance " << etaDistance);
                                    hits.push_back(*cit);
                                } else if (std::abs(lastDistance - distance) < 0.001) {
                                    hits.push_back(*cit);
                                    ATH_MSG_DEBUG(" added etaDistance " << etaDistance << " size " << hits.size());
                                } else {
                                    ATH_MSG_DEBUG(" reset update best etaDistance " << etaDistance);
                                    hits.clear();
                                    hits.push_back(*cit);
                                }
                            }
                        }
                    }
                }
                if (hits.size() == 0) continue;
                if (m_idHelperSvc->stgcIdHelper().multilayer(hits.front()->identify()) == 1)
                    sTgc1.push_back(hits);
                else
                    sTgc2.push_back(hits);
            }
        }

        ATH_MSG_DEBUG(" sTgc1.size() " << sTgc1.size() << " sTgc2.size() " << sTgc2.size());

        if (sTgc1.size() == 0 || sTgc2.size() == 0) return seeds;
        // store reference surfaces
        const sTgcPrepData* prdL1 = dynamic_cast<const sTgcPrepData*>(sTgc1.front().front()->prepRawData());
        const sTgcPrepData* prdL2 = dynamic_cast<const sTgcPrepData*>(sTgc2.back().front()->prepRawData());
        const Trk::PlaneSurface* surf1 = dynamic_cast<const Trk::PlaneSurface*>(&(sTgc1.front().front())->associatedSurface());
        const Trk::PlaneSurface* surf2 = dynamic_cast<const Trk::PlaneSurface*>(&(sTgc2.back().front())->associatedSurface());

        // ensure there are 4 entries per sTgc chamber
        while (sTgc1.size() < 4) {
            ATH_MSG_DEBUG("Adding empty layer to the sTgc chlay 1");
            std::vector<const Muon::MuonClusterOnTrack*> hits;
            sTgc1.push_back(hits);
        }
        while (sTgc2.size() < 4) {
            ATH_MSG_DEBUG("Adding empty layer to the sTgc chlay 2");
            std::vector<const Muon::MuonClusterOnTrack*> hits;
            sTgc2.push_back(hits);
        }
        if (!prdL1 || !prdL2) {
            ATH_MSG_WARNING("No prd associated to one of the sTgc pads!");
            return seeds;
        }

        if (!surf1 || !surf2) {
            ATH_MSG_WARNING("No surface associated to one of the sTgc pads!");
            return seeds;
        }

        if (sTgc1.size() != 4 || sTgc2.size() != 4) {
            ATH_MSG_WARNING("Something is wrong with the sTgc phi pads!  Found " << sTgc1.size() << "/" << sTgc2.size() << " layers");
        }
        std::vector<std::pair<double, double> > sTgc1_phi = getPadPhiOverlap(sTgc1);
        std::vector<std::pair<double, double> > sTgc2_phi = getPadPhiOverlap(sTgc2);

        // make combinations of pad clusters
        std::vector<Amg::Vector3D> phiPos1, phiPos2;
        for (unsigned int i1 = 0; i1 < sTgc1_phi.size(); ++i1) {
            double midPhi1 = 0.5 * (sTgc1_phi[i1].first + sTgc1_phi[i1].second);
            // change to global position
            Amg::Vector2D lp1(midPhi1, prdL1->localPosition().y());
            Amg::Vector3D gpL1;
            prdL1->detectorElement()->surface().localToGlobal(lp1, gpL1, gpL1);
            for (unsigned int i2 = 0; i2 < sTgc2_phi.size(); ++i2) {
                // change to global position
                double midPhi2 = 0.5 * (sTgc2_phi[i2].first + sTgc2_phi[i2].second);
                Amg::Vector2D lp2(midPhi2, prdL2->localPosition().y());
                Amg::Vector3D gpL2;
                prdL2->detectorElement()->surface().localToGlobal(lp2, gpL2, gpL2);
                phiPos1.push_back(gpL1);
                phiPos2.push_back(gpL2);
            }
        }

        for (unsigned int i = 0; i < phiPos1.size(); ++i) {
            // create the seed pair
            // Peter this is badly defined -> use phi from positions
            //      Amg::Vector3D gdir(
            //      (phiPos2[i].x()-phiPos1[i].x()),(phiPos2[i].y()-phiPos1[i].y()),(phiPos2[i].z()-phiPos1[i].z()) );
            Amg::Vector3D gdir((phiPos2[i].x() + phiPos1[i].x()), (phiPos2[i].y() + phiPos1[i].y()), (phiPos2[i].z() + phiPos1[i].z()));
            gdir = gdir / gdir.mag();
            std::pair<Amg::Vector3D, Amg::Vector3D> seedPair(phiPos1[i], gdir);
            seeds.push_back(seedPair);
        }

        ATH_MSG_DEBUG(" segmentSeedFromPads: seeds.size() " << seeds.size());
        return seeds;
    }

    std::vector<std::pair<Amg::Vector3D,Amg::Vector3D> >
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
	seeds.emplace_back(phiStereo[good_indices[i].first],segdir);
      }
      return seeds;
    }

    std::vector<std::pair<double, double> > MuonClusterSegmentFinderTool::getPadPhiOverlap(
        std::vector<std::vector<const Muon::MuonClusterOnTrack*> >& pads) const {
        std::vector<std::pair<double, double> > phiOverlap;

        // calculate the max/min phi values for each pad
        std::vector<std::vector<double> > padMax, padMin;
        std::vector<double> padMean;
        unsigned npadsMean = 0;
        for (unsigned int i = 0; i < pads.size(); ++i) {
            std::vector<double> padLayerMax, padLayerMin;
            for (std::vector<const Muon::MuonClusterOnTrack*>::const_iterator cit = pads[i].begin(); cit != pads[i].end(); ++cit) {
                Identifier id = (*cit)->identify();
                const sTgcPrepData* prd = dynamic_cast<const sTgcPrepData*>((*cit)->prepRawData());
                if (!prd) {
                    ATH_MSG_WARNING("No prd found for " << m_idHelperSvc->toString(id));
                    continue;
                }
                const MuonGM::MuonPadDesign* design = prd->detectorElement()->getPadDesign(id);
                if (!design) {
                    ATH_MSG_WARNING("No design found for " << m_idHelperSvc->toString(id));
                    continue;
                }
                double chWidth = 0.5 * design->channelWidth(prd->localPosition(), true);
                Amg::Vector2D lp1(prd->localPosition().x() + chWidth, prd->localPosition().y());
                Amg::Vector2D lp2(prd->localPosition().x() - chWidth, prd->localPosition().y());

                bool keep = true;
                int jmean = -1;
                for (unsigned int j = 0; j < npadsMean; j++) {
                    if (padMean[j] > lp2.x() && padMean[j] < lp1.x()) {
                        keep = false;
                        jmean = (int)j;
                    }
                }
                if (keep) {
                    // only store phi positions that are different
                    padLayerMax.push_back(lp1.x());
                    padLayerMin.push_back(lp2.x());
                    padMean.push_back((lp1.x() + lp2.x()) / 2.);
                    ATH_MSG_DEBUG(" keep pad id " << m_idHelperSvc->toString(id) << " lp1.x() " << lp1.x() << " lp1.y() " << lp1.y()
                                                  << " index " << npadsMean << " padMean " << padMean[npadsMean]);
                    npadsMean++;
                } else {
                    ATH_MSG_DEBUG(" reject pad id " << m_idHelperSvc->toString(id) << " lp1.x() " << lp1.x() << " lp1.y() " << lp1.y()
                                                    << " matched index " << jmean << " padMean " << padMean[jmean]);
                }
            }
            padMax.push_back(padLayerMax);
            padMin.push_back(padLayerMin);
        }

        // find all combinations of pads (1 per layer)
        unsigned int nCombos(1);
        for (unsigned int i = 1; i < padMax.size(); ++i) {
            if (padMax[i].size() != 0) nCombos = nCombos * padMax[i].size();
        }
        bool makeCombinations = true;
        if (nCombos > 8) makeCombinations = false;

        if (makeCombinations) {
            ATH_MSG_DEBUG("MakeCombinations for Pads " << nCombos);
            for (unsigned int l0 = 0; l0 < padMax[0].size(); ++l0) {
                std::vector<double> phiMax(nCombos, padMax[0][l0]);
                std::vector<double> phiMin(nCombos, padMin[0][l0]);
                for (unsigned int l1 = 0; l1 < padMax[1].size(); ++l1) {
                    for (unsigned int i = 0; i < nCombos; ++i) {
                        if (nCombos % padMax[1].size() == 0) {
                            if (padMax[1][l1] < phiMax[i]) phiMax[i] = padMax[1][l1];
                            if (padMin[1][l1] > phiMin[i]) phiMin[i] = padMin[1][l1];
                        }
                    }
                    for (unsigned int l2 = 0; l2 < padMax[2].size(); ++l2) {
                        for (unsigned int i = 0; i < nCombos; ++i) {
                            if (nCombos % padMax[2].size() == 0) {
                                if (padMax[2][l2] < phiMax[i]) phiMax[i] = padMax[2][l2];
                                if (padMin[2][l2] > phiMin[i]) phiMin[i] = padMin[2][l2];
                            }
                        }
                        for (unsigned int l3 = 0; l3 < padMax[3].size(); ++l3) {
                            for (unsigned int i = 0; i < nCombos; ++i) {
                                if (nCombos % padMax[3].size() == 0) {
                                    if (padMax[3][l3] < phiMax[i]) phiMax[i] = padMax[3][l3];
                                    if (padMin[3][l3] > phiMin[i]) phiMin[i] = padMin[3][l3];
                                }
                            }
                        }  // end layer 3 loop
                    }      // end layer 2 loop
                }          // end layer 1 loop
                // store the overlaps regions
                for (unsigned int i = 0; i < nCombos; ++i) { phiOverlap.push_back(std::pair<double, double>(phiMin[i], phiMax[i])); }
            }  // end layer 0 loop

        } else {
            for (unsigned int i0 = 0; i0 < 4; ++i0) {
                for (unsigned int l0 = 0; l0 < padMax[i0].size(); ++l0) {
                    phiOverlap.push_back(std::pair<double, double>(padMin[i0][l0], padMax[i0][l0]));
                    ATH_MSG_DEBUG(" layer " << i0 << " mst " << l0 << " phiMin " << padMin[i0][l0] << " phiMax " << padMax[i0][l0]);
                }
            }
            ATH_MSG_DEBUG("Do not make combinations for Pads " << nCombos << " phiOverlap size " << phiOverlap.size());
        }

        return phiOverlap;
    }

    bool MuonClusterSegmentFinderTool::belowThreshold(std::vector<const Muon::MuonClusterOnTrack*>& muonClusters, int threshold) const {
        int n_surf_with_hits = 0;
        if (muonClusters.size() > 1) {
            for (std::vector<const Muon::MuonClusterOnTrack*>::const_iterator cit = muonClusters.begin() + 1; cit != muonClusters.end();
                 ++cit) {
                if ((*cit) && (*(cit - 1))) {
                    if ((*cit)->detectorElement()->center((*cit)->identify()) !=
                        (*(cit - 1))->detectorElement()->center((*(cit - 1))->identify())) {
                        n_surf_with_hits++;
                    }
                }
            }
        } else
            n_surf_with_hits = muonClusters.size();
        if (n_surf_with_hits < threshold)
            return true;
        else
            return false;
    }

    std::vector<const Muon::MuonClusterOnTrack*> MuonClusterSegmentFinderTool::getCalibratedClusters(
        std::vector<const Muon::MuonClusterOnTrack*>& clusters, std::pair<Amg::Vector3D, Amg::Vector3D>& seed) const {
        std::vector<const Muon::MuonClusterOnTrack*> calibratedClusters;

        /// loop on the segment clusters and use the phi of the seed to correct them
        for (const Muon::MuonClusterOnTrack* clus : clusters) {
            const Muon::MuonClusterOnTrack* newClus = nullptr;
            /// get the intercept of the seed direction with the cluster surface
            const Trk::PlaneSurface* surf = dynamic_cast<const Trk::PlaneSurface*>(&clus->associatedSurface());
            if (surf) {
                Amg::Vector3D posOnSurf = intersectPlane(*surf, seed.first, seed.second);
                Amg::Vector2D lpos;
                surf->globalToLocal(posOnSurf, posOnSurf, lpos);
                /// correct the eta position of the MM stereo layers only, based on the
                Identifier clus_id = clus->identify();
                if (m_idHelperSvc->isMM(clus_id)) {
                    /// build a  new MM cluster on track with correct position
                    newClus = m_mmClusterCreator->calibratedCluster(*(clus->prepRawData()), posOnSurf);
                    // newClus = clus;
                    ATH_MSG_VERBOSE("Position before correction: " << clus->globalPosition().x() << " " << clus->globalPosition().y() << " "
                                                                   << clus->globalPosition().z());
                    ATH_MSG_VERBOSE("Position after correction: " << newClus->globalPosition().x() << " " << newClus->globalPosition().y()
                                                                  << " " << newClus->globalPosition().z());
                }

                else if (m_idHelperSvc->issTgc(clus->identify())) {
                    /// if it's STGC just copy the cluster -> calibration to be added
                    newClus = clus;
                }
            } else {
                ATH_MSG_WARNING("Surface associated to cluster not found, not calibrating");
                newClus = clus;
            }

            calibratedClusters.push_back(newClus);
        }

        return calibratedClusters;
    }

    std::vector<Amg::Vector3D>
    MuonClusterSegmentFinderTool::getPhiFromStereo( std::vector<const Muon::MuonClusterOnTrack*>& clusters  ) const
    {
  
      //vector of 3D points of all phi obtained from the stereo-stereo combination                                                                              
      std::vector<Amg::Vector3D> phiStereo;
      
      /// loop on the clusters and look for the stereo                                                                                                       
      for ( unsigned int iclu1 = 1; iclu1 <= clusters.size()-1; ++iclu1 ) {
        Identifier id1 = clusters[iclu1]->identify();
        if ( !m_idHelperSvc->isMM(id1) ) continue;
        if ( !m_idHelperSvc->mmIdHelper().isStereo(id1) ) continue;
  	// get the intersections of the stereo clusters with the other layers of the same multilayer                                                       
	for ( unsigned int iclu2 = 0; iclu2 < iclu1; ++iclu2 ) {
  	  Identifier id2 = clusters[iclu2]->identify();
	  if ( !m_idHelperSvc->isMM(id2) ) continue;
	  if ( !m_idHelperSvc->mmIdHelper().isStereo(id2) ) continue; //remove this if you want to use eta+stereo or stereo+stereo. Now use only stereo+stereo
	  int lay1 = m_idHelperSvc->mmIdHelper().gasGap(id1);
	  int ml1 = m_idHelperSvc->mmIdHelper().multilayer(id1);
	  int lay2 = m_idHelperSvc->mmIdHelper().gasGap(id2);
	  int ml2 = m_idHelperSvc->mmIdHelper().multilayer(id2);
	  /// skip clusters if not in the same multilayer and if on the same layer                                                                         
	  if ( ml1 != ml2 || lay1 == lay2 ) continue;
	  const MMPrepData* prd1 = dynamic_cast<const MMPrepData*>(clusters[iclu1]->prepRawData());
	  const MMPrepData* prd2 = dynamic_cast<const MMPrepData*>(clusters[iclu2]->prepRawData());
	  /// get the local position of the clusters
	  Trk::LocalParameters loc1(prd1->localPosition());
  	  Trk::LocalParameters loc2(prd2->localPosition());
  	  
  	  //head Amg::Vector3D localToGlobal(const LocalParameters& locpars) const;
  	  Amg::Vector3D gPos1 = clusters[iclu1]->associatedSurface().localToGlobal(loc1);
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
