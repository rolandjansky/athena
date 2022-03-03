/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SubtractedPlaneLayer.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKGEOMETRY_SUBTRACTEDPLANELAYER_H
#define TRKGEOMETRY_SUBTRACTEDPLANELAYER_H

class MsgStream;

#include "TrkEventPrimitives/PropDirection.h"
#include "TrkGeometry/PlaneLayer.h"
#include "TrkGeometrySurfaces/SubtractedPlaneSurface.h"
#include "TrkSurfaces/PlaneSurface.h"

namespace Trk {

class LayerMaterialProperties;

/**
 @class SubtractedPlaneLayer

 Class to describe a planar detector layer for tracking, with subtraction

 @author Sarka.Todorova@cern.ch
 */

class SubtractedPlaneLayer final
  : public SubtractedPlaneSurface
  , public Layer
{
public:
  /**Default Constructor*/
  SubtractedPlaneLayer() {}

  /**Constructor with SubtractedPlaneSurface
     and MaterialProperties  */
  SubtractedPlaneLayer(const SubtractedPlaneSurface* subtrPlaneSurf,
                       const LayerMaterialProperties& laymatprop,
                       double thickness = 0., OverlapDescriptor* od = nullptr,
                       int laytyp = int(Trk::active));

  /**Copy constructor of SubtractedPlaneLayer*/
  SubtractedPlaneLayer(const SubtractedPlaneLayer& pla);

  /**Copy constructor with shift*/
  SubtractedPlaneLayer(const SubtractedPlaneLayer& pla,
                       const Amg::Transform3D& tr);

  /**Assignment operator for PlaneLayers */
  SubtractedPlaneLayer& operator=(const SubtractedPlaneLayer&);

  /**Destructor*/
  ~SubtractedPlaneLayer() override {}

  /** Transforms the layer into a Surface representation for extrapolation */
  const SubtractedPlaneSurface& surfaceRepresentation() const override final;

  /** getting the MaterialProperties back - for pre-update*/
  double preUpdateMaterialFactor(const Trk::TrackParameters& par,
                                 Trk::PropDirection dir) const override final;

  /** getting the MaterialProperties back - for post-update*/
  double postUpdateMaterialFactor(const Trk::TrackParameters& par,
                                  Trk::PropDirection dir) const override final;
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

#endif  // TRKGEOMETY_SUBTRACTEDPLANELAYER_H

