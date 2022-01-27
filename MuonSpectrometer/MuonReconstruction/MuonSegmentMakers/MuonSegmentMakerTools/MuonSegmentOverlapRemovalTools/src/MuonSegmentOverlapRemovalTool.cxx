/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonSegmentOverlapRemovalTool.h"

#include "MuonSegment/MuonSegment.h"
#include "MuonSegmentMakerUtils/CompareMuonSegmentKeys.h"
#include "MuonSegmentMakerUtils/MuonSegmentKey.h"
#include "TrkEventPrimitives/FitQuality.h"

namespace Muon {

    MuonSegmentOverlapRemovalTool::MuonSegmentOverlapRemovalTool(const std::string& ty, const std::string& na, const IInterface* pa) :
        AthAlgTool(ty, na, pa) {
        declareInterface<IMuonSegmentOverlapRemovalTool>(this);
    }

    StatusCode MuonSegmentOverlapRemovalTool::initialize() {
        ATH_CHECK(AlgTool::initialize());
        ATH_CHECK(m_edmHelperSvc.retrieve());
        ATH_CHECK(m_printer.retrieve());
        ATH_CHECK(m_idHelperSvc.retrieve());
        return StatusCode::SUCCESS;
    }

    void MuonSegmentOverlapRemovalTool::removeDuplicates(Trk::SegmentCollection& segments) const {
        ATH_MSG_DEBUG(" working on segment vector of size " << segments.size());

        CompareMuonSegmentKeys compareSegmentKeys{};

        // create a vector with pairs of MuonSegmentKey and a pointer to the corresponding segment to resolve ambiguities
        std::vector<std::pair<MuonSegmentKey, MuonSegment*>> goodSegments;
        goodSegments.reserve(segments.size());
        std::vector<MuonSegment*> segmentsToErase;

        // loop over new segments, copy them into collection
        for (Trk::Segment* tseg : segments) {
            // create segment key object
            MuonSegment* seg = dynamic_cast<MuonSegment*>(tseg);
            if (!seg) {
                ATH_MSG_WARNING("segment is not MuonSegment");
                continue;
            }
            MuonSegmentKey sk{*seg};
            Identifier chId = m_edmHelperSvc->chamberId(*seg);
            bool isCsc = m_idHelperSvc->isCsc(chId);

            // should this segment be inserted?
            bool insertAsGood = true;

            // loop over already accepted segments and compare the current segment with the good ones
            for (std::pair<MuonSegmentKey, MuonSegment*>& good_seg : goodSegments) {
                CompareMuonSegmentKeys::OverlapResult overlapResult = compareSegmentKeys(good_seg.first, sk);
                if (overlapResult == CompareMuonSegmentKeys::Identical) {
                    // for now keep the one with the best chi2
                    const Trk::FitQuality* fq1 = good_seg.second->fitQuality();
                    const Trk::FitQuality* fq2 = seg->fitQuality();
                    double chi2good = fq1 ? fq1->chiSquared() : 1e10;
                    double chi2current = fq2 ? fq2->chiSquared() : 1e10;
                    // we should not insert this segment
                    if (chi2good > chi2current) {
                        ATH_MSG_VERBOSE(" replacing (chi2) " << m_printer->print(*good_seg.second) << std::endl
                                                             << " with      " << m_printer->print(*seg));

                        // good segment of worse quality, replace it
                        segmentsToErase.push_back(good_seg.second);
                        good_seg.first = sk;
                        good_seg.second = seg;

                    } else {
                        ATH_MSG_VERBOSE(" discarding (chi2) " << m_printer->print(*seg));
                        segmentsToErase.push_back(seg);
                    }
                    insertAsGood = false;
                    break;
                } else if (overlapResult == CompareMuonSegmentKeys::SuperSet) {
                    // good segment of better quality, keep it and discard current segment
                    ATH_MSG_VERBOSE(" discarding (subset) " << m_printer->print(*seg));
                    segmentsToErase.push_back(seg);
                    insertAsGood = false;
                    break;
                } else if (overlapResult == CompareMuonSegmentKeys::SubSet) {
                    // good segment of worse quality, replace it
                    ATH_MSG_VERBOSE(" replacing (superset) " << m_printer->print(*good_seg.second) << std::endl
                                                             << " with      " << m_printer->print(*seg));
                    segmentsToErase.push_back(good_seg.second);
                    good_seg.first = sk;
                    good_seg.second = seg;
                    insertAsGood = false;
                    break;
                } else if (overlapResult == CompareMuonSegmentKeys::PartialOverlap && m_removePartialOverlaps) {
                    // keep CSC segments with partial overlap
                    if (isCsc) continue;

                    // partial overlap, for now keep the one most hits
                    if (compareSegmentKeys.segment1Size < compareSegmentKeys.segment2Size) {
                        ATH_MSG_VERBOSE(" replacing (mdt hits) " << m_printer->print(*good_seg.second) << std::endl
                                                                 << " with      " << m_printer->print(*seg));

                        // good segment of worse quality, replace it
                        segmentsToErase.push_back(good_seg.second);
                        good_seg.first = sk;
                        good_seg.second = seg;
                        insertAsGood = false;
                        break;
                    } else if (compareSegmentKeys.segment1Size == compareSegmentKeys.segment2Size) {
                        // same size

                        // if the original entry has more trigger hits discard current segment
                        if (compareSegmentKeys.segment1SizeTrigger > compareSegmentKeys.segment2SizeTrigger) {
                            ATH_MSG_VERBOSE(" discarding (trigger hits) " << m_printer->print(*seg));
                            segmentsToErase.push_back(seg);
                            insertAsGood = false;
                            break;
                        } else if (compareSegmentKeys.segment1SizeTrigger < compareSegmentKeys.segment2SizeTrigger) {
                            // good segment of worse quality, replace it
                            ATH_MSG_VERBOSE(" replacing (trigger hits) " << m_printer->print(*good_seg.second) << std::endl
                                                                         << " with      " << m_printer->print(*seg));
                            segmentsToErase.push_back(good_seg.second);
                            good_seg.first = sk;
                            good_seg.second = seg;
                            insertAsGood = false;
                            break;
                        } else if (compareSegmentKeys.segment1SizeTrigger == compareSegmentKeys.segment2SizeTrigger) {
                            // for now look at the overlap fraction
                        }
                        // calculate the overlap fraction
                        double overlapFraction = (double)compareSegmentKeys.intersectionSize / (double)compareSegmentKeys.segment1Size;
                        if (overlapFraction > m_overlapFractionCut) {
                            ATH_MSG_VERBOSE(" discarding (overlap fraction) " << m_printer->print(*seg));
                            segmentsToErase.push_back(seg);
                            insertAsGood = false;
                            break;
                        }

                        continue;
                    } else {
                        ATH_MSG_VERBOSE(" discarding " << m_printer->print(*seg));
                        segmentsToErase.push_back(seg);
                    }
                    insertAsGood = false;
                    break;
                } else if (overlapResult == CompareMuonSegmentKeys::NoOverlap) {
                    // no overlap, move on to next segment
                    continue;
                } else if (overlapResult == CompareMuonSegmentKeys::Unknown) {
                    ATH_MSG_WARNING(" Got invalid return argument comparing segments: " << compareSegmentKeys.print(overlapResult));
                    continue;
                }
            }

            // add segment if needed
            if (insertAsGood) {
                ATH_MSG_VERBOSE(" no overlap " << m_printer->print(*seg));
                goodSegments.emplace_back(std::move(sk), seg);
            }
        }

        // erase rejected segments
        for (MuonSegment* to_erase : segmentsToErase) {
            for (Trk::SegmentCollection::iterator sit = segments.begin(); sit != segments.end(); ++sit) {
                // remove bad segments from collection, then delete them
                Trk::Segment* tseg = *sit;
                if (to_erase == dynamic_cast<MuonSegment*>(tseg)) {
                    segments.erase(sit);
                    break;
                }
            }
        }
    }

