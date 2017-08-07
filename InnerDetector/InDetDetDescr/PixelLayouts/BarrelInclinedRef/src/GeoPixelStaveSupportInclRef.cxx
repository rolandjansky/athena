/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


// Build detailed stave support : face plate + carbon foam + cable flex + cooling pipe + end blocks
// This is built one time per layer. 

#include "BarrelInclinedRef/GeoPixelStaveSupportInclRef.h"
#include "BarrelInclinedRef/PixelInclRefStaveXMLHelper.h"

#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoTube.h"
#include "GeoModelKernel/GeoSimplePolygonBrep.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "GeoModelKernel/GeoNameTag.h"
#include "GeoModelKernel/GeoShape.h"
#include "GeoModelKernel/GeoShapeShift.h"
#include "GeoModelKernel/GeoShapeUnion.h"

#include "InDetGeoModelUtils/InDetMaterialManager.h" 
 
#include "GeoModelKernel/GeoTransform.h"
#include "PathResolver/PathResolver.h"

#include <algorithm>
using std::max;

GeoPixelStaveSupportInclRef::GeoPixelStaveSupportInclRef(const PixelGeoBuilderBasics* basics, int iLayer,  const GeoDetModulePixel& barrelModule,
							 double barrelTilt, double endcapTilt,
							 double gapPlanar, double endcapRmin, double endcapRmax, double zEndOfNBarrelModulePos):
  PixelGeoBuilder(basics),								     
  m_layer(iLayer), m_barrelModule(barrelModule),
  m_barrelTilt(barrelTilt),
  m_endcapTilt(endcapTilt),
  m_planarGap(gapPlanar),
  m_endcapMinRadialPos(endcapRmin),
  m_endcapMaxRadialPos(endcapRmax),
  m_barrelZMax(zEndOfNBarrelModulePos)
{
  preBuild();
}

GeoVPhysVol* GeoPixelStaveSupportInclRef::Build() {
  return m_physVol;
}

