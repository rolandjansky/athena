/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonSegmentCombinationCleanerTool.h"

#include <algorithm>
#include <iostream>
#include <sstream>
#include <vector>

#include "MuonPattern/MuonPatternCombination.h"
#include "MuonSegment/MuonSegment.h"
#include "MuonSegment/MuonSegmentCombination.h"
#include "MuonSegment/MuonSegmentCombinationCollection.h"
#include "MuonSegmentMakerUtils/CompareMuonSegmentKeys.h"
#include "MuonSegmentMakerUtils/MuonSegmentKey.h"
#include "TrkEventPrimitives/FitQuality.h"

namespace Muon {

    MuonSegmentCombinationCleanerTool::MuonSegmentCombinationCleanerTool(const std::string& t, const std::string& n, const IInterface* p) :
        AthAlgTool(t, n, p) {
        declareInterface<IMuonSegmentCombinationCleanerTool>(this);
    }

    StatusCode MuonSegmentCombinationCleanerTool::initialize() {
        ATH_MSG_VERBOSE(" MuonSegmentCombinationCleanerTool::Initializing ");
        ATH_CHECK(m_printer.retrieve());
        ATH_CHECK(m_edmHelperSvc.retrieve());
        ATH_CHECK(m_idHelperSvc.retrieve());
        ATH_CHECK(m_overlapRemovalTool.retrieve());
        ATH_MSG_VERBOSE("End of Initializing");
        return StatusCode::SUCCESS;
    }

    std::unique_ptr<MuonSegmentCombinationCollection> MuonSegmentCombinationCleanerTool::clean(
        const MuonSegmentCombinationCollection& combiCol, MuonSegmentCombPatternCombAssociationMap* segPattMap) const {
        std::unique_ptr<MuonSegmentCombinationCollection> combiCleanCol = std::make_unique<MuonSegmentCombinationCollection>();

        cleanAndMergeCombis(combiCol, combiCleanCol, segPattMap);

        return combiCleanCol;
    }

    void MuonSegmentCombinationCleanerTool::cleanAndMergeCombis(const MuonSegmentCombinationCollection& combiCol,
                                                                std::unique_ptr<MuonSegmentCombinationCollection>& combiCleanCol,
                                                                MuonSegmentCombPatternCombAssociationMap* segPattMap) const {
        ATH_MSG_DEBUG(" cleaning combis " << combiCol.size());

        std::vector<MuonSegmentCombiSummary> summaries;

        for (const Muon::MuonSegmentCombination* combi : combiCol) {
            if (!combi) {
                ATH_MSG_INFO(" empty MuonSegmentCombination!!! ");
                continue;
            }

            MuonSegmentCombiSummary summary{*combi};
            fillSummary(summary);
            ATH_MSG_VERBOSE(print(summary));

            if (summary.stations.size() < 2) {
                // keep combination if Endcap middle
                if (summary.stations.count(MuonStationIndex::EM)) {
                    ATH_MSG_VERBOSE(" Keeping single station endcap middle MuonSegmentCombination ");

                } else {
                    ATH_MSG_DEBUG(" MuonSegmentCombination has too few station layers, rejecting  ");
                    continue;
                }
            }

            if (summary.stationsGood.empty()) {
                ATH_MSG_DEBUG(" MuonSegmentCombination has no station layer with good segment, rejecting  ");
                continue;
            }

            if (!subSetOfPrevious(summary, summaries)) { summaries.emplace_back(std::move(summary)); }
        }

        for (MuonSegmentCombiSummary& summary : summaries) {
            // check whether there is an original, if so pass the pointer else clone the combi as it comes from the input
            // collection

            std::unique_ptr<MuonSegmentCombination> finalCombi = m_overlapRemovalTool->removeDuplicates(summary.segmentCombination());

            /// No duplicates were actually removed
            if (!finalCombi) {
                if (summary.has_ownership()) {
                    finalCombi = summary.release();
                } else {
                    finalCombi = std::make_unique<MuonSegmentCombination>(*summary.get());
                }
            }

            const MuonSegmentCombination* originalCombi = summary.get();

            // lookup the patterncombi and add association
            unsigned int count = segPattMap->count(originalCombi);
            if (count != 1) {
                ATH_MSG_INFO(" This list should only have one entry!! ");
            } else {
                std::pair<MuonSegmentCombPatternCombAssociationMap::const_iterator,
                          MuonSegmentCombPatternCombAssociationMap::const_iterator>
                    range = segPattMap->equal_range(originalCombi);
                const Muon::MuonPatternCombination* pat = (range.first)->second;
                if (pat)
                    segPattMap->insert(std::make_pair(finalCombi.get(), pat));
                else
                    ATH_MSG_INFO(" The pattern pointer should never be zero!!!! ");
            }

            combiCleanCol->push_back(std::move(finalCombi));
        }
    }

