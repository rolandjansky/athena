/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// Layer.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKGEOMETRY_LAYER_H
#define TRKGEOMETRY_LAYER_H

class MsgStream;

// Amg
#include "GeoPrimitives/GeoPrimitives.h"
// Trk
#include "CxxUtils/checker_macros.h"
#include "TrkDetDescrUtils/BinnedArray.h"
#include "TrkDetDescrUtils/Intersection.h"
#include "TrkDetDescrUtils/LayerIndex.h"
#include "TrkDetDescrUtils/SharedObject.h"
#include "TrkGeometry/LayerMaterialProperties.h"
#include "TrkGeometry/OverlapDescriptor.h"
#include "TrkNeutralParameters/NeutralParameters.h"
#include "TrkParameters/TrackParameters.h"

namespace Trk {

class Surface;
class MaterialProperties;
class BinUtility;
class Volume;
class VolumeBounds;
class TrackingVolume;
class DetachedTrackingVolume;
class ICompatibilityEstimator;

typedef ObjectIntersection<Surface> SurfaceIntersection;
typedef BinnedArray<Surface> SurfaceArray;

/**
   @enum LayerType

   For readability
*/
enum LayerType { passive = 0, active = 1 };

/**
   @class Layer

   Base Class for a Detector Layer in the Tracking realm.
   An actual implemented Detector Layer inheriting from this base
   class has to inherit from a specific type of Surface as well.
   In addition, a Layer can carry:

   - a SurfaceArray of Surfaces holding the actual detector elements or
   subSurfaces.
   - LayerMaterialProperties for Surface-based materialUpdates
   - an OverlapDescriptor (mainly used for blind extrapolation)
   - a pointer to the TrackingVolume (can only be set by such)
   - an active/passive code :
   0      - activ
   1      - passive
   [....] - other

   @author Andreas.Salzburger@cern.ch

*/

class Layer {
 public:
  /**Default Constructor*/
  Layer();

  /**Constructor with MaterialProperties */
  Layer(const LayerMaterialProperties& laymatprop, double thickness = 0.,
        OverlapDescriptor* od = nullptr, int ltype = int(passive));

  /**Constructor with pointer to SurfaceArray (passing ownership) */
  Layer(SurfaceArray* surfaceArray, double thickness = 0.,
        OverlapDescriptor* od = nullptr, int ltype = int(passive));

  /**Constructor with MaterialProperties and pointer SurfaceArray (passing
   * ownership) */
  Layer(SurfaceArray* surfaceArray, const LayerMaterialProperties& laymatprop,
        double thickness = 0., OverlapDescriptor* od = nullptr,
        int ltype = int(passive));

  /**Copy Constructor for Layers */
  Layer(const Layer& lay);

  /**Destructor*/
  virtual ~Layer();

  /** Assignment operator */
  Layer& operator=(const Layer& lay);

  /** Return the entire SurfaceArray, returns 0 if no SurfaceArray*/
  const SurfaceArray* surfaceArray() const;

  /** If no subSurface array is defined or no subSurface can be found
      to the given Amg::Vector3D, it would return 0
  */
  const Surface* subSurface(const Amg::Vector3D& gp) const;

  /** If no subSurface array is defined or no subSurface can be found
      to the given Amg::Vector2D, it would return 0
  */
  const Surface* subSurface(const Amg::Vector2D& lp) const;

  /** Return a reference sub surface of the layer, usually the first one
      in the array
      - if an index is given (and the index is in range), this surface is
     returned
  */
  const Surface* subSurfaceReference(unsigned int idx = 0) const;

  /** Transforms the layer into a Surface representation for extrapolation */
  virtual const Surface& surfaceRepresentation() const = 0;

  /** Return the Thickness of the Layer */
  double thickness() const;

  /** templated on Layer method */
  template <class T>
  bool onLayer(const T& parameters,
               const BoundaryCheck& bcheck = BoundaryCheck(true)) const;

  /** isOnLayer() method, using isOnSurface() with Layer specific tolerance */
  virtual bool isOnLayer(
      const Amg::Vector3D& gp,
      const BoundaryCheck& bcheck = BoundaryCheck(true)) const;

