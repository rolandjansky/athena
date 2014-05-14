/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TrackingVolumeManipulator.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKGEOMETRY_TRACKINGVOLUMEMANIPULATOR_H
#define TRKGEOMETRY_TRACKINGVOLUMEMANIPULATOR_H

// Trk
#include "TrkDetDescrUtils/BinnedArray.h"
#include "TrkDetDescrUtils/SharedObject.h"
#include "TrkVolumes/BoundarySurface.h"
#include "TrkVolumes/BoundarySurfaceFace.h"

namespace Trk {

  class TrackingVolume;

  /** @class TrackingVolumeManipulator

     The only way to change BoundarySurfaces etc from Tracking Volumes.
     Tools have to inherit from this one to have friend rights to the TrackingVolume;

     @author Andreas.Salzburger@cern.ch
    */

  class TrackingVolumeManipulator {
    public:
     /** constructor */
     TrackingVolumeManipulator(){}
     /** Destructor */
     ~TrackingVolumeManipulator(){}

    protected:
     
      /** protected method to glue two Volumes together 
          input:
           - first TrackingVolume that keeps boundary surface
           - face of the BoundarySurface to be kept
           - second volume that gets glued to the first volume
           - face of the BoundarySurface to be shared
          */ 
      void glueVolumes(const TrackingVolume& firstVol,
                       BoundarySurfaceFace firstFace,
                       const TrackingVolume& secondVol,
                       BoundarySurfaceFace secondFace) const;
    
      /** protected method to set the boundary surface of a tracking volume */
      void setBoundarySurface(const TrackingVolume& tvol,
                              SharedObject<const BoundarySurface<TrackingVolume> > bsurf,
                              BoundarySurfaceFace face) const;

      /** protected method to set inside Volume of a BoundarySurface:
          input:
           - the volume that holdes the BoundarySurface 
           - the face type of the boundary to be set
           - the volume to be set as inside volume */
      void setInsideVolume(const TrackingVolume& tvol,
                           BoundarySurfaceFace face,
                           const TrackingVolume* insidevol) const;

      /** protected method to set inside VolumeArray of a BoundarySurface:
          input:
           - ithe volume that holdes the BoundarySurface 
           - the face type of the boundary to be set
           - the volume array to be set as inside volume array */
      void setInsideVolumeArray(const TrackingVolume& tvol,
                                BoundarySurfaceFace face,
                                BinnedArray<TrackingVolume>* insidevolarray) const;
      
      /** protected method to set inside VolumeArray of a BoundarySurface:
          input:
           - ithe volume that holdes the BoundarySurface 
           - the face type of the boundary to be set
           - the volume array to be set as inside volume array */
      void setInsideVolumeArray(const TrackingVolume& tvol,
                                BoundarySurfaceFace face,
                                SharedObject<BinnedArray<TrackingVolume> >insidevolarray) const;
      
      /** protected method to set outside Volume of a BoundarySurface:
          input:
           - the volume that holdes the BoundarySurface 
           - the face type of the boundary to be set
           - the volume to be set as outside volume */
      void setOutsideVolume(const TrackingVolume& tvol,
                            BoundarySurfaceFace face,
                            const TrackingVolume* outsidevol) const;

      /** protected method to set outside VolumeArray of a BoundarySurface:
          input:
           - the volume that holdes the BoundarySurface 
           - the face type of the boundary to be set
           - the volume array to be set as outside volume array */
      void setOutsideVolumeArray(const TrackingVolume& tvol,
                                 BoundarySurfaceFace face,
                                 BinnedArray<TrackingVolume>* outsidevolarray) const;

      /** protected method to set outside VolumeArray of a BoundarySurface:
          input:
           - the volume that holdes the BoundarySurface 
           - the face type of the boundary to be set
           - the volume array to be set as outside volume array */
      void setOutsideVolumeArray(const TrackingVolume& tvol,
                                 BoundarySurfaceFace face,
                                 SharedObject<BinnedArray<TrackingVolume> > outsidevolarray) const;

      /** protected method to confine (dense) volumes:
          input:
           - the volume that holdes the BoundarySurfaces (embedded) 
           - the volume to be set as outside volume  */
      void confineVolume(const TrackingVolume& tvol,
                         const TrackingVolume* outsidevol) const;
 
    };
}

#endif