    bool MuonSegmentCombinationCleanerTool::subSetOfPrevious(MuonSegmentCombiSummary& summary,
                                                             std::vector<MuonSegmentCombiSummary>& summaries) const {
        if (summaries.empty()) return false;

        ATH_MSG_DEBUG(" comparing combi with aleady handled ones ");
        bool hasOverlap = false;

        for (MuonSegmentCombiSummary& existing : summaries) {
            MuonSegmentCombiOverlapSummary overlap = calculateOverlap(summary, existing);

            // simplest case: no subsets
            if (overlap.subsetFirst.empty() && overlap.subsetSecond.empty()) {
                // no overlap
                if (overlap.shared.empty()) {
                    if (!m_mergeAllCombis) continue;
                }
                // complete overlap, keep previous
                if (overlap.uniqueFirst.empty() && overlap.uniqueSecond.empty()) {
                    ATH_MSG_VERBOSE("    -> complete overlap, keeping first ");
                    hasOverlap = true;
                    break;
                }
            }

            // second simple case, no unique segments in first and no subsets in second combi
            if (overlap.uniqueFirst.empty() && overlap.subsetSecond.empty()) {
                ATH_MSG_VERBOSE("    -> keeping selected combi ");
                hasOverlap = true;
                break;
            } else if (overlap.uniqueSecond.empty() && overlap.subsetFirst.empty()) {
                ATH_MSG_VERBOSE("    -> replacing selected combi ");
                existing = std::move(summary);
                hasOverlap = true;
                break;
            }

            // large overlap

            unsigned int overlappingSegments1 = overlap.subsetFirst.size() + overlap.shared.size();
            unsigned int overlappingSegments2 = overlap.subsetSecond.size() + overlap.shared.size();

            unsigned int uniqueSegments1 = overlap.uniqueFirst.size();
            unsigned int uniqueSegments2 = overlap.uniqueSecond.size();
            if (overlappingSegments1 > uniqueSegments1 || overlappingSegments2 > uniqueSegments2 || m_mergeAllCombis) {
                ATH_MSG_VERBOSE("    -> large overlap, merge candidate ");

                // set to make sure we are not adding segments twice
                std::set<MuonSegment*> addedSegments;
                std::vector<std::vector<MuonSegment*> > segmentsPerChamberLayer{MuonStationIndex::ChIndexMax};

                // first add shared segments, take one with best chi2
                for (std::pair<MuonSegment*, MuonSegment*>& sh_iter : overlap.shared) {
                    // select segment with best chi2
                    const Trk::FitQuality* fq1 = sh_iter.first->fitQuality();
                    const Trk::FitQuality* fq2 = sh_iter.second->fitQuality();
                    double chi2First = fq1 ? fq1->chiSquared() : 1e10;
                    double chi2Second = fq2 ? fq2->chiSquared() : 1e10;
                    MuonSegment* bestSegment = chi2First < chi2Second ? sh_iter.first : sh_iter.second;

                    // check whether already added
                    if (addedSegments.count(bestSegment)) continue;
                    addedSegments.insert(bestSegment);
                    Identifier chId = m_edmHelperSvc->chamberId(*bestSegment);
                    MuonStationIndex::ChIndex chIndex = m_idHelperSvc->chamberIndex(chId);
                    segmentsPerChamberLayer[chIndex].push_back(bestSegment);
                }

                for (MuonSegment* bestSegment : overlap.uniqueFirst) {
                    // check whether already added
                    if (addedSegments.count(bestSegment)) continue;
                    addedSegments.insert(bestSegment);
                    Identifier chId = m_edmHelperSvc->chamberId(*bestSegment);
                    MuonStationIndex::ChIndex chIndex = m_idHelperSvc->chamberIndex(chId);
                    segmentsPerChamberLayer[chIndex].push_back(bestSegment);
                }

                for (MuonSegment* bestSegment : overlap.uniqueSecond) {
                    // check whether already added
                    if (addedSegments.count(bestSegment)) continue;
                    addedSegments.insert(bestSegment);
                    Identifier chId = m_edmHelperSvc->chamberId(*bestSegment);
                    MuonStationIndex::ChIndex chIndex = m_idHelperSvc->chamberIndex(chId);
                    segmentsPerChamberLayer[chIndex].push_back(bestSegment);
                }

                for (std::pair<MuonSegment*, MuonSegment*>& sh_iter : overlap.subsetFirst) {
                    // check whether already added
                    MuonSegment* bestSegment = sh_iter.second;

                    // check whether already added
                    if (addedSegments.count(bestSegment)) continue;
                    addedSegments.insert(bestSegment);
                    Identifier chId = m_edmHelperSvc->chamberId(*bestSegment);
                    MuonStationIndex::ChIndex chIndex = m_idHelperSvc->chamberIndex(chId);
                    segmentsPerChamberLayer[chIndex].push_back(bestSegment);
                }

                for (const std::pair<MuonSegment*, MuonSegment*>& sh_iter : overlap.subsetSecond) {
                    // check whether already added
                    MuonSegment* bestSegment = sh_iter.first;
                    // check whether already added
                    if (addedSegments.count(bestSegment)) continue;
                    addedSegments.insert(bestSegment);
                    Identifier chId = m_edmHelperSvc->chamberId(*bestSegment);
                    MuonStationIndex::ChIndex chIndex = m_idHelperSvc->chamberIndex(chId);
                    segmentsPerChamberLayer[chIndex].push_back(bestSegment);
                }

                // now create new combi
                std::unique_ptr<MuonSegmentCombination> newCombi = std::make_unique<MuonSegmentCombination>();

                // loop over layers and add segments
                for (const std::vector<MuonSegment*>& chamber_hits : segmentsPerChamberLayer) {
                    // skip empty ones
                    if (chamber_hits.empty()) continue;

                    std::unique_ptr<MuonSegmentCombination::SegmentVec> segVec = std::make_unique<MuonSegmentCombination::SegmentVec>();
                    segVec->reserve(chamber_hits.size());
                    for (MuonSegment* seg : chamber_hits) segVec->emplace_back(seg->clone());
                    newCombi->addSegments(std::move(segVec));
                }

                // create new summary
                MuonSegmentCombiSummary newSummary{std::move(newCombi)};
                fillSummary(newSummary);

                ATH_MSG_VERBOSE("    -> merged combis ");
                existing = std::move(newSummary);
                hasOverlap = true;
                break;
            }
        }

        return hasOverlap;
    }

