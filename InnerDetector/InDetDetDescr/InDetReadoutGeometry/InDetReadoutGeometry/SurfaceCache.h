/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SurfaceCache.h
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef InDetReadoutGeometry_SurfaceCache_h
#define InDetReadoutGeometry_SurfaceCache_h

#include "GeoPrimitives/GeoPrimitives.h"
#include "TrkSurfaces/SurfaceBounds.h"

namespace InDetDD {

/** @class SurfaceCache

   This class is for internal use in InDetReadoutGeometry.
   It holds all the cached values related to the element surface.

   It's a simple class to still alllow the
   Trk::Surface::transform() { m_associatedDetectorElement->transform() }
   method to work, hence, it basically copies the Trk::Surface structure.

   The surface cache class alows also to store 0 pointers,
   indicating the the holder of the cache has a commonly shared object,
   e.g. the straw bounds of all straws in one detector element.

   @author: Grant Gorfine, Andreas Salzburger
   @author: Chirstos Anastopoulos AthenaMT modifications
*/

class SurfaceCache
{

public:
  SurfaceCache(const Amg::Transform3D& transform,
               const Amg::Vector3D& center,
               std::unique_ptr<Amg::Vector3D> normal,
               std::unique_ptr<Trk::SurfaceBounds> bounds);
  ///
  ~SurfaceCache() = default;
  /// move c'tor
  SurfaceCache(SurfaceCache&&) = default;
  /// move assignment
  SurfaceCache& operator=(SurfaceCache&&) = default;
  /// delete copy c'tor
  SurfaceCache(const SurfaceCache&) = delete;
  /// delete assignment
  SurfaceCache& operator=(const SurfaceCache&) = delete;

  const Amg::Transform3D& transform() const { return (m_transform); }

  const Amg::Vector3D& center() const { return (m_center); }

  const Amg::Vector3D* normal() const { return (m_normal.get()); }

  const Trk::SurfaceBounds* bounds() const { return (m_bounds.get()); }

private:
  Amg::Transform3D m_transform{};
  Amg::Vector3D m_center{};
  std::unique_ptr<Amg::Vector3D> m_normal = nullptr;
  std::unique_ptr<Trk::SurfaceBounds> m_bounds = nullptr;
};

inline SurfaceCache::SurfaceCache(const Amg::Transform3D& transform,
                                  const Amg::Vector3D& center,
                                  std::unique_ptr<Amg::Vector3D> normal,
                                  std::unique_ptr<Trk::SurfaceBounds> bounds)
  : m_transform(transform)
  , m_center(center)
  , m_normal(std::move(normal))
  , m_bounds(std::move(bounds))
{}

} // end namespace

#endif // InDetReadoutGeometry_SurfaceCache_h
