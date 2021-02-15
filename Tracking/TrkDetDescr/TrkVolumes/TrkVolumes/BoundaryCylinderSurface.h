/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// BoundaryCylinderSurface.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKVOLUMES_BOUNDARYCYLIMDERSURFACE_H
#define TRKVOLUMES_BOUNDARYCYLIMDERSURFACE_H

//Trk
#include "TrkSurfaces/CylinderSurface.h"
#include "TrkVolumes/BoundarySurface.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkEventPrimitives/PropDirection.h"
#include "GaudiKernel/SystemOfUnits.h"

namespace Trk {

//class TrackParameters;
class Volume;

  /** 
   @class BoundaryCylinderSurface

   BoundaryCylinderSurface description inside the tracking realm,
   it extends the Surface description to make a surface being a boundary of a
   Trk::Volume
    
   @author Andreas.Salzburger@cern.ch
   */
      
  template <class Tvol> class BoundaryCylinderSurface final: 
                              virtual public BoundarySurface<Tvol>, public CylinderSurface {
  
    /** typedef the BinnedArray */
    typedef BinnedArray<Tvol> VolumeArray;                            


    public:
     /** Default Constructor - needed for pool and inherited classes */
     BoundaryCylinderSurface():
      BoundarySurface<Tvol>(),
      CylinderSurface()
     {}
     
     /** Copy constructor */                            
     BoundaryCylinderSurface(const BoundaryCylinderSurface<Tvol>& bcs) :
       BoundarySurface<Tvol>(bcs),
       CylinderSurface(bcs)
     {}
     
     /** Constructor for a Boundary with exact two Volumes attached to it*/
     BoundaryCylinderSurface(const Tvol* inside, const Tvol* outside, const CylinderSurface& csf) :
       BoundarySurface<Tvol>(inside, outside),
       CylinderSurface(csf)
     {}
       
     /** Constructor for a Boundary with two VolumeArrays attached to it*/
     BoundaryCylinderSurface(SharedObject<VolumeArray> insideArray, SharedObject<VolumeArray> outsideArray, const CylinderSurface& csf) :
       BoundarySurface<Tvol>(insideArray, outsideArray),
       CylinderSurface(csf)
     {}       
          
     /** Copy constructor with a shift */
     BoundaryCylinderSurface(const Tvol* inside, const Tvol* outside, const CylinderSurface& csf, const Amg::Transform3D& tr) :
       BoundarySurface<Tvol>(inside,outside),
       CylinderSurface(csf,tr)
     {}

     /**Virtual Destructor*/
     virtual ~BoundaryCylinderSurface() = default;

     /** Get the next Volume depending on the TrackParameters and the requested
        direction, gives back 0 if there's no volume attached to the requested
        direction
         - this is speed optimized as it doesn't invoke a local to global
        transformation
       */
     virtual const Tvol* attachedVolume(const TrackParameters& parms,
                                        PropDirection dir) const override final;

     /** Get the next Volume depending on position, momentum, dir
      on the TrackParameters and the requested direction */
     virtual const Tvol* attachedVolume(const Amg::Vector3D& pos,
                                        const Amg::Vector3D& mom,
                                        PropDirection dir) const override final;

     /** The Surface Representation of this */
     virtual const Surface& surfaceRepresentation() const override final;

     /**Assignment operator*/
     BoundaryCylinderSurface& operator=(const BoundaryCylinderSurface& vol);
     
   protected:
                             
  };

template <class Tvol> inline const Surface& BoundaryCylinderSurface<Tvol>::surfaceRepresentation() const { return *this; }

// Hash include the inline functions
#include "TrkVolumes/BoundaryCylinderSurface.icc"

  
} // end of namespace Trk

#endif // TRKVOLUMES_BOUNDARYCYLIMDERSURFACE_H

