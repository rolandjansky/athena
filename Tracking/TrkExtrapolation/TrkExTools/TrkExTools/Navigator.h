/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// Navigator.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKEXTOOLS_NAVIGATOR_H
#define TRKEXTOOLS_NAVIGATOR_H

// Gaudi
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
// Trk
#include "TrkExInterfaces/INavigator.h"
#include "TrkEventPrimitives/PropDirection.h"
#include "TrkEventPrimitives/ParticleHypothesis.h"
#include "TrkVolumes/BoundarySurface.h"
#include "TrkGeometry/MagneticFieldProperties.h"
#include "TrkParameters/TrackParameters.h"
// STD
#include <cstring>
#include <exception>

#define TRKEXTOOLS_MAXNAVSTEPS 100

class TTree;

namespace Trk {

  class ITrackingGeometrySvc;


  /** Exception to be thrown when TrackingGeometry not found */

  class NavigatorException : public std::exception
  {
     virtual const char* what() const throw()
     { return "Problem with TrackingGeometry loading"; } 
   
  };

  class IGeometryBuilder;
  class IPropagator;
  class Surface;
  class Track;
  class TrackingVolume;
  class TrackingGeometry;

  typedef std::pair<const NavigationCell*,const NavigationCell*> NavigationPair;   

 /** 
     @class Navigator
      
     Main AlgTool for Navigation in the TrkExtrapolation realm :
     It retrieves the TrackingGeometry from the DetectorStore 
     as the reference Geometry.            

     There's an experimental possibility to use a straightLineApproximation for the 
     Navigation. This is unstable due to wrong cylinder intersections.

     @author Andreas.Salzburger@cern.ch
     */

