/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONSEGMENTCOMBINATIONCLEANERTOOL_H
#define MUONSEGMENTCOMBINATIONCLEANERTOOL_H

#include <map>
#include <string>
#include <vector>

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "MuonRecHelperTools/IMuonEDMHelperSvc.h"
#include "MuonRecHelperTools/MuonEDMPrinterTool.h"
#include "MuonSegment/MuonSegmentCombinationCollection.h"
#include "MuonSegmentCombinerToolInterfaces/IMuonSegmentCombinationCleanerTool.h"
#include "MuonSegmentMakerToolInterfaces/IMuonSegmentOverlapRemovalTool.h"
#include "MuonSegmentMakerUtils/MuonSegmentCombiOverlapSummary.h"
#include "MuonSegmentMakerUtils/MuonSegmentCombiSummary.h"

namespace Muon {

class MuonSegment;

class MuonSegmentCombinationCleanerTool : virtual public IMuonSegmentCombinationCleanerTool, public AthAlgTool {

  public:
    /** constructor */
    MuonSegmentCombinationCleanerTool(const std::string&, const std::string&, const IInterface*);

    /** destructor */
    virtual ~MuonSegmentCombinationCleanerTool(){};

    /** initializes private members */
    virtual StatusCode initialize();

    /** clean segment combination collections */
    std::unique_ptr<MuonSegmentCombinationCollection> clean(const MuonSegmentCombinationCollection&   combiCol,
                                                            MuonSegmentCombPatternCombAssociationMap* segPattMap);

    /** remove duplicates from a segment combination, returns a pointer to the original combi if unchanged */
    MuonSegmentCombination* removeDuplicateSegments(MuonSegmentCombination& combi) const;

  private:
    /** remove overlaps between combination and merge combinations with large overlap */
    void cleanAndMergeCombis(const MuonSegmentCombinationCollection&   combiCol,
                             MuonSegmentCombinationCollection*         combiCleanCol,
                             MuonSegmentCombPatternCombAssociationMap* segPattMap);

    /** check whether current summary is a subset of the previous once */
    bool subSetOfPrevious(MuonSegmentCombiSummary& summary, std::vector<MuonSegmentCombiSummary>& summaries) const;

    /** calculate overlap between two summaries */
    MuonSegmentCombiOverlapSummary calculateOverlap(MuonSegmentCombiSummary& summary1,
                                                    MuonSegmentCombiSummary& summary2) const;

    /* resolve overlap between two vectors of segments */
    void resolveLayerOverlap(const std::vector<MuonSegment*>& chamberVec1, const std::vector<MuonSegment*>& chamberVec2,
                             MuonSegmentCombiOverlapSummary& summary) const;

    /** create summary */
    MuonSegmentCombiSummary createSummary(const MuonSegmentCombination& combi) const;

    /** print summaries */
    std::string print(MuonSegmentCombiSummary& summary) const;
    std::string print(MuonSegmentCombiOverlapSummary& summary) const;

    ToolHandle<Muon::MuonEDMPrinterTool> m_printer{
        this,
        "Printer",
        "Muon::MuonEDMPrinterTool/MuonEDMPrinterTool",
    };
    ToolHandle<IMuonSegmentOverlapRemovalTool> m_overlapRemovalTool{
        this,
        "SegmentOverlapRemovalTool",
        "Muon::MuonSegmentOverlapRemovalTool/MuonSegmentOverlapRemovalTool",
        "tool to removal overlaps in segment combinations",
    };

    ServiceHandle<Muon::IMuonEDMHelperSvc> m_edmHelperSvc{
        this,
        "edmHelper",
        "Muon::MuonEDMHelperSvc/MuonEDMHelperSvc",
        "Handle to the service providing the IMuonEDMHelperSvc interface",
    };
    ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc{
        this,
        "MuonIdHelperSvc",
        "Muon::MuonIdHelperSvc/MuonIdHelperSvc",
    };


    /** If set to true, all combinaties will be merged into one big one */
    bool m_mergeAllCombis;
};

}  // namespace Muon

#endif  // MUONSEGMENTCOMBINATIONCLEANER_H
