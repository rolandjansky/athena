/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// CylinderLayer.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKGEOMETRY_CYLINDERLAYER_H
#define TRKGEOMETRY_CYLINDERLAYER_H

class MsgStream;

// Amg
#include "GeoPrimitives/GeoPrimitives.h"
// Trk
#include "TrkDetDescrUtils/BinnedArray.h"
#include "TrkEventPrimitives/PropDirection.h"
#include "TrkGeometry/ApproachDescriptor.h"
#include "TrkGeometry/Layer.h"
#include "TrkSurfaces/CylinderSurface.h"
// STL sorting
#include <algorithm>

namespace Trk {

class CylinderBounds;
class LayerMaterialProperties;
class OverlapDescriptor;

/**
 @class CylinderLayer

 Class to describe a cylindrical detector layer for tracking, it inhertis from
 both, Layer base class and CylinderSurface class

 @author Andreas.Salzburger@cern.ch
*/

class CylinderLayer final
  : public CylinderSurface
  , public Layer
{
public:
  /**Default Constructor*/
  CylinderLayer() {}

  /**Constructor with CylinderSurface components and  MaterialProperties */
  CylinderLayer(const Amg::Transform3D& transform,
                CylinderBounds* cbounds,
                const LayerMaterialProperties& laymatprop,
                double thickness = 0.,
                OverlapDescriptor* od = nullptr,
                int laytyp = int(Trk::active));

  /**Constructor with CylinderSurface and  MaterialProperties */
  CylinderLayer(CylinderSurface* cyl,
                const LayerMaterialProperties& laymatprop,
                double thickness = 0.,
                OverlapDescriptor* od = nullptr,
                int laytyp = int(Trk::active));

  /**Constructor with CylinderSurface components and pointer to SurfaceArray
     (passing ownership)
      - has optionally an OverlapDescriptor and ApproachDescriptor for the
     surfaceArray
      */
  CylinderLayer(const Amg::Transform3D& transform,
                CylinderBounds* cbounds,
                SurfaceArray* surfaceArray,
                double thickness = 0.,
                OverlapDescriptor* od = nullptr,
                IApproachDescriptor* ad = nullptr,
                int laytyp = int(Trk::active));

  /**Constructor with CylinderSurface components,
     MaterialProperties and pointer SurfaceArray (passing ownership) */
  CylinderLayer(const Amg::Transform3D& transform,
                CylinderBounds* cbounds,
                SurfaceArray* surfaceArray,
                const LayerMaterialProperties& laymatprop,
                double thickness = 0.,
                OverlapDescriptor* od = nullptr,
                IApproachDescriptor* ad = nullptr,
                int laytyp = int(Trk::active));

  /**Concentric Layer: Constructor with CylinderSurface components and
   * MaterialProperties */
  CylinderLayer(CylinderBounds* cbounds,
                const LayerMaterialProperties& laymatprop,
                double thickness = 0., OverlapDescriptor* od = nullptr,
                int laytyp = int(Trk::active));

  /**Concentric Layer: Constructor with CylinderSurface components and pointer
   * to SurfaceArray (passing ownership) */
  CylinderLayer(CylinderBounds* cbounds, SurfaceArray* surfaceArray,
                double thickness = 0., OverlapDescriptor* od = nullptr,
                IApproachDescriptor* ad = nullptr,
                int laytyp = int(Trk::active));

  /**Concentric Layer: Constructor with CylinderSurface components,
     MaterialProperties and pointer SurfaceArray (passing ownership) */
  CylinderLayer(CylinderBounds* cbounds, SurfaceArray* surfaceArray,
                const LayerMaterialProperties& laymatprop,
                double thickness = 0., OverlapDescriptor* od = nullptr,
                IApproachDescriptor* ad = nullptr,
                int laytyp = int(Trk::active));

  /**Copy constructor of CylinderLayer*/
  CylinderLayer(const CylinderLayer& cla);

  /**Copy constructor with shift*/
  CylinderLayer(const CylinderLayer& cla, const Amg::Transform3D& tr);

  /**Assignment operator for CylinderLayers */
  CylinderLayer& operator=(const CylinderLayer&);

  /**Destructor*/
  virtual ~CylinderLayer() = default;

  /** Transforms the layer into a Surface representation for extrapolation */
  virtual const CylinderSurface& surfaceRepresentation() const override final;

  /** getting the MaterialProperties back - for pre-update*/
  virtual double preUpdateMaterialFactor(
      const Trk::TrackParameters& par,
      Trk::PropDirection dir) const override final;

  /** getting the MaterialProperties back - for post-update*/
  virtual double postUpdateMaterialFactor(
      const Trk::TrackParameters& par,
      Trk::PropDirection dir) const override final;

  /** Surface seen on approach - if not defined differently, it is the
   * surfaceRepresentation() */
  virtual const Surface& surfaceOnApproach(
      const Amg::Vector3D& pos, const Amg::Vector3D& dir, PropDirection pdir,
      const BoundaryCheck& bcheck, bool resolveSubSurfaces = 0,
      const ICompatibilityEstimator* ice = nullptr) const override final;

  /** move the Layer */
  virtual void moveLayer(Amg::Transform3D& shift) override final;

 private:
  /** Resize the layer to the tracking volume - only works for
   * CylinderVolumeBouns */
  virtual void resizeLayer(const VolumeBounds& vBounds,
                           double envelope) override final;


  /** Resize the layer to the tracking volume */
  virtual void resizeAndRepositionLayer(const VolumeBounds& vBounds,
                                        const Amg::Vector3D& cCenter,
                                        double envelope) override final;


  /** Surface seen on approach - if not defined differently, it is the
   * surfaceRepresentation() */
  const Surface& approachSurface(const Amg::Vector3D& pos,
                                 const Amg::Vector3D& dir,
                                 const BoundaryCheck& bcheck) const;

  /** build approach surfaces */
  void buildApproachDescriptor();

 protected:
  /** surfaces on approach to the layer */
  std::unique_ptr<IApproachDescriptor> m_approachDescriptor;
};

/** @class CylinderLayerSorterR
     Functor for CylinderLayer R-Sorting */

class CylinderLayerSorterR {
 public:
  /** Default Constructor */
  CylinderLayerSorterR() {}

  bool operator()(const CylinderLayer* one, const CylinderLayer* two) const {
    return (one->surfaceRepresentation().bounds().r() <
            two->surfaceRepresentation().bounds().r());
  }
};

}  // namespace Trk

#endif  // TRKGEOMETY_CYLINDERLAYER_H

