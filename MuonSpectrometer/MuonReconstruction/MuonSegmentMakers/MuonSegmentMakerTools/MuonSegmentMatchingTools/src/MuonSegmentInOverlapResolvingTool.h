/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_MUONSEGMENTSOVERLAPRESOLVINGTOOL_H
#define MUON_MUONSEGMENTSOVERLAPRESOLVINGTOOL_H

#include "MuonSegmentMakerToolInterfaces/IMuonSegmentInOverlapResolvingTool.h"

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

#include "GeoPrimitives/GeoPrimitives.h"
#include "TrkGeometry/MagneticFieldProperties.h"

#include <vector>
#include <string>

class MsgStream;

namespace Trk {
  class IPropagator;
  class IMagneticFieldTool;
  class MagneticFieldProperties;
  class MeasurementBase;
  class IResidualPullCalculator;
}

namespace Muon {
  class MuonIdHelperTool;
  class MuonEDMHelperTool;
  class MuonEDMPrinterTool;
}

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
    MuonSegmentInOverlapResolvingTool(const std::string&,const std::string&,const IInterface*);

    /** @brief destructor */
    virtual ~MuonSegmentInOverlapResolvingTool ();
    
    /** @brief AlgTool initilize */
    StatusCode initialize();
    
    /** @brief AlgTool finalize */
    StatusCode finalize();
    
    /** @brief performance match and return result */
    SegmentMatchResult matchResult( const MuonSegment& seg1, const MuonSegment& seg2 ) const; 

    /** @brief calculate the position along a tube direction of the first segment
	that results in the smallest position residual in the precision plane of the second segment given 
	the segment direction for segment 1 in global coordinates */
    SegmentPositionMatchResult bestPositionAlongTubeMatch( const MuonSegment& seg1, const MuonSegment& seg2,
							   const Amg::Vector3D& segDir1Min ) const;
    
    /** calculate the angle phi for which the angular residual in the precision plane of 
	the second segment is zero */
    SegmentPhiMatchResult bestPhiMatch( const MuonSegment& seg1, const MuonSegment& seg2 ) const;

    /** calculate the angle phi for which the angular residual in the precision plane of 
	the second segment is zero */
    SegmentPhiMatchResult bestPhiMatchAnalytic( const MuonSegment& seg1, const MuonSegment& seg2 ) const;

    /** recalculate the segment direction give a new angle phi, keeps the angle in the precision plane fixed */
    Amg::Vector3D updateSegmentDirection( const MuonSegment& seg, double phi ) const;

    /** calculate geometrical information for a segment */
    SegmentGeometrySummary segmentGeometrySummary( const MuonSegment& seg ) const;
  private:
    
    /** compare phi hits with segment parameters, return average pull of the phi hits */
    double checkPhiHitConsistency( const Muon::MuonSegment& segment, SegmentPhiMatchResult& phiMatchResult,
				   SegmentPositionMatchResult& matchResult ) const;

    Amg::Vector3D estimateSegmentDirection( const MuonSegment& seg1, const MuonSegment& seg2, double& phi, double& stereoangle ) const;

    ToolHandle<MuonIdHelperTool>               m_idHelperTool;     //!< IdHelper tool
    ToolHandle<MuonEDMHelperTool>              m_helperTool;       //!< EDM Helper tool
    ToolHandle<MuonEDMPrinterTool>             m_printer;          //!< EDM printer tool
    ToolHandle<Trk::IPropagator>               m_propagator;
    Trk::MagneticFieldProperties               m_magFieldProperties; //!< magnetic field properties
    ToolHandle<Trk::IResidualPullCalculator>   m_pullCalculator;
    
  };

}

#endif
