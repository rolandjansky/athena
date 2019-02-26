/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_MUONSEGMENTSOVERLAPREMOVALTOOL_H
#define MUON_MUONSEGMENTSOVERLAPREMOVALTOOL_H

#include "MuonSegmentMakerToolInterfaces/IMuonSegmentOverlapRemovalTool.h"

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "MuonSegment/MuonSegmentCombination.h"

namespace Muon {
  
  class MuonSegment;
  class MuonEDMHelperTool;
  class MuonEDMPrinterTool;
  class MuonIdHelperTool;

  /**
     @brief tool to remove overlaps between segments

  */
  class MuonSegmentOverlapRemovalTool : virtual public IMuonSegmentOverlapRemovalTool, public AthAlgTool {
  public:
    /** @brief constructor */
    MuonSegmentOverlapRemovalTool(const std::string&,const std::string&,const IInterface*);

    /** @brief destructor */
    virtual ~MuonSegmentOverlapRemovalTool ();
    
    /** @brief AlgTool initilize */
    StatusCode initialize();
    
    /** @brief AlgTool finalize */
    StatusCode finalize();
 
     /** @brief remove duplicates from a vector of segments. The caller should take 
         ownership of the segments */
    void removeDuplicates( Trk::SegmentCollection* segments ) const;

    /** @brief remove duplicates from a MuonSegmentCombination.
        The callers should take ownership of the new SegmentCombination. If the combination is 
        unchanged the routine will return a pointer to the input combination. The caller should
        check whether the two pointers are identical and avoid a double delete */
    const MuonSegmentCombination* removeDuplicates( const MuonSegmentCombination& combi ) const;
   
  private:

    //this is only needed now by the call to remove duplicates from a MuonSegmentCombination
    //cleaning of MuonSegmentCombinations is turned off, so perhaps this can be removed entirely
    SegVec removeDuplicates( MuonSegmentCombination::SegmentVec& segments ) const;

    ToolHandle<Muon::MuonIdHelperTool>               m_idHelperTool;     //!< IdHelper tool
    ToolHandle<Muon::MuonEDMHelperTool>              m_helperTool;       //!< EDM Helper tool
    ToolHandle<Muon::MuonEDMPrinterTool>             m_printer;          //!< EDM printer tool

    bool m_removePartialOverlaps; //!< remove partial overlaps between segments
    double m_overlapFractionCut;    //!< cut on overlap fraction for segments 
  };

}

#endif