void GeoPixelStaveSupportInclRef::preBuild() {

  // Access XML file
  PixelInclRefStaveXMLHelper staveDBHelper(m_layer, getBasics());

  msg(MSG::DEBUG) <<"Build detailed stave support description :  layout "<<m_layer<<endreq;

  double thickness = staveDBHelper.getStaveSupportThick();
  double width     = staveDBHelper.getStaveSupportWidth();
  double length    = staveDBHelper.getStaveSupportLength();
  std::string matName = staveDBHelper.getStaveSupportMaterial();
  double xOffset = staveDBHelper.getServiceOffsetX();
  double yOffset = staveDBHelper.getServiceOffsetY();
  if(m_barrelTilt<0) yOffset=-yOffset;
  double ec_xOffset = staveDBHelper.getServiceECOffsetX();
  if(width<0.01) width = m_barrelModule.Width()*.7;
  m_svcRouting = staveDBHelper.getSvcRoutingPos();

  double endcapRadialPos=m_endcapMaxRadialPos;
  if(m_svcRouting=="inner")endcapRadialPos=m_endcapMinRadialPos;

  msg(MSG::DEBUG)<<"Stave sizes LxWxT: "<<length<<"  "<<width<<"  "<<thickness<<"   "<<matName<<endreq;
  msg(MSG::DEBUG)<<"   EC radial pos : "<<m_endcapMinRadialPos<<" "<<m_endcapMaxRadialPos<<" -> "<<endcapRadialPos<<"   "<<m_svcRouting<<endreq;
  msg(MSG::DEBUG)<<"   offsets       : " <<xOffset<<"/"<<ec_xOffset<<"   "<<yOffset<<endreq;

  // Simple parallelepipedic stave
  if(m_barrelZMax<0) {

    GeoBox * shape = new GeoBox(0.5*thickness, 0.5*width, 0.5*length);
    const GeoMaterial* material = matMgr()->getMaterialForVolume(matName,shape->volume());
    GeoLogVol* logVol = new GeoLogVol("StaveSupport",shape,material);
    
    m_thicknessP =  xOffset + 0.5*thickness;
    m_thicknessN =  -xOffset + 0.5*thickness;
    m_length = length;
    m_width = width;
    if(m_barrelModule.Width()>width) m_width = m_barrelModule.Width()+.01;
    
    m_thicknessP_barrel = m_thicknessP;
    m_thicknessP_endcap = m_thicknessP;
    m_thicknessN_endcap = m_thicknessN;

    if (m_svcRouting=="inner") xOffset = -xOffset;
    m_transform = HepGeom::Translate3D(xOffset,yOffset,0);
    
    msg(MSG::DEBUG)<<"Simple stave sizes LxWxT: "<<m_length<<"  "<<m_width<<"  "<<m_thicknessN+m_thicknessP<<"   "<<matName<<endreq;
    
    m_physVol = new GeoPhysVol(logVol);

    return;
  }

  // Complex shape
  
  const GeoShape * lastShape = 0;

  double xOffset_stave = xOffset;
  double yOffset_stave = yOffset;

  xOffset=0.;
  yOffset=0.;

  // central section
  double delta= m_planarGap;
  length =2.*(m_barrelZMax+delta);
  GeoBox* middle_box = new GeoBox(0.5*thickness, 0.5*width, 0.5*length);
  HepGeom::Transform3D middle_trf = HepGeom::Translate3D(xOffset,yOffset,0)*HepGeom::RotateZ3D(m_barrelTilt);
  lastShape = addShape(lastShape, middle_box, middle_trf); 
   
  // end of stave
  double loc_xOffset = xOffset+ec_xOffset+endcapRadialPos;
  length = staveDBHelper.getStaveSupportLength()*.5-(m_barrelZMax+delta);
  double zpos = m_barrelZMax+delta+length*.5;

  GeoBox* eos_box = new GeoBox(0.5*thickness, 0.5*width, 0.5*length);
  HepGeom::Transform3D eosA_trf = HepGeom::Translate3D(loc_xOffset,yOffset,zpos);
  lastShape = addShape(lastShape, eos_box, eosA_trf); 
  HepGeom::Transform3D eosC_trf = HepGeom::Translate3D(loc_xOffset,yOffset,-zpos);
  lastShape = addShape(lastShape, eos_box, eosC_trf); 

  loc_xOffset = xOffset+endcapRadialPos*.5;
  zpos = m_barrelZMax+delta+thickness*.5;

  //lateral junction  (between planar and end of stave)
  GeoBox* lat_box = new GeoBox(0.5*fabs(endcapRadialPos), 0.5*width, 0.5*thickness);
  // HepGeom::Transform3D latA_trf = HepGeom::Translate3D(loc_xOffset,yOffset,zpos)*HepGeom::RotateZ3D(m_barrelTilt);
  HepGeom::Transform3D latA_trf(HepGeom::Translate3D(loc_xOffset,yOffset,zpos));
  lastShape = addShape(lastShape, lat_box, latA_trf); 
  HepGeom::Transform3D latC_trf(HepGeom::Translate3D(loc_xOffset,yOffset,-zpos));
  lastShape = addShape(lastShape, lat_box, latC_trf);

  double shapeVolume = middle_box->volume() + 2.0*( eos_box->volume() + lat_box->volume());
  const GeoMaterial* material = matMgr()->getMaterialForVolume(matName, shapeVolume );
  GeoLogVol* logVol = new GeoLogVol("StaveSupport",lastShape,material);
  
  m_thicknessP =  xOffset_stave + 0.5*thickness;
  m_thicknessP_barrel = m_thicknessP;
  m_thicknessN =  -xOffset_stave + 0.5*thickness;
  m_length = staveDBHelper.getStaveSupportLength();
  m_width = width;
 
  if(endcapRadialPos>0) m_thicknessP+=(endcapRadialPos);
  m_thicknessP_endcap = m_thicknessP;
  if(endcapRadialPos<0) m_thicknessN+=(-endcapRadialPos);
  m_thicknessN_endcap = m_thicknessP_endcap-thickness;
    
  if (m_svcRouting=="inner") xOffset_stave = -xOffset_stave;
  m_transform = HepGeom::Translate3D(xOffset_stave,yOffset_stave,0);

  msg(MSG::DEBUG)<<"Complex stave sizes LxWxT: "<<m_length<<"  "<<m_width<<"  "<<m_thicknessN+m_thicknessP<<"   "<<matName<<endreq;
  
  m_physVol = new GeoPhysVol(logVol);
  
  return;

}

const GeoShape* GeoPixelStaveSupportInclRef::addShape(const GeoShape * lastShape, const GeoShape * nextShape, const HepGeom::Transform3D & trans)
{
  const GeoShape * shiftedShape = &(*nextShape << trans);
  if (lastShape) {
    lastShape = &(lastShape->add(*shiftedShape));
  } else {
    lastShape = shiftedShape;
  }
  return lastShape;
}
