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

/** @class SurfaceCacheBase/SurfaceCache

   These are for internal use in TRT_RedoutGeometry.
   They hold all the cached values related to the element surface.

   We want to  alllow the
   Trk::Surface::transform() { m_associatedDetectorElement->transform() }
   method to work.


   @author: Grant Gorfine, Andreas Salzburger
   @author: Chirstos Anastopoulos AthenaMT modifications
*/
class SurfaceCacheBase
{

public:
  SurfaceCacheBase(const Amg::Transform3D& transform,
                   const Amg::Vector3D& center);

  ///
  SurfaceCacheBase() = default;
  ///
  ~SurfaceCacheBase() = default;
  /// move c'tor
  SurfaceCacheBase(SurfaceCacheBase&&) = default;
  /// move assignment
  SurfaceCacheBase& operator=(SurfaceCacheBase&&) = default;
  /// delete copy c'tor
  SurfaceCacheBase(const SurfaceCacheBase&) = delete;
  /// delete assignment
  SurfaceCacheBase& operator=(const SurfaceCacheBase&) = delete;

  const Amg::Transform3D& transform() const { return (m_transform); }

  const Amg::Vector3D& center() const { return (m_center); }

private:
  Amg::Transform3D m_transform{};
  Amg::Vector3D m_center{};
};

inline SurfaceCacheBase::SurfaceCacheBase(const Amg::Transform3D& transform,
                                          const Amg::Vector3D& center)
  : m_transform(transform)
  , m_center(center)
{}

class SurfaceCache : public SurfaceCacheBase
{

public:
  SurfaceCache(const Amg::Transform3D& transform,
               const Amg::Vector3D& center,
               const Amg::Vector3D& normal,
               std::unique_ptr<Trk::SurfaceBounds> bounds);
  ///
  SurfaceCache() = default;
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

  const Amg::Vector3D& normal() const { return (m_normal); }

  const Trk::SurfaceBounds* bounds() const { return (m_bounds.get()); }

private:
  Amg::Vector3D m_normal;
  std::unique_ptr<Trk::SurfaceBounds> m_bounds = nullptr;
};

inline SurfaceCache::SurfaceCache(const Amg::Transform3D& transform,
                                  const Amg::Vector3D& center,
                                  const Amg::Vector3D& normal,
                                  std::unique_ptr<Trk::SurfaceBounds> bounds)
  : SurfaceCacheBase(transform, center)
  , m_normal(normal)
  , m_bounds(std::move(bounds))
{}

} // end namespace

#endif // InDetReadoutGeometry_SurfaceCache_h
