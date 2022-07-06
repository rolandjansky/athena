/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonSegmentFinderAlg.h"

#include "MuonPattern/MuonPatternChamberIntersect.h"
#include "MuonPattern/MuonPatternCombination.h"
#include "MuonPattern/MuonPatternCombinationCollection.h"
#include "MuonPatternSegmentMaker/MuonPatternCalibration.h"
#include "MuonPrepRawData/MuonCluster.h"
#include "MuonPrepRawData/MuonPrepDataCollection.h"
#include "MuonPrepRawData/MuonPrepDataContainer.h"
#include "MuonRIO_OnTrack/MuonClusterOnTrack.h"
#include "MuonReadoutGeometry/MuonReadoutElement.h"
#include "MuonSegment/MuonSegment.h"
#include "MuonSegment/MuonSegmentCombination.h"
#include "MuonSegment/MuonSegmentCombinationCollection.h"
#include "TrkParameters/TrackParameters.h"

using HepGeom::Transform3D;

MuonSegmentFinderAlg::MuonSegmentFinderAlg(const std::string& name, ISvcLocator* pSvcLocator) : AthReentrantAlgorithm(name, pSvcLocator) {}

StatusCode MuonSegmentFinderAlg::initialize() {
    ATH_CHECK(m_idHelperSvc.retrieve());
    ATH_CHECK(m_printer.retrieve());
    ATH_CHECK(m_patternCalibration.retrieve());
    ATH_CHECK(m_patternSegmentMaker.retrieve());
    ATH_CHECK(m_segmentOverlapRemovalTool.retrieve());
    ATH_CHECK(m_segmentMaker.retrieve());
    ATH_CHECK(m_clusterSegMaker.retrieve());

    ATH_CHECK(m_clusterCreator.retrieve());
    const bool doNSW = m_idHelperSvc->recoMM() || m_idHelperSvc->recosTgc();
    if (doNSW){
        ATH_CHECK(m_mmClusterCreator.retrieve());
        ATH_CHECK(m_clusterSegMakerNSW.retrieve());
    }

    if (!m_csc2dSegmentFinder.empty())
        ATH_CHECK(m_csc2dSegmentFinder.retrieve());
    else
        m_csc2dSegmentFinder.disable();

    if (!m_csc4dSegmentFinder.empty())
        ATH_CHECK(m_csc4dSegmentFinder.retrieve());
    else
        m_csc4dSegmentFinder.disable();

    ATH_CHECK(m_segmentCollectionKey.initialize());
    ATH_CHECK(m_segmentNSWCollectionKey.initialize(doNSW)); //used to perform the alignment in the NSW
    ATH_CHECK(m_cscPrdsKey.initialize(!m_cscPrdsKey.empty()));  // check for layouts without CSCs
    ATH_CHECK(m_mdtPrdsKey.initialize(m_doTGCClust || m_doRPCClust));
    ATH_CHECK(m_rpcPrdsKey.initialize());
    ATH_CHECK(m_tgcPrdsKey.initialize());
    ATH_CHECK(m_patternCollKey.initialize());
    ATH_CHECK(m_tgcTruth.initialize(m_doClusterTruth));
    ATH_CHECK(m_rpcTruth.initialize(m_doClusterTruth));

    return StatusCode::SUCCESS;
}

