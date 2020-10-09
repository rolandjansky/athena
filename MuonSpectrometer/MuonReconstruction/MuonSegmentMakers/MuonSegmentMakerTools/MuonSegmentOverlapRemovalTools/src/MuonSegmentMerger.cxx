/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonSegmentMerger.h"

#include "MuonClusterization/RpcHitClustering.h"
#include "MuonClusterization/TgcHitClustering.h"
#include "MuonCompetingRIOsOnTrack/CompetingMuonClustersOnTrack.h"
#include "MuonRIO_OnTrack/MuonClusterOnTrack.h"
#include "MuonSegment/MuonSegment.h"
#include "MuonSegment/MuonSegmentCombination.h"
#include "MuonSegmentMakerUtils/CompareMuonSegmentKeys.h"
#include "MuonSegmentMakerUtils/MuonSegmentKey.h"
#include "TrkEventPrimitives/FitQuality.h"

namespace Muon {

MuonSegmentMerger::MuonSegmentMerger(const std::string& ty, const std::string& na, const IInterface* pa)
    : AthAlgTool(ty, na, pa)
{
    declareInterface<IMuonSegmentMerger>(this);
}

StatusCode
MuonSegmentMerger::initialize()
{
    ATH_CHECK(AthAlgTool::initialize());
    ATH_CHECK(m_edmHelperSvc.retrieve());
    ATH_CHECK(m_printer.retrieve());
    ATH_CHECK(m_idHelperSvc.retrieve());
    ATH_CHECK(m_triggerHitsAssociator.retrieve());
    return StatusCode::SUCCESS;
}

MuonSegmentMerger::SegVec
MuonSegmentMerger::findDuplicates(const MuonSegmentMerger::SegVec& segments) const
{

    ATH_MSG_DEBUG(" working on segment vector of size " << segments.size());

    CompareMuonSegmentKeys compareSegmentKeys;
    SegVec                 outputSegments;

    // create a vector with pairs of MuonSegmentKey and a pointer to the corresponding segment to resolve ambiguities
    std::vector<std::pair<MuonSegmentKey, std::vector<const MuonSegment*> > > identicalSegments;
    identicalSegments.reserve(segments.size());

    // loop over new segments, copy them into collection
    SegCit sit     = segments.begin();
    SegCit sit_end = segments.end();
    for (; sit != sit_end; ++sit) {

        bool inserted = false;

        // create segment key object
        MuonSegmentKey sk(**sit);
        Identifier     chId = m_edmHelperSvc->chamberId(**sit);
        if (!m_idHelperSvc->isMdt(chId)) {
            outputSegments.push_back(new MuonSegment(**sit));
            continue;
        }
        // loop over already accepted segments and compare the current segment with the good ones
        std::vector<std::pair<MuonSegmentKey, std::vector<const MuonSegment*> > >::iterator gsit =
            identicalSegments.begin();
        std::vector<std::pair<MuonSegmentKey, std::vector<const MuonSegment*> > >::iterator gsit_end =
            identicalSegments.end();
        for (; gsit != gsit_end; ++gsit) {
            CompareMuonSegmentKeys::OverlapResult overlapResult = compareSegmentKeys(gsit->first, sk, true);
            if (overlapResult == CompareMuonSegmentKeys::Identical) {
                gsit->second.push_back(*sit);
                // 	  ATH_MSG_DEBUG(" found identical segment " << m_printer->print(**sit) << " index " <<
                // std::distance(identicalSegments.begin(),gsit)
                // 			  << " total size " << identicalSegments.size()  );
                inserted = true;
                break;
            }
        }

        // add segment if needed
        if (!inserted) {
            // 	ATH_MSG_DEBUG(" no overlap, adding " << m_printer->print(**sit) << " total size " <<
            // identicalSegments.size());
            std::vector<const MuonSegment*> vec;
            vec.push_back(*sit);
            identicalSegments.push_back(std::make_pair(sk, vec));
        }
    }


    ATH_MSG_DEBUG(" input segment size " << segments.size() << " after overlap merging " << identicalSegments.size());


    std::vector<std::pair<MuonSegmentKey, std::vector<const MuonSegment*> > >::iterator gsit =
        identicalSegments.begin();
    std::vector<std::pair<MuonSegmentKey, std::vector<const MuonSegment*> > >::iterator gsit_end =
        identicalSegments.end();
    for (; gsit != gsit_end; ++gsit) {

        if (gsit->second.size() > 0) {
            std::stable_sort(gsit->second.begin(), gsit->second.end(), LocalSortSegmentsByChi2());
            std::vector<SegVec>           resolvedSegments = resolveClusterOverlaps(gsit->second);
            std::vector<SegVec>::iterator it               = resolvedSegments.begin();
            std::vector<SegVec>::iterator it_end           = resolvedSegments.end();
            for (; it != it_end; ++it) {

                const MuonSegment* mergedSegment = merge(*it);
                if (mergedSegment)
                    outputSegments.push_back(mergedSegment);
                else {
                    // outputSegments.push_back( new MuonSegment( *gsit->second.front() ) );
                    ATH_MSG_DEBUG(
                        "Failed to merge segments of a total size identical ones: " << identicalSegments.size());
                }
            }
        }
    }
    return outputSegments;
}

std::vector<MuonSegmentMerger::SegVec>
MuonSegmentMerger::resolveClusterOverlaps(const MuonSegmentMerger::SegVec& segments) const
{

    if (segments.empty()) return std::vector<MuonSegmentMerger::SegVec>();

    const MuonSegment* aSegment = 0;

    std::map<IdentifierHash, std::vector<const RpcPrepData*> >     rpcsPerCollection;
    std::map<IdentifierHash, std::vector<const TgcPrepData*> >     tgcsPerCollection;
    std::map<const MuonCluster*, std::vector<const MuonSegment*> > prdToSegmentMap;

    ATH_MSG_DEBUG(" resolving cluster overlap: segments " << segments.size());
    SegCit                                 sit     = segments.begin();
    SegCit                                 sit_end = segments.end();
    std::vector<const MuonClusterOnTrack*> clusters;
    for (; sit != sit_end; ++sit) {
        ATH_MSG_DEBUG(" " << m_printer->print(**sit));

        // Identifier chid = m_edmHelperSvc->chamberId(**sit);

        if (!aSegment) aSegment = *sit;

        std::vector<const Trk::MeasurementBase*>::const_iterator hit     = (*sit)->containedMeasurements().begin();
        std::vector<const Trk::MeasurementBase*>::const_iterator hit_end = (*sit)->containedMeasurements().end();
        for (; hit != hit_end; ++hit) {
            const MuonClusterOnTrack* clot = dynamic_cast<const MuonClusterOnTrack*>(*hit);
            if (clot) {
                ATH_MSG_DEBUG("   " << m_printer->print(*clot));
                const MuonCluster* cl = clot->prepRawData();
                if (cl) {

                    prdToSegmentMap[cl].push_back(*sit);

                    const RpcPrepData* rpc = dynamic_cast<const RpcPrepData*>(cl);
                    if (rpc) {
                        rpcsPerCollection[rpc->collectionHash()].push_back(const_cast<RpcPrepData*>(rpc));
                    } else {
                        const TgcPrepData* tgc = dynamic_cast<const TgcPrepData*>(cl);
                        if (tgc) {
                            tgcsPerCollection[tgc->collectionHash()].push_back(const_cast<TgcPrepData*>(tgc));
                        }
                    }
                }
            }

            const CompetingMuonClustersOnTrack* crot = dynamic_cast<const CompetingMuonClustersOnTrack*>(*hit);
            if (crot) {
                std::vector<const MuonClusterOnTrack*>::const_iterator clit     = crot->containedROTs().begin();
                std::vector<const MuonClusterOnTrack*>::const_iterator clit_end = crot->containedROTs().end();
                for (; clit != clit_end; ++clit) {
                    const MuonClusterOnTrack* clot = *clit;

                    if (clot) {
                        ATH_MSG_DEBUG("   " << m_printer->print(*clot));
                        const MuonCluster* cl = clot->prepRawData();
                        if (cl) {

                            const RpcPrepData* rpc = dynamic_cast<const RpcPrepData*>(cl);
                            if (rpc) {
                                rpcsPerCollection[rpc->collectionHash()].push_back(const_cast<RpcPrepData*>(rpc));
                            } else {
                                const TgcPrepData* tgc = dynamic_cast<const TgcPrepData*>(cl);
                                if (tgc) {
                                    tgcsPerCollection[tgc->collectionHash()].push_back(const_cast<TgcPrepData*>(tgc));
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    ATH_MSG_DEBUG("collected trigger hits: RPCs  " << rpcsPerCollection.size() << " TGCs   "
                                                   << tgcsPerCollection.size());

    if (prdToSegmentMap.empty()) {
        std::vector<SegVec> overlappingSegments;
        overlappingSegments.push_back(segments);
        return overlappingSegments;
    }

    std::map<int, std::set<const MuonSegment*> > clusterToSegmentMap;
    std::map<const MuonSegment*, std::set<int> > segmentToClusterMap;

    if (!rpcsPerCollection.empty()) {
        std::map<IdentifierHash, std::vector<const RpcPrepData*> >::const_iterator rcit     = rpcsPerCollection.begin();
        std::map<IdentifierHash, std::vector<const RpcPrepData*> >::const_iterator rcit_end = rpcsPerCollection.end();
        RpcHitClusteringObj rpcClustering(&m_idHelperSvc->rpcIdHelper());
        int                 offset = 0;
        for (; rcit != rcit_end; ++rcit) {
            ATH_MSG_DEBUG(" rpc Prd size " << rcit->second.size());
            if (!rpcClustering.cluster(rcit->second)) {
                continue;
            }
            ATH_MSG_DEBUG("RpcHitClusteringObj size " << rpcClustering.clustersPhi.size());
            for (unsigned int i = 0; i < rpcClustering.clustersPhi.size(); ++i) {
                ATH_MSG_DEBUG(" RpcHitClusteringObj nr " << i << " hits "
                                                         << rpcClustering.clustersPhi[i].hitList.size());
                RpcClusterObj::HitCit hit     = rpcClustering.clustersPhi[i].hitList.begin();
                RpcClusterObj::HitCit hit_end = rpcClustering.clustersPhi[i].hitList.end();
                for (; hit != hit_end; ++hit) {
                    std::map<const MuonCluster*, std::vector<const MuonSegment*> >::iterator pos =
                        prdToSegmentMap.find(*hit);
                    if (pos == prdToSegmentMap.end()) {
                        ATH_MSG_WARNING("Prd not found, this should not happen!!");
                        continue;
                    }
                    int index = 1000 * offset + i;
                    clusterToSegmentMap[index].insert(pos->second.begin(), pos->second.end());
                    std::vector<const MuonSegment*>::iterator sit     = pos->second.begin();
                    std::vector<const MuonSegment*>::iterator sit_end = pos->second.end();
                    for (; sit != sit_end; ++sit) segmentToClusterMap[*sit].insert(index);
                }
            }
            ++offset;
        }
    }
    if (!tgcsPerCollection.empty()) {
        std::map<IdentifierHash, std::vector<const TgcPrepData*> >::const_iterator rcit     = tgcsPerCollection.begin();
        std::map<IdentifierHash, std::vector<const TgcPrepData*> >::const_iterator rcit_end = tgcsPerCollection.end();
        TgcHitClusteringObj tgcClustering(&m_idHelperSvc->tgcIdHelper());
        int                 offset = 0;
        for (; rcit != rcit_end; ++rcit) {
            if (!tgcClustering.cluster(rcit->second)) {
                continue;
            }
            for (unsigned int i = 0; i < tgcClustering.clustersPhi.size(); ++i) {
                TgcClusterObj::HitCit hit     = tgcClustering.clustersPhi[i].hitList.begin();
                TgcClusterObj::HitCit hit_end = tgcClustering.clustersPhi[i].hitList.end();
                for (; hit != hit_end; ++hit) {
                    std::map<const MuonCluster*, std::vector<const MuonSegment*> >::iterator pos =
                        prdToSegmentMap.find(*hit);
                    if (pos == prdToSegmentMap.end()) {
                        ATH_MSG_WARNING("Prd not found, this should not happen!!");
                        continue;
                    }
                    int tgcIndex = 100000 + 1000 * offset + i;
                    clusterToSegmentMap[tgcIndex].insert(pos->second.begin(), pos->second.end());
                    std::vector<const MuonSegment*>::iterator sit     = pos->second.begin();
                    std::vector<const MuonSegment*>::iterator sit_end = pos->second.end();
                    for (; sit != sit_end; ++sit) segmentToClusterMap[*sit].insert(tgcIndex);
                }
            }
            ++offset;
        }
    }

    // now try to merge segments that have a 100% overlap
    std::set<const MuonSegment*> usedSegments;
    std::vector<SegVec>          overlappingSegments;
    sit     = segments.begin();
    sit_end = segments.end();
    for (; sit != sit_end; ++sit) {

        // check whether this segment was already added
        if (usedSegments.count(*sit)) continue;
        usedSegments.insert(*sit);

        std::map<const MuonSegment*, std::set<int> >::iterator scit = segmentToClusterMap.find(*sit);
        if (scit == segmentToClusterMap.end()) continue;

        SegVec segmentsFound;
        segmentsFound.push_back(scit->first);

        // loop over clusters associated with this segment
        std::set<int>::iterator cit     = scit->second.begin();
        std::set<int>::iterator cit_end = scit->second.end();
        for (; cit != cit_end; ++cit) {

            // look up segments associated with this cluster
            std::map<int, std::set<const MuonSegment*> >::iterator pos = clusterToSegmentMap.find(*cit);
            if (pos == clusterToSegmentMap.end()) {
                ATH_MSG_WARNING("no segments found for this cluster, this should not happen!");
                continue;
            }

            SegCit sit2     = segments.begin();
            SegCit sit2_end = segments.end();
            for (; sit2 != sit2_end; ++sit2) {
                if (usedSegments.count(*sit2)) continue;
                if (!pos->second.count(*sit2)) continue;
                segmentsFound.push_back(*sit2);
                usedSegments.insert(*sit2);
            }
        }
        overlappingSegments.push_back(segmentsFound);
    }

    return overlappingSegments;
}


const MuonSegment*
MuonSegmentMerger::merge(const MuonSegmentMerger::SegVec& segments) const
{

    if (segments.empty()) return 0;

    const MuonSegment* aSegment = 0;

    ATH_MSG_DEBUG(" merging overlap " << segments.size());
    SegCit                                 sit     = segments.begin();
    SegCit                                 sit_end = segments.end();
    std::vector<const MuonClusterOnTrack*> clusters;
    for (; sit != sit_end; ++sit) {
        ATH_MSG_DEBUG(" " << m_printer->print(**sit));

        Identifier chid     = m_edmHelperSvc->chamberId(**sit);
        bool       isEndcap = m_idHelperSvc->isEndcap(chid);

        if (!aSegment) aSegment = *sit;

        std::vector<const Trk::MeasurementBase*>::const_iterator hit     = (*sit)->containedMeasurements().begin();
        std::vector<const Trk::MeasurementBase*>::const_iterator hit_end = (*sit)->containedMeasurements().end();
        for (; hit != hit_end; ++hit) {
            const MuonClusterOnTrack* clot = dynamic_cast<const MuonClusterOnTrack*>(*hit);
            if (clot) {
                ATH_MSG_DEBUG("   " << m_printer->print(*clot));

                if (m_idHelperSvc->isEndcap(clot->identify()) != isEndcap) {

                    if (segments.size() == 1) {
                        ATH_MSG_DEBUG("Inconsistent phi hit topology, keeping original");
                        return new MuonSegment(**sit);
                    } else {
                        ATH_MSG_DEBUG("Inconsistent phi hit topology, dropping phi hit");
                        continue;
                    }
                }
                clusters.push_back(clot);
                continue;
            }

            const CompetingMuonClustersOnTrack* crot = dynamic_cast<const CompetingMuonClustersOnTrack*>(*hit);
            if (crot) {
                std::vector<const MuonClusterOnTrack*>::const_iterator clit     = crot->containedROTs().begin();
                std::vector<const MuonClusterOnTrack*>::const_iterator clit_end = crot->containedROTs().end();
                for (; clit != clit_end; ++clit) {
                    const MuonClusterOnTrack* clot = *clit;

                    if (clot) {
                        ATH_MSG_DEBUG("   " << m_printer->print(*clot));

                        if (m_idHelperSvc->isEndcap(clot->identify()) != isEndcap) {

                            if (segments.size() == 1) {
                                ATH_MSG_DEBUG("Inconsistent phi hit topology, keeping original");
                                return new MuonSegment(**sit);
                            } else {
                                ATH_MSG_DEBUG("Inconsistent phi hit topology, dropping phi hit");
                                continue;
                            }
                        }
                        clusters.push_back(clot);
                        continue;
                    }
                }
            }
        }
    }
    if (!aSegment) return 0;

    ATH_MSG_DEBUG(" collected clusters " << clusters.size());

    const MuonSegment* seg = m_triggerHitsAssociator->associateTriggerHits(*aSegment, clusters, true);

    if (seg)
        ATH_MSG_DEBUG(" Merged segment  " << m_printer->print(*seg));
    else
        ATH_MSG_DEBUG(" trigger hit association failed");
    return seg;
}
}  // namespace Muon
