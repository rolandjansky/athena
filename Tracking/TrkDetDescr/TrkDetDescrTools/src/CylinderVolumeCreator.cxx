/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// CylinderVolumeCreator.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

//Gaudi
#include "GaudiKernel/SystemOfUnits.h"
// Trk include
#include "TrkDetDescrTools/CylinderVolumeCreator.h"
#include "TrkDetDescrInterfaces/ILayerArrayCreator.h"
#include "TrkDetDescrInterfaces/ITrackingVolumeArrayCreator.h"
#include "TrkDetDescrInterfaces/ITrackingVolumeHelper.h"
#include "TrkSurfaces/CylinderBounds.h"
#include "TrkSurfaces/DiscBounds.h"
#include "TrkVolumes/AbstractVolume.h"
#include "TrkVolumes/CylinderVolumeBounds.h"
#include "TrkGeometry/TrackingVolume.h"
#include "TrkGeometry/CylinderLayer.h"
#include "TrkGeometry/DiscLayer.h"
#include "TrkGeometry/Material.h"
#include "TrkGeometry/BinnedLayerMaterial.h"
#include "TrkGeometry/GlueVolumesDescriptor.h"
#include "TrkDetDescrUtils/BinUtility.h"
// Amg
#include "EventPrimitives/EventPrimitivesToStringConverter.h"
#include "GeoPrimitives/GeoPrimitives.h"


// constructor
Trk::CylinderVolumeCreator::CylinderVolumeCreator(const std::string& t, const std::string& n, const IInterface* p)
: AthAlgTool(t,n,p),
  m_layerArrayCreator("Trk::LayerArrayCreator/LayerArrayCreator"),
  m_trackingVolumeArrayCreator("Trk::TrackingVolumeArrayCreator/TrackingVolumeArrayCreator"),
  m_trackingVolumeHelper("Trk::TrackingVolumeHelper/TrackingVolumeHelper"),
  m_passiveLayerThickness(1*Gaudi::Units::mm),
  m_passiveLayerPhiBins(1),
  m_passiveLayerRzBins(100)
{
    declareInterface<ITrackingVolumeCreator>(this);
    // the helper tools
    declareProperty("LayerArrayCreator",          m_layerArrayCreator);
    declareProperty("TrackingVolumeArrayCreator", m_trackingVolumeArrayCreator);
    declareProperty("TrackingVolumeHelper",       m_trackingVolumeHelper);
    // the parameters for the passive layers
    declareProperty("PassiveLayerThickness",      m_passiveLayerThickness);
    declareProperty("PassiveLayerBinsPhi",        m_passiveLayerPhiBins);
    declareProperty("PassiveLayerBinsRZ",         m_passiveLayerRzBins);
}

// destructor
Trk::CylinderVolumeCreator::~CylinderVolumeCreator()
{}


// the interface methods
StatusCode Trk::CylinderVolumeCreator::initialize()
{

    // Retrieve the layer array creator  ----------------------------------------------------
    if (m_layerArrayCreator.retrieve().isFailure())
    {
        ATH_MSG_FATAL( "Failed to retrieve tool " << m_layerArrayCreator );
        return StatusCode::FAILURE;
    } else 
        ATH_MSG_DEBUG( "Retrieved tool " << m_layerArrayCreator );


    // Retrieve the volume array creator  ----------------------------------------------------
    if (m_trackingVolumeArrayCreator.retrieve().isFailure())
    {
        ATH_MSG_FATAL( "Failed to retrieve tool " << m_trackingVolumeArrayCreator );
        return StatusCode::FAILURE;
    } else 
        ATH_MSG_DEBUG( "Retrieved tool " << m_trackingVolumeArrayCreator );


    // Retrieve the volume array creator  ----------------------------------------------------
    if (m_trackingVolumeHelper.retrieve().isFailure())
    {
        ATH_MSG_FATAL( "Failed to retrieve tool " << m_trackingVolumeHelper );
        return StatusCode::FAILURE;
    } else 
        ATH_MSG_DEBUG( "Retrieved tool " << m_trackingVolumeHelper );

    ATH_MSG_DEBUG( "initialize() successful" );

    return StatusCode::SUCCESS;
}

Trk::TrackingVolume* Trk::CylinderVolumeCreator::createTrackingVolume
ATLAS_NOT_THREAD_SAFE(const std::vector<const Trk::Layer*>& layers,
                      Trk::Material& matprop,
                      Trk::VolumeBounds* volBounds,
                      Amg::Transform3D* transform,
                      const std::string& volumeName,
                      Trk::BinningType btype) const

