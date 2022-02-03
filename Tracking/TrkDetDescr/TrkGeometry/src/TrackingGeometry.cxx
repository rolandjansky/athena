/*
  Copyright (C) 2002-2022  CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TrackingGeometry.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// Trk
#include "TrkGeometry/TrackingGeometry.h"

#include "TrkGeometry/DetachedTrackingVolume.h"
#include "TrkGeometry/Layer.h"
#include "TrkGeometry/MagneticFieldProperties.h"
#include "TrkGeometry/MaterialProperties.h"
#include "TrkGeometry/TrackingVolume.h"
// GaudiKernel
#include "GaudiKernel/MsgStream.h"

Trk::TrackingGeometry::TrackingGeometry(Trk::TrackingVolume* highestVolume,
                                        Trk::NavigationLevel navLev)
  : m_world(highestVolume)
  , m_navigationLevel(navLev)
{
  // for the time being only world
  if (m_world)
    registerTrackingVolumes(*m_world);
}

Trk::TrackingGeometry::~TrackingGeometry()
{
  delete m_world;
  auto bLayerIter = m_boundaryLayers.begin();
  auto bLayerIterE = m_boundaryLayers.end();
  for (; bLayerIter != bLayerIterE; ++bLayerIter)
    delete bLayerIter->first;
}

const Trk::TrackingVolume*
Trk::TrackingGeometry::lowestTrackingVolume(const Amg::Vector3D& gp) const
{
  const Trk::TrackingVolume* searchVolume = m_world;
  const Trk::TrackingVolume* currentVolume = nullptr;
  while (currentVolume != searchVolume && searchVolume) {
    currentVolume = searchVolume;
    searchVolume = searchVolume->associatedSubVolume(gp);
  }
  return (currentVolume);
}

std::vector<const Trk::DetachedTrackingVolume*>*
Trk::TrackingGeometry::lowestDetachedTrackingVolumes(
  const Amg::Vector3D& gp) const
{
  double tol = 0.001;
  const Trk::TrackingVolume* currentVolume = lowestStaticTrackingVolume(gp);
  if (currentVolume)
    return currentVolume->assocDetachedSubVolumes(gp, tol);
  return nullptr;
}

const Trk::TrackingVolume*
Trk::TrackingGeometry::lowestStaticTrackingVolume(const Amg::Vector3D& gp) const
{
  const Trk::TrackingVolume* searchVolume = m_world;
  const Trk::TrackingVolume* currentVolume = nullptr;
  while (currentVolume != searchVolume && searchVolume) {
    currentVolume = searchVolume;
    if (!(searchVolume->confinedDetachedVolumes()))
      searchVolume = searchVolume->associatedSubVolume(gp);
  }
  return (currentVolume);
}

void Trk::TrackingGeometry::registerTrackingVolumes
ATLAS_NOT_THREAD_SAFE(const Trk::TrackingVolume& tvol,
                      const Trk::TrackingVolume* mvol,
                      int lvl)
{
  int sublvl = lvl + 1;
  std::string indent = "";
  for (int l = 0; l < lvl; ++l, indent += "  ")
    ;

  const_cast<Trk::TrackingVolume&>(tvol).setMotherVolume(mvol);
  m_trackingVolumes[tvol.volumeName()] = (&tvol);
  const Trk::BinnedArray<const Trk::TrackingVolume>* confinedVolumes = tvol.confinedVolumes();
  if (confinedVolumes) {
    Trk::BinnedArraySpan<Trk::TrackingVolume const * const> volumes =
      confinedVolumes->arrayObjects();
    for (const auto& volumesIter : volumes)
      if (volumesIter)
        registerTrackingVolumes(*volumesIter, &tvol, sublvl);
  }

  const std::vector<const Trk::TrackingVolume*>* confinedDenseVolumes =
    tvol.confinedDenseVolumes();
  if (confinedDenseVolumes) {
    for (const auto& volumesIter : (*confinedDenseVolumes))
      if (volumesIter)
        registerTrackingVolumes(*volumesIter, &tvol, sublvl);
  }
  /** should detached tracking volumes be part of the tracking geometry ? */
  const std::vector<const Trk::DetachedTrackingVolume*>*
    confinedDetachedVolumes = tvol.confinedDetachedVolumes();
  if (confinedDetachedVolumes) {
    for (const auto& volumesIter : (*confinedDetachedVolumes))
      if (volumesIter &&
          tvol.inside(volumesIter->trackingVolume()->center(), 0.))
        registerTrackingVolumes(
          *(volumesIter->trackingVolume()), &tvol, sublvl);
  }
  /** register the boundary layers */
  // boundary layers
  for (const auto& bSurface : tvol.boundarySurfaces()) {
    const Trk::Layer* bLayer =
      bSurface->surfaceRepresentation().materialLayer();
    if (bLayer) {
      auto bfIter = m_boundaryLayers.find(bLayer);
      if (bfIter != m_boundaryLayers.end())
        ++(bfIter->second);
      else
        m_boundaryLayers[bLayer] = 0;
    }
  }
}

void Trk::TrackingGeometry::compactify
ATLAS_NOT_THREAD_SAFE(MsgStream& msg, const TrackingVolume* vol)
{
  msg << MSG::VERBOSE
      << "====== Calling TrackingGeometry::compactify() ===== " << std::endl;
  const Trk::TrackingVolume* tVolume = vol ? vol : m_world;
  size_t cSurfaces = 0;
  size_t tSurfaces = 0;
  if (tVolume) {
    const_cast<TrackingVolume*>(tVolume)->compactify(cSurfaces, tSurfaces);
  }
  msg << MSG::VERBOSE << "  --> set TG ownership of " << cSurfaces << " out of "
      << tSurfaces << std::endl;
  for (auto bLayerIter = m_boundaryLayers.begin();
       bLayerIter != m_boundaryLayers.end();
       ++bLayerIter) {
    const_cast<Trk::Surface&>(bLayerIter->first->surfaceRepresentation())
      .setOwner(Trk::TGOwn);
  }
  msg << MSG::VERBOSE << "  --> set TG ownership of " << m_boundaryLayers.size()
      << " boundary layers." << std::endl;
  msg << MSG::VERBOSE << endmsg;
}

