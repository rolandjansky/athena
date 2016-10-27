/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "BarrelInclinedRef/GeoPixelEndcapModuleSvcRef.h"
#include "PixelGeoModelModule/GeoDetModulePixel.h"

#include "InDetGeoModelUtils/InDetMaterialManager.h" 
#include "BarrelInclinedRef/PixelInclRefStaveXMLHelper.h"

#include "GeoModelKernel/GeoShape.h"
#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoTube.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "GeoModelKernel/GeoSimplePolygonBrep.h"
#include "GeoModelKernel/GeoTransform.h"

							
GeoPixelEndcapModuleSvcRef::GeoPixelEndcapModuleSvcRef(const PixelGeoBuilderBasics* basics,
						       GeoPixelStaveSupportInclRef* staveSupport,
						   int iLayer, 
						   const GeoDetModulePixel* endcapModule, const GeoDetModulePixel* transModule, 
						   double endcapTilt, double transTilt)
						   
  : PixelGeoBuilder(basics), m_staveSupport(staveSupport),
    m_layer(iLayer), m_endcapModule(endcapModule), m_transModule(transModule), 
    m_endcapTiltAngle(endcapTilt), m_transTiltAngle(transTilt),
    m_endcapFoam(0),m_transFoam(0)
{
  preBuild();
}


void GeoPixelEndcapModuleSvcRef::preBuild()
{

  msg(MSG::DEBUG) <<"Foam description for layer "<<m_layer<<endreq;

  // Access XML file
  PixelInclRefStaveXMLHelper staveDBHelper(m_layer, getBasics());

  m_FoamBaseWidth = m_staveSupport->width();     // ST stave width not in XML, get it from stave object 
  m_MountainEdge = staveDBHelper.getMountainEdge();
  m_svcRouting = staveDBHelper.getSvcRoutingPos()=="inner" ? -1. : 1.;

  msg(MSG::DEBUG) <<"Foam base derived from ladder width "<<m_FoamBaseWidth<<endreq;

  if (m_layer==0) {
    m_FoamBaseWidth += -2.;
    msg(MSG::DEBUG) <<"Foam base width for layer 0 shrunk to "<< m_FoamBaseWidth<<endreq;
  }
  
  buildFoamModules();
}

GeoVPhysVol* GeoPixelEndcapModuleSvcRef::Build()
{
  return 0;
}


