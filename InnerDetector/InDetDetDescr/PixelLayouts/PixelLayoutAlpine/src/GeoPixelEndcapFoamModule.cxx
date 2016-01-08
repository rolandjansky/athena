/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelLayoutAlpine/GeoPixelEndcapFoamModule.h"
#include "PixelGeoModelModule/GeoDetModulePixel.h"

#include "InDetGeoModelUtils/InDetMaterialManager.h" 
#include "PixelLayoutAlpine/PixelStaveSupportXMLHelper.h"

#include "GeoModelKernel/GeoShape.h"
#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoTube.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "GeoModelKernel/GeoSimplePolygonBrep.h"
#include "GeoModelKernel/GeoTransform.h"

							
GeoPixelEndcapFoamModule::GeoPixelEndcapFoamModule(const PixelGeoBuilderBasics* basics,
						   int iLayer, 
						   const GeoDetModulePixel* endcapModule, const GeoDetModulePixel* transModule, 
						   double endcapTilt, double transTilt)
						   
  : PixelGeoBuilder(basics),
    m_layer(iLayer), m_endcapModule(endcapModule), m_transModule(transModule), 
    m_endcapTiltAngle(endcapTilt), m_transTiltAngle(transTilt),
    m_endcapFoam(0),m_transFoam(0)
{
  preBuild();
}


void GeoPixelEndcapFoamModule::preBuild()
{

  //  msg(MSG::INFO) <<"Foam description "<<m_layer<<endreq;

  PixelStaveSupportXMLHelper staveDBHelper(m_layer);
  int geoIndex = staveDBHelper.getStaveGeoIndexEC();
  
  //  msg(MSG::INFO) <<"Foam description "<<m_layer<<" "<<geoIndex<<endreq;

  m_FacePlateThick = staveDBHelper.getFacePlateThick(geoIndex);
  m_FacePlateGlueThick = staveDBHelper.getFacePlateGlueThick(geoIndex);
  m_FacePlateGreaseThick = staveDBHelper.getFacePlateGreaseThick(geoIndex);

  m_FoamBaseThick = staveDBHelper.getFoamBaseThick(geoIndex);
  m_FoamBaseThickTrans = staveDBHelper.getFoamBaseThickTrans(geoIndex);
  m_FoamEdgeThick = staveDBHelper.getFoamEdgeThick(geoIndex);
  m_FoamEdgeThickTrans = staveDBHelper.getFoamEdgeThickTrans(geoIndex);


//   m_FacePlateThick = getDouble("PixelStaveSupportGeo", geoIndex, "FacePlateThick");
//   m_FacePlateGlueThick = getDouble("PixelStaveSupportGeo", geoIndex, "FacePlateGlueThick");
//   m_FacePlateGreaseThick = getDouble("PixelStaveSupportGeo", geoIndex, "FacePlateGreaseThick");
//   m_FoamBaseThick = getDouble("PixelStaveSupportGeo", geoIndex, "FoamBaseThick");
//   m_FoamEdgeThick = getDouble("PixelStaveSupportGeo", geoIndex, "FoamEdgeThick");
//   m_FoamBaseThickTrans = getDouble("PixelStaveSupportGeo", geoIndex, "FoamBaseThickTR");
//   m_FoamEdgeThickTrans = getDouble("PixelStaveSupportGeo", geoIndex, "FoamEdgeThickTR");
  
  buildFoamModules();
}

GeoVPhysVol* GeoPixelEndcapFoamModule::Build()
{
  return 0;
}


