/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ITrackingVolumeCreator.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKDETDESCRINTERFACES_ITRACKINGVOLUMECREATOR_H
#define TRKDETDESCRINTERFACES_ITRACKINGVOLUMECREATOR_H

// Gaudi
#include "GaudiKernel/IAlgTool.h"
// Trk
#include "TrkDetDescrUtils/BinningType.h"
// STL
#include <string>
#include <vector>

#include "CxxUtils/checker_macros.h"
#include "GeoPrimitives/GeoPrimitives.h"

namespace Trk {

class Layer;
class TrackingVolume;
class VolumeBounds;
class Material;
class MagneticFieldProperties;

/** Interface ID for ITrackingVolumeCreators*/
static const InterfaceID IID_ITrackingVolumeCreator("ITrackingVolumeCreator",
                                                    1,
                                                    0);

/** @class ITrackingVolumeCreator

  Interface class ITrackingVolumeCreators
  It inherits from IAlgTool.

   Gives an interface to Helper Tool implemntations

   @author Andreas.Salzburger@cern.ch
  */
class ITrackingVolumeCreator : virtual public IAlgTool
{

public:
  /**Virtual destructor*/
  virtual ~ITrackingVolumeCreator() {}

  /** AlgTool and IAlgTool interface methods */
  static const InterfaceID& interfaceID() { return IID_ITrackingVolumeCreator; }

  /** create a TrackingVolume* from a set of layers and (optional) parameters

      @param layers : vector of static layers confined by the TrackingVolume
                      if no bounds or HepTransform is given, they define the
     size together with the volume enevlope parameters
      @param matprop : dense material properties for this TrackingVolume
      @param volBounds : (optional) bounds of this TrackingVolume - ownership
     given
      @param transform : (optional) placement of this TrackingVolume - ownership
     given
      @param entryLayers : switch to build entry layers
      @param volumeName  : volume name to be given

      */
  virtual TrackingVolume* createTrackingVolume ATLAS_NOT_THREAD_SAFE(
    const std::vector<const Layer*>& layers,
    Material& matprop,
    VolumeBounds* volBounds = 0,
    Amg::Transform3D* transform = 0,
    const std::string& volumeName = "UndefinedVolume",
    BinningType btype = arbitrary) const = 0;

  /** create a TrackingVolume* from a set of layers and (optional) parameters

      @param layers : vector of static layers confined by the TrackingVolume
                      if no bounds or HepTransform is given, they define the
     size together with the volume enevlope parameters
      @param matprop : dense material properties for this TrackingVolume
      @param loc1Min, loc1Max, loc2Min, loc2Max : local position in space,
                       this TrackingVolume is restricted to Translation only
      @param volumeName  : volume name to be given

      */
  virtual TrackingVolume* createTrackingVolume ATLAS_NOT_THREAD_SAFE(
    const std::vector<const Layer*>& layers,
    Material& matprop,
    double loc1Min,
    double loc1Max,
    double loc2Min,
    double loc2Max,
    const std::string& volumeName = "UndefinedVolume",
    BinningType btype = arbitrary) const = 0;

  /** create a gap volume from dimensions and

     @param matprop : dense material properties for this TrackingVolume
     @param loc1Min, loc1Max, loc2Min, loc2Max : local position in space,
                      this TrackingVolume is restricted to Translation only
     @param materialLayers : number of material layers (aequidistant
     binning)
     @param cylinder : type of layers
     @param volumeName  : volume name to be given

   */
  virtual TrackingVolume* createGapTrackingVolume ATLAS_NOT_THREAD_SAFE(
    Material& matprop,
    double loc1Min,
    double loc1Max,
    double loc2Min,
    double loc2Max,
    unsigned int materialLayers,
    bool cylinder = true,
    const std::string& volumeName = "UndefinedVolume") const = 0;

  /** create a gap volume from dimensions and

     @param matprop : dense material properties for this TrackingVolume
     @param layerPositions : custom layer positions
     @param materialLayers : number of material layers (aequidistant
     binning)
     @param cylinder : type of layers
     @param volumeName  : volume name to be given

   */
  virtual TrackingVolume* createGapTrackingVolume
  ATLAS_NOT_THREAD_SAFE(Material& matprop,
                        double loc1Min,
                        double loc1Max,
                        double loc2Min,
                        double loc2Max,
                        const std::vector<double>& layerPositions,
                        bool cylinder = true,
                        const std::string& volumeName = "UndefinedVolume",
                        BinningType btype = arbitrary) const = 0;

  /** Create a one level higher TrackingVolue

      @param volumes : the volumes to be comnbined
      @param matprop : dense material properties for this TrackingVolume
      @param volumeName  : volume name to be given

  */
  virtual TrackingVolume* createContainerTrackingVolume
  ATLAS_NOT_THREAD_SAFE(const std::vector<const TrackingVolume*>& volumes,
                        Material& matprop,
                        const std::string& volumeName = "UndefinedVolume",
                        bool buildBoundaryLayers = false,
                        bool replaceBoundaryFace = false) const = 0;
};

} // end of namespace

#endif // TRKDETDESCRINTERFACES_ITRACKINGVOLUMECREATOR_H
