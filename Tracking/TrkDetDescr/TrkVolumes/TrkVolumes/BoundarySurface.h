/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// BoundarySurface.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKVOLUMES_BOUNDARYSURFACE_H
#define TRKVOLUMES_BOUNDARYSURFACE_H

// Trk
#include "TrkVolumes/Volume.h"
#include "TrkDetDescrUtils/BinnedArray.h"
#include "TrkDetDescrUtils/SharedObject.h"
#include "TrkEventPrimitives/PropDirection.h"
#include "TrkParameters/TrackParameters.h"
// Gaudi
#include "GaudiKernel/MsgStream.h"

#include "GeoPrimitives/GeoPrimitives.h"

namespace Trk {
 
//class TrackParameters;
class Surface;

  /** 
   @class BoundarySurface

   Description of a BoundarySurface inside the tracking realm,
   it extends the Surface description to make a surface being a boundary of a
   Trk::Volume, Trk::TrackingVolume or a Trk::MagneticFieldVolume.
    
   To avoid dynamic_cast operations the BoundarySurface class is realized as a templated class,
   with the Volume type as the template argument.
   
   A Trk::BoundarySurface can have an inside Volume and an outside Volume, resp.
   a Trk::BinnedArray for inside or outside direction.
    
   The GeometryBuilder as defined in the TrkDetDescrTools Package is declared 
   to be friend, so that it can glue Volumes together by sharing the same
   Boundary Surface.
    
   @author Andreas.Salzburger@cern.ch 
  */
  
  template <class Tvol> class BoundarySurface{

    /** typedef the BinnedArray */
    typedef BinnedArray<Tvol> VolumeArray; 
    
    /** delcare the TrackingVolume Manipulator as friend */
    friend class TrackingVolumeManipulator;
    /** delcare the TrackingVolume as friend */
    friend class TrackingVolume;

    public:
     /** Default Constructor - needed for pool and inherited classes */
     BoundarySurface() :
       m_insideVolume(0),
       m_outsideVolume(0),
       m_insideVolumeArray(),
       m_outsideVolumeArray()
       {}
              
     /** Constructor for a Boundary with exact two Volumes attached to it*/
     BoundarySurface(const Tvol* inside, const Tvol* outside) :
       m_insideVolume(inside),
       m_outsideVolume(outside),
       m_insideVolumeArray(),
       m_outsideVolumeArray()
       {}     
       
     /** Constructor for a Boundary with exact two Volumes attached to it*/
     BoundarySurface(SharedObject<VolumeArray> insideArray, SharedObject<VolumeArray> outsideArray) :
       m_insideVolume(),
       m_outsideVolume(),
       m_insideVolumeArray(insideArray),
       m_outsideVolumeArray(outsideArray)
       {}         
       
     /** Get the next Volume depending on the TrackParameters and the requested direction */
     virtual const Tvol* attachedVolume(const TrackParameters& parms,
                                        PropDirection dir) const = 0;
                                          
     /** Get the next Volume depending on
      GlobalPosition, GlobalMomentum, dir
      on the TrackParameters and the requested direction */
     virtual const Tvol* attachedVolume(const Amg::Vector3D& pos,
                                        const Amg::Vector3D& mom,
                                        PropDirection dir) const = 0;

                                               
     /** templated onBoundary method */
     template <class T> bool onBoundary(const T& pars) const
     { return surfaceRepresentation().onSurface(pars); }
                                                                              

     /** The Surface Representation of this */
     virtual const Surface& surfaceRepresentation() const = 0;
     
     /**Virtual Destructor*/
     virtual ~BoundarySurface()
     {}
     
     /** output debug information */
     void debugInfo(MsgStream& msg) const;
         
   protected:         
     mutable const Tvol*                 m_insideVolume;
     mutable const Tvol*                 m_outsideVolume;
     mutable SharedObject<VolumeArray>   m_insideVolumeArray;
     mutable SharedObject<VolumeArray>   m_outsideVolumeArray;
                             
  };
  
  template <class Tvol> inline void BoundarySurface<Tvol>::debugInfo(MsgStream& msg) const
  {
  msg << "BoundarySurface debug information: " << std::endl;
  msg << "     -> pointer to insideVolume         = " << m_insideVolume       << std::endl;
  msg << "     -> pointer to insideVolumeArray    = " << m_insideVolumeArray.getPtr()  << std::endl;
  msg << "     -> pointer to outsideVolume        = " << m_outsideVolume      << std::endl;
  msg << "     -> pointer to outsideVolumeArray   = " << m_outsideVolumeArray.getPtr() << endreq;
  
  
  }
  
                         
} // end of namespace Trk

#endif // TRKVOLUMES_BOUNDARYSURFACE_H

