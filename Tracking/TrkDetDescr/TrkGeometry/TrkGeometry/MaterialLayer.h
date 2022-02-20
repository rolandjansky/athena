/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MaterialLayer.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKGEOMETRY_NAVIGATIONLAYER_H
#define TRKGEOMETRY_NAVIGATIONLAYER_H

class MsgStream;

#include "GeoPrimitives/GeoPrimitives.h"
#include "TrkDetDescrUtils/SharedObject.h"
#include "TrkEventPrimitives/PropDirection.h"
#include "TrkGeometry/Layer.h"
#include "TrkParameters/TrackParameters.h"

namespace Trk {

class Surface;
class LayerMaterialProperties;

/**
 @class MaterialLayer

 A material layer is a simple helper class to attach material information to a
 boundary surface, it is not associated to the layer navigation, but only with
 the

 @author Andreas.Salzburger@cern.ch

 */

class MaterialLayer final : public Layer
{
public:
  /** Constructor with already existing boundary surface
    - the surface representation is the according boundary surface */
  MaterialLayer(const Surface& sf, const LayerMaterialProperties& mlprop);

  /** Constructor with new surface, as of a combined boundary surface */
  MaterialLayer(const SharedObject<const Surface>& sfso,
                const LayerMaterialProperties& mlprop);

  /**Copy Constructor - */
  MaterialLayer(const MaterialLayer& lay);

  /**Destructor*/
  virtual ~MaterialLayer() override;

  /** Assignment operator */
  MaterialLayer& operator=(const MaterialLayer& lay);

  /** Transforms the layer into a Surface representation for extrapolation */
  virtual const Surface& surfaceRepresentation() const override final;

  /** isOnLayer() method, using isOnSurface() with Layer specific
   * tolerance */
  virtual bool isOnLayer(
      const Amg::Vector3D& gp,
      const BoundaryCheck& bcheck = BoundaryCheck(true)) const override final;

 protected:
  /** Resize the layer to the tracking volume - not implemented */
  virtual void resizeLayer(const VolumeBounds&, double) override final {}

  /** Resize the layer to the tracking volume - not implemented */
  virtual void resizeAndRepositionLayer(const VolumeBounds&,
                                        const Amg::Vector3D&,
                                        double) override final {}

  SharedObject<const Surface>
      m_surfaceRepresentation;  //!< for the navigation Volume the surface is
                                //!< a private member */

 private:
  /**Default Constructor*/
  MaterialLayer() {}
};

inline const Surface& MaterialLayer::surfaceRepresentation() const {
  return (*(m_surfaceRepresentation.get()));
}



}  // namespace Trk

#endif  // TRKGEOMETRY_NAVIGATIONLAYER_H