    std::unique_ptr<MuonSegmentCombination> MuonSegmentOverlapRemovalTool::removeDuplicates(const MuonSegmentCombination& combi) const {
        // store pointers to segments that should be kept
        std::set<MuonSegment*> segmentsToBeKept;
        unsigned int nsegments = 0;  // total number of segments

        unsigned int nstations = combi.numberOfStations();

        ATH_MSG_DEBUG(" removing duplicate from MuonSegmentCombination with  " << nstations << " stations");

        // loop over segments, add them to the chamber layers
        // loop over chambers in combi and extract segments
        for (unsigned int i = 0; i < nstations; ++i) {
            // loop over segments in station
            MuonSegmentCombination::SegmentVec* stationSegs = combi.stationSegments(i);

            // check if not empty
            if (!stationSegs || stationSegs->empty()) continue;

            nsegments += stationSegs->size();

            // clean segments
            SegVec cleanedSegments = removeDuplicates(*stationSegs);
            segmentsToBeKept.insert(cleanedSegments.begin(), cleanedSegments.end());
        }

        // check whether all segments were kept
        if (nsegments == segmentsToBeKept.size()) {
            ATH_MSG_DEBUG("  no segments removed ");
            return nullptr;
        }

        std::unique_ptr<MuonSegmentCombination> newCombi = std::make_unique<MuonSegmentCombination>();

        // create a new combination removing segments that overlap
        for (unsigned int i = 0; i < nstations; ++i) {
            // loop over segments in station
            MuonSegmentCombination::SegmentVec* stationSegs = combi.stationSegments(i);

            // check if not empty
            if (!stationSegs || stationSegs->empty()) continue;

            std::unique_ptr<std::vector<std::unique_ptr<MuonSegment>>> segVec =
                std::make_unique<std::vector<std::unique_ptr<MuonSegment>>>();
            segVec->reserve(stationSegs->size());

            // loop over segments and add them if they are in the keep set
            for (const std::unique_ptr<MuonSegment>& seg : *stationSegs) {
                if (!segmentsToBeKept.count(seg.get())) {
                    continue;
                } else {
                    segVec->emplace_back(seg->clone());
                }
            }
            // if not empty add segments
            if (!segVec->empty()) newCombi->addSegments(std::move(segVec));
        }

        ATH_MSG_DEBUG(" removed " << nsegments - segmentsToBeKept.size() << " duplicates, new size " << segmentsToBeKept.size());

        return newCombi;
    }