  class Navigator : public AthAlgTool,
                    virtual public INavigator {
    public:
      /** Constructor */
      Navigator(const std::string&,const std::string&,const IInterface*);
      /** Destructor */
      virtual ~Navigator();
       
      /** AlgTool initailize method.*/
      StatusCode initialize();
      /** AlgTool finalize method */
      StatusCode finalize();
      
      /** INavigator interface method - returns the TrackingGeometry used for navigation */
      const TrackingGeometry* trackingGeometry() const;   
      
      /** INavigator interface methods - global search for the Volume one is in */
      const TrackingVolume*               volume(const Amg::Vector3D& gp) const;
            
      /** INavigator interface method - forward hightes TrackingVolume */
      const TrackingVolume*               highestVolume() const;
        
      /** INavigator interface methods - getting the next BoundarySurface not knowing the Volume*/
      const BoundarySurface<TrackingVolume>* nextBoundarySurface( const IPropagator& prop,
                                                       const TrackParameters& parms,
                                                       PropDirection dir) const;  
      
      /** INavigator interface methods - getting the next BoundarySurface when knowing the Volume*/
      const BoundarySurface<TrackingVolume>* nextBoundarySurface( const IPropagator& prop,
                                                       const TrackParameters& parms,
                                                       PropDirection dir,
                                                       const TrackingVolume& vol  ) const;

      /** INavigator interface method - getting the next Volume and the parameter for the next Navigation*/
      const NavigationCell nextTrackingVolume( const IPropagator& prop,
                                               const TrackParameters& parms,
                                               PropDirection dir,
                                               const TrackingVolume& vol) const;

      /** INavigator interface method - getting the next Volume and the parameter for the next Navigation
        - contains full loop over volume boundaries
      */
      const NavigationCell nextDenseTrackingVolume( const IPropagator& prop,
						    const TrackParameters& parms,
						    const Surface* destination,
						    PropDirection dir, 
						    ParticleHypothesis particle, 
						    const TrackingVolume& vol,
						    double& path) const;
     
      /** INavigator interface method - getting the closest TrackParameters from a Track to a Surface*/
      const TrackParameters*      closestParameters( const Track& trk,
                                                     const Surface& sf,
                                                     const IPropagator* prop = 0) const;

      /** INavigator method to resolve navigation at boundary */
      bool atVolumeBoundary( const Trk::TrackParameters* parms, 
							const Trk::TrackingVolume* vol,  
							Trk::PropDirection dir, 
							const Trk::TrackingVolume*& nextVol, 
							double tol) const;
      /** Validation Action:
          Can be implemented optionally, outside access to internal validation steps */
      virtual void validationAction() const;
                                                            
    private:
      //!< retrieve TrackingGeometry (almost callback ready!)
      StatusCode                           updateTrackingGeometry() const; 

      mutable const TrackingGeometry*           m_trackingGeometry;          //!< the tracking geometry owned by the navigator
      ServiceHandle<Trk::ITrackingGeometrySvc>  m_trackingGeometrySvc;       //!< ToolHandle to the TrackingGeometrySvc
      std::string                               m_trackingGeometryName;      //!< Name of the TrackingGeometry as given in Detector Store
      double                                    m_insideVolumeTolerance;     //!< Tolerance for inside() method of Volumes
      double                                    m_isOnSurfaceTolerance;      //!< Tolerance for isOnSurface() method of BoundarySurfaces
                                                
      bool                                      m_useStraightLineApproximation; //!< use the straight line approximation for the next boundary sf
      bool                                      m_searchWithDistance;        //!< search with new distanceToSurface() method

      //------VALIDATION MODE SECTION ----------------------------------//
      bool                                      m_validationMode;            //!< boolean to switch to validation mode
      std::string                               m_validationTreeName;        //!< validation tree name - to be acessed by this from root
      std::string                               m_validationTreeDescription; //!< validation tree description - second argument in TTree
      std::string                               m_validationTreeFolder;      //!< stream/folder to for the TTree to be written out
                                                
                                                
      TTree*                                    m_validationTree;            //!< Root Validation Tree
                                                
      mutable int                               m_boundariesCounter;         //!< counter for boundary surfaces hit   
                                                
      mutable int                               m_boundaries;                           //!< associated Ntuple variable
      mutable float                             m_boundaryHitX[TRKEXTOOLS_MAXNAVSTEPS]; //!< x Position of interseciton with BoundarySurface
      mutable float                             m_boundaryHitY[TRKEXTOOLS_MAXNAVSTEPS]; //!< y Position of interseciton with BoundarySurface
      mutable float                             m_boundaryHitR[TRKEXTOOLS_MAXNAVSTEPS]; //!< Radius of interseciton with BoundarySurface
      mutable float                             m_boundaryHitZ[TRKEXTOOLS_MAXNAVSTEPS]; //!< z Position of interseciton with BoundarySurface

      // ------ PERFORMANCE STATISTICS -------------------------------- //

      mutable int                               m_forwardCalls;              //!< couter for forward nextBounday calls
      mutable int                               m_forwardFirstBoundSwitch;   //!< couter for failed first forward nextBounday calls
      mutable int                               m_forwardSecondBoundSwitch;  //!< couter for failed second forward nextBounday calls
      mutable int                               m_forwardThirdBoundSwitch;   //!< couter for failed third forward nextBounday calls
                                                
      mutable int                               m_backwardCalls;             //!< couter for backward nextBounday calls
      mutable int                               m_backwardFirstBoundSwitch;  //!< couter for failed first backward nextBounday calls
      mutable int                               m_backwardSecondBoundSwitch; //!< couter for failed second backward nextBounday calls
      mutable int                               m_backwardThirdBoundSwitch;  //!< couter for failed third backward nextBounday calls
                                                
      mutable int                               m_outsideVolumeCase;         //!< counter for navigation-break in outside volume cases (ovc)
      mutable int                               m_sucessfulBackPropagation;  //!< counter for sucessful recovery of navigation-break in ovc 

      /** static magnetic field properties (empty to fake straight line intersects) */
      static Trk::MagneticFieldProperties       s_zeroMagneticField;        //!< no magnetic field there
      
      //------------ Magnetic field properties
      bool                                      m_fastField;
      Trk::MagneticFieldProperties              m_fieldProperties;
  };

} // end of namespace


#endif // TRKEXTOOLS_NAVIGATOR_H