void GeoPixelEndcapFoamModule::buildFoamModules()
{

  //  double facePlateThick = m_FacePlateThick+m_FacePlateGlueThick+m_FacePlateGreaseThick;

  double greaseThick = m_FacePlateGreaseThick*cos(m_endcapTiltAngle);
  double glueThick = m_FacePlateGlueThick*cos(m_endcapTiltAngle);
  double plateThick = m_FacePlateThick*cos(m_endcapTiltAngle);
  double marginThick = 0.001*cos(m_endcapTiltAngle);

  const HepGeom::Transform3D trfId = HepGeom::Transform3D();
  GeoTransform* xformId = new GeoTransform(trfId);

  // --------------------------------------------
  // Build encap foam module
  // --------------------------------------------

  double moduleLength = m_endcapModule->Length();
  double moduleWidth  = m_endcapModule->Width();

  //  std::cout<<"Endcap module WxL : "<<moduleWidth<<" "<<moduleLength<<std::endl;

  double heightFoam = fabs(moduleLength*cos(m_endcapTiltAngle)); 
  double edgeThick=m_FoamBaseThick-fabs(moduleLength*sin(m_endcapTiltAngle));

  // ***** geometry  **************************************

  GeoSimplePolygonBrep *shapeBrep = new GeoSimplePolygonBrep(moduleWidth*.5);
  double foam1y = 0.0,             foam1x = -0.5*m_FoamBaseThick;
  double foam2y = -heightFoam,     foam2x = -0.5*m_FoamBaseThick;
  double foam3y = -heightFoam,     foam3x = -0.5*m_FoamBaseThick+edgeThick;
  double foam4y = 0.0,             foam4x =  0.5*m_FoamBaseThick;
  
  m_zEndcapFoamShift = fabs((foam2x+foam3x)*.5-foam3x);

   // Construct global object 2D envelopp
  shapeBrep->addVertex(foam1x,foam1y);
  shapeBrep->addVertex(foam2x,foam2y);
  shapeBrep->addVertex(foam3x,foam3y);
  shapeBrep->addVertex(foam4x,foam4y);  
  
  //  const GeoMaterial* foam_material = mat_mgr->getMaterial("pix::CarbonFoam");
//   GeoLogVol * foam_logVol = new GeoLogVol("FoamModule",shapeBrep,foam_material);
//   m_endcapFoam = new GeoPhysVol(foam_logVol);

  const GeoMaterial* air_material = matMgr()->getMaterial("std::Air");
  GeoLogVol * air_logVol = new GeoLogVol("EndcapFoam",shapeBrep,air_material);
  m_endcapFoam = new GeoPhysVol(air_logVol);

  // grease ------------------------------
  // -------------------------------------
  double endPos = 0.;
  double thick = greaseThick;
  GeoSimplePolygonBrep *shapeBrep_grease = new GeoSimplePolygonBrep(moduleWidth*.5);
  shapeBrep_grease->addVertex(foam4x-thick,foam4y);
  shapeBrep_grease->addVertex(foam3x-thick,foam3y);
  shapeBrep_grease->addVertex(foam3x,foam3y);
  shapeBrep_grease->addVertex(foam4x,foam4y);  

//   std::cout<<"FOAM MODULE - SHAPE grease "<<greaseThick<<std::endl;
//   int nbVertices=shapeBrep_grease->getNVertices();
//   for(int iPt=0; iPt<nbVertices; iPt++)
//     {
//       std::cout<<"  "<<iPt<<" "<<shapeBrep_grease->getXVertex(iPt)<<" "<<shapeBrep_grease->getYVertex(iPt)<<std::endl;
//     }

  const GeoMaterial* grease_material = matMgr()->getMaterial("pix::ThermGrease");
  GeoLogVol * grease_logVol = new GeoLogVol("FacePlateGrease",shapeBrep_grease,grease_material);
  GeoPhysVol* greaseFoam = new GeoPhysVol(grease_logVol);
  m_endcapFoam->add(xformId);
  m_endcapFoam->add(greaseFoam);

  // plate ------------------------------
  // -------------------------------------
  endPos += (greaseThick+marginThick);
  thick = plateThick;
  GeoSimplePolygonBrep *shapeBrep_plate = new GeoSimplePolygonBrep(moduleWidth*.5);
  shapeBrep_plate->addVertex(foam4x-endPos-thick,foam4y);
  shapeBrep_plate->addVertex(foam3x-endPos-thick,foam3y);
  shapeBrep_plate->addVertex(foam3x-endPos,foam3y);
  shapeBrep_plate->addVertex(foam4x-endPos,foam4y);  

//   std::cout<<"FOAM MODULE - SHAPE plate "<<plateThick<<std::endl;
//   nbVertices=shapeBrep_plate->getNVertices();
//   for(int iPt=0; iPt<nbVertices; iPt++)
//     {
//       std::cout<<"  "<<iPt<<" "<<shapeBrep_plate->getXVertex(iPt)<<" "<<shapeBrep_plate->getYVertex(iPt)<<std::endl;
//     }

  const GeoMaterial* plate_material = matMgr()->getMaterial("pix::FacePlate");
  GeoLogVol * plate_logVol = new GeoLogVol("FacePlate",shapeBrep_plate,plate_material);
  GeoPhysVol* plateFoam = new GeoPhysVol(plate_logVol);
  m_endcapFoam->add(xformId);
  m_endcapFoam->add(plateFoam);

  // glue ------------------------------
  // -------------------------------------
  endPos += (plateThick+marginThick);
  thick = glueThick;
  GeoSimplePolygonBrep *shapeBrep_glue = new GeoSimplePolygonBrep(moduleWidth*.5);
  shapeBrep_glue->addVertex(foam4x-endPos-thick,foam4y);
  shapeBrep_glue->addVertex(foam3x-endPos-thick,foam3y);
  shapeBrep_glue->addVertex(foam3x-endPos,foam3y);
  shapeBrep_glue->addVertex(foam4x-endPos,foam4y);  

  const GeoMaterial* glue_material = matMgr()->getMaterial("pix::Stycast2850FT");
  GeoLogVol * glue_logVol = new GeoLogVol("FacePlateGlue",shapeBrep_glue,glue_material);
  GeoPhysVol* glueFoam = new GeoPhysVol(glue_logVol);
  m_endcapFoam->add(glueFoam);

//   std::cout<<"FOAM MODULE - SHAPE glue "<<glueThick<<std::endl;
//   nbVertices=shapeBrep_plate->getNVertices();
//   for(int iPt=0; iPt<nbVertices; iPt++)
//     {
//       std::cout<<"  "<<iPt<<" "<<shapeBrep_plate->getXVertex(iPt)<<" "<<shapeBrep_plate->getYVertex(iPt)<<std::endl;
//     }

  // foam ------------------------------
  // -------------------------------------
  endPos += (glueThick+marginThick);
  GeoSimplePolygonBrep *shapeBrep_foam = new GeoSimplePolygonBrep(moduleWidth*.5);
  shapeBrep_foam->addVertex(foam1x,foam1y);
  shapeBrep_foam->addVertex(foam2x,foam2y);
  shapeBrep_foam->addVertex(foam3x-endPos,foam3y);
  shapeBrep_foam->addVertex(foam4x-endPos,foam4y);  

  const GeoMaterial* foam_material = matMgr()->getMaterial("pix::CarbonFoam");
  GeoLogVol * foam_logVol = new GeoLogVol("CarbonFoam",shapeBrep_foam,foam_material);
  GeoPhysVol* foamFoam = new GeoPhysVol(foam_logVol);
  m_endcapFoam->add(xformId);
  m_endcapFoam->add(foamFoam);


//   std::cout<<"FOAM MODULE - SHAPE "<<std::endl;
//   nbVertices=shapeBrep->getNVertices();
//   for(int iPt=0; iPt<nbVertices; iPt++)
//     {
//       std::cout<<"  "<<iPt<<" "<<shapeBrep->getXVertex(iPt)<<" "<<shapeBrep->getYVertex(iPt)<<std::endl;
//     }




  // --------------------------------------------
  // Build foam foam module
  // --------------------------------------------

  moduleLength = m_transModule->Length();
  moduleWidth  = m_transModule->Width();

  heightFoam = fabs(moduleLength*cos(m_transTiltAngle)); 
  edgeThick=m_FoamBaseThick-fabs(moduleLength*sin(m_transTiltAngle));

  // ***** geometry  **************************************

  GeoSimplePolygonBrep *shapeBrepTr = new GeoSimplePolygonBrep(moduleWidth*.5);
  foam1y = 0.0;             foam1x = -0.5*m_FoamBaseThick;
  foam2y = -heightFoam;     foam2x = -0.5*m_FoamBaseThick;
  foam3y = -heightFoam;     foam3x = -0.5*m_FoamBaseThick+edgeThick;
  foam4y = 0.0;             foam4x =  0.5*m_FoamBaseThick;
  
  m_zTransFoamShift = fabs((foam2x+foam3x)*.5-foam3x);

   // Construct global object 2D envelopp
  shapeBrepTr->addVertex(foam1x,foam1y);
  shapeBrepTr->addVertex(foam2x,foam2y);
  shapeBrepTr->addVertex(foam3x,foam3y);
  shapeBrepTr->addVertex(foam4x,foam4y);  
  
//   GeoLogVol * foam_logVolTr = new GeoLogVol("FoamModule",shapeBrepTr,foam_material);
//   m_transFoam = new GeoPhysVol(foam_logVolTr);

//   std::cout<<"FOAM MODULE - SHAPE "<<std::endl;
//   nbVertices=shapeBrepTr->getNVertices();
//   for(int iPt=0; iPt<nbVertices; iPt++)
//     {
//       std::cout<<"  "<<iPt<<" "<<shapeBrepTr->getXVertex(iPt)<<" "<<shapeBrepTr->getYVertex(iPt)<<std::endl;
//    }

  GeoLogVol * air_logVolTr = new GeoLogVol("TransFoam",shapeBrepTr,air_material);
  m_transFoam = new GeoPhysVol(air_logVolTr);

  // grease ------------------------------
  // -------------------------------------
  endPos = 0.;
  thick = greaseThick;
  GeoSimplePolygonBrep *shapeBrepTr_grease = new GeoSimplePolygonBrep(moduleWidth*.5);
  shapeBrepTr_grease->addVertex(foam4x-thick,foam4y);
  shapeBrepTr_grease->addVertex(foam3x-thick,foam3y);
  shapeBrepTr_grease->addVertex(foam3x,foam3y);
  shapeBrepTr_grease->addVertex(foam4x,foam4y);  

  GeoLogVol * grease_logVolTr = new GeoLogVol("FacePlateGrease",shapeBrepTr_grease,grease_material);
  GeoPhysVol* greaseFoamTr = new GeoPhysVol(grease_logVolTr);
  m_transFoam->add(xformId);
  m_transFoam->add(greaseFoamTr);

//   std::cout<<"FOAM MODULE - SHAPE grease "<<std::endl;
//   nbVertices=shapeBrepTr_grease->getNVertices();
//   for(int iPt=0; iPt<nbVertices; iPt++)
//     {
//       std::cout<<"  "<<iPt<<" "<<shapeBrepTr_grease->getXVertex(iPt)<<" "<<shapeBrepTr_grease->getYVertex(iPt)<<std::endl;
//     }

  // plate ------------------------------
  // -------------------------------------
  endPos += (greaseThick+marginThick);
  thick = plateThick;
  GeoSimplePolygonBrep *shapeBrepTr_plate = new GeoSimplePolygonBrep(moduleWidth*.5);
  shapeBrepTr_plate->addVertex(foam4x-endPos-thick,foam4y);
  shapeBrepTr_plate->addVertex(foam3x-endPos-thick,foam3y);
  shapeBrepTr_plate->addVertex(foam3x-endPos,foam3y);
  shapeBrepTr_plate->addVertex(foam4x-endPos,foam4y);  

  GeoLogVol * plate_logVolTr = new GeoLogVol("FacePlate",shapeBrepTr_plate,plate_material);
  GeoPhysVol* plateFoamTr = new GeoPhysVol(plate_logVolTr);
  m_transFoam->add(xformId);
  m_transFoam->add(plateFoamTr);

//   std::cout<<"FOAM MODULE - SHAPE plate "<<std::endl;
//   nbVertices=shapeBrepTr_plate->getNVertices();
//   for(int iPt=0; iPt<nbVertices; iPt++)
//     {
//       std::cout<<"  "<<iPt<<" "<<shapeBrepTr_plate->getXVertex(iPt)<<" "<<shapeBrepTr_plate->getYVertex(iPt)<<std::endl;
//     }


  // glue ------------------------------
  // -------------------------------------
  endPos += (plateThick+marginThick);
  thick = glueThick;
  GeoSimplePolygonBrep *shapeBrepTr_glue = new GeoSimplePolygonBrep(moduleWidth*.5);
  shapeBrepTr_glue->addVertex(foam4x-endPos-thick,foam4y);
  shapeBrepTr_glue->addVertex(foam3x-endPos-thick,foam3y);
  shapeBrepTr_glue->addVertex(foam3x-endPos,foam3y);
  shapeBrepTr_glue->addVertex(foam4x-endPos,foam4y);  

  GeoLogVol * glue_logVolTr = new GeoLogVol("FacePlateGlue",shapeBrepTr_glue,glue_material);
  GeoPhysVol* glueFoamTr = new GeoPhysVol(glue_logVolTr);
  m_transFoam->add(glueFoamTr);


  // foam ------------------------------
  // -------------------------------------
  endPos += (glueThick+marginThick);
  GeoSimplePolygonBrep *shapeBrepTr_foam = new GeoSimplePolygonBrep(moduleWidth*.5);
  shapeBrepTr_foam->addVertex(foam1x,foam1y);
  shapeBrepTr_foam->addVertex(foam2x,foam2y);
  shapeBrepTr_foam->addVertex(foam3x-endPos,foam3y);
  shapeBrepTr_foam->addVertex(foam4x-endPos,foam4y);  

  GeoLogVol * carbon_logVolTr = new GeoLogVol("CarbonFoam",shapeBrepTr_foam,foam_material);
  GeoPhysVol* carbonFoamTr = new GeoPhysVol(carbon_logVolTr);
  m_transFoam->add(xformId);
  m_transFoam->add(carbonFoamTr);



//   std::cout<<"FOAM MODULE - SHAPE "<<std::endl;
//   nbVertices=shapeBrepTr->getNVertices();
//   for(int iPt=0; iPt<nbVertices; iPt++)
//     {
//       std::cout<<"  "<<iPt<<" "<<shapeBrepTr->getXVertex(iPt)<<" "<<shapeBrep->getYVertex(iPt)<<std::endl;
//     }


}
