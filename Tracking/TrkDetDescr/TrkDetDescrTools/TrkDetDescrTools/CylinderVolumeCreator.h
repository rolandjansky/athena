/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// CylinderVolumeCreator.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKDETDESCRTOOLS_CYLINDERVOLUMECREATOR_H
#define TRKDETDESCRTOOLS_CYLINDERVOLUMECREATOR_H

#ifndef TRKDETDESCR_TAKESMALLERBIGGER
#define TRKDETDESCR_TAKESMALLERBIGGER
#define takeSmaller(current, test) current = current < test ? current : test
#define takeBigger(current, test) current = current > test ? current : test
#define takeSmallerBigger(cSmallest, cBiggest, test)                           \
  takeSmaller(cSmallest, test);                                                \
  takeBigger(cBiggest, test)
#endif

// Trk
#include "TrkDetDescrInterfaces/ITrackingVolumeCreator.h"
#include "TrkVolumes/BoundarySurfaceFace.h"
// Amg
#include "GeoPrimitives/GeoPrimitives.h"
// Gaudi & Athena
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
// STL
#include <string>
#include <vector>

#include "CxxUtils/checker_macros.h"
namespace Trk {

class ILayerArrayCreator;
class ITrackingVolumeArrayCreator;
class ITrackingVolumeHelper;
class Layer;
class CylinderLayer;
class DiscLayer;
class TrackingVolume;
class VolumeBounds;
class CylinderVolumeBounds;
class Material;

/** @class CylinderVolumeCreator

  The concrete implementation for cylindrical TrackingVolume
  objects of the ITrackingVolumeCreator interface

  @author Andreas.Salzburger@cern.ch
 */

class CylinderVolumeCreator
  : public AthAlgTool
  , virtual public ITrackingVolumeCreator
{

public:
  /** Constructor */
  CylinderVolumeCreator(const std::string&,
                        const std::string&,
                        const IInterface*);
  /** Destructor */
  ~CylinderVolumeCreator();

  /** AlgTool initialize method */
  virtual StatusCode initialize() override;

  /** @copydoc ITrackingVolumeCreator::createTrackingVolume(const
   * std::vector<const Layer*>&, Material&, VolumeBounds*,
   * Amg::Transform3D*,bool, const std::string&) const;
   */
  //Not safe due to TrackingVolume ctor with interlink
  //layers modifies the const Layers
  virtual TrackingVolume* createTrackingVolume ATLAS_NOT_THREAD_SAFE(
    const std::vector<const Layer*>& layers,
    Material& matprop,
    VolumeBounds* volBounds = 0,
    Amg::Transform3D* transform = 0,
    const std::string& volumeName = "UndefinedVolume",
    BinningType btype = arbitrary) const override final;

  /** @copydoc ITrackingVolumeCreator::createTrackingVolume(const
   * std::vector<const Layer*>& , Material&,
   * ,double,double,double,double,bool,const std::string&) const;
   */
  //Not safe as it calls the previous overload
  virtual TrackingVolume* createTrackingVolume ATLAS_NOT_THREAD_SAFE(
    const std::vector<const Layer*>& layers,
    Material& matprop,
    double loc1Min,
    double loc1Max,
    double loc2Min,
    double loc2Max,
    const std::string& volumeName = "UndefinedVolume",
    BinningType btype = arbitrary) const override final;

  /** @copydoc
   * ITrackingVolumeCreator::createGapTrackingVolume(Material&,,double,double,double,double,int,bool,const
   * std::string&) const;
   */
 virtual TrackingVolume* createGapTrackingVolume ATLAS_NOT_THREAD_SAFE( 
    Material& matprop,
    double rMin,
    double rMax,
    double zMin,
    double zMax,
    unsigned int materialLayers,
    bool cylinder = true,
    const std::string& volumeName = "UndefinedVolume") const override final;

  /** @copydoc
   * ITrackingVolumeCreator::createGaoTrackingVolume(Material&,,std::vector<double>&,int,bool,const
   * std::string&) const;
   */
  virtual TrackingVolume* createGapTrackingVolume ATLAS_NOT_THREAD_SAFE(
    Material& matprop,
    double rMin,
    double rMax,
    double zMin,
    double zMax,
    const std::vector<double>& layerPositions,
    bool cylinder = true,
    const std::string& volumeName = "UndefinedVolume",
    BinningType btype = arbitrary) const override final;

  /** @copydoc ITrackingVolumeCreator::createContainerTrackingVolume(
          const std::vector<const TrackingVolume*>, Material&, ,
          const std::string&) const;
   */
  virtual TrackingVolume* createContainerTrackingVolume
  ATLAS_NOT_THREAD_SAFE(const std::vector<const TrackingVolume*>& volumes,
                        Material& matprop,
                        const std::string& volumeName = "UndefinedVolume",
                        bool buildBoundaryLayers = false,
                        bool replaceBoundaryFace = false) const override final;

private:
  /** Private method - it estimates the CylinderBounds and Translation of
     layers,
              if given, these are checked against the layer
     positions/dimensions. */

  StatusCode estimateAndCheckDimension ATLAS_NOT_THREAD_SAFE(
    const std::vector<const Layer*>& layers,
    Trk::CylinderVolumeBounds*& cylBounds,
    Amg::Transform3D*& translation,
    std::vector<CylinderLayer*>& cylLayers,
    std::vector<DiscLayer*>& discLayers,
    double& rMinClean,
    double& rMaxClean,
    double& zMinClean,
    double& zMaxClean,
    BinningType bType = arbitrary) const;

  /** Private method - interglue all volumes contained by a TrackingVolume
              and set the outside glue volumes in the descriptor */
  StatusCode interGlueTrackingVolume
  ATLAS_NOT_THREAD_SAFE(TrackingVolume& tVolume,
                        bool rBinned,
                        bool buildBoundaryLayers,
                        bool replaceBoundaryFace = false) const;

  /** Private method - helper method not to duplicate code */
  void addFaceVolumes
  ATLAS_NOT_THREAD_SAFE(const TrackingVolume& tvol,
                        Trk::BoundarySurfaceFace bsf,
                        std::vector<const Trk::TrackingVolume*>& vols) const;

  /** Private method - glue volume to the other -- use trackingVolume helper */
  void glueTrackingVolumes
  ATLAS_NOT_THREAD_SAFE(const TrackingVolume& volumeOne,
                        BoundarySurfaceFace faceOne,
                        const TrackingVolume& volumeTwo,
                        BoundarySurfaceFace faceTwo,
                        bool buildBoundaryLayers,
                        bool replaceBoundaryFace = false) const;

  /** Private method - helper method to save some code */
  CylinderLayer* createCylinderLayer(double z,
                                     double r,
                                     double halflength,
                                     double thickness,
                                     int binsPhi,
                                     int binsZ) const;

  /** Private method - helper method to save some code */
  DiscLayer* createDiscLayer(double z,
                             double rMin,
                             double rMax,
                             double thickness,
                             int binsPhi,
                             int binsR) const;

  // helper tools
  ToolHandle<ILayerArrayCreator>
    m_layerArrayCreator; //!< A Tool for coherent LayerArray creation
  ToolHandle<ITrackingVolumeArrayCreator>
    m_trackingVolumeArrayCreator; //!< Helper Tool to create TrackingVolume
                                  //!< Arrays
  ToolHandle<ITrackingVolumeHelper>
    m_trackingVolumeHelper; //!< TrackingVolume helper

  double m_passiveLayerThickness; //!< thickness of passive layers
  int m_passiveLayerPhiBins;      //!< bins in phi for the passive layer
  int m_passiveLayerRzBins;       //!< bins in r/z for the passive layer
};
}

#endif