{
    
    // the final one to build / sensitive Volume / Bounds
    Trk::TrackingVolume* tVolume = nullptr;

    // cases are:
    // (1) volBounds && transform   : use both information
    // (2) volBounds && !transform  : centered around 0, but with given bounds
    // (3) !volBounds && transform  : estimate size from layers, use transform
    // (4) !volBounds && !transform : estimate size & translation from layers
    Trk::CylinderVolumeBounds* cylinderBounds = nullptr;
    // this is the implementation of CylinderVolumeCreator
    if (volBounds) {
        cylinderBounds = dynamic_cast<Trk::CylinderVolumeBounds*>(volBounds);
        if (!cylinderBounds){
            ATH_MSG_WARNING( "[!] Problem: given bounds were not cylindrical - return 0" );
            return tVolume;
        }
    }
    std::vector<Trk::CylinderLayer*> cylLayers;
    cylLayers.reserve(layers.size());
    std::vector<Trk::DiscLayer*> discLayers;
    discLayers.reserve(layers.size());

    // the raw data
    double rMinRaw = 0.;
    double rMaxRaw = 0.;
    double zMinRaw = 0.;
    double zMaxRaw = 0.;
       
    // check the dimension and fill raw data
    if (estimateAndCheckDimension(layers,
                                  cylinderBounds,
                                  transform,
                                  cylLayers,
                                  discLayers,
                                  rMinRaw,rMaxRaw,
                                  zMinRaw,zMaxRaw,
                                  btype).isFailure()) {
        ATH_MSG_WARNING( "[!] Problem with given dimensions - return 0 and delete provided objects" );
        delete volBounds; delete transform;
        delete cylinderBounds;
        return tVolume;
    }

    // get the zMin/Max
    double zMin = ( transform ? transform->translation().z() : 0. ) +
                  ( cylinderBounds ? -cylinderBounds->halflengthZ() : 0. );
    double zMax = ( transform ? transform->translation().z() : 0. ) +
                  ( cylinderBounds ?  cylinderBounds->halflengthZ() : 0. );

    double rMin = 0.;
    double rMax = 0.;

    // overrule the zMin/zMax for biequidistant binning
    if (btype == Trk::biequidistant) {
        // set rMin/rMax and zMin/zMax 
        zMin = zMinRaw;
        zMax = zMaxRaw;
        rMin = rMinRaw;
        rMax = rMaxRaw;
    } else {
        if (!cylinderBounds) {
          ATH_MSG_WARNING( "[!] No cylindrical bounds given - return 0" );
          return tVolume;
        }
        rMin = cylinderBounds->innerRadius();
        rMax = cylinderBounds->outerRadius();
    }

    ATH_MSG_VERBOSE("Filling the layers into an appropriate layer array");
    // create the Layer Array
    Trk::BinnedArray<Trk::Layer>*  layerArray = !cylLayers.empty() ?
            m_layerArrayCreator->cylinderLayerArray(cylLayers,
                                                    rMin,
                                                    rMax,
                                                    btype) :
            m_layerArrayCreator->discLayerArray(discLayers,
                                                zMin,
                                                zMax,
                                                btype);

    // finally create the TrackingVolume
    tVolume = new Trk::TrackingVolume(transform,
                                      cylinderBounds,
                                      matprop,
                                      layerArray,nullptr,
                                      volumeName);
    // screen output
    ATH_MSG_VERBOSE( "Created cylindrical volume at z-position :" <<  tVolume->center().z() );
    ATH_MSG_VERBOSE( "   created bounds : " << tVolume->volumeBounds() );   

    // return the constructed TrackingVolume
    return tVolume;
}

Trk::TrackingVolume* Trk::CylinderVolumeCreator::createTrackingVolume
ATLAS_NOT_THREAD_SAFE(const std::vector<const Trk::Layer*>& layers,
                      Trk::Material& matprop,
                      double rMin,
                      double rMax,
                      double zMin,
                      double zMax,
                      const std::string& volumeName,
                      Trk::BinningType btype) const
{
  // that's what is needed
  Trk::CylinderVolumeBounds* cBounds = nullptr;

  // screen output
  ATH_MSG_VERBOSE("Create cylindrical TrackingVolume '" << volumeName << "'.");
  ATH_MSG_VERBOSE("    -> with given dimensions of (rMin/rMax/zMin/Max) = "
                  << rMin << " / " << rMax << " / " << zMin << " / " << zMax);

  // check for consistency
  if (zMin > zMax || rMin > rMax) {
    ATH_MSG_WARNING("Inconsistent dimensions given :"
                    << ((zMin > zMax) ? " zMin > zMax (" : " rMin > rMax (")
                    << ((zMin > zMax) ? zMin : rMin) << " > "
                    << ((zMin > zMax) ? zMax : rMax) << " ) - return 0");
    return nullptr;
  }

  // create a Amg::Transform3D and VolumeBounds out of the zMin/zMax
  double halflengthZ = 0.5 * (zMax - zMin);
  double zPosition = 0.5 * (zMin + zMax);
  zPosition = fabs(zPosition) < 0.1 ? 0. : zPosition;

  // now create the cylinder volume bounds
  cBounds = rMin > 0.1 ? new Trk::CylinderVolumeBounds(rMin, rMax, halflengthZ)
                       : new Trk::CylinderVolumeBounds(rMax, halflengthZ);
  // transform
  Amg::Transform3D* transform =
    (zPosition != 0) ? new Amg::Transform3D : nullptr;
  if (transform)
    (*transform) = Amg::Translation3D(0., 0., zPosition);

  // call to the Bounds/Amg::Translation3D method
  return createTrackingVolume(
    layers, matprop, cBounds, transform, volumeName, btype);
}