    MuonSegmentCombiOverlapSummary MuonSegmentCombinationCleanerTool::calculateOverlap(MuonSegmentCombiSummary& summary1,
                                                                                       MuonSegmentCombiSummary& summary2) const {
        ATH_MSG_DEBUG(" calculating overlap, size first " << summary1.nsegments << " size second " << summary2.nsegments);

        MuonSegmentCombiOverlapSummary summary;

        // loop over all chamber layers and compare per layer
        for (unsigned int i = 0; i < MuonStationIndex::ChIndexMax; ++i) {
            // get segment in chamber layer
            MuonSegmentCombiSummary::SegVec& chamberVec1 = summary1.chamberSegments(i);
            MuonSegmentCombiSummary::SegVec& chamberVec2 = summary2.chamberSegments(i);

            // if both no segments do nothing
            if (chamberVec1.empty() && chamberVec2.empty()) continue;

            // if both not empty compare the two, exception for CSCs
            if (!chamberVec1.empty() && !chamberVec2.empty()) {
                ATH_MSG_DEBUG(" resolving chambe layer " << MuonStationIndex::chName((MuonStationIndex::ChIndex)i));

                resolveLayerOverlap(chamberVec1, chamberVec2, summary);

            } else {
                if (!chamberVec1.empty()) summary.uniqueFirst.insert(summary.uniqueFirst.begin(), chamberVec1.begin(), chamberVec1.end());
                if (!chamberVec2.empty()) summary.uniqueSecond.insert(summary.uniqueSecond.begin(), chamberVec2.begin(), chamberVec2.end());
            }
        }
        ATH_MSG_VERBOSE(print(summary));
        return summary;
    }

