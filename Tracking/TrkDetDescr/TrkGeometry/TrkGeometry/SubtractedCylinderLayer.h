/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SubtractedCylinderLayer.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKGEOMETRY_SUBTRACTEDCYLINDERLAYER_H
#define TRKGEOMETRY_SUBTRACTEDCYLINDERLAYER_H

class MsgStream;

#include "TrkDetDescrUtils/BinnedArray.h"
#include "TrkEventPrimitives/PropDirection.h"
#include "TrkGeometry/Layer.h"
#include "TrkGeometrySurfaces/SubtractedCylinderSurface.h"

namespace Trk {

class LayerMaterialProperties;
class OverlapDescriptor;

/**
 @class SubtractedCylinderLayer

 Class to describe a cylindrical detector layer for tracking, with subtraction;
 it inhertis from both, Layer base class and SubtractedCylinderSurface class

 @author Sarka.Todorova@cern.ch
*/

class SubtractedCylinderLayer final
  : public SubtractedCylinderSurface
  , public Layer
{
public:
  /**Default Constructor*/
  SubtractedCylinderLayer() {}

  /**Constructor with SubtractedCylinderSurface components and
   * MaterialProperties */
  SubtractedCylinderLayer(const SubtractedCylinderSurface* subCyl,
                          const LayerMaterialProperties& laymatprop,
                          double thickness = 0.,
                          OverlapDescriptor* od = nullptr,
                          int laytyp = int(Trk::active));

  /**Copy constructor*/
  SubtractedCylinderLayer(const SubtractedCylinderLayer& cla);

  /**Copy constructor with shift*/
  SubtractedCylinderLayer(const SubtractedCylinderLayer& cla,
                          const Amg::Transform3D& tr);

  /**Assignment operator */
  SubtractedCylinderLayer& operator=(const SubtractedCylinderLayer&);

  /**Destructor*/
  virtual ~SubtractedCylinderLayer() override {}

  /** Transforms the layer into a Surface representation for extrapolation */
  virtual const SubtractedCylinderSurface& surfaceRepresentation() const override final;


  /** getting the MaterialProperties back - for pre-update*/
  virtual double preUpdateMaterialFactor(
      const Trk::TrackParameters& par,
      Trk::PropDirection dir) const override final;

  /** getting the MaterialProperties back - for post-update*/
  virtual double postUpdateMaterialFactor(
      const Trk::TrackParameters& par,
      Trk::PropDirection dir) const override final;

  /** use the base class insideBounds (Vector2d, BoundaryCheck) */
  using CylinderSurface::insideBounds;

  /** move the Layer */
  virtual void moveLayer(Amg::Transform3D& shift) override final;

 private:
  /** Resize the layer to the tracking volume - not implemented*/
  virtual void resizeLayer(const VolumeBounds&, double) override final {}

  /** Resize the layer to the tracking volume - not implemented */
  virtual void resizeAndRepositionLayer(const VolumeBounds&,
                                        const Amg::Vector3D&,
                                        double) override final {}
};

}  // namespace Trk

#endif  // TRKGEOMETRY_SUBTRACTEDCYLINDERLAYER_H

