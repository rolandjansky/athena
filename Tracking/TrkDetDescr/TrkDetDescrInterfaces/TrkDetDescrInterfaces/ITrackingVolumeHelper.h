/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ITrackingVolumeHelper.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKDETDESCRINTERFACES_ITRACKINGVOLUMEHELPER_H
#define TRKDETDESCRINTERFACES_ITRACKINGVOLUMEHELPER_H

// Gaudi
#include "GaudiKernel/IAlgTool.h"
// Trk - template classes & enums
#include "GeoPrimitives/GeoPrimitives.h"
#include "TrkDetDescrUtils/BinnedArray.h"
#include "TrkDetDescrUtils/BinningType.h"
#include "TrkDetDescrUtils/SharedObject.h"
#include "TrkVolumes/BoundarySurface.h"
#include "TrkVolumes/BoundarySurfaceFace.h"
// STL
#include <string>

#include "CxxUtils/checker_macros.h"
namespace Trk {

class VolumeBounds;
class CylinderVolumeBounds;
class Material;
class Layer;
class TrackingVolume;

/** Interface ID for ITrackingVolumeHelpers*/
static const InterfaceID IID_ITrackingVolumeHelper("ITrackingVolumeHelper",
                                                   1,
                                                   0);

/** @class ITrackingVolumeHelper
  Interface class ITrackingVolumeHelpers
  It inherits from IAlgTool.

   Gives an interface to Helper Tool implemntations

   @author Andreas.Salzburger@cern.ch
  */
class ITrackingVolumeHelper : virtual public IAlgTool
{

public:
  /**Virtual destructor*/
  virtual ~ITrackingVolumeHelper() {}

  /** AlgTool and IAlgTool interface methods */
  static const InterfaceID& interfaceID() { return IID_ITrackingVolumeHelper; }

  /** Glue Volume method: One to one
       --- Neccessary as friendship cannot be inherited: your father's friend
     isn't necessary yours ---
      */
  virtual void glueTrackingVolumes
  ATLAS_NOT_THREAD_SAFE(const TrackingVolume& firstVol,
                        BoundarySurfaceFace firstFace,
                        const TrackingVolume& secondVol,
                        BoundarySurfaceFace secondFace,
                        bool buildBoundaryLayer = false) const = 0;

  /** Glue Volume method: One to many
       --- Neccessary as friendship cannot be inherited: your father's friend
     isn't necessary yours ---
      */
  virtual void glueTrackingVolumes
  ATLAS_NOT_THREAD_SAFE(const TrackingVolume& firstVol,
                        BoundarySurfaceFace firstFace,
                        const std::vector<const TrackingVolume*>& secondVolumes,
                        BoundarySurfaceFace secondFace,
                        bool buildBoundaryLayer = false,
                        bool boundaryFaceExchange = false) const = 0;

  /** Method to glue two VolumeArrays together (at navigation level) - without
     output
     --- Necessary as friendship cannot be inherited: your father's friend isn't
     necessary yours ---
  */
  virtual void glueTrackingVolumes
  ATLAS_NOT_THREAD_SAFE(const std::vector<const TrackingVolume*>& firstVolumes,
                        BoundarySurfaceFace firstFace,
                        const std::vector<const TrackingVolume*>& secondVolumes,
                        BoundarySurfaceFace secondFace,
                        bool buildBoundaryLayer = false,
                        bool boundaryFaceExchange = false) const = 0;

  /** Glue Volume method: many to many plus enveloping volume
       --- Neccessary as friendship cannot be inherited: your father's friend
     isn't necessary yours ---
      */
  virtual Trk::TrackingVolume* glueTrackingVolumeArrays
  ATLAS_NOT_THREAD_SAFE(const TrackingVolume& firstVol,
                        BoundarySurfaceFace firstFace,
                        const TrackingVolume& secondVol,
                        BoundarySurfaceFace secondFace,
                        std::string name) const = 0;

  /**  Glue Volume method: set inside Volume
     --- Neccessary as friendship cannot be inherited: your father's friend
     isn't necessary yours ---
  */
  virtual void setInsideTrackingVolume ATLAS_NOT_THREAD_SAFE(
    const TrackingVolume& tvol,
    BoundarySurfaceFace face,
    const TrackingVolume* insidevol) const = 0;

  /**  Glue Volume mehtod : set inside Volume Array
      --- Neccessary as friendship cannot be inherited: your father's friend
     isn't necessary yours ---
  */
  virtual void setInsideTrackingVolumeArray ATLAS_NOT_THREAD_SAFE(
    const TrackingVolume& tvol,
    BoundarySurfaceFace face,
    BinnedArray<const TrackingVolume>* insidevolarray) const = 0;

  virtual void setInsideTrackingVolumeArray ATLAS_NOT_THREAD_SAFE(
    const TrackingVolume& tvol,
    BoundarySurfaceFace face,
    SharedObject<BinnedArray<const TrackingVolume>> insidevolarray) const = 0;

  /**  Glue Volume method: set outside Volume
       --- Neccessary as friendship cannot be inherited: your father's friend
     isn't necessary yours ---
  */
  virtual void setOutsideTrackingVolume
  ATLAS_NOT_THREAD_SAFE(const TrackingVolume& tvol,
                        BoundarySurfaceFace face,
                        const TrackingVolume* outsidevol) const = 0;

  /**  Glue Volume method: set outside Volume array
       --- Neccessary as friendship cannot be inherited: your father's friend
     isn't necessary yours ---
   */
  virtual void setOutsideTrackingVolumeArray
  ATLAS_NOT_THREAD_SAFE(const TrackingVolume& tvol,
                        BoundarySurfaceFace face,
                        BinnedArray<const TrackingVolume>* outsidevolarray) const = 0;

  virtual void setOutsideTrackingVolumeArray ATLAS_NOT_THREAD_SAFE(
    const TrackingVolume& tvol,
    BoundarySurfaceFace face,
    SharedObject<BinnedArray<const TrackingVolume>> outsidevolarray) const = 0;

protected:
  /** Protected method to register the Layer to the Surface */
  void associateLayer(const Layer& lay, Surface& sf) const
  {
    sf.associateLayer(lay);
  }
};

} // end of namespace

#endif // TRKDETDESCRINTERFACES_IITRACKINGVOLUMEHELPER_H
