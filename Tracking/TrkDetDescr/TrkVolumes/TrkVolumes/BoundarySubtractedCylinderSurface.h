/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// BoundarySubtractedCylinderSurface.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKVOLUMES_BOUNDARYSUBTRACTEDCYLIMDERSURFACE_H
#define TRKVOLUMES_BOUNDARYSUBTRACTEDCYLIMDERSURFACE_H

//Trk
#include "TrkGeometrySurfaces/SubtractedCylinderSurface.h"
#include "TrkVolumes/BoundarySurface.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkEventPrimitives/PropDirection.h"

#include "GeoPrimitives/GeoPrimitives.h"

namespace Trk {

//class TrackParameters;
class Volume;

  /** 
   @class BoundarySubtractedCylinderSurface

   BoundarySubtractedCylinderSurface description inside the tracking realm,
   it extends the Surface description to make a surface being a boundary of a
   Trk::Volume
    
   @author Andreas.Salzburger@cern.ch
   */
      
  template <class Tvol> class BoundarySubtractedCylinderSurface final: 
                              virtual public BoundarySurface<Tvol>, public SubtractedCylinderSurface {
  
    /** typedef the BinnedArray */
    typedef BinnedArray<Tvol> VolumeArray;                            

    public:
     /** Default Constructor - needed for pool and inherited classes */
     BoundarySubtractedCylinderSurface():
      BoundarySurface<Tvol>(),
      SubtractedCylinderSurface()
     {}
     
     /** Copy constructor */                            
     BoundarySubtractedCylinderSurface(const BoundarySubtractedCylinderSurface<Tvol>& bcs) :
       BoundarySurface<Tvol>(bcs),
       SubtractedCylinderSurface(bcs)
     {}
     
     /** Constructor for a Boundary with exact two Volumes attached to it*/
     BoundarySubtractedCylinderSurface(const Tvol* inside, const Tvol* outside, const SubtractedCylinderSurface& csf) :
       BoundarySurface<Tvol>(inside, outside),
       SubtractedCylinderSurface(csf)
     {}     
     
     /** Constructor for a Boundary with two VolumeArrays attached to it*/
     BoundarySubtractedCylinderSurface(SharedObject<VolumeArray> insideArray, SharedObject<VolumeArray> outsideArray, const SubtractedCylinderSurface& csf) :
       BoundarySurface<Tvol>(insideArray, outsideArray),
       SubtractedCylinderSurface(csf)
     {}         
     
     /** Copy constructor with a shift */
     BoundarySubtractedCylinderSurface(const Tvol* inside, const Tvol* outside, const SubtractedCylinderSurface& csf, const Amg::Transform3D& tr) :
       BoundarySurface<Tvol>(inside,outside),
       SubtractedCylinderSurface(csf,tr)
     {}     
     
     /**Virtual Destructor*/
     virtual ~BoundarySubtractedCylinderSurface() = default;
     
     /** Get the next Volume depending on the TrackParameters and the requested direction,
         gives back 0 if there's no volume attached to the requested direction
         - this is speed optimized as it doesn't invoke a local to global transformation
       */
     virtual const Tvol* attachedVolume(const TrackParameters& parms,
                                        PropDirection dir) const override final;

     /** Get the next Volume depending on GlobalPosition, GlobalMomentum, dir
      on the TrackParameters and the requested direction */
     virtual const Tvol* attachedVolume(const Amg::Vector3D& pos,
                                        const Amg::Vector3D& mom,
                                        PropDirection dir) const override final;

     /** The Surface Representation of this */
     virtual const Surface& surfaceRepresentation() const override final;

     /**Assignment operator*/
     BoundarySubtractedCylinderSurface& operator=(const BoundarySubtractedCylinderSurface& vol);
     
   protected:
                             
  };

template <class Tvol> inline const Surface& BoundarySubtractedCylinderSurface<Tvol>::surfaceRepresentation() const { return *this; }

// Hash include the inline functions
#include "TrkVolumes/BoundarySubtractedCylinderSurface.icc"

  
} // end of namespace Trk

#endif // TRKVOLUMES_BOUNDARYSUBTRACTEDCYLINDERSURFACE_H

