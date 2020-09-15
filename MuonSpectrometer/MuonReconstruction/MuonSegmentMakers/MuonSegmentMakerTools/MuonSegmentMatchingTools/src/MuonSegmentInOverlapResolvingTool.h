/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_MUONSEGMENTSOVERLAPRESOLVINGTOOL_H
#define MUON_MUONSEGMENTSOVERLAPRESOLVINGTOOL_H

#include <string>
#include <vector>

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "MuonRecHelperTools/IMuonEDMHelperSvc.h"
#include "MuonRecHelperTools/MuonEDMPrinterTool.h"
#include "MuonSegmentMakerToolInterfaces/IMuonSegmentInOverlapResolvingTool.h"
#include "TrkExInterfaces/IPropagator.h"
#include "TrkGeometry/MagneticFieldProperties.h"
#include "TrkToolInterfaces/IResidualPullCalculator.h"

class MsgStream;

namespace Trk {
class MagneticFieldProperties;
class MeasurementBase;
}  // namespace Trk

namespace MuonGM {
class MdtReadoutElement;
}

namespace Muon {

class MuonSegment;
/**
   @brief tool to match segments

*/
class MuonSegmentInOverlapResolvingTool : virtual public IMuonSegmentInOverlapResolvingTool, public AthAlgTool {
  public:
    /** @brief constructor */
    MuonSegmentInOverlapResolvingTool(const std::string&, const std::string&, const IInterface*);

    /** @brief destructor */
    virtual ~MuonSegmentInOverlapResolvingTool(){};

    /** @brief AlgTool initilize */
    StatusCode initialize();

    /** @brief performance match and return result */
    SegmentMatchResult matchResult(const MuonSegment& seg1, const MuonSegment& seg2) const;

    /** @brief calculate the position along a tube direction of the first segment
        that results in the smallest position residual in the precision plane of the second segment given
        the segment direction for segment 1 in global coordinates */
    SegmentPositionMatchResult bestPositionAlongTubeMatch(const MuonSegment& seg1, const MuonSegment& seg2,
                                                          const Amg::Vector3D& segDir1Min) const;

    /** calculate the angle phi for which the angular residual in the precision plane of
        the second segment is zero */
    SegmentPhiMatchResult bestPhiMatch(const MuonSegment& seg1, const MuonSegment& seg2) const;

    /** calculate the angle phi for which the angular residual in the precision plane of
        the second segment is zero */
    SegmentPhiMatchResult bestPhiMatchAnalytic(const MuonSegment& seg1, const MuonSegment& seg2) const;

    /** recalculate the segment direction give a new angle phi, keeps the angle in the precision plane fixed */
    Amg::Vector3D updateSegmentDirection(const MuonSegment& seg, double phi) const;

    /** calculate geometrical information for a segment */
    SegmentGeometrySummary segmentGeometrySummary(const MuonSegment& seg) const;

  private:
    /** compare phi hits with segment parameters, return average pull of the phi hits */
    double checkPhiHitConsistency(const Muon::MuonSegment& segment, SegmentPhiMatchResult& phiMatchResult,
                                  SegmentPositionMatchResult& matchResult) const;

    Amg::Vector3D estimateSegmentDirection(const MuonSegment& seg1, const MuonSegment& seg2, double& phi,
                                           double& stereoangle) const;

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
    ToolHandle<Trk::IPropagator> m_propagator{
        this,
        "AtlasRungeKuttaPropagator",
        "Trk::RungeKuttaPropagator/AtlasRungeKuttaPropagator",
    };
    ToolHandle<Trk::IResidualPullCalculator> m_pullCalculator{
        this,
        "ResidualPullCalculator",
        "Trk::ResidualPullCalculator/ResidualPullCalculator",
    };

    Trk::MagneticFieldProperties m_magFieldProperties;  //!< magnetic field properties
};

}  // namespace Muon

#endif