Trk::TrackingVolume*
Trk::CylinderVolumeCreator::createGapTrackingVolume(Trk::Material& matprop,
                                                    double rMin,
                                                    double rMax,
                                                    double zMin,
                                                    double zMax,
                                                    unsigned int materialLayers,
                                                    bool cylinder,
                                                    const std::string& volumeName) const
{

    // screen output
    ATH_MSG_VERBOSE( "Create cylindrical gap TrackingVolume '" << volumeName << "' with (rMin/rMax/zMin/Max) = ");
    ATH_MSG_VERBOSE( '\t' << rMin << " / " << rMax << " / " << zMin << " / " << zMax );

    // assing min/max
    double min = cylinder ? rMin : zMin;
    double max = cylinder ? rMax : zMax;

    // create the layer r/z positions
    std::vector<double> layerPositions;
    layerPositions.reserve(materialLayers);
    if (materialLayers > 1){
        //double step = cylinder ? (max-min)/(materialLayers-1) : (max-min)/(materialLayers-1);
        const double step=(max-min)/(materialLayers-1);
        for (unsigned int il = 0; il < materialLayers; ++il)
            layerPositions.push_back(min+il*step);
    } else
        layerPositions.push_back(0.5*(min+max));

    // now call the main method
    return createGapTrackingVolume(matprop,
                                   rMin,
                                   rMax,
                                   zMin,
                                   zMax,
                                   layerPositions,
                                   cylinder,
                                   volumeName,
                                   (layerPositions.size() == 1 ? Trk::arbitrary : Trk::biequidistant));

}

Trk::TrackingVolume* Trk::CylinderVolumeCreator::createGapTrackingVolume
ATLAS_NOT_THREAD_SAFE(Trk::Material& matprop,
                      double rMin,
                      double rMax,
                      double zMin,
                      double zMax,
                      const std::vector<double>& layerPositions,
                      bool cylinder,
                      const std::string& volumeName,
                      BinningType btype) const
{

  // screen output
  ATH_MSG_VERBOSE("Create cylindrical gap TrackingVolume '"
                  << volumeName << "' with (rMin/rMax/zMin/Max) = ");
  ATH_MSG_VERBOSE('\t' << rMin << " / " << rMax << " / " << zMin << " / "
                       << zMax);

  // create the layers
  std::vector<const Trk::Layer*> layers;
  layers.reserve(layerPositions.size());

  std::vector<double>::const_iterator layerPropIter = layerPositions.begin();
  std::vector<double>::const_iterator layerPropEnd = layerPositions.end();
  for (; layerPropIter != layerPropEnd; ++layerPropIter) {
    // create cylinder layers
    if (cylinder) {
      // take envelopes into account
      double zMinLayer = zMin;
      double zMaxLayer = zMax;
      // create the layer
      layers.push_back(createCylinderLayer(0.5 * (zMinLayer + zMaxLayer),
                                           (*layerPropIter),
                                           fabs(0.5 * (zMaxLayer - zMinLayer)),
                                           m_passiveLayerThickness,
                                           m_passiveLayerPhiBins,
                                           m_passiveLayerRzBins));

    } else {
      // take the envelopes into account
      double rMinLayer = rMin;
      double rMaxLayer = rMax;
      // create the layer
      layers.push_back(createDiscLayer((*layerPropIter),
                                       rMinLayer,
                                       rMaxLayer,
                                       m_passiveLayerThickness,
                                       m_passiveLayerPhiBins,
                                       m_passiveLayerRzBins));
    }
  }
  // now call the createTrackingVolume() method
  return createTrackingVolume(
    layers, matprop, rMin, rMax, zMin, zMax, volumeName, btype);
}

