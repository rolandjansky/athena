/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TrackingGeometry.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKGEOMETRY_TRACKINGGEOMETRY_H
#define TRKGEOMETRY_TRACKINGGEOMETRY_H

// Amg
#include "GeoPrimitives/GeoPrimitives.h"
// Trk
#include "TrkDetDescrUtils/GeometrySignature.h"
#include "TrkGeometry/TrackingVolume.h"
// CLASS DEF
#include "AthenaKernel/CLASS_DEF.h"
// STL
#include <map>
// ATH_MSG macros
#include "AthenaBaseComps/AthMsgStreamMacros.h"
#include "AthenaKernel/MsgStreamMember.h"

#include "CxxUtils/checker_macros.h"

class MsgStream;

// must be known to declare friend class
namespace Muon {
class MuonTrackingGeometryBuilderCond;
}

namespace Trk {

/** @enum NavigationLevel
    destinguishes an association TrackingGeometry with
    one for global search */

enum NavigationLevel
{
  noNavigation = 0,
  globalSearch = 1,
  association = 2
};

class TrackingVolume;
class Layer;
class MaterialProperties;

/**
  @class TrackingGeometry

  The TrackingGeometry class is the owner of the constructed TrackingVolumes.
  It enables both, a global search for an asociatedVolume
  (respectively, if existing, a global search of an associated Layer or the next
  associated Layer), such as a continous navigation by BoundarySurfaces between
  the confined TrackingVolumes.

  @author Andreas.Salzburger@cern.ch
  @author Christos Anastopoulos (Athena MT modifications)

  */
class TrackingGeometry
{

  /** Give the GeometryBuilder friend rights */
  friend class GeometryBuilder;

  friend class GeometryBuilderCond;

  // gives ownership over MuonTG constituents
  friend class Muon::MuonTrackingGeometryBuilderCond;

public:
  /** Constructor */
  TrackingGeometry(TrackingVolume* highestVolume,
                   NavigationLevel navlevel = globalSearch)
    ATLAS_CTORDTOR_NOT_THREAD_SAFE; // unsafe registerTrackingVolumes called;
  /** Destructor */
  virtual ~TrackingGeometry();

  /** return the world */
  const TrackingVolume* highestTrackingVolume() const;

  /** return the lowest tracking Volume */
  const TrackingVolume* lowestTrackingVolume(const Amg::Vector3D& gp) const;

  /** return the vector of lowest detached tracking Volume(->overlaps) */
  std::vector<const Trk::DetachedTrackingVolume*>*
  lowestDetachedTrackingVolumes(const Amg::Vector3D& gp) const;

  /** return the lowest static tracking Volume */
  const TrackingVolume* lowestStaticTrackingVolume(
    const Amg::Vector3D& gp) const;

  /** return the tracking Volume by name, 0 if it doesn't exist */
  const TrackingVolume* trackingVolume(const std::string& name) const;

  /** Forward the associated Layer information */
  const Layer* associatedLayer(const Amg::Vector3D& gp) const;

  /** Forward the next Layer information */
  const Layer* nextLayer(const Amg::Vector3D& gp,
                         const Amg::Vector3D& mom,
                         bool skipNavLayer = false) const;

  /** Closest Material Layer - used for the mapping option */
  template<class T>
  LayerIntersection<Amg::Vector3D> closestMaterialLayer(
    const T& pars,
    PropDirection pDir = Trk::alongMomentum,
    const BoundaryCheck& bchk = true) const;
  /** check position at volume boundary */
  static bool atVolumeBoundary(const Amg::Vector3D& gp,
                               const TrackingVolume* vol,
                               double tol);

  /** check position at volume boundary + navigation link */
  static bool atVolumeBoundary(const Amg::Vector3D& gp,
                               const Amg::Vector3D& mom,
                               const TrackingVolume* vol,
                               const TrackingVolume*& nextVol,
                               Trk::PropDirection dir,
                               double tol);

  /** Return the unique BoundarySurfaces with MaterialInformation */
  const std::map<const Layer*, int>& boundaryLayers() const;

  /** Return the Navigation Level - only one TrackingGeometry can have full
     association to GeoModel */
  NavigationLevel navigationLevel() const;

  /** Print the summary of volume Hierarchy of the TrackingGeometry */
  void printVolumeHierarchy(MsgStream& msgstream) const;

  /** indexLayers : method to re-set the index of the layers, depending on
   * geometrySignature */
  void indexStaticLayers ATLAS_NOT_THREAD_SAFE(GeometrySignature geosit,
                                               int offset = 0);

private:
  /** Geometry Builder busineess:
      the geometry builder has to sign*/
  void sign ATLAS_NOT_THREAD_SAFE(GeometrySignature geosit,
                                  GeometryType geotype = Static);

  /** Geometry Builder busineess:
      set all contained surfaces TG owned - this should save memory and avoid
     surface copying
      - prints compactification statistics */
  void compactify ATLAS_NOT_THREAD_SAFE(MsgStream& msgstream,
                                        const TrackingVolume* vol = nullptr);

  /**  Geometry Builder busineess:
       synchronize all layers to enclosed volume dimensions */
  void synchronizeLayers
  ATLAS_NOT_THREAD_SAFE(MsgStream& msgstream,
                        const TrackingVolume* vol = nullptr);

  /** private method the Navigation Level */
  void registerNavigationLevel(NavigationLevel navlevel);

  /** private method to register recursively the tracking volumes */
  void registerTrackingVolumes
  ATLAS_NOT_THREAD_SAFE(const TrackingVolume& tvol,
                        const TrackingVolume* mvol = nullptr,
                        int lvl = 0);

  /**  private method to be called from GeometryBuilder: return the world with
   * ownership */
  const TrackingVolume* checkoutHighestTrackingVolume() const;
  TrackingVolume* checkoutHighestTrackingVolume();

  /** print VolumeInformation with Level */
  void printVolumeInformation(MsgStream& msgstream,
                              const TrackingVolume& tvol,
                              int lvl) const;

  /** The known world - and the beam */
  TrackingVolume* m_world;

  /** The unique boundary Layers */
  std::map<const Layer*, int> m_boundaryLayers;

  /** The Volumes in a map for later finding */
  std::map<const std::string, const TrackingVolume*> m_trackingVolumes;

  /** The Navigation level for identification */
  NavigationLevel m_navigationLevel;

  /** keep ownership of MuonTrackingGeometry elements in here */
  std::unique_ptr<std::vector<
    std::vector<std::pair<std::unique_ptr<const Trk::Volume>, float>>>>
    m_muonInertMaterialConstituents;
  // muon chambers
  std::unique_ptr<
    const std::vector<std::unique_ptr<const Trk::DetachedTrackingVolume>>>
    m_muonStations;
  // muon inert material
  std::unique_ptr<
    const std::vector<std::unique_ptr<const Trk::DetachedTrackingVolume>>>
    m_muonInertObjs;
};

} // end of namespace
#include "TrkGeometry/TrackingGeometry.icc"
CLASS_DEF(Trk::TrackingGeometry, 167645219, 1)
#include "AthenaKernel/CondCont.h"
CONDCONT_MIXED_DEF(Trk::TrackingGeometry, 119021535);

#endif // TRKGEOMETRY_TRACKINGGEOMETRY_H
