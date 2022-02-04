/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IApproachDescriptor.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKGEOMETRY_IAPPROACHDESCRIPTOR_H
#define TRKGEOMETRY_IAPPROACHDESCRIPTOR_H

// Trk
#include <memory>

#include "GeoPrimitives/GeoPrimitives.h"
#include "TrkDetDescrUtils/BinnedArray.h"
#include "TrkSurfaces/Surface.h"
namespace Trk {

/**
  @class ApproachSurfaces
  just implement the delete on the objects
*/

class ApproachSurfaces : public std::vector<Surface*> {
 public:
  // Default constructor
  ApproachSurfaces() : std::vector<Surface*>() {}

  // Desctructur with cleanup
  ~ApproachSurfaces() {
    for (auto& sf : (*this)) {
      delete sf;
    }
  }
};

/**
@class IApproachDescriptor

CVirtual class to decide and return which approaching surface to be taken.
@author Andreas.Salzburger@cern.ch, made virtual by Remi.Lafaye@cern.ch
*/

class IApproachDescriptor {
 public:
  // Default constructor
  IApproachDescriptor(std::unique_ptr<ApproachSurfaces> aSurfaces,
                      bool rebuild = true)
      : m_approachSurfaces(std::move(aSurfaces)),
        m_approachSurfaceArraySurface(nullptr),
        m_approachSurfaceArray(nullptr),
        m_rebuild(rebuild) {}

  // Default constructor
  IApproachDescriptor(
      std::unique_ptr<BinnedArray<const ApproachSurfaces>> aSurfaceArray,
      std::unique_ptr<Surface> aSurfaceArraySurface = nullptr)
      : m_approachSurfaces(nullptr),
        m_approachSurfaceArraySurface(std::move(aSurfaceArraySurface)),
        m_approachSurfaceArray(std::move(aSurfaceArray)),
        m_rebuild(false) {}

  virtual ~IApproachDescriptor() = default;

  // register Layer
  void registerLayer(const Layer& lay);

  // Telling you if it needs to be rebuilt
  bool rebuild() const;

  /** get the compatible surfaces
      - return : a boolean indicating if an actual intersection had been tried
      - fill vector of intersections
      - primary bin surface : sf
      - position & direction : pos, dir
  */
  virtual const ApproachSurfaces* approachSurfaces(
      const Amg::Vector3D& pos, const Amg::Vector3D& dir) const = 0;

 private:
  // register the Layer
  void registerLayerToSurfaces(const Layer& lay,
                               const ApproachSurfaces& aSurfaces);

  IApproachDescriptor(const IApproachDescriptor&) = delete;
  IApproachDescriptor& operator=(const IApproachDescriptor&) = delete;

 protected:
  std::unique_ptr<ApproachSurfaces> m_approachSurfaces;
  std::unique_ptr<Surface> m_approachSurfaceArraySurface;
  std::unique_ptr<BinnedArray<const ApproachSurfaces>> m_approachSurfaceArray;
  bool m_rebuild;
};

inline bool IApproachDescriptor::rebuild() const { return m_rebuild; }

inline void IApproachDescriptor::registerLayer(const Layer& lay) {
  if (m_approachSurfaces) registerLayerToSurfaces(lay, *m_approachSurfaces);
  if (m_approachSurfaceArraySurface) {
    m_approachSurfaceArraySurface->associateLayer(lay);
    m_approachSurfaceArraySurface->setOwner(Trk::TGOwn);
  }
  if (m_approachSurfaceArray) {
   BinnedArraySpan<ApproachSurfaces const * const> aSurfaceObjects =
        m_approachSurfaceArray->arrayObjects();
    for (auto& aSurfaces : aSurfaceObjects) {
      registerLayerToSurfaces(lay, *aSurfaces);
    }
  }
}

inline void IApproachDescriptor::registerLayerToSurfaces(
    const Layer& lay, const ApproachSurfaces& aSurfaces) {
  for (Trk::Surface* aSurface : aSurfaces) {
    aSurface->associateLayer(lay);
    aSurface->setOwner(Trk::TGOwn);
  }
}
}  // namespace Trk

#endif  // TRKGEOMETRY_IAPPROACHDESCRIPTOR_H

