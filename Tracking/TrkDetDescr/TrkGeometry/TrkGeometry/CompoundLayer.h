/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// CompoundLayer.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKGEOMETRY_COMPOUNDLAYER_H
#define TRKGEOMETRY_COMPOUNDLAYER_H

class MsgStream;

#include "GeoPrimitives/GeoPrimitives.h"
// Trk
#include "TrkDetDescrUtils/BinnedArray.h"
#include "TrkDetDescrUtils/LayerIndex.h"
#include "TrkEventPrimitives/PropDirection.h"
#include "TrkGeometry/Layer.h"
#include "TrkGeometry/LayerMaterialProperties.h"
#include "TrkGeometry/OverlapDescriptor.h"
#include "TrkParameters/TrackParameters.h"

// STL
#include <vector>

namespace Trk {

/**
   @class CompoundLayer

   @brief A class to represent complex geometries requiring a
   description using multiple surfaces.

   Base class for complex layers, where a few different Surfaces
   need to be stitched together to create the geometry of the
   Layer. The CompoundLayer has a single surface representation
   which is a single simple Surface that gives a rough description
   of the whole geometry (eg. for a cylider with conical end-pieces,
   the barrel cylinder extended to the edges of the cones are
   used).

   The CompoundLayer class provides routines for the extrapolator to
   use to test the various Surfaces in order of most to least likely
   given a particular position and direction.

   Classes which derived from this class should also derive from
   Layer to be used by the extrapolator properly, the CompoundLayer
   class just ensures that the extra functionality required is
   present.

   The best practice is to derive from an already specified
   layer type, for instance ConeCylinderConeLayer derives from
   CylinderLayer, so in places like LayerBuilders, the compound
   layer can be used without needing to redevelop the interface.

   @author Ian.Watson@cern.ch
*/

class CompoundLayer {
 public:
  /** Default construactor */
  CompoundLayer() {}
  /** Destructor, deletes the consituent surfaces */
  virtual ~CompoundLayer();

  virtual const Layer& layerRepresentation() const = 0;

  std::vector<const Surface*> constituentSurfaces() const;

  virtual std::vector<const Surface*> closestSurfaces(
      const Amg::Vector3D& gp, const Amg::Vector3D& dir,
      bool alsoBackward = false) const;

  /// Searches through the surfaces with Layer specific tolerance to
  /// see if the gp is on one of the subsurfaces
  bool isOnCompoundLayer(const Amg::Vector3D& gp, double thickness) const;

 protected:
  /** Resize the layer to the tracking volume - not (yet) supported for Compound
   * Layer */
  void resize(const VolumeBounds&, double) const {};

  std::vector<const Surface*> m_surfaces;  //!< The constituent surfaces
};

inline std::vector<const Surface*> CompoundLayer::constituentSurfaces() const {
  return m_surfaces;
}
inline std::vector<const Surface*> CompoundLayer::closestSurfaces(
    const Amg::Vector3D&, const Amg::Vector3D&, bool) const {
  return m_surfaces;
}

}  // namespace Trk

#endif  // TRKGEOMETRY_COMPOUNDLAYER_H
