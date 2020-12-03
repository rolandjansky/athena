/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// BoundarySubtractedPlaneSurface.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKVOLUMES_BOUNDARYSUBTRACTEDPLANESURFACE_H
#define TRKVOLUMES_BOUNDARYSUBTRACTEDPLANESURFACE_H

//Trk
#include "TrkGeometrySurfaces/SubtractedPlaneSurface.h"
#include "TrkVolumes/BoundarySurface.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkEventPrimitives/PropDirection.h"

#include "GeoPrimitives/GeoPrimitives.h"

namespace Trk {

//class TrackParameters;
class Volume;

  /** 
   @class BoundarySubtractedPlaneSurface

   BoundarySubtractedPlaneSurface description inside the tracking realm,
   it extends the SubtractedPlaneSurface description to make a surface being a boundary of a
   Trk::Volume (used for all volume shapes).
   It inherits from BoundarySurface to get the interface of boundaries. 
    
   @author Sarka.Todorova@cern.ch 
  */
  
  template <class Tvol> class BoundarySubtractedPlaneSurface final: 
                               virtual public BoundarySurface<Tvol>, public SubtractedPlaneSurface {

    /** typedef the BinnedArray */
    typedef BinnedArray<Tvol> VolumeArray;                            

    public:
     /** Default Constructor - needed for pool and inherited classes */
     BoundarySubtractedPlaneSurface() :
       BoundarySurface<Tvol>(),
       SubtractedPlaneSurface()
     {}
     
     /** Copy constructor */                            
     BoundarySubtractedPlaneSurface(const BoundarySubtractedPlaneSurface<Tvol>& bps) :
       BoundarySurface<Tvol>(bps),
       SubtractedPlaneSurface(bps)
     {}
     
     /** Constructor for a Boundary with exact two Volumes attached to it*/
     BoundarySubtractedPlaneSurface(const Tvol* inside, const Tvol* outside, const SubtractedPlaneSurface& psf) :
       BoundarySurface<Tvol>(inside, outside),
       SubtractedPlaneSurface(psf)
     {}
     
     /** Constructor for a Boundary with two VolumeArrays attached to it*/
     BoundarySubtractedPlaneSurface(SharedObject<VolumeArray> insideArray, SharedObject<VolumeArray> outsideArray, const SubtractedPlaneSurface& psf) :
       BoundarySurface<Tvol>(insideArray, outsideArray),
       SubtractedPlaneSurface(psf)
     {}        
     
     /** Copy constructor with a shift */
     BoundarySubtractedPlaneSurface(const Tvol* inside, const Tvol* outside, const SubtractedPlaneSurface& psf, const Amg::Transform3D& tr) :
       BoundarySurface<Tvol>(inside,outside),
       SubtractedPlaneSurface(psf,tr)
     {}

     /** Get the next Volume depending on the TrackParameters and the requested
      direction, gives back 0 if there's no volume attached to the requested
      direction
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

     /**Virtual Destructor*/
     virtual ~BoundarySubtractedPlaneSurface() = default;

     /**Assignment operator*/
     BoundarySubtractedPlaneSurface& operator=(const BoundarySubtractedPlaneSurface& vol);
         
   protected:
                             
  };

template <class Tvol> inline const Surface& BoundarySubtractedPlaneSurface<Tvol>::surfaceRepresentation() const { return *this; }

// Hash include inline functions
#include "TrkVolumes/BoundarySubtractedPlaneSurface.icc"
  
} // end of namespace Trk

#endif // TRKVOLUMES_BOUNDARYSUBTRACTEDPLANESURFACE_H