  /** getting the LayerMaterialProperties including full/pre/post update */
  const LayerMaterialProperties* layerMaterialProperties() const;

  /** getting the MaterialProperties back - for full update*/
  const MaterialProperties* fullUpdateMaterialProperties(
      const TrackParameters& par) const;

  /** getting the MaterialProperties back - for pre-update*/
  virtual double preUpdateMaterialFactor(const TrackParameters&,
                                         PropDirection) const {
    return 1.;
  }

  /** getting the MaterialProperties back - for pre-update*/
  virtual double postUpdateMaterialFactor(const TrackParameters&,
                                          PropDirection) const {
    return 0.;
  }

  /** gettint hte overlap descriptor */
  const OverlapDescriptor* overlapDescriptor() const;
  /** getting what's stored to be the previous Layer, boolean to skip navigation
   * layers */
  const Layer* previousLayer(bool skipNavLayer = false) const;
  /** set the previous Layer*/
  void setPreviousLayer(const Layer*);

  /** getting the next/previous Layer if registered - unit for direction vector
   * required */
  const Layer* nextLayer(const Amg::Vector3D& gp,
                         const Amg::Vector3D& udir) const;
  /** getting what's stored to be the next Layer, boolean to skip navigation
   * layers*/
  const Layer* nextLayer(bool skipNavLayer = false) const;
  /** set the next Layer*/
  void setNextLayer(const Layer*);

  /** access the BinUtility*/
  const BinUtility* binUtility() const;
  /** set the BinUtility*/
  void setBinUtility(const BinUtility*);

  /** Surface seen on approach - if not defined differently, it is the
   * surfaceRepresentation() */
  virtual const Surface& surfaceOnApproach(
      const Amg::Vector3D& pos, const Amg::Vector3D& dir, PropDirection pdir,
      const BoundaryCheck& bcheck, bool resolveSubSurfaces = 0,
      const ICompatibilityEstimator* ice = nullptr) const;

  /** get compatible surfaces starting from charged parameters */
  size_t compatibleSurfaces(std::vector<SurfaceIntersection>& cSurfaces,
                            const TrackParameters& pars, PropDirection pdir,
                            const BoundaryCheck& bcheck,
                            bool materialSurfacesOnly = true,
                            const Surface* startSurface = nullptr,
                            const Surface* endSurface = nullptr,
                            const ICompatibilityEstimator* ice = nullptr) const;

  /** get compatible surfaces starting from neutral parameters */
  size_t compatibleSurfaces(std::vector<SurfaceIntersection>& cSurfaces,
                            const NeutralParameters& pars, PropDirection pdir,
                            const BoundaryCheck& bcheck,
                            bool materialSurfacesOnly = true,
                            const Surface* startSurface = nullptr,
                            const Surface* endSurface = nullptr,
                            const ICompatibilityEstimator* ice = nullptr) const;

  /** Has sub-structure method:
      - sub-structure depending on :
        (a) only when required to resolve sub surfaces for sensitive hits
        (b) also material is ordered with sub structure */
  virtual bool hasSubStructure(bool resolveSensitive = false) const;

  /** get the confining TrackingVolume */
  const TrackingVolume* enclosingTrackingVolume() const;

  /** get the confining DetachedTrackingVolume */
  const DetachedTrackingVolume* enclosingDetachedTrackingVolume() const;

  /** get the layerIndex */
  const LayerIndex& layerIndex() const;

  /** get the Layer coding */
  int layerType() const;

  /** set the Layer coding */
  void setLayerType(int identifier);

  /** boolean method to check if the layer needs a LayerMaterialProperties */
  bool needsMaterialProperties() const;

  /** assignMaterialPropeties */
  void assignMaterialProperties(const LayerMaterialProperties&,
                                double scale = 1.0);

  void assignMaterialProperties ATLAS_NOT_THREAD_SAFE(
      const LayerMaterialProperties&, double scale = 1.0) const;

  /** move the Layer */
  virtual void moveLayer(Amg::Transform3D&){};

  /** move the Layer */
  virtual void moveLayer ATLAS_NOT_THREAD_SAFE(Amg::Transform3D&) const {};

