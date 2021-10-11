/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ConeCylinderConeLayer.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKGEOMETRY_CONECYLINDERCONELAYER_H
#define TRKGEOMETRY_CONECYLINDERCONELAYER_H

class MsgStream;

#include "TrkDetDescrUtils/BinnedArray.h"
#include "TrkEventPrimitives/PropDirection.h"
#include "TrkGeometry/CompoundLayer.h"
#include "TrkGeometry/CylinderLayer.h"
#include "TrkGeometry/Layer.h"
#include "TrkSurfaces/ConeSurface.h"
#include "TrkSurfaces/CylinderSurface.h"
// STL sorting
#include <algorithm>

namespace Trk {

class ConeCylinderConeLayer final: public CompoundLayer, public CylinderLayer {
 public:
  /** Default constructor */
   ConeCylinderConeLayer() = default;

   /** Build the layer out of the given parameters */
   ConeCylinderConeLayer(const Amg::Transform3D& transform,
                         double radius,
                         double halfLength,
                         double zStitch,
                         double alpha,
                         SurfaceArray* surfaceArray,
                         const LayerMaterialProperties& laymatprop,
                         double thickness = 0.,
                         OverlapDescriptor* od = nullptr,
                         int laytyp = int(Trk::active));

   virtual const ConeCylinderConeLayer& layerRepresentation()
     const override final
   {
     return (*this);
  }

  virtual const CylinderSurface& surfaceRepresentation() const override final {
    return (*this);
  }

  virtual bool isOnLayer(const Amg::Vector3D& gp,
                         const BoundaryCheck&) const override final {
    return isOnCompoundLayer(gp, 0.5 * thickness());
  }

  double radius(const Amg::Vector3D& gp) const {
    Amg::Vector3D gploc;
    if (m_transform)
      gploc = (*m_transform).inverse() * gp;
    else
      gploc = gp;

    if (fabs(gploc.z()) > m_halfLength) return 0.;
    if (fabs(gploc.z()) < m_zStitch) return m_radius;
    /// its on one of the cones
    return m_radius - (fabs(gploc.z()) - m_zStitch) * tan(m_alpha);
  }
  double pathCorr(double pathTheta, const Amg::Vector3D& gp) const {
    Amg::Vector3D gploc;
    if (m_transform) {
      gploc = (*m_transform).inverse() * gp;
    } else {
      gploc = gp;
    }

    if (std::abs(gploc.z()) > m_halfLength) {
      return 0.;
    }
    if (std::abs(gploc.z()) < m_zStitch) {
      return std::abs(sin(pathTheta));
    }
    /// its on one of the cones
    return std::abs(sin(pathTheta + m_alpha));
  }

 private:
   std::unique_ptr<Amg::Transform3D> m_transform = nullptr;
   double m_radius = 0;
   double m_halfLength = 0;
   double m_zStitch = 0;
   double m_alpha = 0;
};
}  // namespace Trk

#endif  // TRKGEOMETY_CONECYLINDERCONELAYER_H
