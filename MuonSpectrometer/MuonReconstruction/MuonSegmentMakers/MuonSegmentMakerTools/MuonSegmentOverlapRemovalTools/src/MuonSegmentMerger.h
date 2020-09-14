/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_MUONSEGMENTMERGER_H
#define MUON_MUONSEGMENTMERGER_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "MuonRecHelperTools/IMuonEDMHelperSvc.h"
#include "MuonRecHelperTools/MuonEDMPrinterTool.h"
#include "MuonSegment/MuonSegment.h"
#include "MuonSegmentMakerToolInterfaces/IMuonSegmentMerger.h"
#include "MuonSegmentMakerToolInterfaces/IMuonSegmentTriggerHitAssociator.h"
#include "TrkEventPrimitives/FitQuality.h"

namespace Muon {
/**
   @brief tool to remove overlaps between segments

*/
class MuonSegmentMerger : virtual public IMuonSegmentMerger, public AthAlgTool {
  public:
    /** @brief constructor */
    MuonSegmentMerger(const std::string&, const std::string&, const IInterface*);

    /** @brief destructor */
    virtual ~MuonSegmentMerger() = default;

    /** @brief AlgTool initilize */
    StatusCode initialize();

    /** @brief find segments sharing all MDT hits but different phi hits The caller should NOT take ownership of the
     * segments */
    SegVec findDuplicates(const SegVec& segments) const;

    /** @brief merges duplicates in a vector of segments. The callers should take ownership of the new Segments.*/
    const MuonSegment* merge(const SegVec& segments) const;

    /** @brief group all segments that have at least one overlapping cluster */
    std::vector<SegVec> resolveClusterOverlaps(const SegVec& segments) const;

  private:
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
        "MuonEDMPrinterTool",
        "Muon::MuonEDMPrinterTool/MuonEDMPrinterTool",
    };  //!< EDM printer tool
    ToolHandle<Muon::IMuonSegmentTriggerHitAssociator> m_triggerHitsAssociator{
        this,
        "TriggerHitAssociator",
        "Muon::DCMathSegmentMaker/DCMathSegmentMaker",
    };  //!< trigger hit associator
};

class LocalSortSegmentsByChi2 {
  public:
    bool operator()(const MuonSegment* seg1, const MuonSegment* seg2)
    {

        // prefer segments with fit quality (always expected)
        const Trk::FitQuality* fq1 = seg1->fitQuality();
        const Trk::FitQuality* fq2 = seg2->fitQuality();
        if (!fq1 && fq2) return false;
        if (fq1 && !fq2) return true;
        if (!fq1 && !fq2) return false;

        if (fq1->numberDoF() > fq2->numberDoF()) return true;
        if (fq1->numberDoF() < fq2->numberDoF()) return false;

        // select candidate with smallest chi2
        double chi2Ndof1 = fq1->chiSquared() / fq1->numberDoF();
        double chi2Ndof2 = fq2->chiSquared() / fq2->numberDoF();
        return chi2Ndof1 < chi2Ndof2;
    }
};

}  // namespace Muon

#endif