StatusCode MuonSegmentFinderAlg::execute(const EventContext& ctx) const {
    // vector to hold segments
    std::unique_ptr<Trk::SegmentCollection> segmentContainer = std::make_unique<Trk::SegmentCollection>();
    std::unique_ptr<Trk::SegmentCollection> nswSegmentContainer = std::make_unique<Trk::SegmentCollection>();   
    
    SG::ReadHandle<Muon::TgcPrepDataContainer> tgcPrds(m_tgcPrdsKey, ctx);   
    const Muon::TgcPrepDataContainer* tgcPrdCont = tgcPrds.cptr();

    SG::ReadHandle<Muon::RpcPrepDataContainer> rpcPrds(m_rpcPrdsKey, ctx);
    const Muon::RpcPrepDataContainer* rpcPrdCont = rpcPrds.cptr();


    SG::ReadHandle<MuonPatternCombinationCollection> patternColl(m_patternCollKey, ctx);
    if (!patternColl.isValid()) {
        ATH_MSG_FATAL("Could not to retrieve the PatternCombinations from StoreGate");
        return StatusCode::FAILURE;
    }
    if (patternColl.isPresent()) {  // pretty sure this is always the case, but can't hurt to check
        ATH_MSG_DEBUG("Processing the pattern collections with  " << patternColl->size() << " Collections ");

        for (MuonPatternCombinationCollection::const_iterator patt = patternColl->begin(); patt != patternColl->end(); ++patt) {
            ATH_MSG_DEBUG("Working on pattern combination " << m_printer->print(**patt));
            // check the technology & call the corresponding segment finder
            std::vector<const Muon::TgcPrepDataCollection*> tgcCols;
            std::vector<const Muon::RpcPrepDataCollection*> rpcCols;
            
            if (tgcPrdCont) {
                tgcCols.reserve(tgcPrdCont->size());
                for (const Muon::TgcPrepDataCollection* p : *tgcPrdCont) {
                    if (!p->empty()) tgcCols.push_back(p);
                }
            } else ATH_MSG_VERBOSE("No tgc container is available");

            if (rpcPrdCont) {
                rpcCols.reserve(rpcPrdCont->size());
                  for (const Muon::RpcPrepDataCollection* p : *rpcPrdCont) {
                    if (!p->empty()) rpcCols.push_back(p);
                }
            } else ATH_MSG_VERBOSE("No rpc container is available");
            
            
            createSegmentsWithMDTs(*patt, segmentContainer.get(), rpcCols, tgcCols, ctx);
            if (m_idHelperSvc->recoMM() || m_idHelperSvc->recosTgc()) {
                createSegmentsFromClusters(ctx, *patt, segmentContainer.get(), nswSegmentContainer.get());
            }
            
        }  // end loop on pattern combinations

    } else {
        ATH_MSG_DEBUG("no pattern collection, moving on ...");
    }

    // do cluster based segment finding
    if (m_doTGCClust || m_doRPCClust) {
        SG::ReadHandle<Muon::MdtPrepDataContainer> mdtPrds(m_mdtPrdsKey, ctx);
        const PRD_MultiTruthCollection* tgcTruthColl = nullptr;
        const PRD_MultiTruthCollection* rpcTruthColl = nullptr;
        if (m_doClusterTruth) {
            SG::ReadHandle<PRD_MultiTruthCollection> tgcTruth(m_tgcTruth, ctx);
            SG::ReadHandle<PRD_MultiTruthCollection> rpcTruth(m_rpcTruth, ctx);
            tgcTruthColl = tgcTruth.cptr();
            rpcTruthColl = rpcTruth.cptr();
        }
        m_clusterSegMaker->getClusterSegments(mdtPrds.cptr(), m_doRPCClust ? rpcPrdCont : nullptr, m_doTGCClust ? tgcPrdCont : nullptr, tgcTruthColl,
                                              rpcTruthColl, segmentContainer.get());
    }

    ATH_MSG_DEBUG("segments before overlap removal: " << segmentContainer->size());
    m_segmentOverlapRemovalTool->removeDuplicates(*segmentContainer);

    ATH_MSG_DEBUG(" Segments after overlap removal: " << segmentContainer->size());

    if (!m_csc2dSegmentFinder.empty() && !m_csc4dSegmentFinder.empty()) {
        std::vector<const Muon::CscPrepDataCollection*> cscCols;

        SG::ReadHandle<Muon::CscPrepDataContainer> cscPrds(m_cscPrdsKey, ctx);
        if (cscPrds.isValid()) {
            for (const Muon::CscPrepDataCollection* prep_coll : *cscPrds) {
                // skip empty collections
                if (prep_coll->empty()) continue;
                cscCols.push_back(prep_coll);
            }
            ATH_MSG_DEBUG("Retrieved CscPrepDataContainer " << cscCols.size());
            // reconstruct segments in the CSC eta and phi plane
            std::unique_ptr<MuonSegmentCombinationCollection> csc2dSegmentCombinations = m_csc2dSegmentFinder->find(cscCols, ctx);
            // combine CSC segments in eta and phi plane if any were found
            if (csc2dSegmentCombinations) {
                std::unique_ptr<MuonSegmentCombinationCollection> csc4dSegmentCombinations =
                    m_csc4dSegmentFinder->find(*csc2dSegmentCombinations, ctx);
                if (csc4dSegmentCombinations) {
                    // now copy the segments into the collection, not optimal as unneeded copy
                    for (const Muon::MuonSegmentCombination* combi: *csc4dSegmentCombinations) {
                        if (!combi) {
                            ATH_MSG_INFO(" empty MuonSegmentCombination!!! ");
                            continue;
                        }
                        unsigned int nstations = combi->numberOfStations();
                        // loop over chambers in combi and extract segments
                        for (unsigned int i = 0; i < nstations; ++i) {
                            // loop over segments in station
                            Muon::MuonSegmentCombination::SegmentVec* segments = combi->stationSegments(i);
                            // check if not empty
                            if (!segments || segments->empty()) continue;
                            // loop over new segments, copy them into collection
                            for (std::unique_ptr<Muon::MuonSegment>& seg : *segments)
                                segmentContainer->push_back(std::move(seg));  // releasing so the handle can take ownership
                        }
                    }
                }
            }
        }
    }
    if (msgLvl(MSG::DEBUG)){
        ATH_MSG_DEBUG("Number of segments found " << segmentContainer->size());
        for (Trk::Segment* tseg : *segmentContainer) {
            ATH_MSG_DEBUG(m_printer->print(*(dynamic_cast<Muon::MuonSegment*>(tseg))));
        }
    }

    SG::WriteHandle<Trk::SegmentCollection> handle(m_segmentCollectionKey, ctx);
    ATH_CHECK(handle.record(std::move(segmentContainer)));
    if (m_idHelperSvc->recoMM() || m_idHelperSvc->recosTgc()) {
        SG::WriteHandle<Trk::SegmentCollection> handle_segNSW(m_segmentNSWCollectionKey, ctx);
        ATH_CHECK(handle_segNSW.record(std::move(nswSegmentContainer)));  
    }
    return StatusCode::SUCCESS;
}  // execute