    void MuonSegmentCombinationCleanerTool::resolveLayerOverlap(const std::vector<MuonSegment*>& chamberVec1,
                                                                const std::vector<MuonSegment*>& chamberVec2,
                                                                MuonSegmentCombiOverlapSummary& summary) const {
        CompareMuonSegmentKeys compareKeys{};

        std::vector<int> uniqueFirst(chamberVec1.size(), 1);
        std::vector<int> uniqueSecond(chamberVec2.size(), 1);

        unsigned int index1 = 0;
        for (MuonSegment* seg_chamb1 : chamberVec1) {
            // identifier(s) of MDT chambers on segment
            std::set<Identifier> chIds1 = m_edmHelperSvc->chamberIds(*seg_chamb1);
            MuonSegmentKey key1(*seg_chamb1);

            unsigned int index2 = 0;
            for (MuonSegment* seg_chamb2 : chamberVec2) {
                if (!uniqueSecond[index2]) {
                    ++index2;
                    continue;
                }

                // identifier(s) of MDT chambers on segment
                std::set<Identifier> chIds2 = m_edmHelperSvc->chamberIds(*seg_chamb2);

                // check whether chamber identifiers overlap
                bool hasOverlap = std::find_if(chIds2.begin(), chIds2.end(),
                                               [&chIds1](const Identifier& id) { return chIds1.count(id) > 0; }) != chIds2.end();
                // if no chamber identifier overlap continue with next segment
                if (!hasOverlap) {
                    ++index2;
                    continue;
                }
                // compare the hits on the two segments

                MuonSegmentKey key2(*seg_chamb2);
                CompareMuonSegmentKeys::OverlapResult overlapResult = compareKeys(key1, key2);
                ATH_MSG_VERBOSE("  overlap Result: " << compareKeys.print(overlapResult));

                if (overlapResult == CompareMuonSegmentKeys::Identical) {
                    summary.shared.emplace_back(seg_chamb1, seg_chamb2);
                    uniqueFirst[index1] = 0;
                    uniqueSecond[index2] = 0;
                } else if (overlapResult == CompareMuonSegmentKeys::SuperSet) {
                    summary.subsetSecond.emplace_back(seg_chamb1, seg_chamb2);
                    uniqueFirst[index1] = 0;
                    uniqueSecond[index2] = 0;
                } else if (overlapResult == CompareMuonSegmentKeys::SubSet) {
                    summary.subsetFirst.emplace_back(seg_chamb1, seg_chamb2);
                    uniqueFirst[index1] = 0;
                    uniqueSecond[index2] = 0;
                } else if (overlapResult == CompareMuonSegmentKeys::PartialOverlap) {
                    // keep partial overlaps for now....

                    // in case of full overlap in the MDTs
                    if (compareKeys.intersectionSize == compareKeys.segment1Size &&
                        compareKeys.intersectionSize == compareKeys.segment2Size) {
                        // mark as same if more than 75% overlap in trigger hits
                        double intersectFrac1 = compareKeys.intersectionSizeTrigger == 0
                                                    ? 0.
                                                    : compareKeys.intersectionSizeTrigger / (double)compareKeys.segment1SizeTrigger;
                        double intersectFrac2 = compareKeys.intersectionSizeTrigger == 0
                                                    ? 0.
                                                    : compareKeys.intersectionSizeTrigger / (double)compareKeys.segment2SizeTrigger;
                        double openingAngle = seg_chamb1->globalDirection().dot(seg_chamb2->globalDirection());
                        ATH_MSG_VERBOSE("  Partial overlap: (" << intersectFrac1 << "," << intersectFrac2 << "), opening angle "
                                                               << openingAngle);
                        if (intersectFrac1 > 0.75 && intersectFrac2 > 0.75 && openingAngle > 0.99) {
                            summary.shared.emplace_back(seg_chamb1, seg_chamb2);
                            uniqueFirst[index1] = 0;
                            uniqueSecond[index2] = 0;
                        }
                    }

                } else if (overlapResult == CompareMuonSegmentKeys::NoOverlap) {
                } else if (overlapResult == CompareMuonSegmentKeys::Unknown) {
                    ATH_MSG_WARNING(" Got invalid return argument comparing segments: " << compareKeys.print(overlapResult));
                }
            }
            ++index1;
        }

        // add unique segments
        index1 = 0;
        for (MuonSegment* seg_chamb1 : chamberVec1) {
            if (uniqueFirst[index1]) summary.uniqueFirst.push_back(seg_chamb1);
            ++index1;
        }

        unsigned int index2 = 0;
        for (MuonSegment* seg_chamb2 : chamberVec2) {
            if (uniqueSecond[index2]) summary.uniqueSecond.push_back(seg_chamb2);
            ++index2;
        }
    }

