/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////////
// HGTD_TrackingGeometryBuilder.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// HGTDet
#include "HGTD_TrackingGeometry/HGTD_TrackingGeometryBuilder.h"
// Trk interfaces
#include "TrkDetDescrInterfaces/ILayerBuilder.h"
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
//Gaudi
#include "GaudiKernel/SystemOfUnits.h"
#include "GaudiKernel/MsgStream.h"
#include <boost/lexical_cast.hpp>
#include <algorithm>

// constructor
HGTDet::HGTD_TrackingGeometryBuilder::HGTD_TrackingGeometryBuilder(const std::string& t, const std::string& n, const IInterface* p) :
  AthAlgTool(t,n,p),
  m_enclosingEnvelopeSvc("AtlasGeometry_EnvelopeDefSvc", n),
  m_trackingVolumeCreator("Trk::CylinderVolumeCreator/CylinderVolumeCreator"),
  m_indexStaticLayers(true),
  m_buildBoundaryLayers(true),
  m_replaceJointBoundaries(true),
  m_layerBinningType(2),
  m_colorCodeConfig(3)
{
  declareInterface<Trk::IGeometryBuilder>(this);
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
HGTDet::HGTD_TrackingGeometryBuilder::~HGTD_TrackingGeometryBuilder()
{
}

// Athena standard methods
// initialize
StatusCode HGTDet::HGTD_TrackingGeometryBuilder::initialize()
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
StatusCode HGTDet::HGTD_TrackingGeometryBuilder::finalize()
{
  ATH_MSG_INFO( "finalize() successful" );
  return StatusCode::SUCCESS;
}

const Trk::TrackingGeometry* HGTDet::HGTD_TrackingGeometryBuilder::trackingGeometry(const Trk::TrackingVolume* innerVol) const
{

  ATH_MSG_VERBOSE( "Starting to build HGTD_TrackingGeometry ..." );   
  
  // the return TG
  const Trk::TrackingGeometry* hgtdTrackingGeometry = 0; 
  
  // the enclosed input volume (ID)
  double enclosedInnerSectorHalflength = std::numeric_limits<float>::max();
  double enclosedOuterRadius = 0.;
  double enclosedInnerRadius = 0.;
  
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
  for (auto& bounds : m_enclosingEnvelopeSvc->getCaloRZValues(0)) { 
    if (std::abs(bounds.second) < enclosedOuterSectorHalflength) {
      enclosedOuterSectorHalflength = std::abs(bounds.second);
    }
  }
  
  // in case you have no inner volume you need to find the 
  // envelope extensions --> beampipe and HGTD
  if (not innerVol) {
    // from the beampipe envelope you get the inner z extension 
    for (auto& bounds : m_enclosingEnvelopeSvc->getBeamPipeRZValues(0)) { 
      if (std::abs(bounds.second) < enclosedInnerSectorHalflength) {
        enclosedInnerSectorHalflength = std::abs(bounds.second);
      }
    }   
    // from the calo envelope you get the outer radius
    for (auto& bounds : m_enclosingEnvelopeSvc->getCaloRZValues(0)) { 
      if (std::abs(bounds.second) == enclosedOuterSectorHalflength) {
        if (bounds.first>enclosedOuterRadius)
          enclosedOuterRadius=bounds.first;
      }
    }
  }
  
  ATH_MSG_VERBOSE( "Got Dimensions Zmin/Rmin - Zmax/Rmax: " << enclosedInnerSectorHalflength << "/" << enclosedInnerRadius << " - " << enclosedOuterSectorHalflength << "/" << enclosedOuterRadius );   
  
  // prepare the layers
  std::vector<const Trk::Layer*> negativeLayers;
  std::vector<const Trk::Layer*> positiveLayers;  
  
  const std::vector<const Trk::DiscLayer*>* discLayers = m_layerBuilder->discLayers();
  
  DiscLayerInfo layerInfo;
  
  // loop and fill positive and negative Layers
  if (discLayers) {
    // loop over and push into the return/cache vector 
    for (auto& discLayer : (*discLayers) ){
      // get the center posituion 
      float zpos = discLayer->surfaceRepresentation().center().z();
      const Trk::DiscBounds* dBounds = dynamic_cast<const Trk::DiscBounds*>(&(discLayer->surfaceRepresentation().bounds()));
      // get an estimate of the maximal extension in R
      if (zpos > 0) {
        positiveLayers.push_back(discLayer);    
        // only saving layer info for positive side
        // as the detector is simmetric
        takeBigger(layerInfo.maxZ, zpos);
        takeSmaller(layerInfo.minZ, zpos);
        takeBigger(layerInfo.maxR, dBounds->rMax());
        takeSmaller(layerInfo.minR, dBounds->rMin());
        takeBigger(layerInfo.thickness, discLayer->thickness());
      }
      else {
        negativeLayers.push_back(discLayer);
      }
    }
  }
  
  // memory cleanup
  delete discLayers;
  
  float envelope = layerInfo.thickness*0.5;
  float minZ_HGTD = layerInfo.minZ-envelope;
  float maxZ_HGTD = enclosedOuterSectorHalflength;
  
  // dummy material property 
  Trk::Material* materialProperties = new Trk::Material;
  
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
    const Trk::LayerArray* dummyLayers = 0;
    const Trk::TrackingVolumeArray* dummyVolumes = 0;
    
    innerVol = new Trk::TrackingVolume(idTr, idBounds, *materialProperties,
                                       dummyLayers, dummyVolumes,
                                       "HGTD::GapVolumes::DummyID");
  }
  
  std::vector<const Trk::TrackingVolume*> inBufferVolumes;
  inBufferVolumes.push_back(negativeInnerGapVolume);  
  inBufferVolumes.push_back(innerVol);     
  inBufferVolumes.push_back(positiveInnerGapVolume);     
   
  const Trk::TrackingVolume* inDetEnclosed = 
           m_trackingVolumeCreator->createContainerTrackingVolume(inBufferVolumes,
                                                                  *materialProperties,
                                                                  "HGTD::Container::EnclosedInnerDetector");
    
  
  // create the tracking volumes 
  // create the three volumes
  const Trk::TrackingVolume* negativeVolume = m_trackingVolumeCreator->createTrackingVolume(negativeLayers,
                                                                                     *materialProperties,
                                                                                     enclosedInnerRadius,enclosedOuterRadius,
                                                                                     -maxZ_HGTD, -minZ_HGTD,
                                                                                     m_layerBuilder->identification()+"::NegativeEndcap",
                                                                                     (Trk::BinningType)m_layerBinningType);
  
  
  const Trk::TrackingVolume* positiveVolume = m_trackingVolumeCreator->createTrackingVolume(positiveLayers,
                                                                                     *materialProperties,
                                                                                     enclosedInnerRadius,enclosedOuterRadius,
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
                                                                "HGTD::Detectors::"+m_layerBuilder->identification(),
                                                                m_buildBoundaryLayers,
                                                                m_replaceJointBoundaries);
                                                                
   ATH_MSG_VERBOSE( '\t' << '\t'<< "Created container volume with bounds: " << tripleContainer->volumeBounds() );
   
   hgtdTrackingGeometry = new Trk::TrackingGeometry(tripleContainer);
   
   if (m_indexStaticLayers and hgtdTrackingGeometry)
    hgtdTrackingGeometry->indexStaticLayers( Trk::Global );   

  return hgtdTrackingGeometry; 
}
