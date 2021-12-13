/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////////
// HGTD_TrackingGeometryBuilderCond.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// HGTD
#include "HGTD_TrackingGeometry/HGTD_TrackingGeometryBuilderCond.h"
// EnvelopeDefinitionService
#include "SubDetectorEnvelopes/IEnvelopeDefSvc.h"
// Trk interfaces
#include "TrkDetDescrInterfaces/ILayerBuilderCond.h"
#include "TrkDetDescrInterfaces/ITrackingVolumeCreator.h"
#include "TrkDetDescrInterfaces/ILayerArrayCreator.h"
#include "TrkDetDescrUtils/BinnedArray.h"
#include "TrkDetDescrUtils/BinnedArray1D1D.h"
#include "TrkDetDescrUtils/SharedObject.h"
#include "TrkDetDescrUtils/BinUtility.h"
#include "TrkDetDescrUtils/GeometryStatics.h"
#include "TrkGeometry/TrackingGeometry.h"
#include "TrkGeometry/TrackingVolume.h"
#include "TrkGeometry/GlueVolumesDescriptor.h"
#include "TrkGeometry/Material.h"
#include "TrkGeometry/BinnedMaterial.h"
#include "TrkGeometry/MaterialProperties.h"
#include "TrkGeometry/DiscLayer.h"
#include "TrkGeometry/HomogeneousLayerMaterial.h"
#include "TrkGeometry/AlignableTrackingVolume.h"
#include "TrkVolumes/VolumeBounds.h"
#include "TrkVolumes/CylinderVolumeBounds.h"
#include "TrkSurfaces/DiscBounds.h"
#include "TrkSurfaces/DiscSurface.h"
// Athena
#include "AthenaKernel/IOVInfiniteRange.h"
#include "CxxUtils/checker_macros.h"
//Gaudi
#include "GaudiKernel/SystemOfUnits.h"
#include "GaudiKernel/MsgStream.h"
#include <boost/lexical_cast.hpp>
#include <algorithm>

// constructor
HGTD_TrackingGeometryBuilderCond::HGTD_TrackingGeometryBuilderCond(const std::string& t, const std::string& n, const IInterface* p) :
  AthAlgTool(t,n,p),
  m_enclosingEnvelopeSvc("AtlasEnvelopeDefSvc", n),
  m_trackingVolumeCreator("Trk::CylinderVolumeCreator/CylinderVolumeCreator"),
  m_indexStaticLayers(true),
  m_buildBoundaryLayers(true),
  m_replaceJointBoundaries(true),
  m_layerBinningType(2),
  m_colorCodeConfig(3)
{
  declareInterface<Trk::IGeometryBuilderCond>(this);
  // envelope definition service
  declareProperty("EnvelopeDefinitionSvc",            m_enclosingEnvelopeSvc );
  declareProperty("LayerBuilder",                     m_layerBuilder);
  declareProperty("TrackingVolumeCreator",            m_trackingVolumeCreator);
  
  declareProperty("IndexStaticLayers",                m_indexStaticLayers);
  declareProperty("BuildBoundaryLayers",              m_buildBoundaryLayers);
  declareProperty("ReplaceAllJointBoundaries",        m_replaceJointBoundaries);
  declareProperty("LayerBinningType",                 m_layerBinningType);
  declareProperty("ColorCode",                        m_colorCodeConfig);  

}

// destructor
HGTD_TrackingGeometryBuilderCond::~HGTD_TrackingGeometryBuilderCond()
{
}

// Athena standard methods
// initialize
StatusCode HGTD_TrackingGeometryBuilderCond::initialize()
{
  // retrieve envelope definition service 
  ATH_CHECK(m_enclosingEnvelopeSvc.retrieve());
  
  // retrieve the layer provider        
  ATH_CHECK(m_layerBuilder.retrieve());
  
  // retrieve the volume creator 
  ATH_CHECK(m_trackingVolumeCreator.retrieve());
    
  ATH_MSG_INFO( "initialize() succesful" );    
  return StatusCode::SUCCESS;
}