Trk::TrackingVolume* Trk::CylinderVolumeCreator::createContainerTrackingVolume
ATLAS_NOT_THREAD_SAFE(const std::vector<const Trk::TrackingVolume*>& volumes,
                      Trk::Material& matprop,
                      const std::string& volumeName,
                      bool buildBoundaryLayers,
                      bool replaceBoundaryFace) const
{
  // check if you have more than one volume
  if (volumes.size() <= (unsigned int)1) {
    ATH_MSG_WARNING("None (only one) TrackingVolume given to create container "
                    "volume (min required: 2) - returning 0 ");
    return nullptr;
  }

  // screen output
  ATH_MSG_VERBOSE("[start] Creating container volume '"
                  << volumeName << "' with " << volumes.size()
                  << " sub volumes:");
  // volumes need to be sorted in either r or z - both increasing
  // set the iterator to the volumes, the first and the end
  auto firstVolume = volumes.begin();
  auto lastVolume = volumes.end();

  for (unsigned int ivol = 0; firstVolume != lastVolume;
       ++firstVolume, ++ivol) {
    ATH_MSG_VERBOSE("   - volume ("
                    << ivol << ") is : " << (*firstVolume)->volumeName());
    ATH_MSG_VERBOSE(
      "     at position : " << Amg::toString((*firstVolume)->center()));
    ATH_MSG_VERBOSE("     with bounds : " << (*firstVolume)->volumeBounds());
  }

  // reset the iterator
  firstVolume = volumes.begin();
  --lastVolume; // set to the last volume

  if (firstVolume == lastVolume) {
    ATH_MSG_WARNING("Only one TrackingVolume given to create Top level volume "
                    "(min required: 2) - returning 0 ");
    return nullptr;
  }

  // get the bounds
  const Trk::CylinderVolumeBounds* firstVolumeBounds =
    dynamic_cast<const Trk::CylinderVolumeBounds*>(
      &((*firstVolume)->volumeBounds()));
  const Trk::CylinderVolumeBounds* lastVolumeBounds =
    dynamic_cast<const Trk::CylinderVolumeBounds*>(
      &((*lastVolume)->volumeBounds()));
  // check the dynamic cast
  if (!firstVolumeBounds || !lastVolumeBounds) {
    ATH_MSG_WARNING("VolumeBounds given are not of type: "
                    "Trk::CylinderVolumeBounds (required) - returning 0 ");
    return nullptr;
  }

  // check whether it is a r-binned case or a z-binned case
  bool rCase = fabs(firstVolumeBounds->innerRadius() -
                    lastVolumeBounds->innerRadius()) > 0.1;
  // fill these ones depending on the rCase though assignment - no parsing at
  // that stage
  double zMin = 0.;
  double zMax = 0.;
  double rMin = 0.;
  double rMax = 0.;
  if (rCase) {
    zMin = (*firstVolume)->center().z() - firstVolumeBounds->halflengthZ();
    zMax = (*firstVolume)->center().z() + firstVolumeBounds->halflengthZ();
    rMin = firstVolumeBounds->innerRadius();
    rMax = lastVolumeBounds->outerRadius();
  } else {
    zMin = (*firstVolume)->center().z() - firstVolumeBounds->halflengthZ();
    zMax = (*lastVolume)->center().z() + lastVolumeBounds->halflengthZ();
    rMin = firstVolumeBounds->innerRadius();
    rMax = firstVolumeBounds->outerRadius();
  }

  // estimate the z - position
  double zPos = 0.5 * (zMin + zMax);
  // create the HEP transform from the stuff known so far
  Amg::Transform3D* topVolumeTransform =
    fabs(zPos) > 0.1 ? new Amg::Transform3D : nullptr;
  if (topVolumeTransform)
    (*topVolumeTransform) = Amg::Translation3D(0., 0., zPos);
  // create the bounds from the information gathered so far
  Trk::CylinderVolumeBounds* topVolumeBounds =
    fabs(rMin) > 0.1
      ? new Trk::CylinderVolumeBounds(rMin, rMax, 0.5 * fabs(zMax - zMin))
      : new Trk::CylinderVolumeBounds(rMax, 0.5 * fabs(zMax - zMin));
  // create the volume array to fill in
  Trk::BinnedArray<const Trk::TrackingVolume>* volumeArray =
    (rCase) ? m_trackingVolumeArrayCreator->cylinderVolumesArrayInR(volumes)
            : m_trackingVolumeArrayCreator->cylinderVolumesArrayInZ(volumes);
  if (!volumeArray) {
    ATH_MSG_WARNING(
      "Creation of TrackingVolume array did not succeed - returning 0 ");
    delete topVolumeTransform;
    delete topVolumeBounds;
    return nullptr;
  }

  // we have the bounds and the volume array, create the volume
  Trk::TrackingVolume* topVolume = new Trk::TrackingVolume(topVolumeTransform,
                                                           topVolumeBounds,
                                                           matprop,
                                                           nullptr,
                                                           volumeArray,
                                                           volumeName);

  // glueing section
  // --------------------------------------------------------------------------------------
  if (interGlueTrackingVolume(
        *topVolume, rCase, buildBoundaryLayers, replaceBoundaryFace)
        .isFailure()) {
    ATH_MSG_WARNING(
      "Problem with inter-glueing of TrackingVolumes (needed) - returning 0 ");
    delete topVolume;
    return nullptr;
  }

  ATH_MSG_VERBOSE(
    "[ end ] return newly created container : " << topVolume->volumeName());

  return topVolume;
}