void Trk::TrackingGeometry::synchronizeLayers
ATLAS_NOT_THREAD_SAFE(MsgStream& msg, const TrackingVolume* vol)
{
  const Trk::TrackingVolume* tVolume = vol ? vol : m_world;
  const_cast<TrackingVolume*>(tVolume)->synchronizeLayers(msg);
}


Trk::TrackingVolume*
Trk::TrackingGeometry::checkoutHighestTrackingVolume()
{
  Trk::TrackingVolume* checkoutVolume = m_world;
  m_world = nullptr;
  // clear the boundary layers they go with the highest volume
  m_boundaryLayers.clear();
  return checkoutVolume;
}



void
Trk::TrackingGeometry::printVolumeHierarchy(MsgStream& msg) const
{
  msg << "TrackingGeometry Summary : " << std::endl;
  const Trk::TrackingVolume* highestVolume = highestTrackingVolume();
  int level = 0;
  if (highestVolume)
    printVolumeInformation(msg, *highestVolume, level);
  msg << endmsg;
}

void
Trk::TrackingGeometry::printVolumeInformation(MsgStream& msg,
                                              const Trk::TrackingVolume& tvol,
                                              int lvl) const
{
  int sublevel = lvl + 1;

  for (int indent = 0; indent < sublevel; ++indent)
    msg << "  ";
  msg << "TrackingVolume ( at : " << (&tvol) << ") name: " << tvol.volumeName()
      << std::endl;

  const Trk::BinnedArray<Trk::Layer>* confinedLayers = tvol.confinedLayers();
  if (confinedLayers) {
    Trk::BinnedArraySpan<Trk::Layer const * const> layers =
      confinedLayers->arrayObjects();
    for (int indent = 0; indent < sublevel; ++indent)
      msg << "  ";
    msg << "- found : " << layers.size() << " confined Layers" << std::endl;
  }

  const Trk::BinnedArray<const Trk::TrackingVolume>* confinedVolumes = tvol.confinedVolumes();
  if (confinedVolumes) {
    Trk::BinnedArraySpan<Trk::TrackingVolume const * const> volumes = confinedVolumes->arrayObjects();

    for (int indent = 0; indent < sublevel; ++indent)
      msg << "  ";
    msg << "- found : " << volumes.size() << " confined TrackingVolumes"
        << std::endl;

    for (const auto& volumesIter : volumes)
      if (volumesIter)
        printVolumeInformation(msg, *volumesIter, sublevel);
  }

  const std::vector<const Trk::TrackingVolume*>* confinedDenseVolumes =
    tvol.confinedDenseVolumes();
  if (confinedDenseVolumes) {
    for (int indent = 0; indent < sublevel; ++indent)
      msg << "  ";
    msg << "- found : " << confinedDenseVolumes->size()
        << " confined unordered (dense) TrackingVolumes" << std::endl;

    for (const auto& volumesIter : (*confinedDenseVolumes))
      if (volumesIter)
        printVolumeInformation(msg, *volumesIter, sublevel);
  }
}

void Trk::TrackingGeometry::indexStaticLayers
ATLAS_NOT_THREAD_SAFE(GeometrySignature geosit, int offset)
{
  if (m_world) {
    m_world->indexContainedStaticLayers(geosit, offset);
    m_world->indexContainedMaterialLayers(geosit, offset);
  }
}

bool
Trk::TrackingGeometry::atVolumeBoundary(const Amg::Vector3D& gp,
                                        const Trk::TrackingVolume* vol,
                                        double tol)
{
  bool isAtBoundary = false;
  if (!vol)
    return isAtBoundary;
  const std::vector<SharedObject<const BoundarySurface<TrackingVolume>>>&
    bounds = vol->boundarySurfaces();
  for (unsigned int ib = 0; ib < bounds.size(); ib++) {
    const Trk::Surface& surf = (bounds[ib].get())->surfaceRepresentation();
    if (surf.isOnSurface(gp, true, tol, tol))
      isAtBoundary = true;
  }
  return isAtBoundary;
}

/** check position at volume boundary + navigation link */
bool
Trk::TrackingGeometry::atVolumeBoundary(const Amg::Vector3D& gp,
                                        const Amg::Vector3D& mom,
                                        const TrackingVolume* vol,
                                        const TrackingVolume*& nextVol,
                                        Trk::PropDirection dir,
                                        double tol)
{
  bool isAtBoundary = false;
  nextVol = nullptr;
  if (!vol)
    return isAtBoundary;
  const std::vector<SharedObject<const BoundarySurface<TrackingVolume>>>&
    bounds = vol->boundarySurfaces();
  for (unsigned int ib = 0; ib < bounds.size(); ib++) {
    const Trk::Surface& surf = (bounds[ib].get())->surfaceRepresentation();
    if (surf.isOnSurface(gp, true, tol, tol)) {
      isAtBoundary = true;
      const Trk::TrackingVolume* attachedVol =
        (bounds[ib].get())->attachedVolume(gp, mom, dir);
      if (!nextVol && attachedVol)
        nextVol = attachedVol;
    }
  }
  return isAtBoundary;
}
