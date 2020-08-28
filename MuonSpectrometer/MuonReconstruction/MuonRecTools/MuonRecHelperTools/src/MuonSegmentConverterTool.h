/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_MUONSEGMENTCONVERTERTOOL_H
#define MUON_MUONSEGMENTCONVERTERTOOL_H

#include "AthLinks/ElementLink.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "MuonRecHelperTools/IMuonEDMHelperSvc.h"
#include "MuonRecToolInterfaces/IMuonHitTimingTool.h"
#include "MuonSegment/MuonSegment.h"
#include "MuonSegmentMakerToolInterfaces/IMuonSegmentHitSummaryTool.h"
#include "TrkSegment/SegmentCollection.h"
#include "xAODMuon/MuonSegment.h"
#include "xAODMuon/MuonSegmentContainer.h"
#include "xAODMuonCnv/IMuonSegmentConverterTool.h"

static const InterfaceID IID_MuonSegmentConverterTool("xAODMaker::IMuonSegmentConverterTool", 1, 0);

namespace Muon {

class MuonSegmentConverterTool : public AthAlgTool, virtual public xAODMaker::IMuonSegmentConverterTool {
  public:
    /** @brief default AlgTool constructor */
    MuonSegmentConverterTool(const std::string&, const std::string&, const IInterface*);

    /** @brief destructor */
    ~MuonSegmentConverterTool(){};

    /** @brief initialize method, method taken from bass-class AlgTool */
    StatusCode initialize();

    /** @brief access to tool interface */
    static const InterfaceID& interfaceID()
    {
        return IID_MuonSegmentConverterTool;
    }

    /** @brief convert a ElementLink to a Trk::Segment (should be of type MuonSegment) to a xAOD::MuonSegment, the
       segment is add to the container if provided so the caller should not delete it */
    xAOD::MuonSegment* convert(const ElementLink< ::Trk::SegmentCollection>& segLink,
                               xAOD::MuonSegmentContainer*                   container = 0) const;

    /** @brief convert a Muon::MuonSegment to a xAOD::MuonSegment, the segment is add to the container if provided
        so the caller should not delete it */
    xAOD::MuonSegment* convert(const MuonSegment& seg, xAOD::MuonSegmentContainer* container = 0) const;

  private:
    /** helper function to dress output segment with cluster hit timing information */
    void addClusterTiming(const MuonSegment& seg, xAOD::MuonSegment& xaodSeg) const;

    ToolHandle<IMuonSegmentHitSummaryTool> m_hitSummaryTool{
        this,
        "MuonSegmentHitSummaryTool",
        "Muon::MuonSegmentHitSummaryTool/MuonSegmentHitSummaryTool",
    };
    ToolHandle<IMuonHitTimingTool> m_hitTimingTool{
        this,
        "MuonHitTimingTool",
        "Muon::MuonHitTimingTool/MuonHitTimingTool",
    };
    ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc{
        this,
        "MuonIdHelperSvc",
        "Muon::MuonIdHelperSvc/MuonIdHelperSvc",
    };
    ServiceHandle<IMuonEDMHelperSvc> m_edmHelper{
        this,
        "edmHelper",
        "Muon::MuonEDMHelperSvc/MuonEDMHelperSvc",
        "Handle to the service providing the IMuonEDMHelperSvc interface",
    };
};

}  // namespace Muon

#endif