/** private helper method to estimate and check the dimensions of a tracking volume */
StatusCode Trk::CylinderVolumeCreator::estimateAndCheckDimension(
                                                 const std::vector<const Trk::Layer*>& layers,
                                                 Trk::CylinderVolumeBounds*& cylinderVolumeBounds,
                                                 Amg::Transform3D*& transform,
                                                 std::vector<Trk::CylinderLayer*>& cylinderLayers,
                                                 std::vector<Trk::DiscLayer*>& discLayers,
                                                 double& rMinClean, double& rMaxClean,
                                                 double& zMinClean, double& zMaxClean,
                                                 Trk::BinningType bType) const
{
    // check and bail out if no layers are given
    if (layers.empty()) {
        ATH_MSG_VERBOSE( "No layers given, you shouldn't use : "<< type() );
        return StatusCode::FAILURE;
    }

    // some verbose output
    ATH_MSG_VERBOSE( "Parsing the " << layers.size() << " layers to gather overall dimensions" );
    if (cylinderVolumeBounds) ATH_MSG_VERBOSE( "Cylinder volume bounds are given." );

    // prepare for parsing the layers
    double layerRmin = 10e10;
    double layerRmax = 0.;
    double layerZmin = 10e10;
    double layerZmax = -10e10;
    bool   radial    = false;

    rMinClean = 10e10;
    rMaxClean = 0.;
    zMinClean = 10e10;
    zMaxClean = -10e10;

    // find out what is there
    for (const auto & layerIter : layers) {
        //class is not thread safe due to this
      Trk::Layer* mutableLayer = const_cast<Trk::Layer*>(layerIter);
        // initialize
        double currentRmin = 0.;
        double currentRmax = 0.;
        double currentZmin = 0.;
        double currentZmax = 0.;
        // dynamic cast the bounds either to CylinderBounds or DiscBounds
        const Trk::CylinderBounds* cylBounds =
                dynamic_cast<const Trk::CylinderBounds*>(&(layerIter->surfaceRepresentation()).bounds());
        // cylinder bounds
        if (cylBounds) {
            radial = true;
            // fill it into the cylinderLayer vector
            cylinderLayers.push_back(dynamic_cast<Trk::CylinderLayer*>(mutableLayer));
            // get the raw data
            double currentR   = cylBounds->r();
            double centerZ     = (layerIter->surfaceRepresentation()).center().z();
            // check for min/max in the cylinder bounds case
            if (bType == Trk::biequidistant){
              currentRmin = currentR; currentRmax = currentR;    
            } else {
              currentRmin = currentR-(0.5*(layerIter)->thickness());
              currentRmax = currentR+(0.5*(layerIter)->thickness());
            }
            currentZmin = centerZ - cylBounds->halflengthZ();
            currentZmax = centerZ + cylBounds->halflengthZ();
        }
        // dynamic cast to the DiscBounds
        const Trk::DiscBounds* discBounds =
                dynamic_cast<const Trk::DiscBounds*>(&(layerIter->surfaceRepresentation()).bounds());
        if (discBounds) {
            // fill it into the discLayer vector
            discLayers.push_back(dynamic_cast<Trk::DiscLayer*>(mutableLayer));
            // check for min/max in the cylinder bounds case
            double centerZ     = (layerIter->surfaceRepresentation()).center().z();
            currentRmin = discBounds->rMin();
            currentRmax = discBounds->rMax();
            if (bType == Trk::biequidistant){
              currentZmin = centerZ; currentZmax = centerZ;
            } else {
              currentZmin = centerZ - (0.5*(layerIter)->thickness());
              currentZmax = centerZ + (0.5*(layerIter)->thickness());
            }
        }
        // the raw data
        takeSmaller(rMinClean,currentRmin); takeBigger(rMaxClean,currentRmax);
        takeSmaller(zMinClean,currentZmin); takeBigger(zMaxClean,currentZmax);
        // assign if they overrule the minima/maxima (with layers thicknesses)
        takeSmaller(layerRmin,currentRmin); takeBigger(layerRmax,currentRmax);
        takeSmaller(layerZmin,currentZmin); takeBigger(layerZmax,currentZmax);
    }

    // special for biequidistant binning - navigation layers are added before / after
    if (bType == Trk::biequidistant){
        if (radial){
            double rStepHalf = 0.5*(layerRmax-layerRmin)/(layers.size()-1);
            layerRmin -= rStepHalf;
            layerRmax += rStepHalf;
        } else {
            double zStepHalf = 0.5*(layerZmax-layerZmin)/(layers.size()-1);
            layerZmin -= zStepHalf;
            layerZmax += zStepHalf;
        }
    }

    ATH_MSG_VERBOSE( "Estimate/check CylinderVolumeBounds from/w.r.t. enclosed layers + envelope covers" );
    // the z from the layers w and w/o envelopes
    double zEstFromLayerEnv =  0.5*((layerZmax)+(layerZmin));
    double halflengthFromLayer = 0.5*fabs((layerZmax)-(layerZmin));

    bool concentric = (zEstFromLayerEnv*zEstFromLayerEnv < 0.001);

    // no CylinderBounds and Translation given - make it
    if (!cylinderVolumeBounds && !transform) {
        // create the CylinderBounds from parsed layer inputs
        cylinderVolumeBounds = new Trk::CylinderVolumeBounds(layerRmin,layerRmax,halflengthFromLayer);
        // and the transform
        transform = concentric ? new Amg::Transform3D : nullptr; 
        if (transform) 
           (*transform) = Amg::Translation3D(0.,0.,zEstFromLayerEnv);
    } else if (cylinderVolumeBounds && !transform &&!concentric){
        transform = new Amg::Transform3D; 
        (*transform) = Amg::Translation3D(0.,0.,zEstFromLayerEnv);
    }
    else if (transform && !cylinderVolumeBounds) {
        // create the CylinderBounds from parsed layer inputs
        double halflengthFromLayer = 0.5*fabs((layerZmax)-(layerZmin));
        cylinderVolumeBounds = new Trk::CylinderVolumeBounds(layerRmin,
                                                             layerRmax,
                                                             halflengthFromLayer);
    }
      
    ATH_MSG_VERBOSE( "    -> dimensions from layers   (rMin/rMax/zMin/zMax) = "
            << layerRmin << " / " << layerRmax << " / " << layerZmin << " / " << layerZmax );
    double zFromTransform = transform ? transform->translation().z() : 0.;
    ATH_MSG_VERBOSE( "    -> while created bounds are (rMin/rMax/zMin/zMax) = "
            << cylinderVolumeBounds->innerRadius() << " / " << cylinderVolumeBounds->outerRadius() << " / "
            << zFromTransform-cylinderVolumeBounds->halflengthZ()  << " / " << zFromTransform+cylinderVolumeBounds->halflengthZ() );


    // both is NOW given --- check it -----------------------------
    if (cylinderVolumeBounds) {
        // only check
        if (zFromTransform-cylinderVolumeBounds->halflengthZ() <= layerZmin &&
            zFromTransform+cylinderVolumeBounds->halflengthZ() >= layerZmax &&
            cylinderVolumeBounds->innerRadius() <= layerRmin &&
            cylinderVolumeBounds->outerRadius() >= layerRmax)
            return StatusCode::SUCCESS;
        else {
            ATH_MSG_WARNING( "Provided layers are not contained by volume ! Bailing out. " );
            return StatusCode::FAILURE;
        }
        ATH_MSG_VERBOSE( "Created/Checked " << *cylinderVolumeBounds );
    }

    
    return StatusCode::SUCCESS;
}
                                           

