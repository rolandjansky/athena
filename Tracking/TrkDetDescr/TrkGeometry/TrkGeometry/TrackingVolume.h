/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TrackingVolume.h, (b) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKGEOMETRY_TRACKINGVOLUME_H
#define TRKGEOMETRY_TRACKINGVOLUME_H

// Amg
#include "GeoPrimitives/GeoPrimitives.h"
// Trk
#include "TrkDetDescrUtils/BinnedArray.h"
#include "TrkDetDescrUtils/GeometrySignature.h"
#include "TrkDetDescrUtils/ObjectAccessor.h"
#include "TrkDetDescrUtils/SharedObject.h"
#include "TrkEventPrimitives/PropDirection.h"
#include "TrkGeometry/DetachedTrackingVolume.h"
#include "TrkGeometry/GlueVolumesDescriptor.h"
#include "TrkGeometry/LayerAttemptsCalculator.h"
#include "TrkGeometry/Material.h"
#include "TrkSurfaces/BoundaryCheck.h"
#include "TrkSurfaces/Surface.h"
#include "TrkVolumes/AbstractVolume.h"
#include "TrkVolumes/BoundaryCylinderSurface.h"
#include "TrkVolumes/BoundaryDiscSurface.h"
#include "TrkVolumes/BoundarySurface.h"
#include "TrkVolumes/BoundarySurfaceFace.h"
#include "TrkVolumes/Volume.h"
// STL
#include <string>
// ATH_MSG macros
#include "AthenaBaseComps/AthMsgStreamMacros.h"
#include "AthenaKernel/MsgStreamMember.h"

#include "CxxUtils/CachedUniquePtr.h"
#include "CxxUtils/checker_macros.h"
#ifndef TRKGEOMETRY_MAXLAYERATTEMPTS
#define TRKGEOMETRY_MAXLAYERATTEMPTS 100
#endif

class MsgStream;

