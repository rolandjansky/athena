/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ITrackingVolumeArrayCreator.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKDETDESCRINTERFACES_ITRACKINGVOLUMEARRAYCREATOR_H
#define TRKDETDESCRINTERFACES_ITRACKINGVOLUMEARRAYCREATOR_H


// Gaudi
#include "GaudiKernel/IAlgTool.h"
// TrkDetDescrUtils - templated classes & enums
#include "TrkDetDescrUtils/BinnedArray.h"
#include "TrkDetDescrUtils/BinningType.h"
// STL
#include <vector>

namespace Trk {

  /** forward declarations*/
  class TrackingVolume;

  /** @typedef TrackingVolumeArray
      simply for the eye */
  typedef BinnedArray<const TrackingVolume> TrackingVolumeArray;
  
  /** Interface ID for ITrackingVolumeArrayCreator*/  
  static const InterfaceID IID_ITrackingVolumeArrayCreator("ITrackingVolumeArrayCreator", 1, 0);
  
  /** @class ITrackingVolumeArrayCreator
    
    Interface class ITrackingVolumeArrayCreators
    It inherits from IAlgTool. The actual implementation of the AlgTool
    can be found in TrkDetDescrTools as the LayerArrayCreator.
    
    It is designed to centralize the code to create
    Arrays of Tracking Volumes for both:
      - confinedment in another TrackingVolume
      - navigation and glueing

    @author Andreas.Salzburger@cern.ch
    */
  class ITrackingVolumeArrayCreator : virtual public IAlgTool {
    
    public:
      /**Virtual destructor*/
      virtual ~ITrackingVolumeArrayCreator(){}
      
      /** AlgTool and IAlgTool interface methods */
      static const InterfaceID& interfaceID() { return IID_ITrackingVolumeArrayCreator; }

      /** TrackingVolumeArrayCreator interface method -
          create a R-binned cylindrical volume array*/
      virtual TrackingVolumeArray* cylinderVolumesArrayInR(const std::vector< const TrackingVolume* >& vols,
                                                           bool navigationtype=false) const = 0; 
      
      /** TrackingVolumeArrayCreator interface method -
          create a R-binned cylindrical volume array*/
      virtual TrackingVolumeArray* cylinderVolumesArrayInZ(const std::vector< const TrackingVolume* >& vols,
                                                           bool navigationtype=false) const = 0; 

      /** TrackingVolumeArrayCreator interface method -
          create a Phi-binned cylindrical volume array*/
      virtual TrackingVolumeArray* cylinderVolumesArrayInPhi(const std::vector< const TrackingVolume* >& vols,
                                                           bool navigationtype=false) const = 0; 

      /** TrackingVolumeArrayCreator interface method -
          create a 2dim cylindrical volume array*/
      virtual TrackingVolumeArray* cylinderVolumesArrayInPhiR(const std::vector< const TrackingVolume* >& vols,
                                                           bool navigationtype=false) const = 0; 

      /** TrackingVolumeArrayCreator interface method -
          create a 2dim cylindrical volume array*/
      virtual TrackingVolumeArray* cylinderVolumesArrayInPhiZ(const std::vector< const TrackingVolume* >& vols,
                                                           bool navigationtype=false) const = 0; 

      /** TrackingVolumeArrayCreator interface method -
           create a Z-binned cuboid volume array*/
      virtual TrackingVolumeArray* cuboidVolumesArrayInZ(const std::vector< const TrackingVolume* >& vols,
                                                            bool navigationtype=false) const = 0;

      /** TrackingVolumeArrayCreator interface method -
           create a cuboid volume array*/
      virtual TrackingVolumeArray* cuboidVolumesArrayNav(const std::vector< const TrackingVolume* >& vols,
                                                         Trk::BinUtility* binUtil,
                                                         bool navigationtype=false) const = 0;

      /** TrackingVolumeArrayCreator interface method -
           create a trapezoid volume array*/
       virtual TrackingVolumeArray* trapezoidVolumesArrayNav(const std::vector< const TrackingVolume* >& vols,
                                                             Trk::BinUtility* binUtil,
                                                             bool navigationtype=false) const = 0;

      /** TrackingVolumeArrayCreator interface method -
           create a doubleTrapezoid volume array*/
       virtual TrackingVolumeArray* doubleTrapezoidVolumesArrayNav(const std::vector< const TrackingVolume* >& vols,
                                                                   Trk::BinUtility* binUtil,
                                                                   bool navigationtype=false) const = 0;
  
  };

} // end of namespace

#endif