StatusCode Trk::CylinderVolumeCreator::interGlueTrackingVolume(Trk::TrackingVolume& tVolume,
                                                               bool rBinned,
                                                               bool createBoundaryLayers,
                                                               bool replaceBoundaryFace) const
{

    ATH_MSG_VERBOSE( "Glue contained TrackingVolumes of container '" << tVolume.volumeName() << "'." );

    // get the glueVolumes descriptor of the top volume to register the outside volumes
    Trk::GlueVolumesDescriptor& glueDescr  = tVolume.glueVolumesDescriptor();
    
    // so far we know that we can do that (private method)
    BinnedArraySpan<Trk::TrackingVolume const * const> volumes = tVolume.confinedVolumes()->arrayObjects();

    // the needed iterators
    auto tVolIter = volumes.begin();
    auto tVolFirst = volumes.begin();
    auto tVolLast = volumes.end(); --tVolLast;
    auto tVolEnd  = volumes.end();

    // the glue volumes for the description
    std::vector<const Trk::TrackingVolume*> glueVolumesInnerTube;
    std::vector<const Trk::TrackingVolume*> glueVolumesOuterTube;
    std::vector<const Trk::TrackingVolume*> glueVolumesNegativeFace;
    std::vector<const Trk::TrackingVolume*> glueVolumesPositiveFace;

    // volumes of increasing r
    if (rBinned) {
        // loop over the volumes -------------------------------
        for ( ; tVolIter != tVolEnd; ) {
            // screen output
            ATH_MSG_VERBOSE("r-binning: Processing volume '" << (*tVolIter)->volumeName() << "'.");
            // for the first one
            if (tVolIter == tVolFirst)
                addFaceVolumes((**tVolIter),Trk::tubeInnerCover,glueVolumesInnerTube);
            // add this or the subvolumes to the negativeFace and positiveFace
            addFaceVolumes((**tVolIter),Trk::negativeFaceXY,glueVolumesNegativeFace);
            addFaceVolumes((**tVolIter),Trk::positiveFaceXY,glueVolumesPositiveFace);
            if (tVolIter == tVolLast) {
                addFaceVolumes((**tVolIter),Trk::tubeOuterCover,glueVolumesOuterTube);
                ++tVolIter;
            } else {
                const Trk::TrackingVolume* tVol1 = (*tVolIter);
                const Trk::TrackingVolume* tVol2 = (*(++tVolIter));
                glueTrackingVolumes(*tVol1,Trk::tubeOuterCover, *tVol2, Trk::tubeInnerCover, createBoundaryLayers, replaceBoundaryFace);
            }
        }
    } else {
        // volumes in increasing z
        // loop over the volumes
        for ( ; tVolIter != tVolEnd; ) {
            // screen output
            ATH_MSG_VERBOSE("z-binning: Processing volume '" << (*tVolIter)->volumeName() << "'.");
            if (tVolIter == tVolFirst)
                addFaceVolumes((**tVolIter),Trk::negativeFaceXY,glueVolumesNegativeFace);
            addFaceVolumes((**tVolIter),Trk::tubeInnerCover,glueVolumesInnerTube);
            addFaceVolumes((**tVolIter),Trk::tubeOuterCover,glueVolumesOuterTube);
            if (tVolIter == tVolLast) {
                addFaceVolumes((**tVolIter),Trk::positiveFaceXY,glueVolumesPositiveFace);
                ++tVolIter;
            } else {
                const Trk::TrackingVolume* tVol1 = (*tVolIter);
                const Trk::TrackingVolume* tVol2 = (*(++tVolIter));
                glueTrackingVolumes(*tVol1,Trk::positiveFaceXY,*tVol2,Trk::negativeFaceXY, createBoundaryLayers, replaceBoundaryFace);
            }
        }
    }

    // register it with the glueVolumeDescriptor
    glueDescr.registerGlueVolumes(Trk::negativeFaceXY,glueVolumesNegativeFace);
    glueDescr.registerGlueVolumes(Trk::positiveFaceXY,glueVolumesPositiveFace);
    glueDescr.registerGlueVolumes(Trk::tubeInnerCover,glueVolumesInnerTube);
    glueDescr.registerGlueVolumes(Trk::tubeOuterCover,glueVolumesOuterTube);

    // return success
    return StatusCode::SUCCESS;
}