void MuonSegmentFinderAlg::createSegmentsFromClusters(const EventContext& ctx, const Muon::MuonPatternCombination* patt, Trk::SegmentCollection* segments, Trk::SegmentCollection* segmentsNSW) const {
    // turn the PRD into MuonCluster
    std::map<int, std::vector<const Muon::MuonClusterOnTrack*> > clustersPerSector;
    for (const Muon::MuonPatternChamberIntersect&  it :patt->chamberData()) {
        if (it.prepRawDataVec().empty()) continue;
        const Identifier& id = it.prepRawDataVec().front()->identify();
        if (!m_idHelperSvc->isMM(id) && !m_idHelperSvc->issTgc(id)) continue;
        for (const Trk::PrepRawData* pit : it.prepRawDataVec()) {
            const Muon::MuonCluster* cl = dynamic_cast<const Muon::MuonCluster*>(pit);
            if (!cl) continue;
            int sector = m_idHelperSvc->sector(id);
            std::vector<const Muon::MuonClusterOnTrack*>& clusters = clustersPerSector[sector];

            if (m_idHelperSvc->isMM(pit->identify())) {
                const Muon::MuonClusterOnTrack* clust = m_mmClusterCreator->createRIO_OnTrack(*cl, cl->globalPosition());
                clusters.push_back(clust);
            } else {  //  must be an sTGC prd
                const Muon::MuonClusterOnTrack* clust = m_clusterCreator->createRIO_OnTrack(*cl, cl->globalPosition());
                clusters.push_back(clust);
            }
        }
    }

    for (auto& sit :clustersPerSector) {
        std::vector<const Muon::MuonClusterOnTrack*>& clusters = sit.second;
        std::vector<std::unique_ptr<Muon::MuonSegment>> segVec;
        m_clusterSegMakerNSW->find(ctx, clusters, segVec, segments, segmentsNSW);

        // cleanup the memory
        for (const Muon::MuonClusterOnTrack* clus : clusters) { delete clus; }
    }
}

