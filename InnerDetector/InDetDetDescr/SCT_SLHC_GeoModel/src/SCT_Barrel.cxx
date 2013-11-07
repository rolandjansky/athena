/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_SLHC_GeoModel/SCT_Barrel.h"
#include "SCT_SLHC_GeoModel/SCT_MaterialManager.h"
#include "SCT_SLHC_GeoModel/SCT_GeometryManager.h"
#include "SCT_SLHC_GeoModel/SCT_BarrelParameters.h"
#include "SCT_SLHC_GeoModel/SCT_Layer.h"
#include "SCT_SLHC_GeoModel/SCT_ServiceBuilder.h"

#include "InDetReadoutGeometry/SCT_DetectorManager.h"

#include "InDetGeoModelUtils/VolumeBuilder.h"

#include "GeoModelKernel/GeoTube.h"
#include "GeoModelKernel/GeoShape.h"
#include "GeoModelKernel/GeoShapeUnion.h"
#include "GeoModelKernel/GeoShapeShift.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelKernel/GeoNameTag.h"
#include "GeoModelKernel/GeoIdentifierTag.h"
#include "GeoModelKernel/GeoTransform.h"
#include "GeoModelKernel/GeoAlignableTransform.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "CLHEP/Units/SystemOfUnits.h"

namespace InDetDDSLHC {

SCT_Barrel::SCT_Barrel(const std::string & name)
  : SCT_UniqueComponentFactory(name),
    m_serviceBuilder(0)
{
  getParameters();
  m_logVolume = preBuild();
}


void SCT_Barrel::getParameters(){
  const SCT_BarrelParameters * parameters = geometryManager()->barrelParameters();
  m_innerRadius = parameters->barrelInnerRadius();
  m_intermediateRadius = parameters->barrelIntermediateRadius();
  m_outerRadius = parameters->barrelOuterRadius();
  m_length =      parameters->barrelLength();
  m_intermediateLength =  parameters->barrelIntermediateLength();
  m_numLayers =   parameters->numLayers(); 

  // Set numerology
  detectorManager()->numerology().setNumLayers(m_numLayers);

}

const GeoLogVol* SCT_Barrel::preBuild(){
  // Create the logical volume: a sphape + material
  SCT_MaterialManager * materials = geometryManager()->materialManager();
  const SCT_BarrelParameters * parameters = geometryManager()->barrelParameters();
  int layoutType = parameters->layoutType();
  GeoLogVol* barrelLog;
  if(layoutType == 1){//projective layout
    //enevelope for short layers
    const GeoTube* sct_barrel_1 = new GeoTube(m_innerRadius, m_outerRadius, 0.5*m_intermediateLength);
    //envelope for the extention from short to long layers
    const GeoTube* sct_barrel_2 = new GeoTube(m_intermediateRadius, m_outerRadius, 0.25*(m_length-m_intermediateLength));
    //make a single envelope for sct barrel
    const GeoShape & barrelEnvelopeShape = (*sct_barrel_1).
      add(*sct_barrel_2 << HepGeom::TranslateZ3D(0.25*(m_length+m_intermediateLength))).
      add(*sct_barrel_2 << HepGeom::TranslateZ3D(-0.25*(m_length+m_intermediateLength)));
    barrelLog = new GeoLogVol(getName(), &barrelEnvelopeShape, materials->gasMaterial());
  }else if(layoutType == 2){//same length barrels
    const GeoTube* barrelEnvelopeShape = new GeoTube(m_innerRadius, m_outerRadius, 0.5*m_length);
    barrelLog = new GeoLogVol(getName(), barrelEnvelopeShape, materials->gasMaterial());
  }else{
    std::cerr<<"--SCT_Barrel::preBuild(), ERROR: layoutType "<<layoutType<<" not known! exit athena."<<std::endl;
    exit(1);
  }
  
  return barrelLog;
}

GeoVPhysVol* SCT_Barrel::build(SCT_Identifier id) const{
  //crate the physical volume: a logical volume which can be transported and plcaed
  GeoFullPhysVol * barrel = new GeoFullPhysVol(m_logVolume);
  // Calculte the diffrence in lenght between the barrel and the layers: 
  //barrel_L = layer_L + shield + ineterlink
  //double barr_lay_diff = 2*m_thermalShieldEndWallThickness - 2*interLinkThickness;
  double barr_lay_diff = 0;
  //create the physical volumes of layers (layers may have different type of modules)
  for(int iLayer=0; iLayer<m_numLayers; iLayer++){
    SCT_Layer layer("Layer"+intToString(iLayer), iLayer, barr_lay_diff);
    barrel->add(new GeoNameTag("Layer#"+intToString(iLayer))); 
    barrel->add(new GeoIdentifierTag(iLayer)); // Identifier layer= iLayer
    id.setLayerDisk(iLayer); 
    GeoAlignableTransform* transform = new GeoAlignableTransform(HepGeom::Transform3D());
    barrel->add(transform);
    GeoVPhysVol* layerPV = layer.build(id);
    barrel->add(layerPV);
    // Store alignable transform
    detectorManager()->addAlignableTransform(2, id.getWaferId(), transform, layerPV);
  }

  // Add common/general services
  if (m_serviceBuilder && m_serviceBuilder->getServMatBuilder()) {
    m_serviceBuilder->getServMatBuilder()->buildAndPlace("B", barrel);
  }

  return barrel; 
}
}
