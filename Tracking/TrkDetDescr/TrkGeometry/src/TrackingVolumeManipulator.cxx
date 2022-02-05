/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TrackingVolumeManipulator.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "TrkGeometry/TrackingVolumeManipulator.h"

#include <utility>

#include "TrkGeometry/TrackingVolume.h"
#include "TrkVolumes/CylinderVolumeBounds.h"

void Trk::TrackingVolumeManipulator::glueVolumes ATLAS_NOT_THREAD_SAFE(
    const Trk::TrackingVolume& firstVol, Trk::BoundarySurfaceFace firstFace,
    const Trk::TrackingVolume& secondVol,
    Trk::BoundarySurfaceFace secondFace) const {
  // check if it is a cylinder volume
  const Trk::CylinderVolumeBounds* cylBounds =
      dynamic_cast<const Trk::CylinderVolumeBounds*>(
          &(firstVol.volumeBounds()));

  if (firstFace == Trk::tubeOuterCover && secondFace == Trk::tubeInnerCover)
    return glueVolumes(secondVol, secondFace, firstVol, firstFace);

  // the second volume gets the face of the first volume assigned
  ((*secondVol.m_boundarySurfaces))[secondFace] =
      (*(firstVol.m_boundarySurfaces))[firstFace];
  // the face of the first volume has been an inner tube
  if (cylBounds && firstFace == Trk::tubeInnerCover &&
      secondFace == Trk::tubeOuterCover){
    const_cast<Trk::BoundarySurface<Trk::TrackingVolume>*>(
      (*secondVol.m_boundarySurfaces)[secondFace].get())
      ->setInsideVolume(&secondVol);
  } else {
    const_cast<Trk::BoundarySurface<Trk::TrackingVolume>*>(
      (*secondVol.m_boundarySurfaces)[secondFace].get())
      ->setOutsideVolume(&secondVol);
  }
}

void Trk::TrackingVolumeManipulator::setBoundarySurface ATLAS_NOT_THREAD_SAFE(
    const Trk::TrackingVolume& tvol,
    Trk::SharedObject<const Trk::BoundarySurface<Trk::TrackingVolume> > bsurf,
    Trk::BoundarySurfaceFace face) {
  (*(tvol.m_boundarySurfaces))[face] = std::move(bsurf);
}

void Trk::TrackingVolumeManipulator::setInsideVolume ATLAS_NOT_THREAD_SAFE(
    const Trk::TrackingVolume& tvol, Trk::BoundarySurfaceFace face,
    const Trk::TrackingVolume* insidevol) {

  const_cast<Trk::BoundarySurface<Trk::TrackingVolume>*>(
    (*tvol.m_boundarySurfaces)[face].get())
    ->setInsideVolume(insidevol);
}

void Trk::TrackingVolumeManipulator::setInsideVolumeArray ATLAS_NOT_THREAD_SAFE(
    const Trk::TrackingVolume& tvol, Trk::BoundarySurfaceFace face,
    Trk::BinnedArray<const Trk::TrackingVolume>* insidevolarray) {

  const_cast<Trk::BoundarySurface<Trk::TrackingVolume>*>(
    (*tvol.m_boundarySurfaces)[face].get())
    ->setInsideVolumeArray(
      Trk::SharedObject<Trk::BinnedArray<const Trk::TrackingVolume>>(insidevolarray));
}

void Trk::TrackingVolumeManipulator::setInsideVolumeArray ATLAS_NOT_THREAD_SAFE(
    const Trk::TrackingVolume& tvol, Trk::BoundarySurfaceFace face,
    const Trk::SharedObject<Trk::BinnedArray<const Trk::TrackingVolume> >&
        insidevolarray) {
  const_cast<Trk::BoundarySurface<Trk::TrackingVolume>*>(
    (*tvol.m_boundarySurfaces)[face].get())
    ->setInsideVolumeArray(insidevolarray);
}

void Trk::TrackingVolumeManipulator::setOutsideVolume ATLAS_NOT_THREAD_SAFE(
    const Trk::TrackingVolume& tvol, Trk::BoundarySurfaceFace face,
    const Trk::TrackingVolume* outsidevol) {
  const_cast<Trk::BoundarySurface<Trk::TrackingVolume>*>(
    (*tvol.m_boundarySurfaces)[face].get())
    ->setOutsideVolume(outsidevol);
}

void Trk::TrackingVolumeManipulator::setOutsideVolumeArray
ATLAS_NOT_THREAD_SAFE(
    const Trk::TrackingVolume& tvol, Trk::BoundarySurfaceFace face,
    Trk::BinnedArray<const Trk::TrackingVolume>* outsidevolarray) {

  const_cast<Trk::BoundarySurface<Trk::TrackingVolume>*>(
    (*tvol.m_boundarySurfaces)[face].get())
    ->setOutsideVolumeArray(
      Trk::SharedObject<Trk::BinnedArray<const Trk::TrackingVolume>>(
        outsidevolarray));
}

void Trk::TrackingVolumeManipulator::setOutsideVolumeArray
ATLAS_NOT_THREAD_SAFE(
  const Trk::TrackingVolume& tvol,
  Trk::BoundarySurfaceFace face,
  const Trk::SharedObject<Trk::BinnedArray<const Trk::TrackingVolume>>&
    outsidevolarray)
{
  const_cast<Trk::BoundarySurface<Trk::TrackingVolume>*>(
    (*tvol.m_boundarySurfaces)[face].get())
    ->setOutsideVolumeArray(outsidevolarray);
}

void Trk::TrackingVolumeManipulator::confineVolume ATLAS_NOT_THREAD_SAFE(
    const TrackingVolume& tvol, const TrackingVolume* outsideVol) {

  const std::vector<SharedObject<const BoundarySurface<TrackingVolume>>>& bounds =
    tvol.boundarySurfaces();
  for (unsigned int ib = 0; ib < bounds.size(); ib++) {
    if (bounds[ib].get()->outsideVolume() == nullptr) {
      const_cast<Trk::BoundarySurface<Trk::TrackingVolume>*>(bounds[ib].get())
        ->setOutsideVolume(outsideVol);
    }
    if (bounds[ib].get()->insideVolume() == nullptr) {
      const_cast<Trk::BoundarySurface<Trk::TrackingVolume>*>(bounds[ib].get())
        ->setInsideVolume(outsideVol);
    }
  }
}
