/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// INavigator.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKDETDESCRINTERFACES_INAVIGATOR_H
#define TRKDETDESCRINTERFACES_INAVIGATOR_H

// GeoPrimitives
#include "GeoPrimitives/GeoPrimitives.h"
// Gaudi
#include "GaudiKernel/IAlgTool.h"
// Trk
#include "TrkEventPrimitives/ParticleHypothesis.h"
#include "TrkVolumes/BoundarySurface.h"
#include "TrkVolumes/BoundarySurfaceFace.h"
#include "TrkParameters/TrackParameters.h"
// STL
#include <utility>

namespace Trk {

  class IPropagator;
  class Surface;
  class Track;
  //class TrackParameters;
  class TrackingVolume;
  class TrackingGeometry; 
  class DetachedTrackingVolume;

  /**useful struct for a single navigation cell*/
  struct NavigationCell {

        mutable const TrackingVolume*      nextVolume;
        mutable const TrackParameters*     parametersOnBoundary;
        BoundarySurfaceFace                exitFace;
        /** Constructor */     
        NavigationCell(const TrackingVolume*  nVol,
                       const TrackParameters* lPar,
                       BoundarySurfaceFace    face=undefinedFace ) :
          nextVolume(nVol),
          parametersOnBoundary(lPar),
          exitFace(face)
        {}
         
  };

  typedef std::pair<int,const NavigationCell*>                   IdNavigationCell;

  /** Interface ID for INavigator */  
  static const InterfaceID IID_INavigator("INavigator", 1, 0);
  
  /** @class INavigator
     Interface class for the navigation AlgTool, it inherits from IAlgTool
     Detailed information about private members and member functions can be 
     found in the actual implementation class Trk::Navigator which inherits from this one.
     
     @author Andreas.Salzburger@cern.ch
    */
  class INavigator : virtual public IAlgTool {
    public:

      /**Virtual destructor*/
      virtual ~INavigator(){}
       
      /** AlgTool and IAlgTool interface methods */
      static const InterfaceID& interfaceID() { return IID_INavigator; }
    
      /** INavigator interface method - returns the TrackingGeometry used for navigation */
      virtual const TrackingGeometry* trackingGeometry() const = 0;    
         
      /** INavigator interface method - global search for the Volume one is in */
      virtual const TrackingVolume* volume(const Amg::Vector3D& gp) const = 0;
      
      /** INavigator interface method - forward hightes TrackingVolume */
      virtual const TrackingVolume* highestVolume() const = 0;
              
      /** INavigator interface method - getting the next BoundarySurface not knowing the Volume*/
      virtual const BoundarySurface<TrackingVolume>* nextBoundarySurface( const IPropagator& prop,
                                                                          const TrackParameters& parms,
                                                                          PropDirection dir) const = 0;  
      
      /** INavigator interface method - getting the next BoundarySurface when knowing the Volume*/
      virtual const BoundarySurface<TrackingVolume>* nextBoundarySurface( const IPropagator& prop,
                                                                          const TrackParameters& parms,
                                                                          PropDirection dir,
                                                                          const TrackingVolume& vol  ) const = 0;
                                                               
     /** INavigator interface method - - getting the next Volume and the parameter for the next Navigation */
     virtual const NavigationCell nextTrackingVolume(const IPropagator& prop,
                                                     const TrackParameters& parms,
                                                     PropDirection dir,
                                                     const TrackingVolume& vol) const = 0;

      /** INavigator interface method - getting the next Volume and the parameter for the next Navigation
        - contains full loop over volume boundaries
      */
      virtual const NavigationCell nextDenseTrackingVolume( const IPropagator& prop,
							                              const TrackParameters& parms,
                                                          const Surface* destination,
							                              PropDirection dir, 
							                              ParticleHypothesis particle, 
							                              const TrackingVolume& vol,
							                              double& path) const=0;

     /** INavigator interface method - getting the closest TrackParameters from a Track to a Surface */
     virtual const TrackParameters*  closestParameters( const Track& trk,
                                                        const Surface& sf,
                                                        const IPropagator* prop = 0) const = 0;

     /** INavigator method to resolve navigation at boundary */
     virtual bool atVolumeBoundary( const Trk::TrackParameters* parms, 
							        const Trk::TrackingVolume* vol,  
							        Trk::PropDirection dir, 
							        const Trk::TrackingVolume*& nextVol, 
							        double tol) const = 0;
 
   /** Validation Action:
        Can be implemented optionally, outside access to internal validation steps */
     virtual void validationAction() const = 0;    
  
  };


} // end of namespace

#endif // TRKDETDESCRINTERFACES_INAVIGATOR_H