    void MuonSegmentCombinationCleanerTool::fillSummary(MuonSegmentCombiSummary& summary) const {
        const MuonSegmentCombination& combi = summary.segmentCombination();
        unsigned int nstations = combi.numberOfStations();

        // loop over segments, add them to the chamber layers
        // loop over chambers in combi and extract segments
        for (unsigned int i = 0; i < nstations; ++i) {
            // loop over segments in station
            const MuonSegmentCombination::SegmentVec* stationSegs = combi.stationSegments(i);

            // check if not empty
            if (!stationSegs || stationSegs->empty()) continue;

            // get chamber identifier, chamber index and station index
            const Identifier chid = m_edmHelperSvc->chamberId(*stationSegs->front());
            const MuonStationIndex::ChIndex chIndex = m_idHelperSvc->chamberIndex(chid);
            const MuonStationIndex::StIndex stIndex = MuonStationIndex::toStationIndex(chIndex);

            summary.stations.insert(stIndex);

            // reserve space for the new segments
            MuonSegmentCombiSummary::SegVec& chamberVec = summary.chamberSegments(chIndex);
            chamberVec.reserve(chamberVec.size() + stationSegs->size());

            summary.nsegments += stationSegs->size();
            for (const std::unique_ptr<MuonSegment>& seg : *stationSegs) {
                if (seg->numberOfContainedROTs() > 3) { summary.stationsGood.insert(stIndex); }
                chamberVec.push_back(seg.get());
            }
        }
    }