    // this applies only to the case of cleaning MuonSegmentCombinations, above
    // currently not used, maybe it can be removed entirely?
    MuonSegmentOverlapRemovalTool::SegVec MuonSegmentOverlapRemovalTool::removeDuplicates(
        MuonSegmentCombination::SegmentVec& segments) const {
        ATH_MSG_DEBUG(" working on segment vector of size " << segments.size());

        CompareMuonSegmentKeys compareSegmentKeys{};

        // create a vector with pairs of MuonSegmentKey and a pointer to the corresponding segment to resolve ambiguities
        std::vector<std::pair<MuonSegmentKey, MuonSegment*>> goodSegments;
        goodSegments.reserve(segments.size());

        // loop over new segments, copy them into collection
        for (const std::unique_ptr<MuonSegment>& seg : segments) {
            // create segment key object
            MuonSegmentKey sk(*seg);
            Identifier chId = m_edmHelperSvc->chamberId(*seg);
            bool isCsc = m_idHelperSvc->isCsc(chId);

            // should this segment be inserted?
            bool insertAsGood(true);

            // loop over already accepted segments and compare the current segment with the good ones
            for (std::pair<MuonSegmentKey, MuonSegment*>& good_seg : goodSegments) {
                CompareMuonSegmentKeys::OverlapResult overlapResult = compareSegmentKeys(good_seg.first, sk);
                if (overlapResult == CompareMuonSegmentKeys::Identical) {
                    // for now keep the one with the best chi2
                    const Trk::FitQuality* fq1 = good_seg.second->fitQuality();
                    const Trk::FitQuality* fq2 = seg->fitQuality();
                    double chi2good = fq1 ? fq1->chiSquared() : 1e10;
                    double chi2current = fq2 ? fq2->chiSquared() : 1e10;
                    // we should not insert this segment
                    if (chi2good > chi2current) {
                        ATH_MSG_VERBOSE(" replacing (chi2) " << m_printer->print(*good_seg.second) << std::endl
                                                             << " with      " << m_printer->print(*seg));

                        // good segment of worse quality, replace it
                        good_seg.first = sk;
                        good_seg.second = seg.get();
                    } else {
                        ATH_MSG_VERBOSE(" discarding (chi2) " << m_printer->print(*seg));
                    }
                    insertAsGood = false;
                    break;
                } else if (overlapResult == CompareMuonSegmentKeys::SuperSet) {
                    // good segment of better quality, keep it and discard current segment
                    ATH_MSG_VERBOSE(" discarding (subset) " << m_printer->print(*seg));
                    insertAsGood = false;
                    break;
                } else if (overlapResult == CompareMuonSegmentKeys::SubSet) {
                    // good segment of worse quality, replace it
                    ATH_MSG_VERBOSE(" replacing (superset) " << m_printer->print(*good_seg.second) << std::endl
                                                             << " with      " << m_printer->print(*seg));
                    good_seg.first = sk;
                    good_seg.second = seg.get();
                    insertAsGood = false;
                    break;
                } else if (overlapResult == CompareMuonSegmentKeys::PartialOverlap && m_removePartialOverlaps) {
                    // keep CSC segments with partial overlap
                    if (isCsc) continue;

                    // partial overlap, for now keep the one most hits
                    if (compareSegmentKeys.segment1Size < compareSegmentKeys.segment2Size) {
                        ATH_MSG_VERBOSE(" replacing (mdt hits) " << m_printer->print(*good_seg.second) << std::endl
                                                                 << " with      " << m_printer->print(*seg));

                        // good segment of worse quality, replace it
                        good_seg.first = sk;
                        good_seg.second = seg.get();
                    } else if (compareSegmentKeys.segment1Size == compareSegmentKeys.segment2Size) {
                        // same size

                        // if the original entry has more trigger hits discard current segment
                        if (compareSegmentKeys.segment1SizeTrigger > compareSegmentKeys.segment2SizeTrigger) {
                            ATH_MSG_VERBOSE(" discarding (trigger hits) " << m_printer->print(*seg));
                            insertAsGood = false;
                            break;
                        } else if (compareSegmentKeys.segment1SizeTrigger < compareSegmentKeys.segment2SizeTrigger) {
                            // good segment of worse quality, replace it
                            ATH_MSG_VERBOSE(" replacing (trigger hits) " << m_printer->print(*good_seg.second) << std::endl
                                                                         << " with      " << m_printer->print(*seg));
                            good_seg.first = sk;
                            good_seg.second = seg.get();
                            insertAsGood = false;
                            break;
                        } else if (compareSegmentKeys.segment1SizeTrigger == compareSegmentKeys.segment2SizeTrigger) {
                            // for now look at the overlap fraction
                        }
                        // calculate the overlap fraction
                        double overlapFraction = (double)compareSegmentKeys.intersectionSize / (double)compareSegmentKeys.segment1Size;
                        if (overlapFraction > m_overlapFractionCut) {
                            ATH_MSG_VERBOSE(" discarding (overlap fraction) " << m_printer->print(*seg));
                            insertAsGood = false;
                            break;
                        }

                        continue;
                    } else {
                        ATH_MSG_VERBOSE(" discarding " << m_printer->print(*seg));
                    }
                    insertAsGood = false;
                    break;
                } else if (overlapResult == CompareMuonSegmentKeys::NoOverlap) {
                    // no overlap, move on to next segment
                    continue;
                } else if (overlapResult == CompareMuonSegmentKeys::Unknown) {
                    ATH_MSG_WARNING(" Got invalid return argument comparing segments: " << compareSegmentKeys.print(overlapResult));
                    continue;
                }
            }

            // add segment if needed
            if (insertAsGood) {
                ATH_MSG_VERBOSE(" no overlap " << m_printer->print(*seg));
                goodSegments.emplace_back(sk, seg.get());
            }
        }

        SegVec cleanedSegments;
        for (const std::pair<MuonSegmentKey, MuonSegment*>& good_seg : goodSegments) { cleanedSegments.push_back(good_seg.second); }
        ATH_MSG_DEBUG(" Segments after removal " << cleanedSegments.size() << " total number of removed segments "
                                                 << segments.size() - cleanedSegments.size());

        return cleanedSegments;
    }

}  // namespace Muon