  /**register Volume associated to the layer */
  void registerRepresentingVolume(const Volume* theVol);

  /**register Volume associated to the layer */
  void registerRepresentingVolume
  ATLAS_NOT_THREAD_SAFE(const Volume* theVol) const;

  /** get the Volume associated to the layer */
  const Volume* representingVolume() const;

  /** set the reference measure */
  void setRef(double);

  /** set the reference measure */
  void setRef ATLAS_NOT_THREAD_SAFE(double) const;

  /** get the reference measure */
  double getRef() const;

  void encloseTrackingVolume(const TrackingVolume& tvol);
  //!< private method to set the enclosed detached TV
  void encloseDetachedTrackingVolume(const DetachedTrackingVolume& tvol);
  void encloseDetachedTrackingVolume
  ATLAS_NOT_THREAD_SAFE(const DetachedTrackingVolume& tvol) const;

  /** get compatible surfaces starting from charged parameters */
  template <class T>
  size_t getCompatibleSurfaces(
      std::vector<SurfaceIntersection>& cSurfaces, const T& pars,
      PropDirection pdir, const BoundaryCheck& bcheck,
      bool materialSurfacesOnly = true, const Surface* startSurface = nullptr,
      const Surface* endSurface = nullptr,
      const ICompatibilityEstimator* ice = nullptr) const;

  //!< propagate TrackingGeometry owner downstream
  void compactify ATLAS_NOT_THREAD_SAFE(size_t& cSurfaces,
                                        size_t& tSurfaces) const;

  //!< register layer index for material map registration
  void registerLayerIndex(const LayerIndex& lIdx);
  void registerLayerIndex ATLAS_NOT_THREAD_SAFE(const LayerIndex& lIdx) const;

  /** private method to set enclosing TrackingVolume, called by friend class
     only optionally, the layer can be resized to the dimensions of the
     TrackingVolume
      - Bounds of the Surface are resized
      - MaterialProperties dimensions are resized
      - SubSurface array boundaries are NOT resized
  */

  /** resize layer to the TrackingVolume dimensions - to be overloaded by the
   * extended classes*/
  virtual void resizeLayer(const VolumeBounds&, double) {}

  /** resize layer to the TrackingVolume dimensions const not thread safe */
  virtual void resizeLayer ATLAS_NOT_THREAD_SAFE(const VolumeBounds&,
                                                 double) const {}

  /** resize and reposition layer : dedicated for entry layers */
  virtual void resizeAndRepositionLayer(const VolumeBounds& vBounds,
                                        const Amg::Vector3D& vCenter,
                                        double envelope = 1.) = 0;

  /** resize and reposition layer : dedicated for entry layers */
  virtual void resizeAndRepositionLayer ATLAS_NOT_THREAD_SAFE(
      const VolumeBounds& vBounds, const Amg::Vector3D& vCenter,
      double envelope = 1.) const = 0;

 protected:
  SurfaceArray* m_surfaceArray;  //!< SurfaceArray on this layer Surface
  SharedObject<LayerMaterialProperties>
      m_layerMaterialProperties;  //!< MaterialPoperties of this layer Surface

  double m_layerThickness;  //!< thickness of the Layer
  OverlapDescriptor*
      m_overlapDescriptor;  //!< descriptor for overlap/next surface

  // These are stored by pointers and never deleted as they belong to the Volume
  const Layer*
      m_previousLayer;       //!< the previous Layer according to BinGenUtils
  const Layer* m_nextLayer;  //!< next Layer according to BinGenUtils
  const BinUtility* m_binUtility;  //!< BinUtility for next/previous decission

  const TrackingVolume*
      m_enclosingTrackingVolume;  //!< Enclosing TrackingVolume
  const DetachedTrackingVolume*
      m_enclosingDetachedTrackingVolume;  //!< Enclosing DetachedTrackingVolume
  LayerIndex m_index;                     //!< LayerIndex
  int m_layerType;                        //!< active passive layer
  const Volume* m_representingVolume;     //!< Representing Volume
  double m_ref;  //!< reference measure for local coordinate convertors
};

}  // namespace Trk
#include "Layer.icc"
#endif  // TRKGEOMETRY_LAYER_H

