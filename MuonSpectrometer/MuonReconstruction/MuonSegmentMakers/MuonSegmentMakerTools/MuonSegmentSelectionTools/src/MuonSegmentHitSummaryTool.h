/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_MUONSEGMENTHITSUMMARYTOOL_H
#define MUON_MUONSEGMENTHITSUMMARYTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonRecHelperTools/IMuonEDMHelperSvc.h"
#include "MuonRecHelperTools/MuonEDMPrinterTool.h"
#include "MuonSegmentMakerToolInterfaces/IMuonSegmentHitSummaryTool.h"

namespace Muon {
class MuonSegment;
}

namespace Muon {
/**
   @brief tool to calculate muon segment hit summary
*/
class MuonSegmentHitSummaryTool : virtual public IMuonSegmentHitSummaryTool, public AthAlgTool {
  public:
    MuonSegmentHitSummaryTool(const std::string&, const std::string&, const IInterface*);

    virtual ~MuonSegmentHitSummaryTool() = default;

    StatusCode initialize();

    /** @brief calculate segment hit counts */
    virtual HitCounts getHitCounts(const MuonSegment& seg) const;

  private:
    ServiceHandle<Muon::IMuonIdHelperSvc>  m_idHelperSvc{this, "MuonIdHelperSvc",
                                                        "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};
    ServiceHandle<Muon::IMuonEDMHelperSvc> m_edmHelperSvc{
        this, "edmHelper", "Muon::MuonEDMHelperSvc/MuonEDMHelperSvc",
        "Handle to the service providing the IMuonEDMHelperSvc interface"};  //!< EDM Helper tool

    ToolHandle<Muon::MuonEDMPrinterTool> m_printer{
        this,
        "Printer",
        "Muon::MuonEDMPrinterTool/MuonEDMPrinterTool",
    };  //!< EDM printer tool

    SG::ReadCondHandleKey<MuonGM::MuonDetectorManager> m_DetectorManagerKey{
        this, "DetectorManagerKey", "MuonDetectorManager", "Key of input MuonDetectorManager condition data"};

    double m_positionAlongTubeCut;  //!< cut on the distance from the tube wall use for hole count
    double m_lowerADCBound;         //!< lower bound for good MDT hits
};

}  // namespace Muon

#endif
