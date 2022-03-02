/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ConeLayer.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKGEOMETRY_CONELAYER_H
#define TRKGEOMETRY_CONELAYER_H

class MsgStream;

// Amg
#include "GeoPrimitives/GeoPrimitives.h"
// Trk
#include "TrkDetDescrUtils/BinnedArray.h"
#include "TrkEventPrimitives/PropDirection.h"
#include "TrkGeometry/Layer.h"
#include "TrkSurfaces/ConeSurface.h"
// STL sorting
#include <algorithm>

namespace Trk {

class ConeBounds;
class LayerMaterialProperties;
class OverlapDescriptor;

/**
   @class ConeLayer

   Class to describe a cylindrical detector layer for tracking, it inhertis from
   both, Layer base class and ConeSurface class

   @author Ian.Watson@cern.ch, Andreas.Salzburger@cern.ch
*/

class ConeLayer final
  : public ConeSurface
  , public Layer
{
public:
  /**Default Constructor*/
  ConeLayer() {}

  /**Constructor with ConeBounds components and  MaterialProperties */
  ConeLayer(const Amg::Transform3D& transform,
            ConeBounds* cbounds,
            const LayerMaterialProperties& laymatprop,
            double thickness = 0.,
            OverlapDescriptor* od = nullptr,
            int laytyp = int(Trk::active));

  /**Constructor with ConeSurface and  MaterialProperties */
  ConeLayer(ConeSurface* cyl,
            const LayerMaterialProperties& laymatprop,
            double thickness = 0.,
            OverlapDescriptor* od = nullptr,
            int laytyp = int(Trk::active));

  /**Constructor with ConeSurface components and pointer to SurfaceArray
   * (passing ownership) */
  ConeLayer(const Amg::Transform3D& transform,
            ConeBounds* cbounds,
            SurfaceArray* surfaceArray,
            double thickness = 0.,
            OverlapDescriptor* od = nullptr,
            int laytyp = int(Trk::active));

  /**Constructor with ConeSurface components,
     MaterialProperties and pointer SurfaceArray (passing ownership) */
  ConeLayer(const Amg::Transform3D& transform,
            ConeBounds* cbounds,
            SurfaceArray* surfaceArray,
            const LayerMaterialProperties& laymatprop,
            double thickness = 0.,
            OverlapDescriptor* od = nullptr,
            int laytyp = int(Trk::active));

  /**Copy constructor of ConeLayer*/
  ConeLayer(const ConeLayer& cla);

  /**Copy constructor with shift*/
  ConeLayer(const ConeLayer& cla, const Amg::Transform3D& tr);

  /**Assignment operator for ConeLayers */
  ConeLayer& operator=(const ConeLayer&);

  /**Destructor*/
  virtual ~ConeLayer() override {}

  /** Transforms the layer into a Surface representation for extrapolation */
  virtual const ConeSurface& surfaceRepresentation() const override final;

  /** getting the MaterialProperties back - for pre-update*/
  virtual double preUpdateMaterialFactor(
    const Trk::TrackParameters& par,
    Trk::PropDirection dir) const override final;

  /** getting the MaterialProperties back - for post-update*/
  virtual double postUpdateMaterialFactor(
    const Trk::TrackParameters& par,
    Trk::PropDirection dir) const override final;

  /** move the Layer */
  virtual void moveLayer(Amg::Transform3D& shift) override final;

private:
  /** Resize the layer to the tracking volume - not (yet) supported for
   * ConeLayer */
  void resize(const VolumeBounds&, double) const {}

  /** Resize the layer to the tracking volume - not supported since this an
   * entry layer method*/
  virtual void resizeAndRepositionLayer(const VolumeBounds&,
                                        const Amg::Vector3D&,
                                        double) override final
  {}
};

} // namespace Trk

#endif // TRKGEOMETY_CONELAYER_H
