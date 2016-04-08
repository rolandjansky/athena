/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuTagMatchingTool_H
#define MuTagMatchingTool_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include <string>
#include "TrkParameters/TrackParameters.h" 
#include "TrkGeometry/TrackingGeometry.h"
#include "MuonSegment/MuonSegment.h"
#include "TrkExInterfaces/IExtrapolator.h"  
#include "TrkExInterfaces/IPropagator.h"  
#include "MuonSegmentMakerToolInterfaces/IMuonSegmentSelectionTool.h"
#include "MuonSegmentMakerToolInterfaces/IMuonSegmentHitSummaryTool.h"
#include "MuonSegmentTaggerToolInterfaces/IMuTagMatchingTool.h"
//#include "TrkParameters/Perigee.h"
#include "TrkParameters/TrackParameters.h"
#include "MuonCombinedEvent/MuonSegmentInfo.h"

class StoreGateSvc;
class MdtIdHelper;
class CscIdHelper;
class RpcIdHelper;
class TgcIdHelper;

  /**
     @class MuTagMatchingTool
   
  @author Zdenko.van.Kesteren@cern.ch
  
  */

namespace Muon {
  class MuonIdHelperTool;
  class MuonEDMPrinterTool;
  class MuonEDMHelperTool;
}

namespace MuonGM {
  class MuonDetectorManager;
}

namespace Trk{
  class Surface;
  class Track;
  class IResidualPullCalculator;
  //  class PropDirection;
}


class MuTagMatchingTool : virtual public IMuTagMatchingTool, public AthAlgTool{
 public:
  MuTagMatchingTool(const std::string& t ,const std::string& n ,const IInterface* p);
  virtual ~MuTagMatchingTool          ();
  
  virtual StatusCode initialize        ();
  virtual StatusCode finalize          ();
  
  std::string segmentStationString( const Muon::MuonSegment* segment ) ;

  void testExtrapolation( const Trk::Surface* pSurface,
			  const Trk::Track* pTrack
			  );
  
  bool match( const Trk::TrackParameters*  atSurface, 
	      const Muon::MuonSegment*     segment, 
	      std::string                  surfaceName );
  
  bool surfaceMatch( const Trk::TrackParameters*  atSurface, 
		     const Muon::MuonSegment*     segment, 
		     std::string                  surfaceName );
  
  bool phiMatch( const Trk::TrackParameters*  atSurface, 
		 const Muon::MuonSegment*     segment, 
		 std::string                  surfaceName );
  
  bool thetaMatch(  const Trk::TrackParameters*  atSurface, 
		    const Muon::MuonSegment*     segment  );

  bool rMatch(  const Trk::TrackParameters*  atSurface, 
		const Muon::MuonSegment*     segment  );

  /** Get extrapolation at MS entrance level*/
  const Trk::TrackParameters* ExtrapolateTrktoMSEntrance(
						       const Trk::Track* pTrack,
						       Trk::PropDirection direction
						       );

  /** Get extrapolation at MSSurface level*/
  const Trk::TrackParameters* ExtrapolateTrktoMSSurface(
						       const Trk::Surface* surface,
						       const Trk::TrackParameters* pTrack,
						       Trk::PropDirection direction
						       );

  /** Get extrapolation at Segment Plane Surface level*/
  const Trk::AtaPlane* ExtrapolateTrktoSegmentSurface(
							    const Muon::MuonSegment*  segment,
							    const Trk::TrackParameters* pTrack,
							    Trk::PropDirection direction
							    );
  bool hasPhi( const Muon::MuonSegment* seg );
   
  double errorProtection( double exTrk_Err, bool isAngle );
 
  bool matchSegmentPosition( MuonCombined::MuonSegmentInfo* info,  bool idHasEtaHits);

  bool matchSegmentDirection( MuonCombined::MuonSegmentInfo* info,  bool idHasEtaHits);
   

  bool matchPtDependentPull( MuonCombined::MuonSegmentInfo* info,
		       const Trk::Track*            trk ) ;

  bool matchDistance( MuonCombined::MuonSegmentInfo* info);

  bool matchCombinedPull( MuonCombined::MuonSegmentInfo* info );
  
  void nrTriggerHits( const Muon::MuonSegment* seg, 
		      int& nRPC, int& nTGC );

  const Trk::Perigee* flipDirection( const Trk::Perigee* inputPars ) ;

  MuonCombined::MuonSegmentInfo muTagSegmentInfo( const Trk::Track* track, const Muon::MuonSegment* segment,  const Trk::AtaPlane* exTrack ) ;

  void calculateLocalAngleErrors( const Trk::AtaPlane* expPars, double& exTrkErrXZ, double& exTrkErrYZ, double& covLocYYZ );

  void calculateLocalAngleErrors( const Muon::MuonSegment* segment, double& exTrkErrXZ, double& exTrkErrYZ );


 private:
  ///////////////////////////////////
   bool         isCscSegment( const Muon::MuonSegment* seg ) const ;
   unsigned int cscHits( const Muon::MuonSegment* seg ) const ;

   // exploit correlation between residual in position and angle
   double matchingDistanceCorrection( double resPos, double resAngle );

   ToolHandle<Trk::IExtrapolator> p_IExtrapolator ;//!< Pointer on IExtrapolator
   ToolHandle<Trk::IPropagator> p_propagator ;//!< Pointer on propagator for SL propagation
   ToolHandle<Muon::MuonIdHelperTool>             m_idHelper;
   ToolHandle<Muon::MuonEDMHelperTool>            m_helper;
   ToolHandle<Muon::MuonEDMPrinterTool>           m_printer;
   ToolHandle<Muon::IMuonSegmentHitSummaryTool>   m_hitSummaryTool;
   ToolHandle<Muon::IMuonSegmentSelectionTool>    m_selectionTool;
   ToolHandle<Trk::IResidualPullCalculator>       m_pullCalculator;
   
   std::string m_t0Location;
   StoreGateSvc* p_StoreGateSvc ; //!< Pointer On StoreGateSvc
   
   const MuonGM::MuonDetectorManager* m_detMgr;
   const MdtIdHelper*  m_mdtIdHelper;
   const CscIdHelper*  m_cscIdHelper;
   const RpcIdHelper*  m_rpcIdHelper;
   const TgcIdHelper*  m_tgcIdHelper;

   bool m_assumeLocalErrors;
   bool m_extrapolatePerigee;

   const Trk::TrackingGeometry *m_trackingGeometry;

   double m_GLOBAL_THETA_CUT;
   double m_GLOBAL_PHI_CUT;
   double m_GLOBAL_R_CUT;

   double m_MATCH_THETA;
   double m_MATCH_PHI;
   double m_MATCH_THETAANGLE;
   double m_MATCH_PHIANGLE;

   double m_ERROR_EX_POS_CUT;
   double m_ERROR_EX_ANGLE_CUT;

   bool m_doDistCut;
   double m_DIST_POS;
   double m_DIST_ANGLE;

   double m_SAFE_THETA;
   double m_SAFE_PHI;
   double m_SAFE_THETAANGLE;
   double m_SAFE_PHIANGLE;

   double m_chamberPullCut;
   double m_combinedPullCut;

};



#endif //MuTagMatchingTool_H