void GeoPixelEndcapModuleSvcRef::buildFoamModules()
{ 
  double modZ = m_endcapModule->Length()*sin(m_endcapTiltAngle);
  double modH = m_endcapModule->Length()*cos(m_endcapTiltAngle);

  // ***** geometry  **************************************

  GeoSimplePolygonBrep *shapeBrep = new GeoSimplePolygonBrep(m_FoamBaseWidth*.5);

  double yShift = m_svcRouting<0. ? 0. : modH;

  m_zEndcapFoamShift = 0.;

  m_FoamBaseThick = modZ + m_MountainEdge; 

  msg(MSG::DEBUG) <<"foam base Z :"<< m_FoamBaseThick<<", height:"<< modH<< ",edge:"<< m_MountainEdge<<
     ",foam volume (trapezoid):"<< 0.5*modZ*modH*m_FoamBaseWidth + modH*m_MountainEdge*m_FoamBaseWidth<< endreq;

  if (m_svcRouting>0) {
    double foam1y = yShift,                        foam1x = -0.5*modZ;
    double foam2y = yShift-modH*m_svcRouting,      foam2x =  0.5*modZ ;
    double foam3y = yShift-modH*m_svcRouting,      foam3x = -0.5*modZ + m_FoamBaseThick;
    double foam4y = yShift,                        foam4x = -0.5*modZ + m_FoamBaseThick;
    // Construct global object 2D envelope
    shapeBrep->addVertex(foam1x,foam1y);
    shapeBrep->addVertex(foam2x,foam2y);
    shapeBrep->addVertex(foam3x,foam3y);
    shapeBrep->addVertex(foam4x,foam4y);

  } else {
    double dz = -modZ;
    double foam1y = yShift,                        foam1x = dz;
    double foam2y = yShift,                        foam2x = dz + m_FoamBaseThick;
    double foam3y = yShift-modH*m_svcRouting,      foam3x = dz - modZ + m_FoamBaseThick ;
    double foam4y = yShift-modH*m_svcRouting,      foam4x = dz;
    // Construct global object 2D envelope
    shapeBrep->addVertex(foam1x,foam1y);
    shapeBrep->addVertex(foam2x,foam2y);
    shapeBrep->addVertex(foam3x,foam3y);
    shapeBrep->addVertex(foam4x,foam4y);

    //m_zEndcapFoamShift += 1.*m_svcRouting;
  }
  
  m_zEndcapFoamShift += 0.5*m_svcRouting;
      

  // ***** material  **************************************
  const GeoMaterial* foam_material = matMgr()->getMaterial("pix::CarbonFoamMountain");
  GeoLogVol * foam_logVol = new GeoLogVol("FoamSupport",shapeBrep,foam_material);
  m_endcapFoam  = new GeoPhysVol(foam_logVol);

  // build the transition foam support
  if (!m_transModule) return;

  double modZ_t = m_transModule->Length()*sin(m_transTiltAngle);
  double modH_t = m_transModule->Length()*cos(m_transTiltAngle);

  // ***** geometry  **************************************

  GeoSimplePolygonBrep *shapeBrep_t = new GeoSimplePolygonBrep(m_FoamBaseWidth*.5);

  double yShift_t = m_svcRouting<0. ? 0. : modH_t;

  m_zTransFoamShift = 0.;

  m_FoamBaseThick_t = modZ_t + m_MountainEdge; 
  
  msg(MSG::DEBUG) <<"transition foam base Z :"<< m_FoamBaseThick_t<<", height:"<< modH_t<< 
    "foam volume (trapezoid):"<< 0.5*modZ_t*modH_t*m_FoamBaseWidth 
      + modH_t*m_MountainEdge*m_FoamBaseWidth<< endreq;

  if (m_svcRouting>0) {
    double foam1y = yShift_t,                         foam1x = -0.5*modZ_t;
    double foam2y = yShift_t-modH_t*m_svcRouting,     foam2x = 0.5*modZ_t;
    double foam3y = yShift_t-modH_t*m_svcRouting,     foam3x = -0.5*modZ_t + m_FoamBaseThick_t;
    double foam4y = yShift_t,                         foam4x = -0.5*modZ_t + m_FoamBaseThick_t;
    // Construct global object 2D envelope
    shapeBrep_t->addVertex(foam1x,foam1y);
    shapeBrep_t->addVertex(foam2x,foam2y);
    shapeBrep_t->addVertex(foam3x,foam3y);
    shapeBrep_t->addVertex(foam4x,foam4y);
    
  } else {
    double dz = -modZ_t;
    double foam1y = yShift_t,                          foam1x = dz;
    double foam2y = yShift_t,                          foam2x = dz + m_FoamBaseThick_t;
    double foam3y = yShift_t-modH_t*m_svcRouting,      foam3x = dz - modZ_t + m_FoamBaseThick_t ;
    double foam4y = yShift_t-modH_t*m_svcRouting,      foam4x = dz;
    // Construct global object 2D envelope
    shapeBrep->addVertex(foam1x,foam1y);
    shapeBrep->addVertex(foam2x,foam2y);
    shapeBrep->addVertex(foam3x,foam3y);
    shapeBrep->addVertex(foam4x,foam4y);
    
    //m_zTransFoamShift += 1.*m_svcRouting;
  }
  
  m_zTransFoamShift += 0.5*m_svcRouting;
    
  GeoLogVol * foam_logVol_t = new GeoLogVol("FoamSupportTransition",shapeBrep_t,foam_material);
  m_transFoam  = new GeoPhysVol(foam_logVol_t);

}
