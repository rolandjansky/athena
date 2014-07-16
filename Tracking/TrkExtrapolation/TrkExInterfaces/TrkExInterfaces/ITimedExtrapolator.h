/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ITimedExtrapolator.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKEXINTERFACES_ITIMEDEXTRAPOLATOR_H
#define TRKEXINTERFACES_ITIMEDEXTRAPOLATOR_H

// Gaudi
#include "GaudiKernel/IAlgTool.h"
// Trk
#include "TrkEventPrimitives/PropDirection.h"
#include "TrkEventPrimitives/ParticleHypothesis.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkExUtils/MaterialUpdateMode.h"
#include "TrkDetDescrUtils/GeometrySignature.h"
#include <utility>
 
namespace Trk {

  struct HitInfo 
  {
    const Trk::TrackParameters*  trackParms;    // intersection
    float time;       // timing  
    int   detID;      // sensitive detector id
    float deposit;    // interaction energy deposit (secondaries subtracted)   

    /**
     * Constructor
     */
    HitInfo( const Trk::TrackParameters* parms, float time, int id, float deposit ) :
    trackParms(parms), time(time), detID(id), deposit(deposit) 
    {} 
  };

  struct PathLimit 
  {
    float x0Max;       // path limit after which sampled process occurs
    float x0Collected; // x0 traversed so far
    float l0Collected; // l0 traversed so far
    float weightedZ;   // type of material traversed (divide by collected x0 to get average Z)
    int   process;     // type of pre-sampled material process

    /**
     * Constructor
     */
    PathLimit( float pathLimit, int proc) :
      x0Max(pathLimit), x0Collected(0.0), l0Collected(0.0),weightedZ(0.0), process(proc)
    {} 

    /**
     * collected material update
     */
    void updateMat( float dX0, float Z, float dL0) { 
      x0Collected += dX0; weightedZ += dX0*Z; 
      l0Collected += dL0>0. ? dL0 : dX0/0.37/Z;     // use approximation if l0 not available
    }
  };
  
  struct TimeLimit 
  {
    float tMax;       // pre-sampled life-time
    float time;       // timing  
    int   process;    // type of pre-sampled decay process

    /**
     * Constructor
     */
    TimeLimit( float timeMax, float time, int proc) :
      tMax(timeMax), time(time), process(proc)
    {} 

    /**
     * timing update
     */
    void updateTime( float dt) { time += dt; }
  };
  
  static const InterfaceID IID_ITimedExtrapolator("ITimedExtrapolator", 1, 0);
  
  /** 
   @class ITimedExtrapolator
   Interface class for the extrapolation AlgTool, it inherits from IAlgTool
   Detailed information about private members and member functions can be found in the actual implementation class
   TimedExtrapolator which inherits from this one.
       
   @author sarka.todorova@cern.ch
   */
      
  class TrackingVolume;
  class TrackingGeometry;
  class TrackStateOnSurface;      

  class ITimedExtrapolator : virtual public IAlgTool {
     public:
     
       /** Virtual destructor */
       virtual ~ITimedExtrapolator(){}

       /** AlgTool interface methods */
       static const InterfaceID& interfaceID() { return IID_ITimedExtrapolator; }

       /** Extrapolation method allowing treatment of unstable particles. Search of intersections with active surfaces on demand.
           The extrapolation is interrupted at subdetector boundary for surviving/stable particles. */
          
       virtual const Trk::TrackParameters*  extrapolateWithPathLimit( const Trk::TrackParameters& parm,
                                                                      Trk::PathLimit& pathLim, Trk::TimeLimit& time,
                                                                      Trk::PropDirection dir,                                    
                                                                      Trk::ParticleHypothesis particle,
                                                                      std::vector<HitInfo>*& hitVector, 
                                                                      Trk::GeometrySignature& nextGeoId,
								      const Trk::TrackingVolume* boundaryVol=0) const = 0;

       /** Transport method for neutral and/or unstable particles. Search of intersections with active surfaces/layers on demand.  
           The extrapolation is interrupted at subdetector boundary for surviving/stable particles.
       */
          
       virtual const Trk::TrackParameters*  transportNeutralsWithPathLimit( const Trk::TrackParameters& parm,
									    Trk::PathLimit& pathLim, Trk::TimeLimit& time,
									    Trk::PropDirection dir,                                    
									    Trk::ParticleHypothesis particle,
									    std::vector<HitInfo>*& hitVector, 
									    Trk::GeometrySignature& nextGeoId,
									    const Trk::TrackingVolume* boundaryVol=0) const = 0;


        /** Return the TrackingGeometry used by the Extrapolator (forwards information from Navigator) */
        virtual const TrackingGeometry* trackingGeometry() const = 0;

        /** Validation Action:
          Can be implemented optionally, outside access to internal validation steps
            */
        virtual void validationAction() const = 0;                                                                       
                                                            
  };


} // end of namespace

#endif // TRKEXINTERFACES_ITIMEDEXTRAPOLATOR_H

