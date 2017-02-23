/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IMuTagMatchingTool_H
#define IMuTagMatchingTool_H

#include "GaudiKernel/IAlgTool.h"
#include "TrkEventPrimitives/PropDirection.h"
#include "MuonSegmentMakerToolInterfaces/IMuonSegmentHitSummaryTool.h"
#include "TrkParameters/TrackParameters.h"
#include "MuonCombinedEvent/MuonSegmentInfo.h"
/**
     @class IMuTagMatchingTool
  
  @author Zdenko.van.Kesteren@cern.ch
  
  */

namespace Muon{
  class MuonSegment;
}
namespace Trk{
  class Track;
}

static const InterfaceID IID_IMuTagMatchingTool("IMuTagMatchingTool",1,0);

class IMuTagMatchingTool : virtual public IAlgTool{
 public:

  static const InterfaceID& interfaceID();


  virtual std::string segmentStationString( const Muon::MuonSegment* segment ) const = 0 ;

  virtual void testExtrapolation( const Trk::Surface* pSurface,
				  const Trk::Track* pTrack
				  ) const = 0;
  
  virtual  bool match( const Trk::TrackParameters*  atSurface, 
		       const Muon::MuonSegment*     segment, 
		       std::string                  surfaceName ) const = 0;
  
  virtual bool surfaceMatch( const Trk::TrackParameters*  atSurface, 
			     const Muon::MuonSegment*     segment, 
			     std::string                  surfaceName ) const = 0;
  
  virtual bool phiMatch( const Trk::TrackParameters*  atSurface, 
			 const Muon::MuonSegment*     segment, 
			 std::string                  surfaceName ) const = 0;
  
  virtual bool thetaMatch(  const Trk::TrackParameters*  atSurface, 
			    const Muon::MuonSegment*     segment  ) const = 0;
  
  virtual bool rMatch(  const Trk::TrackParameters*  atSurface, 
			 const Muon::MuonSegment*     segment  ) const = 0;

   /** Get extrapolation at MS entrance level*/
   virtual  const Trk::TrackParameters* ExtrapolateTrktoMSEntrance(
							  const Trk::Track* pTrack,
							  Trk::PropDirection direction
							  ) const = 0;

   /** Get extrapolation at MSSurface level*/
   virtual  const Trk::TrackParameters* ExtrapolateTrktoMSSurface(
								  const Trk::Surface* surface,
								  const Trk::TrackParameters* pTrack,
								  Trk::PropDirection direction
								  ) const = 0;

   /** Get extrapolation at Segment Plane Surface level*/
   virtual const Trk::AtaPlane* ExtrapolateTrktoSegmentSurface(
								       const Muon::MuonSegment*  segment,
								       const Trk::TrackParameters* pTrack,
								       Trk::PropDirection direction
								       ) const = 0;
   virtual bool hasPhi( const Muon::MuonSegment* seg ) const = 0;
   
   virtual bool matchSegmentPosition( MuonCombined::MuonSegmentInfo* info, 
				      bool idHasEtaHits ) const = 0;
   
   
   virtual bool matchSegmentDirection( MuonCombined::MuonSegmentInfo* info, 
				       bool idHasEtaHits ) const = 0;
   
   virtual bool matchPtDependentPull( MuonCombined::MuonSegmentInfo* info,
				const Trk::Track*            trk ) const = 0;
 
   virtual bool matchDistance( MuonCombined::MuonSegmentInfo* info ) const = 0;

   virtual bool matchCombinedPull( MuonCombined::MuonSegmentInfo* info ) const = 0;

   virtual void nrTriggerHits( const Muon::MuonSegment* seg, 
			       int& nRPC, int& nTGC ) const = 0;
   
   virtual const Trk::Perigee* flipDirection( const Trk::Perigee* inputPars ) const = 0;

   virtual  MuonCombined::MuonSegmentInfo muTagSegmentInfo( const Trk::Track* track, const Muon::MuonSegment*  segment,  const Trk::AtaPlane* exTrack ) const  = 0;

   virtual void calculateLocalAngleErrors( const Trk::AtaPlane* expPars, double& exTrkErrXZ, double& exTrkErrYZ, double& covLocYYZ ) const = 0;

   virtual void calculateLocalAngleErrors( const Muon::MuonSegment* segment, double& exTrkErrXZ, double& exTrkErrYZ ) const = 0;


   
};

inline const InterfaceID& IMuTagMatchingTool::interfaceID()
{
  return IID_IMuTagMatchingTool;
}

#endif //IMuTagMatchingTool_H