namespace Trk {

// classes
class Layer;
class PlaneLayer;
class TrackingVolume;
class DetachedTrackingVolume;
class VolumeBounds;

typedef BinnedArray<Layer> LayerArray;
typedef BinnedArray<const TrackingVolume> TrackingVolumeArray;

template<class T>
using LayerIntersection = FullIntersection<Layer, Surface, T>;
template<class T>
using BoundaryIntersection =
  FullIntersection<BoundarySurface<TrackingVolume>, Surface, T>;

/**
 @class TrackingVolume

 Full Volume description used in Tracking,
 it inherits from Volume to get the geometrical structure,
 such as from MaterialProperties and from MagneticFieldProperties

     A TrackingVolume can provide the (layer) material information / internal
 navigation with in 5 different ways:

         --- a) Static confinement of Layers
         --- b) detached sub volumes
         --- b) unordered (arbitrarily oriented) layers
         --- d) unordered sub volumes
         --- e) unordered layers AND unordered subvolumes

 Static configurations can have a BendingCorrector that allows for correction of
 the path due to the bending of the track

         --- dedicated entry/exit layers can be defined for holding
             two perpenticular layers that are accessed in an entry/exit action

 In addition it is capable of holding a subarray of Layers and TrackingVolumes,
 Layers and TrackingVolumes are then owned by the TrackingVolume holding them.

 @author Andreas.Salzburger@cern.ch
 @author Christos Anastopoulos (Athena MT modifications)
 */

class TrackingVolume
  : public Volume
  , public Material
{

  /** The TrackingVolumeManipulator has to be a friend */
  friend class TrackingVolumeManipulator;

  /** Give the TrackingGeometry friend rights */
  friend class TrackingGeometry;

  /** Give the DetachedTrackingVolume friend rights */
  friend class DetachedTrackingVolume;

public:
  /** Default Constructor */
  TrackingVolume();

  /** Constructor for a full equipped Tracking Volume
  - explicitely  ======> 1 a) static confinement */
  TrackingVolume(Amg::Transform3D* htrans,
                 VolumeBounds* volbounds,
                 LayerArray* subLayers = nullptr,
                 TrackingVolumeArray* subVolumes = nullptr,
                 const std::string& volumeName = "undefined");
  // unsafe intelink layers

  /** Constructor for a full equipped Tracking Volume
     - full by inheritance  ======> 2 a) static confinement */
  TrackingVolume(const Volume& volume,
                 const Material& matprop,
                 LayerArray* subLayers = nullptr,
                 TrackingVolumeArray* subVolumes = nullptr,
                 const std::string& volumeName = "undefined");
  // unsafe intelink layers

  /** Constructor for a full equipped Tracking Volume
    - mixed  ======> 3 a) static confinement */
  TrackingVolume(Amg::Transform3D* htrans,
                 VolumeBounds* volbounds,
                 const Material& matprop,
                 LayerArray* subLayers = nullptr,
                 TrackingVolumeArray* subVolumes = nullptr,
                 const std::string& volumeName = "undefined");
  // unsafe intelink layers

  /** Constructor for a full equipped Tracking Volume with detached subvolumes
    -  mixed =======> 1 b) detached volumes */
  TrackingVolume(
    Amg::Transform3D* htrans,
    VolumeBounds* volbounds,
    const Material& matprop,
    const std::vector<const DetachedTrackingVolume*>* detachedSubVolumes,
    const std::string& volumeName = "undefined");

  /** Constructor for a full equipped Tracking Volume with detached subvolumes
    -  mixed =======> 2 b) detached volumes */
  TrackingVolume(
    const Volume& volume,
    const Material& matprop,
    const std::vector<const DetachedTrackingVolume*>* detachedSubVolumes,
    const std::string& volumeName = "undefined");

  /** Constructor for a full equipped Tracking Volume with unordered subvolumes
    - mixed =======> 1 d) unordered volumes  */
  TrackingVolume(Amg::Transform3D* htrans,
                 VolumeBounds* volbounds,
                 const Material& matprop,
                 const std::vector<const TrackingVolume*>* unorderedSubVolumes,
                 const std::string& volumeName = "undefined");

  /** Constructor for a full equipped Tracking Volume with unordered subvolumes
    - mixed =======> 2 d) unordered volumes  */
  TrackingVolume(const Volume& volume,
                 const Material& matprop,
                 const std::vector<const TrackingVolume*>* unorderedSubVolumes,
                 const std::string& volumeName = "undefined");

  /** Constructor for a full equipped Tracking Volume with arbitrary layers
    -  mixed =======> 1 c) arbitrarily oriented layers */
  TrackingVolume(Amg::Transform3D* htrans,
                 VolumeBounds* volbounds,
                 const Material& matprop,
                 const std::vector<const Layer*>* arbitraryLayers,
                 const std::string& volumeName = "undefined");

  /** Constructor for a full equipped Tracking Volume with arbitrary layers
    -  mixed =======> 2 c) arbitrarily oriented layers */
  TrackingVolume(const Volume& volume,
                 const Material& matprop,
                 const std::vector<const Layer*>* arbitraryLayers,
                 const std::string& volumeName = "undefined");

  /** Constructor for a full equipped Tracking Volume with arbitrary layers AND
    subVolumes -
    -  mixed =======> 1 e) unordered layers AND unordered subvolumes */
  TrackingVolume(Amg::Transform3D* htrans,
                 VolumeBounds* volbounds,
                 const std::vector<const Layer*>* arbitraryLayers,
                 const std::vector<const TrackingVolume*>* unorderedSubVolumes,
                 const Material& matprop,
                 const std::string& volumeName = "undefined");

  /** Constructor for a full equipped Tracking Volume with arbitrary layers AND
    subVolumes -
    -  mixed =======> 2 e) unordered layers AND unordered subvolumes */
  TrackingVolume(const Volume& volume,
                 const std::vector<const Layer*>* arbitraryLayers,
                 const std::vector<const TrackingVolume*>* unorderedSubVolumes,
                 const Material& matprop,
                 const std::string& volumeName = "undefined");

  /** copy constructor with shift */
  TrackingVolume(const TrackingVolume& trVol, Amg::Transform3D& transform);

  /** Destructor */
  ~TrackingVolume() override;

  /** Return the associated Layer */
  const Layer* associatedLayer(const Amg::Vector3D& gp) const;

  /** Return the next Layer if existing, NULL if no next layer corresponds */
  const Layer* nextLayer(const Amg::Vector3D& gp,
                         const Amg::Vector3D& mom,
                         bool asres = true,
                         bool skipNavLayer = false) const;

  /** Return the entry Layer to a TrackingVolume depending on the entry point
   * **/
  const Layer* entryLayer(const Amg::Vector3D& gp,
                          const Amg::Vector3D& dir) const;

  /** Return the exit Layer from a TrackingVolume */
  const Layer* exitLayer(const Amg::Vector3D& gp,
                         const Amg::Vector3D& dir) const;

  /** Return the material layers ordered based on straight line intersections:
      - startLayer and endLayer are included in the list
  */
  template<class T>
  std::vector<LayerIntersection<T>> materialLayersOrdered(
    const Layer* sLayer,
    const Layer* eLayer,
    const T& parameters,
    PropDirection pDir = alongMomentum,
    const BoundaryCheck& bchk = true,
    bool resolveSubSurfaces = false) const;

  /** Return the closest layer with material description
        - it assumes to be on lowest navigation level
        - does not step down to eventually confined TrackingVolumes
        - navigates to the next trackign volume IF PropDirection == mappingMode
    */
  LayerIntersection<Amg::Vector3D> closestMaterialLayer(
    const Amg::Vector3D& gp,
    const Amg::Vector3D& dir,
    PropDirection pDir = alongMomentum,
    const BoundaryCheck& bchk = true) const;

  /** Return the associated sub Volume, returns THIS if no subVolume exists */
  const TrackingVolume* associatedSubVolume(const Amg::Vector3D& gp) const;

  /** Return the next volume along the navigation stream */
  const TrackingVolume* nextVolume(const Amg::Vector3D& gp,
                                   const Amg::Vector3D& dir,
                                   PropDirection pDir = alongMomentum) const;

  /** Return the next sub Volume if existing, returns THIS if no subVolume
   * exists */
  const TrackingVolume* nextSubVolume(const Amg::Vector3D& gp,
                                      const Amg::Vector3D& dir) const;

  /** Return the associated detached subvolumes */
  std::vector<const DetachedTrackingVolume*>* assocDetachedSubVolumes(
    const Amg::Vector3D& gp,
    double tol) const;

  /** Layer attempts - as provided by the LayerAttemptCreator */
  unsigned int layerAttempts(BoundarySurfaceFace exitFace) const;

  /** Layer attempts - as provided by the LayerAttemptCreator */
  unsigned int maxLayerAttempts() const;

  /** Return the subLayer array */
  const LayerArray* confinedLayers() const;
  
  /** Return the subLayer array */
  LayerArray* confinedLayers();

  /** Return the subLayer array - not the ownership*/
  const std::vector<const Layer*>* confinedArbitraryLayers() const;

  /** Return the subLayer array - not the ownership*/
  const std::vector<const Layer*>* confinedArbitraryLayers();

  /** Return the subLayerarray including the ownership*/
  const LayerArray* checkoutConfinedLayers() const;

  /** Return the subLayer array */
  const TrackingVolumeArray* confinedVolumes() const;

  /** Return the subLayer array */
  TrackingVolumeArray* confinedVolumes();

 /** Return detached subVolumes - not the ownership */
  const std::vector<const DetachedTrackingVolume*>* confinedDetachedVolumes()
    const;

  /** Return unordered subVolumes - not the ownership */
  const std::vector<const TrackingVolume*>* confinedDenseVolumes() const;

  /** Returns the VolumeName - for debug reason, might be depreciated later */
  const std::string& volumeName() const;

  /** Method to return the BoundarySurfaces */
  const std::vector<SharedObject<const BoundarySurface<TrackingVolume>>>&
  boundarySurfaces() const;

  /** Returns the boundary surfaces ordered in probability to hit them based on
   * straight line intersection */
  template<class T>
  std::vector<BoundaryIntersection<T>> boundarySurfacesOrdered(
    const T& parameters,
    PropDirection pDir = alongMomentum,
    bool startOffBoundary = false) const;

  /** Get the BoundarySurface to the appointed Accessor state */
  const BoundarySurface<TrackingVolume>* boundarySurface(
    const ObjectAccessor::value_type& oa) const;

  /** show if you are on a boundary surface */
  template<class T>
  bool onVolumeBoundary(const T& pars) const;

  /** Register the outside glue volumes -
      ordering is in the TrackingVolume Frame:
       - negativeFaceXY
       - (faces YZ, ZY, radial faces)
       - positiveFaceXY
  */

  void registerOutsideGlueVolumes(GlueVolumesDescriptor* gvd);

  GlueVolumesDescriptor& glueVolumesDescriptor();
  const GlueVolumesDescriptor& glueVolumesDescriptor() const;

  /** sign the volume - the geometry builder has to do that */
  void sign ATLAS_NOT_THREAD_SAFE(GeometrySignature signat,
                                  GeometryType gtype = Static) const;

  /** return the Signature */
  GeometrySignature geometrySignature() const;

  /** return the Signature */
  GeometryType geometryType() const;

  /** Register the color code */
  void registerColorCode(unsigned int icolor);

  /** Get the color code */
  unsigned int colorCode() const;

  /** force a navigation check */
  void forceNavigationCheck();

  /** Boolean, if true navigation needs to be redone when hitting this volume */
  bool redoNavigation() const;

  /** Return the MotherVolume - if it exists */
  const TrackingVolume* getMotherVolume() const;

  /** set the MotherVolume */
  void setMotherVolume(const TrackingVolume* mvol);

  /** move Volume */
  void moveVolume(Amg::Transform3D& shift);

  /** add Material */
  void addMaterial(const Material& mat, float fact = 1.);

  virtual bool isAlignable() const;
  /** remove content */
  void clear();

  void screenDump(MsgStream& msg) const;

protected:
  /** clone at new position */
  TrackingVolume* cloneTV
  ATLAS_NOT_THREAD_SAFE(Amg::Transform3D& transform) const;

private:
  /** reIndex the static layers of the TrackingVolume */
  void indexContainedStaticLayers
  ATLAS_NOT_THREAD_SAFE(GeometrySignature geoSig, int& offset) const;

  /** reIndex the material layers of the TrackingVolume */
  void indexContainedMaterialLayers
  ATLAS_NOT_THREAD_SAFE(GeometrySignature geoSig, int& offset) const;

  /** Create Boundary Surface */
  void createBoundarySurfaces();

  /** Create Layer Attempts Caluclator */
  void createLayerAttemptsCalculator();

  /** compactify the memory usage in the event by setting ownership to
    TackingGeometry the referenced types are the number of registered surfaces &
    total surfaces
  */
  void compactify ATLAS_NOT_THREAD_SAFE(size_t& rSurfaces,
                                        size_t& tSurfaces) const;

  /** method to synchronize the layers with potentially updated volume bounds:
      - adapts the layer dimensions to the new volumebounds + envelope
      - adapts entry layer position where necessary to the new volumebounds
  */
  void synchronizeLayers ATLAS_NOT_THREAD_SAFE(MsgStream& msgstream, double envelope = 1.) const;

  /** Register Next - Previous for Layers, set volumelink */
  void interlinkLayers ();

  /** Helper method - find closest of two layers */
  static const Layer* closest(const Amg::Vector3D& pos,
                              const Amg::Vector3D& dir,
                              const Layer& first,
                              const Layer& second);

  /** move the Tracking Volume*/
  void moveTV ATLAS_NOT_THREAD_SAFE(Amg::Transform3D& transform) const;

  /** Forbidden copy constructor */
  TrackingVolume(const TrackingVolume&) = delete;

  /** Forbid assignment. */
  TrackingVolume& operator=(const TrackingVolume&) = delete;

  const TrackingVolume* m_motherVolume; //!< mother volume of this volume

  //!< boundary Surfaces
  std::vector<SharedObject<const BoundarySurface<TrackingVolume>>>* m_boundarySurfaces{};
  //(a)
  LayerArray* m_confinedLayers; //!< Array of Layers inside the Volume
  //!< Array of Volumes inside the Volume
  TrackingVolumeArray* m_confinedVolumes;
  //(b)
  //!< Detached subvolumes
  const std::vector<const DetachedTrackingVolume*>* m_confinedDetachedVolumes;
  // additionally
  //!< Unordered subvolumes
  const std::vector<const TrackingVolume*>* m_confinedDenseVolumes; 
  //(b)
  //!< Unordered Layers inside the Volume
  const std::vector<const Layer*>* m_confinedArbitraryLayers;

  ////!< Volumes to glue Volumes from the outside
  CxxUtils::CachedUniquePtrT<GlueVolumesDescriptor> m_outsideGlueVolumes;

  //!< provided the number of layer attempts
  LayerAttemptsCalculator* m_layerAttemptsCalculator;

  //!< The Signature done by the GeometryBuilder
  GeometrySignature m_geometrySignature;
  //!< defines how the Extrapolator propagates through this
  GeometryType m_geometryType;

  std::string m_name;       //!< Volume name for debug reasons
  unsigned int m_colorCode; //!< Color code for displaying
  bool m_redoNavigation; //!< Navigation boolean. If true navigation needs to be
                         //!< redone when entering this volume
};

} // end of namespace
#include "TrkGeometry/TrackingVolume.icc"
#endif // TRKGEOMETRY_TRACKINGVOLUME_H

