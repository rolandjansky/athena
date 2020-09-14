/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_MUONSEGMENTPAIRMATCHINGTOOL_H
#define MUON_MUONSEGMENTPAIRMATCHINGTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "MuonRecHelperTools/IMuonEDMHelperSvc.h"
#include "MuonSegmentMakerToolInterfaces/IMuonSegmentPairMatchingTool.h"
#include "TrkTrack/TrackCollection.h"

namespace Muon {

class MuonSegment;
class MuonEDMPrinterTool;
/**
   @brief tool to match segments
*/
class MuonSegmentPairMatchingTool : virtual public IMuonSegmentPairMatchingTool, public AthAlgTool {
  public:
    /** @brief constructor */
    MuonSegmentPairMatchingTool(const std::string&, const std::string&, const IInterface*);

    /** @brief destructor */
    virtual ~MuonSegmentPairMatchingTool(){};

    /** @brief AlgTool initilize */
    StatusCode initialize();

    /** @brief performance match and return result */
    SegmentMatchResult                      matchResult(const MuonSegment& seg1, const MuonSegment& seg2) const;
    std::pair<Amg::Vector3D, Amg::Vector3D> getShortestTubePos(const Muon::MuonSegment& seg) const;

  private:
    ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc{
        this,
        "MuonIdHelperSvc",
        "Muon::MuonIdHelperSvc/MuonIdHelperSvc",
    };
    ServiceHandle<IMuonEDMHelperSvc> m_edmHelperSvc{
        this,
        "edmHelper",
        "Muon::MuonEDMHelperSvc/MuonEDMHelperSvc",
        "Handle to the service providing the IMuonEDMHelperSvc interface",
    };  //!< EDM Helper tool

    ToolHandle<MuonEDMPrinterTool> m_printer{
        this,
        "Printer",
        "Muon::MuonEDMPrinterTool/MuonEDMPrinterTool",
    };  //!< EDM printer tool
};

}  // namespace Muon

#endif