void MuonSegmentFinderAlg::createSegmentsWithMDTs(const Muon::MuonPatternCombination* patcomb, Trk::SegmentCollection* segs,
                                                  const std::vector<const Muon::RpcPrepDataCollection*>& rpcCols,
                                                  const std::vector<const Muon::TgcPrepDataCollection*>& tgcCols,
                                                  const EventContext& ctx) const {
    if (m_idHelperSvc->hasMM() && m_idHelperSvc->hasSTgc()) {
        // break the pattern combination into regions and calibrate the PRDs
        std::set<int> calibratedRegions;
        std::vector<Muon::MuonPatternChamberIntersect>::const_iterator it = patcomb->chamberData().begin();
        for (; it != patcomb->chamberData().end(); ++it) {
            if ((*it).prepRawDataVec().empty()) continue;
            Identifier id = (*it).prepRawDataVec().front()->identify();
            if (m_idHelperSvc->isMM(id) || m_idHelperSvc->issTgc(id)) continue;

            int regionID = m_patternCalibration->getRegionId(id);

            // check the chamber is not part of a region already calibrated
            std::pair<std::set<int>::iterator, bool> val = calibratedRegions.insert(regionID);
            if (!val.second) continue;
            ATH_MSG_VERBOSE(" Region " << regionID << " adding chamber " << m_idHelperSvc->toStringChamber(id) << " size "
                                       << (*it).prepRawDataVec().size());

            std::vector<Muon::MuonPatternChamberIntersect> chambers;
            chambers.push_back(*it);
            for (std::vector<Muon::MuonPatternChamberIntersect>::const_iterator rit = (it + 1); rit != patcomb->chamberData().end();
                 ++rit) {
                if ((*rit).prepRawDataVec().empty()) continue;
                Identifier id2 = (*rit).prepRawDataVec().front()->identify();
                if (m_idHelperSvc->isMM(id2) || m_idHelperSvc->issTgc(id2)) continue;

                int region = m_patternCalibration->getRegionId(id2);
                if (region == regionID) {
                    ATH_MSG_VERBOSE("  adding additional chamber " << m_idHelperSvc->toStringChamber(id2) << " size "
                                                                   << (*rit).prepRawDataVec().size());
                    chambers.push_back(*rit);
                }
            }

            Trk::TrackParameters* trkpars = nullptr;
            if (patcomb->trackParameter()) trkpars = patcomb->trackParameter()->clone();
            Muon::MuonPatternCombination pattern(trkpars, chambers);
            ATH_MSG_VERBOSE(" created pattern for region: number of chambers " << chambers.size());
            bool hasPhiMeasurements = m_patternCalibration->checkForPhiMeasurements(pattern);
            // calibrate the hits
            Muon::IMuonPatternCalibration::ROTsPerRegion hitsPerRegion;
            m_patternCalibration->calibrate(pattern, hitsPerRegion, ctx);

            Muon::IMuonPatternCalibration::ROTsPerRegion::iterator rotit = hitsPerRegion.begin();
            for (; rotit != hitsPerRegion.end(); ++rotit) {
                std::vector<std::vector<const Muon::MdtDriftCircleOnTrack*> >::iterator mdtit = rotit->mdts.begin();
                for (; mdtit != rotit->mdts.end(); ++mdtit) {
                    if (mdtit->empty()) continue;
                    ATH_MSG_VERBOSE("Calling segment finding for sector " << m_idHelperSvc->toString(mdtit->front()->identify()));
                    // fit the segments
                    m_segmentMaker->find(rotit->regionPos, rotit->regionDir, *mdtit, rotit->clusters, hasPhiMeasurements, segs,
                                         rotit->regionDir.mag());

                }  // end loop on hits per region

            }  // end loop on regions (rotit)

            // memory cleanup
            m_patternCalibration->clearRotsPerRegion(hitsPerRegion);
            // store the region ID
            calibratedRegions.insert(regionID);

        }  // end loop on chambers in the pattern
    } else {
        m_patternSegmentMaker->find(*patcomb, rpcCols, tgcCols, segs);
    }
}
