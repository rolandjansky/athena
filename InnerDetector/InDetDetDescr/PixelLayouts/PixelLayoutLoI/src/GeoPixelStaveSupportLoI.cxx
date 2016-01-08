/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


// Build detailed stave support : face plate + carbon foam + cable flex + cooling pipe + end blocks
// This is built one time per layer. 

#include "PixelLayoutLoI/GeoPixelStaveSupportLoI.h"
#include "PixelLayoutLoI/PixelLoIStaveXMLHelper.h"

#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoTube.h"
#include "GeoModelKernel/GeoSimplePolygonBrep.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "GeoModelKernel/GeoNameTag.h"

#include "InDetGeoModelUtils/InDetMaterialManager.h" 
 
#include "GeoModelKernel/GeoTransform.h"
#include "PathResolver/PathResolver.h"

#include <algorithm>
using std::max;

GeoPixelStaveSupportLoI::GeoPixelStaveSupportLoI(const PixelGeoBuilderBasics* basics, int iLayer,  const GeoDetModulePixel& barrelModule):
  PixelGeoBuilder(basics),								     
  m_layer(iLayer), m_barrelModule(barrelModule)
{
  preBuild();
}

GeoVPhysVol* GeoPixelStaveSupportLoI::Build() {
  return m_physVol;
}

void GeoPixelStaveSupportLoI::preBuild() {

  // Access XML file -> build a simple parallepipedic stave support
  PixelLoIStaveXMLHelper staveDBHelper(m_layer);

  msg(MSG::INFO) <<"Build detailed stave support description :  layout "<<m_layer<<endreq;

  double thickness = staveDBHelper.getStaveSupportThick();
  double width     = staveDBHelper.getStaveSupportWidth();
  double length    = staveDBHelper.getStaveSupportLength();
  std::string matName = staveDBHelper.getStaveSupportMaterial();
  double xOffset = staveDBHelper.getServiceOffsetX();
  double yOffset = staveDBHelper.getServiceOffsetY();
  if(width<0.01) width = m_barrelModule.Width();

  std::cout<<"Stave sizes LxWxT: "<<length<<"  "<<width<<"  "<<thickness<<"   "<<matName<<std::endl;

  GeoBox * shape = new GeoBox(0.5*thickness, 0.5*width, 0.5*length);
  const GeoMaterial* material = matMgr()->getMaterialForVolume(matName,shape->volume());
  GeoLogVol* logVol = new GeoLogVol("StaveSupport",shape,material);

  m_transform = HepGeom::Translate3D(xOffset,yOffset,0);

  m_thicknessP =  xOffset + 0.5*thickness;
  m_thicknessN =  -xOffset + 0.5*thickness;
  m_length = length;
  m_width = width;

  m_physVol = new GeoPhysVol(logVol);

}