// finalize
StatusCode HGTD_TrackingGeometryBuilderCond::finalize()
{
  ATH_MSG_INFO( "finalize() successful" );
  return StatusCode::SUCCESS;
}

std::pair<EventIDRange, Trk::TrackingGeometry*> HGTD_TrackingGeometryBuilderCond::trackingGeometry
  ATLAS_NOT_THREAD_SAFE // Thread unsafe TrackingGeometry::indexStaticLayers method is used.
  (const EventContext& ctx, std::pair<EventIDRange, const Trk::TrackingVolume*> tVolPair) const

{

  ATH_MSG_VERBOSE( "Starting to build HGTD_TrackingGeometry ..." );   
  
  // the return TG
  Trk::TrackingGeometry* hgtdTrackingGeometry = nullptr; 
  
  // the enclosed input volume (ID)
  double enclosedInnerSectorHalflength = std::numeric_limits<float>::max();
  double enclosedOuterRadius = 0.;
  double enclosedInnerRadius = 0.;
  
  //Start with a range covering 0 - inf, then narrow down
  EventIDRange range = IOVInfiniteRange::infiniteMixed();
  
  const Trk::TrackingVolume* innerVol = nullptr;
  if(tVolPair.second != nullptr){
    range = tVolPair.first;
    innerVol = tVolPair.second;
  }
  
  if (innerVol) {  
    ATH_MSG_VERBOSE( "Got Inner Detector Volume: " << innerVol->volumeName() ); 
    innerVol->screenDump(msg(MSG::VERBOSE)); 

    // retrieve dimensions
    const Trk::CylinderVolumeBounds* innerDetectorBounds 
      = dynamic_cast<const Trk::CylinderVolumeBounds*>(&(innerVol->volumeBounds()));
    if (!innerDetectorBounds) std::abort();
    
    enclosedInnerSectorHalflength = innerDetectorBounds->halflengthZ();
    enclosedOuterRadius = innerDetectorBounds->outerRadius();
    enclosedInnerRadius = innerDetectorBounds->innerRadius();
  }  
  
  float enclosedOuterSectorHalflength = std::numeric_limits<float>::max();
  // for the HGTD we only need the first envelope definition
  for (const auto & bounds : m_enclosingEnvelopeSvc->getCaloRZBoundary()) { 
    if (std::abs(bounds.second) < enclosedOuterSectorHalflength) {
      enclosedOuterSectorHalflength = std::abs(bounds.second);
    }
  }
  
  // in case you have no inner volume you need to find the 
  // envelope extensions --> beampipe and HGTD
  if (not innerVol) {
    // from the beampipe envelope you get the inner z extension 
    for (const auto & bounds : m_enclosingEnvelopeSvc->getBeamPipeRZBoundary()) { 
      if (std::abs(bounds.second) < enclosedInnerSectorHalflength) {
        enclosedInnerSectorHalflength = std::abs(bounds.second);
      }
    }   
    // from the calo envelope you get the outer radius
    for (const auto & bounds : m_enclosingEnvelopeSvc->getCaloRZBoundary()) { 
      if (std::abs(bounds.second) == enclosedOuterSectorHalflength) {
        if (bounds.first>enclosedOuterRadius)
          enclosedOuterRadius=bounds.first;
      }
    }
  }

  ATH_MSG_VERBOSE("Got Dimensions Zmin/Rmin - Zmax/Rmax: "
                  << enclosedInnerSectorHalflength << "/" << enclosedInnerRadius
                  << " - " << enclosedOuterSectorHalflength << "/"
                  << enclosedOuterRadius);

  // prepare the layers
  std::vector<const Trk::Layer*> negativeLayers;
  std::vector<const Trk::Layer*> positiveLayers;  
  
  std::pair<EventIDRange, const std::vector<Trk::DiscLayer*>*> discLayersPair = m_layerBuilder->discLayers(ctx);
  const auto *discLayers = discLayersPair.second;
  
  float maxZ = -9999.;
  float minZ =  9999.;
  float thickness = -9999;
  
  // loop and fill positive and negative Layers
  if (discLayers && !discLayers->empty()){
    range=EventIDRange::intersect(range,discLayersPair.first);
    // loop over and push into the return/cache vector 
    for (const auto & discLayer : (*discLayers) ){
      // get the center posituion 
      float zpos = discLayer->surfaceRepresentation().center().z();
      if (zpos > 0) {
        positiveLayers.push_back(discLayer);    
        // only saving layer info for positive side
        // as the detector is simmetric
        maxZ = std::max(maxZ, zpos);
        minZ = std::min(minZ, zpos);
        thickness = std::max(thickness, float(discLayer->thickness()));
      }
      else {
        negativeLayers.push_back(discLayer);
      }
    }
  }
  
  // memory cleanup
  delete discLayers;
  
  float envelope = thickness*0.5;
  float minZ_HGTD = minZ-envelope;
  float maxZ_HGTD = maxZ+envelope;
  float maxZ_HGTDEnclosure = enclosedOuterSectorHalflength;
  
  // dummy material property 
  auto materialProperties = std::make_unique<Trk::Material>();
  
  float zGapPos = 0.5*(minZ_HGTD+enclosedInnerSectorHalflength);
  float gapHalfLengthZ = 0.5*(minZ_HGTD-enclosedInnerSectorHalflength);
  
  // create the gap between the ID and the HGTD endcap volumes
  Amg::Transform3D* negativeInnerGapTrans = new Amg::Transform3D(Amg::Translation3D(Amg::Vector3D(0.,0.,-zGapPos)));
  Trk::CylinderVolumeBounds* negativeInnerGapBounds = new Trk::CylinderVolumeBounds(enclosedInnerRadius,enclosedOuterRadius,gapHalfLengthZ);
  
  const Trk::TrackingVolume * negativeInnerGapVolume = 
      new Trk::TrackingVolume(negativeInnerGapTrans,
                              negativeInnerGapBounds,
                              *materialProperties,
                              nullptr, nullptr,
                              m_layerBuilder->identification()+"::NegativeInnerGap");

  Amg::Transform3D* positiveInnerGapTrans = new Amg::Transform3D(Amg::Translation3D(Amg::Vector3D(0.,0.,zGapPos)));
  Trk::CylinderVolumeBounds* positiveInnerGapBounds = new Trk::CylinderVolumeBounds(enclosedInnerRadius,enclosedOuterRadius,gapHalfLengthZ);
  
  const Trk::TrackingVolume * positiveInnerGapVolume = 
       new Trk::TrackingVolume(positiveInnerGapTrans,
                               positiveInnerGapBounds,
                               *materialProperties,
                               nullptr, nullptr,
                               m_layerBuilder->identification()+"::PositiveInnerGap");
       
  // create dummy inner volume if not built already
  if (not innerVol) {
    Trk::CylinderVolumeBounds* idBounds = new Trk::CylinderVolumeBounds(enclosedInnerRadius,
                                                                        enclosedInnerSectorHalflength);
    Amg::Transform3D* idTr = new Amg::Transform3D(Trk::s_idTransform);
    // dummy objects
    const Trk::LayerArray* dummyLayers = nullptr;
    const Trk::TrackingVolumeArray* dummyVolumes = nullptr;
    
    innerVol = new Trk::TrackingVolume(idTr, idBounds, *materialProperties,
                                       dummyLayers, dummyVolumes,
                                       "HGTD::GapVolumes::DummyID");
  }
  
  std::vector<const Trk::TrackingVolume*> inBufferVolumes;
  inBufferVolumes.push_back(negativeInnerGapVolume);  
  inBufferVolumes.push_back(innerVol);     
  inBufferVolumes.push_back(positiveInnerGapVolume);     
   
  Trk::TrackingVolume* inDetEnclosed = 
    m_trackingVolumeCreator->createContainerTrackingVolume(inBufferVolumes,
                                                           *materialProperties,
                                                           "HGTD::Container::EnclosedInnerDetector");    
  
  // create the tracking volumes 
  // create the three volumes
  Trk::TrackingVolume* negativeVolume = 
    m_trackingVolumeCreator->createTrackingVolume(negativeLayers,
                                                  *materialProperties,
                                                  enclosedInnerRadius, enclosedOuterRadius,
                                                  -maxZ_HGTD, -minZ_HGTD,
                                                  m_layerBuilder->identification()+"::NegativeEndcap",
                                                  (Trk::BinningType)m_layerBinningType);
  
  
  Trk::TrackingVolume* positiveVolume = 
    m_trackingVolumeCreator->createTrackingVolume(positiveLayers,
                                                  *materialProperties,
                                                  enclosedInnerRadius, enclosedOuterRadius,
                                                  minZ_HGTD, maxZ_HGTD,
                                                  m_layerBuilder->identification()+"::PositiveEndcap",
                                                  (Trk::BinningType)m_layerBinningType);
  
  // the base volumes have been created
  ATH_MSG_VERBOSE('\t' << '\t'<< "Volumes have been created, now pack them into a triple.");
  negativeVolume->registerColorCode(m_colorCodeConfig);   
  inDetEnclosed->registerColorCode(m_colorCodeConfig);
  positiveVolume->registerColorCode(m_colorCodeConfig);
                                                        
  // pack them together
  std::vector<const Trk::TrackingVolume*> tripleVolumes;
  tripleVolumes.push_back(negativeVolume);
  tripleVolumes.push_back(inDetEnclosed);
  tripleVolumes.push_back(positiveVolume);
  
  // create the tiple container
  const Trk::TrackingVolume* tripleContainer =
    m_trackingVolumeCreator->createContainerTrackingVolume(tripleVolumes,
                                                           *materialProperties,
                                                           "HGTD::Containers::" + m_layerBuilder->identification(),
                                                           m_buildBoundaryLayers,
                                                           m_replaceJointBoundaries);

  ATH_MSG_VERBOSE( '\t' << '\t'<< "Created container volume with bounds: " << tripleContainer->volumeBounds() );

  // finally create the two endplates: negative
  const Trk::TrackingVolume* negativeEnclosure = m_trackingVolumeCreator->createGapTrackingVolume(
    *materialProperties,
    enclosedInnerRadius,
    enclosedOuterRadius,
    -maxZ_HGTDEnclosure,
    -maxZ_HGTD,
    1,
    false,
    "HGTD::Gaps::NegativeEnclosure" + m_layerBuilder->identification());

  // finally create the two endplates: positive
  const Trk::TrackingVolume* positiveEnclosure = m_trackingVolumeCreator->createGapTrackingVolume(
    *materialProperties,
    enclosedInnerRadius,
    enclosedOuterRadius,
    maxZ_HGTD,
    maxZ_HGTDEnclosure,
    1,
    false,
    "HGTD::Gaps::PositiveEnclosure" + m_layerBuilder->identification());
  // and the final tracking volume
  std::vector<const Trk::TrackingVolume*> enclosedVolumes;
  enclosedVolumes.push_back(negativeEnclosure);
  enclosedVolumes.push_back(tripleContainer);
  enclosedVolumes.push_back(positiveEnclosure);
  
   Trk::TrackingVolume* enclosedDetector = 
      m_trackingVolumeCreator->createContainerTrackingVolume(enclosedVolumes,     
                                                             *materialProperties,
                                                             "HGTD::Detectors::"+m_layerBuilder->identification(),
                                                             m_buildBoundaryLayers,
                                                             m_replaceJointBoundaries);
      
  ATH_MSG_VERBOSE( '\t' << '\t'<< "Created enclosed HGTD volume with bounds: " << enclosedDetector->volumeBounds() );

  //  create the TrackingGeometry ------------------------------------------------------  
  hgtdTrackingGeometry = new Trk::TrackingGeometry(enclosedDetector);
  
  if (m_indexStaticLayers and hgtdTrackingGeometry)
   hgtdTrackingGeometry->indexStaticLayers( geometrySignature() );   
  if (msgLvl(MSG::VERBOSE) && hgtdTrackingGeometry)
    hgtdTrackingGeometry->printVolumeHierarchy(msg(MSG::VERBOSE)); 

  return std::make_pair(range, hgtdTrackingGeometry);
  
}
