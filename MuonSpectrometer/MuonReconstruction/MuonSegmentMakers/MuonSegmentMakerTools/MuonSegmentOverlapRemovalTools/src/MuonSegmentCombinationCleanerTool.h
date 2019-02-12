/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONSEGMENTCOMBINATIONCLEANERTOOL_H
#define MUONSEGMENTCOMBINATIONCLEANERTOOL_H


#include <string>
#include <map>
#include <vector>

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

#include "MuonSegment/MuonSegmentCombinationCollection.h"
#include "MuonSegmentCombinerToolInterfaces/IMuonSegmentCombinationCleanerTool.h"
#include "MuonSegmentMakerUtils/MuonSegmentCombiSummary.h"
#include "MuonSegmentMakerUtils/MuonSegmentCombiOverlapSummary.h"


namespace Muon {

  class MuonSegment;
  class MuonEDMPrinterTool;
  class MuonEDMHelperTool;
  class MuonIdHelperTool;
  class IMuonSegmentOverlapRemovalTool;

  class MuonSegmentCombinationCleanerTool : virtual public IMuonSegmentCombinationCleanerTool, public AthAlgTool
  {

  public:
    /** constructor */
    MuonSegmentCombinationCleanerTool(const std::string&, const std::string&, const IInterface*);

    /** destructor */
    virtual ~MuonSegmentCombinationCleanerTool();

    /** initializes private members */
    virtual StatusCode initialize();

    /** deletes private members */
    virtual StatusCode finalize();

    /** clean segment combination collections */
    std::unique_ptr<MuonSegmentCombinationCollection> clean( const MuonSegmentCombinationCollection& combiCol, MuonSegmentCombPatternCombAssociationMap* segPattMap );

    /** remove duplicates from a segment combination, returns a pointer to the original combi if unchanged */
    MuonSegmentCombination* removeDuplicateSegments( MuonSegmentCombination& combi ) const;

  private:
    /** remove overlaps between combination and merge combinations with large overlap */
    void cleanAndMergeCombis( const MuonSegmentCombinationCollection& combiCol, MuonSegmentCombinationCollection* combiCleanCol,
			      MuonSegmentCombPatternCombAssociationMap* segPattMap );

    /** check whether current summary is a subset of the previous once */
    bool subSetOfPrevious( MuonSegmentCombiSummary& summary, std::vector<MuonSegmentCombiSummary>& summaries) const;

    /** calculate overlap between two summaries */
    MuonSegmentCombiOverlapSummary calculateOverlap( MuonSegmentCombiSummary& summary1, MuonSegmentCombiSummary& summary2 ) const;

    /* resolve overlap between two vectors of segments */
    void resolveLayerOverlap( const std::vector<MuonSegment*>& chamberVec1, const std::vector<MuonSegment*>& chamberVec2,
			      MuonSegmentCombiOverlapSummary& summary) const;

    /** create summary */
    MuonSegmentCombiSummary createSummary( const MuonSegmentCombination& combi ) const;

    /** print summaries */
    std::string print( MuonSegmentCombiSummary& summary ) const;
    std::string print( MuonSegmentCombiOverlapSummary& summary ) const;

    /** ToolHandle for EDM printing of segments */
    ToolHandle<Muon::MuonEDMPrinterTool> m_printer;
    ToolHandle<Muon::MuonEDMHelperTool>  m_helperTool;
    ToolHandle<Muon::MuonIdHelperTool>   m_idHelperTool;
    ToolHandle<IMuonSegmentOverlapRemovalTool> m_overlapRemovalTool;    

    /** If set to true, all combinaties will be merged into one big one */ 
    bool m_mergeAllCombis;

  };

}

#endif //MUONSEGMENTCOMBINATIONCLEANER_H 

