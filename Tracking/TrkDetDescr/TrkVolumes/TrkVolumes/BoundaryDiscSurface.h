/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// BoundaryDiscSurface.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKVOLUMES_BOUNDARYDISCSURFACE_H
#define TRKVOLUMES_BOUNDARYDISCSURFACE_H

//Trk
#include "TrkSurfaces/DiscSurface.h"
#include "TrkVolumes/BoundarySurface.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkEventPrimitives/PropDirection.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkDetDescrUtils/SharedObject.h"
// CLHEP
#include "GeoPrimitives/GeoPrimitives.h"

namespace Trk {

//class TrackParameters;
class Volume;

  /** 
    @class BoundaryDiscSurface

    BoundaryDiscSurface description inside the tracking realm,
    it extends the DiscSurface description to make a surface being a boundary of a
    Trk::Volume (used for cylindrical shape). 
    It inherits from BoundarySurface to get the interface of boundaries. 
    
    @author Andreas.Salzburger@cern.ch 
   */
  
  template <class Tvol> class BoundaryDiscSurface final: 
                       virtual public BoundarySurface<Tvol>, public DiscSurface {
                           
    /** typedef the BinnedArray */
    typedef BinnedArray<Tvol> VolumeArray;                            

    public:
     /** Default Constructor - needed for pool and inherited classes */
     BoundaryDiscSurface():
      BoundarySurface<Tvol>(),
      DiscSurface()
     {}
     
     /** Copy constructor */                            
     BoundaryDiscSurface(const BoundaryDiscSurface<Tvol>& bds) :
       BoundarySurface<Tvol>(bds),
       DiscSurface(bds)
     {}
     
     /** Constructor for a Boundary with exact two Volumes attached to it*/
     BoundaryDiscSurface(const Tvol* inside, const Tvol* outside, const DiscSurface& dsf) :
       BoundarySurface<Tvol>(inside, outside),
       DiscSurface(dsf)
     {}
     
     /** Constructor for a Boundary with two VolumeArrays attached to it*/
     BoundaryDiscSurface(SharedObject<VolumeArray> insideArray, SharedObject<VolumeArray> outsideArray, const DiscSurface& dsf) :
       BoundarySurface<Tvol>(insideArray, outsideArray),
       DiscSurface(dsf)
     {}
     
     /** Copy constructor with a shift */
     BoundaryDiscSurface(const Tvol* inside, const Tvol* outside, const DiscSurface& dsf, const Amg::Transform3D& tr) :
       BoundarySurface<Tvol>(inside,outside),
       DiscSurface(dsf,tr)
     {}

     /** Get the next Volume depending on the TrackParameters and the requested
        direction, gives back 0 if there's no volume attached to the requested
        direction
         - this is speed optimized as it doesn't invoke a local to global
        transformation
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
     virtual ~BoundaryDiscSurface() = default;

     /**Assignment operator*/
     BoundaryDiscSurface& operator=(const BoundaryDiscSurface& vol);
             
   protected:
                             
  };

template <class Tvol> inline const Surface& BoundaryDiscSurface<Tvol>::surfaceRepresentation() const { return *this; }

// Hash include inline functions
#include "TrkVolumes/BoundaryDiscSurface.icc"
  
} // end of namespace Trk

#endif // TRKVOLUMES_BOUNDARYDISCSURFACE_H

