/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_MUONSEGMENTSOVERLAPREMOVALTOOL_H
#define MUON_MUONSEGMENTSOVERLAPREMOVALTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "MuonRecHelperTools/IMuonEDMHelperSvc.h"
#include "MuonRecHelperTools/MuonEDMPrinterTool.h"
#include "MuonSegment/MuonSegmentCombination.h"
#include "MuonSegmentMakerToolInterfaces/IMuonSegmentOverlapRemovalTool.h"

namespace Muon {

class MuonSegment;

/**
   @brief tool to remove overlaps between segments

*/
class MuonSegmentOverlapRemovalTool : virtual public IMuonSegmentOverlapRemovalTool, public AthAlgTool {
  public:
    /** @brief constructor */
    MuonSegmentOverlapRemovalTool(const std::string&, const std::string&, const IInterface*);

    /** @brief destructor */
    virtual ~MuonSegmentOverlapRemovalTool(){};

    /** @brief AlgTool initilize */
    StatusCode initialize();

    /** @brief remove duplicates from a vector of segments. The caller should take
        ownership of the segments */
    void removeDuplicates(Trk::SegmentCollection* segments) const;

    /** @brief remove duplicates from a MuonSegmentCombination.
        The callers should take ownership of the new SegmentCombination. If the combination is
        unchanged the routine will return a pointer to the input combination. The caller should
        check whether the two pointers are identical and avoid a double delete */
    const MuonSegmentCombination* removeDuplicates(const MuonSegmentCombination& combi) const;

  private:
    // this is only needed now by the call to remove duplicates from a MuonSegmentCombination
    // cleaning of MuonSegmentCombinations is turned off, so perhaps this can be removed entirely
    SegVec removeDuplicates(MuonSegmentCombination::SegmentVec& segments) const;

    ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc{
        this,
        "MuonIdHelperSvc",
        "Muon::MuonIdHelperSvc/MuonIdHelperSvc",
    };
    ServiceHandle<Muon::IMuonEDMHelperSvc> m_edmHelperSvc{
        this,
        "edmHelper",
        "Muon::MuonEDMHelperSvc/MuonEDMHelperSvc",
        "Handle to the service providing the IMuonEDMHelperSvc interface",
    };  //!< EDM Helper tool

    ToolHandle<Muon::MuonEDMPrinterTool> m_printer{
        this,
        "Printer",
        "Muon::MuonEDMPrinterTool/MuonEDMPrinterTool",
    };  //!< EDM printer tool

    bool   m_removePartialOverlaps;  //!< remove partial overlaps between segments
    double m_overlapFractionCut;     //!< cut on overlap fraction for segments
};

}  // namespace Muon

#endif
