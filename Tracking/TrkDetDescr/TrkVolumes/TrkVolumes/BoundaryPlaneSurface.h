/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// BoundaryPlaneSurface.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKVOLUMES_BOUNDARYPLANESURFACE_H
#define TRKVOLUMES_BOUNDARYPLANESURFACE_H

//Trk
#include "TrkSurfaces/PlaneSurface.h"
#include "TrkVolumes/BoundarySurface.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkEventPrimitives/PropDirection.h"
#include "TrkParameters/TrackParameters.h"

#include "GeoPrimitives/GeoPrimitives.h"

namespace Trk {

//class TrackParameters;
class Volume;

  /** 
   @class BoundaryPlaneSurface

   BoundaryPlaneSurface description inside the tracking realm,
   it extends the PlaneSurface description to make a surface being a boundary of a
   Trk::Volume (used for all volume shapes).
   It inherits from BoundarySurface to get the interface of boundaries. 
    
   @author Andreas.Salzburger@cern.ch 
  */
  
  template <class Tvol> class BoundaryPlaneSurface final: 
                               virtual public BoundarySurface<Tvol>, public PlaneSurface {

    /** typedef the BinnedArray */
    typedef BinnedArray<Tvol> VolumeArray;                            

    public:
     /** Default Constructor - needed for pool and inherited classes */
     BoundaryPlaneSurface() :
       BoundarySurface<Tvol>(),
       PlaneSurface()
     {}
     
     /** Copy constructor */                            
     BoundaryPlaneSurface(const BoundaryPlaneSurface<Tvol>& bps) :
       BoundarySurface<Tvol>(bps),
       PlaneSurface(bps)
     {}
     
     /** Constructor for a Boundary with exact two Volumes attached to it*/
     BoundaryPlaneSurface(const Tvol* inside, const Tvol* outside, const PlaneSurface& psf) :
       BoundarySurface<Tvol>(inside, outside),
       PlaneSurface(psf)
     {}

     /** Constructor for a Boundary with two VolumeArrays attached to it*/
     BoundaryPlaneSurface(SharedObject<VolumeArray> insideArray, SharedObject<VolumeArray> outsideArray, const PlaneSurface& psf) :
       BoundarySurface<Tvol>(insideArray, outsideArray),
       PlaneSurface(psf)
     {}
     
     /** Copy constructor with a shift */
     BoundaryPlaneSurface(const Tvol* inside, const Tvol* outside, const PlaneSurface& psf, const Amg::Transform3D& tr) :
       BoundarySurface<Tvol>(inside,outside),
       PlaneSurface(psf,tr)
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
     virtual ~BoundaryPlaneSurface() = default;

     /**Assignment operator*/
     BoundaryPlaneSurface& operator=(const BoundaryPlaneSurface& vol);
         
   protected:
                             
  };

template <class Tvol> inline const Surface& BoundaryPlaneSurface<Tvol>::surfaceRepresentation() const { return *this; }

// Hash include inline functions
#include "TrkVolumes/BoundaryPlaneSurface.icc"
  
} // end of namespace Trk

#endif // TRKVOLUMES_BOUNDARYPLANESURFACE_H

