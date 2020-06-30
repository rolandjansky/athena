/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// AbstractVolume.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKVOLUMES_ABSTRACTVOLUME_H
#define TRKVOLUMES_ABSTRACTVOLUME_H

// Trk
#include "TrkVolumes/Volume.h"
#include "TrkVolumes/BoundarySurface.h"
#include "TrkDetDescrUtils/SharedObject.h"
// STD
#include <vector>
// Eigen
#include "GeoPrimitives/GeoPrimitives.h"

#include "CxxUtils/checker_macros.h"

class MsgStream;

namespace Trk {

  class VolumeBounds;

  /** 
   @class AbstractVolume
    
   AbstractVolume description inside the tracking realm. This is the purely geometrical object volume.
    
   The Trk::AbstractVolume is constructed by giving a pointer to a HepGeom::Transform3D 
   and a pointer to Trk::VolumeBounds, this implies that the ownership of the 
   objects pointed to is passed as well.
     
   A Trk::AbstractVolume is in first place a collection class of Trk::BoundarySurface,
   the vector of Trk::BoundarySurface is returned by the Trk::VolumeBounds that
   carry a decompose method.
    
   Boundary surfaces can be shared between AbstractVolumes to enhance automatic navigation
   between AbstractVolumes, therefor they are reference counted by a Trk::SharedObject holder class.
    
   @image html VolumeShapes.gif       
    
   @author Andreas.Salzburger@cern.ch 
   */
  
  class AbstractVolume : public Volume {

    public:
     /**Default Constructor - needed for pool and inherited classes */
     AbstractVolume();
     
     /**Copy constructor */                            
     AbstractVolume(const AbstractVolume& vol);
     
     /**Constructor with Amg::Transform3D*, VolumeBounds*, passing ownership */
     AbstractVolume(Amg::Transform3D* htrans, VolumeBounds* volbounds) ATLAS_CTORDTOR_NOT_THREAD_SAFE;
     
     /**Virtual Destructor*/
     virtual ~AbstractVolume();
     
     /**Assignment operator*/
     AbstractVolume& operator=(const AbstractVolume& vol);
     
     /**Implicit constructor*/
     AbstractVolume* clone() const;
     
     /** Method to return the BoundarySurfaces */
     const std::vector< SharedObject<const BoundarySurface<AbstractVolume> > >& boundarySurfaces() const;
         
   private:
     /**Private method to create BoundarySurfaces */  
     void createBoundarySurfaces ATLAS_NOT_THREAD_SAFE ();
     
     std::vector< SharedObject<const BoundarySurface<AbstractVolume> > >* m_boundarySurfaces;  //!< boundary Surfaces
                               
  };
  
  inline AbstractVolume* AbstractVolume::clone() const { return new AbstractVolume(*this); }    
  
} // end of namespace

#endif // TRKVOLUMES_ABSTRACTVOLUME_H