    std::unique_ptr<MuonSegmentCombination> MuonSegmentCombinationCleanerTool::removeDuplicateSegments(
        MuonSegmentCombination& combi) const {
        CompareMuonSegmentKeys compareKeys{};

        // store pointers to segments that should be kept
        std::set<MuonSegment*> segmentsToBeKept;
        unsigned int nsegments = 0;  // total number of segments

        unsigned int nstations = combi.numberOfStations();

        // loop over segments, add them to the chamber layers
        // loop over chambers in combi and extract segments
        for (unsigned int i = 0; i < nstations; ++i) {
            // loop over segments in station
            MuonSegmentCombination::SegmentVec* stationSegs = combi.stationSegments(i);
            // check if not empty
            if (!stationSegs || stationSegs->empty()) continue;
            nsegments += stationSegs->size();

            std::vector<std::pair<MuonSegmentKey, MuonSegment*> > keys;
            keys.reserve(stationSegs->size());

            for (const std::unique_ptr<MuonSegment>& seg : *stationSegs) {
                ATH_MSG_VERBOSE("  segment " << m_printer->print(*seg) << std::endl << m_printer->print(seg->containedMeasurements()));
                // create key
                MuonSegmentKey key{*seg};

                // loop over already added keys
                bool addSegment = true;
                for (std::pair<MuonSegmentKey, MuonSegment*>& key_pair : keys) {
                    CompareMuonSegmentKeys::OverlapResult overlapResult = compareKeys(key, key_pair.first);
                    ATH_MSG_VERBOSE("  overlap Result: " << compareKeys.print(overlapResult));

                    if (overlapResult == CompareMuonSegmentKeys::Identical || overlapResult == CompareMuonSegmentKeys::SubSet) {
                        addSegment = false;
                        break;
                    } else if (overlapResult == CompareMuonSegmentKeys::SuperSet) {
                        key_pair.first = key;
                        key_pair.second = seg.get();
                        addSegment = false;
                        break;
                    } else if (overlapResult == CompareMuonSegmentKeys::Unknown) {
                        ATH_MSG_WARNING(" Got invalid return argument comparing segments: " << compareKeys.print(overlapResult));
                    }
                }
                if (addSegment) keys.emplace_back(std::move(key), seg.get());
            }
            for (const std::pair<MuonSegmentKey, MuonSegment*>& key_pair : keys) segmentsToBeKept.insert(key_pair.second);
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

            std::unique_ptr<MuonSegmentCombination::SegmentVec> segVec = std::make_unique<MuonSegmentCombination::SegmentVec>();
            segVec->reserve(stationSegs->size());

            for (std::unique_ptr<MuonSegment>& seg : *stationSegs) {
                if (!segmentsToBeKept.count(seg.get())) {
                    ATH_MSG_VERBOSE(" dropping segment  " << m_printer->print(*seg));
                    continue;
                } else
                    ATH_MSG_VERBOSE(" adding   segment  " << m_printer->print(*seg));
                segVec->push_back(std::move(seg));
            }
            newCombi->addSegments(std::move(segVec));
        }

        return newCombi;
    }

    std::string MuonSegmentCombinationCleanerTool::print(MuonSegmentCombiSummary& summary) const {
        std::ostringstream sout;

        // loop over chamber layers
        int index = 0;
        MuonSegmentCombiSummary::ChSegVec& segmentsPerLayer = summary.segmentsPerLayer();

        sout << "SegmentCombi " << std::endl;

        for (MuonSegmentCombiSummary::SegVec& chiit : segmentsPerLayer) {
            // skip empty layers
            if (chiit.empty()) continue;

            sout << " Chamber Layer: " << MuonStationIndex::chName((MuonStationIndex::ChIndex)index) << " with " << chiit.size()
                 << " segments " << std::endl;

            for (MuonSegment* seg : chiit) sout << m_printer->print(*seg) << std::endl;

            ++index;
        }

        return sout.str();
    }

    std::string MuonSegmentCombinationCleanerTool::print(MuonSegmentCombiOverlapSummary& summary) const {
        std::ostringstream sout;

        sout << "SegmentCombi overlap " << std::endl
             << "  shared:       " << summary.shared.size() << std::endl
             << "  uniqueFirst:  " << summary.uniqueFirst.size() << std::endl
             << "  uniqueSecond: " << summary.uniqueSecond.size() << std::endl
             << "  subsetFirst:  " << summary.subsetFirst.size() << std::endl
             << "  subsetSecond: " << summary.subsetSecond.size() << std::endl;
        return sout.str();
    }

}  // namespace Muon
