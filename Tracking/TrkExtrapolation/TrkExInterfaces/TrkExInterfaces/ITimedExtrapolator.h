/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
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
#include "HelperStructs.h"
#include <utility>
 
namespace Trk {
  
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
								      const Trk::TrackingVolume* boundaryVol=nullptr) const = 0;

       /** Transport method for neutral and/or unstable particles. Search of intersections with active surfaces/layers on demand.  
           The extrapolation is interrupted at subdetector boundary for surviving/stable particles.
       */
          
       virtual const Trk::TrackParameters*  transportNeutralsWithPathLimit( const Trk::TrackParameters& parm,
									    Trk::PathLimit& pathLim, Trk::TimeLimit& time,
									    Trk::PropDirection dir,                                    
									    Trk::ParticleHypothesis particle,
									    std::vector<HitInfo>*& hitVector, 
									    Trk::GeometrySignature& nextGeoId,
									    const Trk::TrackingVolume* boundaryVol=nullptr) const = 0;


        /** Return the TrackingGeometry used by the Extrapolator (forwards information from Navigator) */
        virtual const TrackingGeometry* trackingGeometry() const = 0;

        /** Validation Action:
          Can be implemented optionally, outside access to internal validation steps
            */
        virtual void validationAction() const = 0;                                                                       
                                                            
  };


} // end of namespace

#endif // TRKEXINTERFACES_ITIMEDEXTRAPOLATOR_H

