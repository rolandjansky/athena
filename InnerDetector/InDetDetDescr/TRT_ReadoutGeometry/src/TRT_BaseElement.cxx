/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TRT_ReadoutGeometry/TRT_BaseElement.h"
#include "TRT_ReadoutGeometry/TRT_Conditions.h"

#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Geometry/Transform3D.h"
#include "CLHEP/Geometry/Vector3D.h"
#include "GeoModelUtilities/GeoAlignmentStore.h"
#include "GeoPrimitives/CLHEPtoEigenConverter.h"

#include "InDetIdentifier/TRT_ID.h"

#include <vector>

namespace InDetDD {

TRT_BaseElement::TRT_BaseElement(const GeoVFullPhysVol* volume,
                                 const Identifier& id,
                                 const TRT_ID* idHelper,
                                 const TRT_Conditions* conditions,
                                 const GeoAlignmentStore* geoAlignStore)
  : Trk::TrkDetElementBase(volume)
  , m_id(id)
  , m_idHelper(idHelper)
  , m_conditions(conditions)
  , m_surfaceCache{}
  , m_surface{}
  , m_surfaces{}
  , m_geoAlignStore(geoAlignStore)
{
  m_idHash = m_idHelper->straw_layer_hash(id);
}

TRT_BaseElement::TRT_BaseElement(const TRT_BaseElement& right,
                                 const GeoAlignmentStore* geoAlignmentStore)
  : Trk::TrkDetElementBase(right.getMaterialGeom())
  , m_id(right.m_id)
  , m_idHash(right.m_idHash)
  , m_idHelper(right.m_idHelper)
  , m_conditions(right.m_conditions)
  , m_geoAlignStore(geoAlignmentStore)
{}

Identifier
TRT_BaseElement::identify() const
{
  return m_id;
}

IdentifierHash
TRT_BaseElement::identifyHash() const
{
  return m_idHash;
}

// [0] GeoModel / CLHEP Access
const HepGeom::Transform3D
TRT_BaseElement::getAbsoluteTransform(int straw) const
{
  return Amg::EigenTransformToCLHEP(strawTransform(straw));
}

// [A] description of the current detector element : Amg first, then CLHEP
// converted methods
const Trk::Surface&
TRT_BaseElement::surface() const
{
  return elementSurface();
}

const Trk::SurfaceBounds&
TRT_BaseElement::bounds() const
{
  if (not m_surfaceCache) {
    createSurfaceCache();
  }
  return *(m_surfaceCache->bounds());
}

const Amg::Transform3D&
TRT_BaseElement::transform() const
{
  if (not m_surfaceCache) {
    createSurfaceCache();
  }
  return *(m_surfaceCache->transform());
}

const Amg::Vector3D&
TRT_BaseElement::center() const
{
  if (not m_surfaceCache) {
    createSurfaceCache();
  }
  return *(m_surfaceCache->center());
}

const Amg::Vector3D&
TRT_BaseElement::normal() const
{
  if (not m_surfaceCache) {
    createSurfaceCache();
  }
  return *(m_surfaceCache->normal());
}

// [B] Description of the individual straws
const Trk::Surface&
TRT_BaseElement::surface(const Identifier& id) const
{
  int straw = m_idHelper->straw(id);
  if (!m_strawSurfaces[straw]) {
    createSurfaceCache(id);
  }
  return *(m_strawSurfaces[straw]);
}

const std::vector<const Trk::Surface*>&
TRT_BaseElement::surfaces() const
{
  if (!m_surfaces.isValid()) {
    std::vector<const Trk::Surface*> tmp_surfaces;
    tmp_surfaces.reserve(nStraws());
    for (unsigned is = 0; is < nStraws(); ++is) {
      tmp_surfaces.push_back(&strawSurface(is));
    }
    m_surfaces.set(tmp_surfaces);
  }
  return *(m_surfaces.ptr());
}

const Trk::SurfaceBounds&
TRT_BaseElement::bounds(const Identifier&) const
{
  return strawBounds();
}

const Amg::Transform3D&
TRT_BaseElement::transform(const Identifier& id) const
{
  int straw = m_idHelper->straw(id);
  if (!m_strawSurfacesCache[straw]) {
    createSurfaceCache(id);
  }
  // forward the transform of the cache
  return *(m_strawSurfacesCache[straw]->transform());
}

const Amg::Transform3D&
TRT_BaseElement::strawTransform(unsigned int straw) const
{
  if (!m_strawSurfacesCache[straw]) {
    Identifier id = m_idHelper->straw_id(identify(), straw);
    createSurfaceCache(id);
  }
  // forward the transform of the cache
  return *(m_strawSurfacesCache[straw]->transform());
}

const Amg::Vector3D&
TRT_BaseElement::normal(const Identifier&) const
{
  // Not sure if the normal of the straw is ever used.
  // nor is there a well defined normal.
  // This wont be corrected for alignments.
  // Just return the element normal
  return normal();
}

const Amg::Vector3D&
TRT_BaseElement::center(const Identifier& id) const
{
  int straw = m_idHelper->straw(id);
  if (!m_strawSurfacesCache[straw]) {
    createSurfaceCache(id);
  }
  // forward the transform of the cache
  return *(m_strawSurfacesCache[straw]->center());
}

const Trk::StraightLineSurface&
TRT_BaseElement::strawSurface(int straw) const
{
  if (!m_strawSurfaces[straw]) {
    // get the straw identifier to the given straw number and element identifier
    Identifier id = m_idHelper->straw_id(identify(), straw);
    createSurfaceCache(id);
  }
  return *(m_strawSurfaces[straw].get());
}

const Amg::Transform3D&
TRT_BaseElement::strawTransform(int straw) const
{
  if (!m_strawSurfacesCache[straw]) {
    Identifier id = m_idHelper->straw_id(identify(), straw);
    createSurfaceCache(id);
  }
  // forward the transform of the cache
  return *(m_strawSurfacesCache[straw]->transform());
}

const Amg::Vector3D&
TRT_BaseElement::strawCenter(int straw) const
{
  if (!m_strawSurfacesCache[straw]) {
    Identifier id = m_idHelper->straw_id(identify(), straw);
    createSurfaceCache(id);
  }
  // forward the transform of the cache
  return *(m_strawSurfacesCache[straw]->center());
}

Amg::Vector3D
TRT_BaseElement::strawAxis(int straw) const
{
  return (strawTransform(straw).linear() * Amg::Vector3D::UnitZ() *
          strawDirection());
}

/// ----- can be removed after full Amg migration -----------------------
/// (start)
// [A] in CLHEP
const HepGeom::Transform3D
TRT_BaseElement::transformCLHEP() const
{
  return Amg::EigenTransformToCLHEP(*(m_surfaceCache->transform()));
}

const HepGeom::Point3D<double>
TRT_BaseElement::centerCLHEP() const
{

  const Amg::Vector3D& cCenter = center();
  return HepGeom::Point3D<double>(cCenter.x(), cCenter.y(), cCenter.z());
}

const HepGeom::Vector3D<double>
TRT_BaseElement::normalCLHEP() const
{
  const Amg::Vector3D& cNormal = normal();
  return HepGeom::Vector3D<double>(cNormal.x(), cNormal.y(), cNormal.z());
}

// [B] in CLHEP
const HepGeom::Transform3D
TRT_BaseElement::transformCLHEP(const Identifier& id) const
{
  return Amg::EigenTransformToCLHEP(transform(id));
}

const HepGeom::Point3D<double>
TRT_BaseElement::centerCLHEP(const Identifier& id) const
{
  const Amg::Vector3D cCenter = center(id);
  return HepGeom::Point3D<double>(cCenter.x(), cCenter.y(), cCenter.z());
}

const HepGeom::Vector3D<double>
TRT_BaseElement::normalCLHEP(const Identifier&) const
{
  // Not sure if the normal of the straw is ever used.
  // nor is there a well defined normal.
  // This wont be corrected for alignments.
  // Just return the element normal
  return normalCLHEP();
}
//(end)
// ----- can be removed after full Amg migration -----------------------

void
TRT_BaseElement::createSurfaceCache(Identifier id) const
{
  int straw = m_idHelper->straw(id);
  // get the StrawTransform from GeoModel
  HepGeom::Transform3D cStrawTransform = calculateStrawTransform(straw);

  // convert neccessary parts to Amg
  if (!m_strawSurfacesCache[straw]) {
    // create the surface cache & fill it
    m_strawSurfacesCache[straw].set(createSurfaceCacheHelper(straw));
  }
  // creaete the surface only if needed (the links are still intact)
  if (!m_strawSurfaces[straw]) {
    m_strawSurfaces[straw].set(
      std::make_unique<Trk::StraightLineSurface>(*this, id));
  }
}

std::unique_ptr<SurfaceCache>
TRT_BaseElement::createSurfaceCacheHelper(int straw) const
{
  // get the StrawTransform from GeoModel
  HepGeom::Transform3D cStrawTransform = calculateStrawTransform(straw);
  auto sTransform = std::make_unique<Amg::Transform3D>(
    Amg::CLHEPTransformToEigen(cStrawTransform));
  auto sCenter = std::make_unique<Amg::Vector3D>(sTransform->translation());
  // create the surface cache & fill it
  return std::make_unique<SurfaceCache>(
    sTransform.release(), sCenter.release(), nullptr, nullptr);
}

void
TRT_BaseElement::invalidate()
{
  // Invalidate the caches
  // Call and barrel or endcap specific invalidation
  invalidateOther();
  // Its enough to delete and zero the caches.
  deleteCache();
}

void
TRT_BaseElement::deleteCache()
{
  // for all straws
  for (size_t i = 0; i < m_strawSurfacesCache.size(); i++) {
    m_strawSurfacesCache[i].store(nullptr);
  }
}

void
TRT_BaseElement::updateAllCaches()
{
  // delete the caches first
  deleteCache();
  // Strawlayer caches
  if (not m_surfaceCache){
    createSurfaceCache();
  }
  // Loop over all straws and request items that get cached.
  for (unsigned int iStraw = 0; iStraw < nStraws(); iStraw++) {
    Identifier strawId = m_idHelper->straw_id(identify(), iStraw);
    createSurfaceCache(strawId);
  }
}

const TRT_Conditions*
TRT_BaseElement::conditions() const
{
  return m_conditions;
}

}
