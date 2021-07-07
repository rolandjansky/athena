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
        virtual ~MuonSegmentOverlapRemovalTool() = default;

        /** @brief AlgTool initilize */
        StatusCode initialize() override;

        /** @brief remove duplicates from a vector of segments. The caller should take
            ownership of the segments */
        void removeDuplicates(Trk::SegmentCollection& segments) const override;

        /** @brief remove duplicates from a MuonSegmentCombination.
            The callers should take ownership of the new SegmentCombination. If the combination is
            unchanged the routine will return a nullptr */
        std::unique_ptr<MuonSegmentCombination> removeDuplicates(const MuonSegmentCombination& combi) const override;

        typedef std::vector<MuonSegment*> SegVec;

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

        Gaudi::Property<bool> m_removePartialOverlaps{
            this, "RemovePartialOverlaps", true,
            "removal partial overlaps between segments"};  //!< remove partial overlaps between segments
        Gaudi::Property<double> m_overlapFractionCut{
            this, "OverlapFractionCut", 0.8,
            "Cut overlap fraction, if fraction is smaller than cut both segments are kept"};  //!< cut on overlap fraction for segments
    };

}  // namespace Muon

#endif