/** Private method - helper method not to duplicate code */
void Trk::CylinderVolumeCreator::addFaceVolumes(const Trk::TrackingVolume& tvol,
                                                Trk::BoundarySurfaceFace glueFace,
                                                std::vector<const Trk::TrackingVolume*>& vols) const
{

    ATH_MSG_VERBOSE( "Adding face volumes of face " << glueFace << " for the volume '" << tvol.volumeName() << "'." );
    // retrieve the gluevolume descriptor
    const Trk::GlueVolumesDescriptor& gvDescriptor = tvol.glueVolumesDescriptor();
    // if volumes are registered: take them
    if (!gvDescriptor.glueVolumes(glueFace).empty()) {
        // get the navigation level subvolumes
        std::vector<const Trk::TrackingVolume*>::const_iterator volIter = gvDescriptor.glueVolumes(glueFace).begin();
        std::vector<const Trk::TrackingVolume*>::const_iterator volEnd  = gvDescriptor.glueVolumes(glueFace).end();
        for ( ; volIter != volEnd; ++volIter){
            ATH_MSG_VERBOSE( "     -> adding volumes : " << (*volIter)->volumeName() );
            vols.push_back(*volIter);
        }
        // screen output
        ATH_MSG_VERBOSE( vols.size() << " navigation volumes registered as glue volumes." );
    } else {
        // the volume itself is on navigation level
        ATH_MSG_VERBOSE( "Volume is on navigation level." );
        vols.push_back(&tvol);
    }
}


/** private helper method to fill the glue volumes (or the volume itself in) */
void Trk::CylinderVolumeCreator::glueTrackingVolumes(const Trk::TrackingVolume& tvolOne,
                                                     Trk::BoundarySurfaceFace faceOne,
                                                     const Trk::TrackingVolume& tvolTwo,
                                                     Trk::BoundarySurfaceFace faceTwo,
                                                     bool createBoundaryLayers,
                                                     bool replaceBoundaryFace) const
{

    // get the two gluevolume descriptors
    const Trk::GlueVolumesDescriptor& gvDescriptorOne = tvolOne.glueVolumesDescriptor();
    const Trk::GlueVolumesDescriptor& gvDescriptorTwo = tvolTwo.glueVolumesDescriptor();

    ATH_MSG_VERBOSE( "Glue method called with " << (replaceBoundaryFace ? "joint boundaries." : "individual boundaries." ) ); 

    size_t volOneGlueVols = gvDescriptorOne.glueVolumes(faceOne).size();
    ATH_MSG_VERBOSE( "GlueVolumeDescriptor of volume '" << tvolOne.volumeName() <<"' has "
            << volOneGlueVols << " @ " << faceOne );
    size_t volTwoGlueVols = gvDescriptorTwo.glueVolumes(faceTwo).size();
    ATH_MSG_VERBOSE( "GlueVolumeDescriptor of volume '" << tvolTwo.volumeName() <<"' has "
            << volTwoGlueVols << " @ " << faceTwo );

    // they could still be a container though
    const TrackingVolume* glueVolOne = volOneGlueVols ?
            gvDescriptorOne.glueVolumes(faceOne)[0] : &tvolOne;

    const TrackingVolume* glueVolTwo = volTwoGlueVols ?
            gvDescriptorTwo.glueVolumes(faceTwo)[0] : &tvolTwo;

    // check the cases
    // (i) easy volume to volume
    if ( volOneGlueVols <= 1 && volTwoGlueVols <= 1) {
        // now glue it
        ATH_MSG_VERBOSE( "      glue : one[ "<< glueVolOne->volumeName() << " @ " << faceOne
                << " ]-to-one[ "<< glueVolTwo->volumeName() << " @ " << faceTwo << " ]" );
        m_trackingVolumeHelper->glueTrackingVolumes(*glueVolOne,
                                                    faceOne,
                                                    *glueVolTwo,
                                                    faceTwo,
                                                    createBoundaryLayers);
    } else if (volOneGlueVols <= 1) { // (ii) one -> many
        ATH_MSG_VERBOSE( "      glue : one[ "<< glueVolOne->volumeName() << " @ " << faceOne
                << " ]-to-many[ "<< tvolTwo.volumeName() << " @ " << faceTwo << " ]" );
        m_trackingVolumeHelper->glueTrackingVolumes(*glueVolOne,
                                                    faceOne,
                                                    gvDescriptorTwo.glueVolumes(faceTwo),
                                                    faceTwo,
                                                    createBoundaryLayers,
                                                    replaceBoundaryFace);
    } else if (volTwoGlueVols <= 1 ) { // (iii) many -> two
        ATH_MSG_VERBOSE( "      glue : many[ "<< tvolOne.volumeName() << " @ " << faceOne
                << " ]-to-one[ "<< glueVolTwo->volumeName() << " @ " << faceTwo << " ]" );
        m_trackingVolumeHelper->glueTrackingVolumes(*glueVolTwo,
                                                    faceTwo,
                                                    gvDescriptorOne.glueVolumes(faceOne),
                                                    faceOne,
                                                    createBoundaryLayers,
                                                    replaceBoundaryFace);
    } else {
        // (iv) glue array to array
        ATH_MSG_VERBOSE( "      glue : many[ "<< tvolOne.volumeName() << " @ " << faceOne
                << " ]-to-many[ "<< tvolTwo.volumeName() << " @ " << faceTwo << " ]" );        
        m_trackingVolumeHelper->glueTrackingVolumes(gvDescriptorOne.glueVolumes(faceOne),
                                                    faceOne,
                                                    gvDescriptorTwo.glueVolumes(faceTwo),
                                                    faceTwo,
                                                    createBoundaryLayers,
                                                    replaceBoundaryFace);
    } // end of case (iv)
}
  
