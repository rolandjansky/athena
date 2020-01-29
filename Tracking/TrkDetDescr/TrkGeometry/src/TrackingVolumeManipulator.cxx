/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TrackingVolumeManipulator.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "TrkVolumes/CylinderVolumeBounds.h"
#include "TrkGeometry/TrackingVolume.h"
#include "TrkGeometry/TrackingVolumeManipulator.h"

void Trk::TrackingVolumeManipulator::glueVolumes(const Trk::TrackingVolume& firstVol,
                                                 Trk::BoundarySurfaceFace firstFace,
                                                 const Trk::TrackingVolume& secondVol,
                                                 Trk::BoundarySurfaceFace secondFace) const
{ 
   // check if it is a cylinder volume
   const Trk::CylinderVolumeBounds* cylBounds = dynamic_cast<const Trk::CylinderVolumeBounds*>(&(firstVol.volumeBounds()));
   
   if (firstFace == Trk::tubeOuterCover && secondFace == Trk::tubeInnerCover)
      return glueVolumes(secondVol,secondFace,firstVol,firstFace);
             
   // the second volume gets the face of the first volume assigned
   ((*secondVol.m_boundarySurfaces))[secondFace] = (*(firstVol.m_boundarySurfaces))[firstFace];
   // the face of the first volume has been an inner tube
   if (cylBounds && firstFace==Trk::tubeInnerCover && secondFace==Trk::tubeOuterCover)
      ((*secondVol.m_boundarySurfaces))[secondFace]->setInsideVolume(&secondVol);
   else 
      ((*secondVol.m_boundarySurfaces))[secondFace]->setOutsideVolume(&secondVol);
}

void Trk::TrackingVolumeManipulator::setBoundarySurface(const Trk::TrackingVolume& tvol,
                                                       Trk::SharedObject<const Trk::BoundarySurface<Trk::TrackingVolume> > bsurf,
                                                       Trk::BoundarySurfaceFace face) const
{ 
   (*(tvol.m_boundarySurfaces))[face] = bsurf;
}                                                 
      
void Trk::TrackingVolumeManipulator::setInsideVolume(const Trk::TrackingVolume& tvol,
                                                     Trk::BoundarySurfaceFace face,
                                                     const Trk::TrackingVolume* insidevol) const
{ ((*tvol.m_boundarySurfaces)[face])->setInsideVolume(insidevol); }

void Trk::TrackingVolumeManipulator::setInsideVolumeArray(const Trk::TrackingVolume& tvol,
                                                          Trk::BoundarySurfaceFace face,
                                                          Trk::BinnedArray<Trk::TrackingVolume>* insidevolarray) const
{   ((*tvol.m_boundarySurfaces)[face])->setInsideVolumeArray(Trk::SharedObject<Trk::BinnedArray<Trk::TrackingVolume> >(insidevolarray)); }
      
void Trk::TrackingVolumeManipulator::setInsideVolumeArray(const Trk::TrackingVolume& tvol,
                                                          Trk::BoundarySurfaceFace face,
                                                          Trk::SharedObject<Trk::BinnedArray<Trk::TrackingVolume> >insidevolarray) const
{   ((*tvol.m_boundarySurfaces)[face])->setInsideVolumeArray(Trk::SharedObject<Trk::BinnedArray<Trk::TrackingVolume> >(insidevolarray));
}
      
void Trk::TrackingVolumeManipulator::setOutsideVolume(const Trk::TrackingVolume& tvol,
                                                      Trk::BoundarySurfaceFace face,
                                                      const Trk::TrackingVolume* outsidevol) const
{ ((*tvol.m_boundarySurfaces)[face])->setOutsideVolume(outsidevol); }

void Trk::TrackingVolumeManipulator::setOutsideVolumeArray(const Trk::TrackingVolume& tvol,
                                                           Trk::BoundarySurfaceFace face,
                                                           Trk::BinnedArray<Trk::TrackingVolume>* outsidevolarray) const         
{  ((*tvol.m_boundarySurfaces)[face])->setOutsideVolumeArray(Trk::SharedObject<Trk::BinnedArray<Trk::TrackingVolume> >(outsidevolarray)); } 

void Trk::TrackingVolumeManipulator::setOutsideVolumeArray(const Trk::TrackingVolume& tvol,
                                                           Trk::BoundarySurfaceFace face,
                                                           Trk::SharedObject<Trk::BinnedArray<Trk::TrackingVolume> >outsidevolarray) const         
{  ((*tvol.m_boundarySurfaces)[face])->setOutsideVolumeArray(Trk::SharedObject<Trk::BinnedArray<Trk::TrackingVolume> >(outsidevolarray)); } 


void Trk::TrackingVolumeManipulator::confineVolume(const TrackingVolume& tvol,
                                                   const TrackingVolume* outsideVol) const
{
  const std::vector< SharedObject<const BoundarySurface<TrackingVolume> > > bounds = tvol.boundarySurfaces();
  for (unsigned int ib = 0; ib < bounds.size(); ib++) {
    if ( bounds[ib].get()->outsideVolume() == 0 )  bounds[ib].get()->setOutsideVolume(outsideVol); 
    if ( bounds[ib].get()->insideVolume() == 0 )   bounds[ib].get()->setInsideVolume(outsideVol); 
  }
}