Trk::CylinderLayer* Trk::CylinderVolumeCreator::createCylinderLayer(double z,
                                                                    double r,
                                                                    double halflengthZ,
                                                                    double thickness,
                                                                    int binsPhi,
                                                                    int binsZ) const
{
    ATH_MSG_VERBOSE( "Creating a CylinderLayer at position " << z << " and radius " << r );
    // prepare the material
    Trk::LayerMaterialProperties* cylinderMaterial = nullptr;
    // positioning
    std::unique_ptr<Amg::Transform3D> transform =
      (fabs(z) > 0.1) ? std::make_unique<Amg::Transform3D>() : nullptr;
    if (transform){
      (*transform) = Amg::Translation3D(0., 0., z);
    }
   
    // z-binning
    Trk::BinUtility layerBinUtility(binsZ,z-halflengthZ,z+halflengthZ,Trk::open,Trk::binZ);
    if (binsPhi==1){
        // the BinUtility for the material
        // ---------------------> create the layer material
        cylinderMaterial = new Trk::BinnedLayerMaterial(layerBinUtility);
        ATH_MSG_VERBOSE( " -> Preparing the binned material with "
                << binsZ << " bins in Z. ");

    } else { // break the phi symmetry
        // update the BinUtility: local position on Cylinder is rPhi, z
        Trk::BinUtility layerBinUtilityRPhiZ(binsPhi,-r*M_PI,+r*M_PI,Trk::closed,Trk::binRPhi);
                               layerBinUtilityRPhiZ += layerBinUtility;
        // ---------------------> create the layer material
        cylinderMaterial = new Trk::BinnedLayerMaterial(layerBinUtilityRPhiZ);

        ATH_MSG_VERBOSE( " -> Preparing the binned material with "
                << binsPhi << " / " <<  binsZ << " bins in R*phi / Z. ");
    }
    // bounds
    Trk::CylinderBounds* cylinderBounds = new Trk::CylinderBounds(r,halflengthZ);
    // create the cylinder
    Trk::CylinderLayer* cylinderLayer = transform ? new Trk::CylinderLayer(*transform,
                                                                           cylinderBounds,
                                                                           *cylinderMaterial,
                                                                           thickness,
                                                                           nullptr, int(Trk::passive)) :
                                                    new Trk::CylinderLayer(cylinderBounds,
                                                                          *cylinderMaterial,
                                                                          thickness,
                                                                          nullptr, int(Trk::passive))  ;
    // delete the material
    delete cylinderMaterial;
    // and return it
    return cylinderLayer;
}


Trk::DiscLayer* Trk::CylinderVolumeCreator::createDiscLayer(double z,
                                                            double rMin,
                                                            double rMax,
                                                            double thickness,
                                                            int binsPhi,
                                                            int binsR) const
{

    ATH_MSG_VERBOSE( "Creating a DiscLayer at position " << z << " and rMin/rMax " << rMin << " / " << rMax);

    // positioning
    std::unique_ptr<Amg::Transform3D> transform =
      fabs(z) > 0.1 ? std::make_unique<Amg::Transform3D>() : nullptr;
    if (transform)
      (*transform) = Amg::Translation3D(0.,0.,z);
    Trk::BinnedLayerMaterial* discMaterial = nullptr;

    // R is the primary binning for the material
    Trk::BinUtility layerBinUtility(binsR, rMin, rMax, Trk::open, Trk::binR);
    if (binsPhi==1) {
        ATH_MSG_VERBOSE( " -> Preparing the binned material with "
                << binsR << " bins in R. ");
    } else {
        // also binning in phi chosen
        layerBinUtility += Trk::BinUtility(binsPhi, -M_PI, M_PI, Trk::closed, Trk::binPhi);
        ATH_MSG_VERBOSE( " -> Preparing the binned material with "
            << binsPhi << " / " <<  binsR << " bins in phi / R. ");
    }
    // ---------------------> create the layer material
    discMaterial = new Trk::BinnedLayerMaterial(layerBinUtility);
    // bounds
    Trk::DiscBounds* discBounds = new Trk::DiscBounds(rMin,rMax);
    // create the disc
    Trk::DiscLayer* discLayer = new Trk::DiscLayer(*transform,
                                                   discBounds,
                                                   *discMaterial,
                                                   thickness,
                                                   nullptr, int(Trk::passive));
    // delete the material
    delete discMaterial;
    // and return it
    return discLayer;
}                                                            

