/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


// Build detailed stave support : face plate + carbon foam + cable flex + cooling pipe + end blocks
// This is built one time per layer. 

#include "PixelLayoutAlpine/GeoPixelStaveSupportAlpineBuilder.h"
#include "PixelGeoModelModule/GeoDetModulePixel.h"

#include "PixelGeoComponent/GeoComponent.h"

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

#include "GeoModelKernel/GeoTransform.h"
#include "PathResolver/PathResolver.h"

#include <algorithm>
using std::max;

GeoPixelStaveSupportAlpineBuilder::GeoPixelStaveSupportAlpineBuilder(const PixelGeoBuilderBasics* basics,
								     int iLayer, int iSector, 
								     const GeoDetModulePixel* brlModule, 
								     double maxTiltedModuleThick,
								     double zEndOfBrlModulePos,
								     std::vector<HepGeom::Point3D<double> > endcapModPos):
  PixelGeoBuilder(basics),								     
  m_layer(iLayer), m_sector(iSector), m_brlModule(brlModule), m_maxTiltedModuleThick(maxTiltedModuleThick), 
  m_zEndOfBrlModulePos(zEndOfBrlModulePos),
  m_endcapModPos(endcapModPos)
{
  m_staveEnvelopShape=0;
  m_bVerbose = false;
  m_bVerbose = true;
  m_staveDBHelper = new PixelStaveSupportXMLHelper(m_layer);

  m_physVol = 0;
  m_geoVol = 0;
  m_bGeoComp = basics->isGeoComponent();
  preBuild();
}

GeoVPhysVol* GeoPixelStaveSupportAlpineBuilder::Build() {
  return m_physVol;
}

void GeoPixelStaveSupportAlpineBuilder::preBuild() {

  // ------------------------------------------------------------------------------------------------------------
  // ------------------------------------------------------------------------------------------------------------
  // Compute stave length : is identical for all the configration ( = IBL one)
  // ------------------------------------------------------------------------------------------------------------
  // ------------------------------------------------------------------------------------------------------------

  //  msg(MSG::INFO) <<"Build detailed stave support description :  layout "<<m_layer<<"  ->   "<<endreq;

  int layerIndex = m_layer;
  int staveGeoIndex = m_staveDBHelper->getStaveGeoIndex();
  int staveGeoIndexEC = m_staveDBHelper->getStaveGeoIndexEC();

  HepGeom::Transform3D trfId = HepGeom::Transform3D(); 
  if(staveGeoIndexEC<0){
    BuildStaveSupport(layerIndex, staveGeoIndex, true, -1, trfId);
    return;
  }

  double MechanicalStaveLength = m_staveDBHelper->getMechanicalStaveLength();
  BuildStaveSupport(layerIndex, staveGeoIndex, true, 2.*m_zEndOfBrlModulePos, trfId);

  double staveLength = MechanicalStaveLength*.5 - m_zEndOfBrlModulePos;
  HepGeom::Transform3D trfECpos = HepGeom::TranslateZ3D(m_zEndOfBrlModulePos + staveLength*.5);
  BuildStaveSupport(layerIndex, staveGeoIndexEC, false, staveLength, trfECpos, 1);

  HepGeom::Transform3D trfECneg = HepGeom::TranslateZ3D(-(m_zEndOfBrlModulePos + staveLength*.5));
  BuildStaveSupport(layerIndex, staveGeoIndexEC, false, staveLength, trfECneg, -1);

}


void GeoPixelStaveSupportAlpineBuilder::BuildStaveSupport(int, int geoIndex, bool bBarrelStave, double zLength, HepGeom::Transform3D trf, int iSide )
{
  m_StaveLength =  m_staveDBHelper->getStaveLength();
  
  // ------------------------------------------------------------------------------------------------------------
  // ------------------------------------------------------------------------------------------------------------
  // Stave geometry
  // ------------------------------------------------------------------------------------------------------------
  // ------------------------------------------------------------------------------------------------------------
  
  double MechanicalStaveWidth = m_staveDBHelper->getMechanicalStaveWidth();
  double MechanicalStaveLength =  m_staveDBHelper->getMechanicalStaveLength();
  //  double MechanicalStaveOffset = getDouble("PixelStaveSupport", layerIndex, "Offset");
  
  //  double StaveModuleOffset = gmt_mgr->IBLStaveMechanicalStaveModuleOffset();
  
  double MechanicalStaveLength_total = MechanicalStaveLength;
  if(zLength>0.01) MechanicalStaveLength = zLength;
  
  double TubeOuterDiam =  m_staveDBHelper->getTubeOuterDiam(geoIndex);
  double TubeInnerDiam =  m_staveDBHelper->getTubeInnerDiam(geoIndex);
  double TubeMiddlePos =  m_staveDBHelper->getTubeMiddlePos(geoIndex);
  double TubeCentralPos = m_staveDBHelper->getTubeCentralPos(geoIndex);
  double TubeGlueThick =  m_staveDBHelper->getTubeGlueThick(geoIndex);
  
  //   double FlexLayerThick = gmt_mgr->IBLStaveFlexLayerThickness();
  //   double FlexBaseThick = gmt_mgr->IBLStaveFlexBaseThickness();
  //   double FlexWidth = gmt_mgr->IBLStaveFlexWidth();
  //   double FlexOffset = gmt_mgr->IBLStaveFlexOffset();
  //   double FlexStaveMiddleGap = gmt_mgr->IBLFlexMiddleGap();
  //   double FlexEOS_zpos = gmt_mgr->IBLFlexPP0Z(0);
  //   double FlexDogLeg_length = gmt_mgr->IBLFlexDoglegLength();
  //   double WingWidth = gmt_mgr->IBLStaveFlexWingWidth();
  //   double WingThick = gmt_mgr->IBLStaveFlexWingThick();
  //   bool bFlexAndWing = gmt_mgr->IBLFlexAndWingDefined();
  
  //   bool bFlexConstantThickness = true;
  //   if(!bFlexAndWing) bFlexConstantThickness = false;
  
  //  double layerRadius = gmt_mgr->PixelLayerRadius();
  
  
  m_FacePlateThick = m_staveDBHelper->getFacePlateThick(geoIndex);
  m_FacePlateWidth = m_staveDBHelper->getFacePlateWidth(geoIndex);
  double FacePlateGlueThick = m_staveDBHelper->getFacePlateGlueThick(geoIndex);
  m_FacePlateGreaseThick = m_staveDBHelper->getFacePlateGreaseThick(geoIndex);
  
  if(bBarrelStave){
    m_staveBarrelWidth = MechanicalStaveWidth;
    m_FoamBaseThick = 0.;
    m_FoamBaseThickTrans = 0.;
  }
  else {
    m_FoamBaseThick = m_staveDBHelper->getFoamBaseThick(geoIndex);
    m_FoamBaseThickTrans = m_staveDBHelper->getFoamBaseThickTrans(geoIndex);
  }
  
  m_OmegaThick = m_staveDBHelper->getOmegaThick(geoIndex);
  m_OmegaGlueThick = m_staveDBHelper->getOmegaGlueThick(geoIndex);
  m_OmegaMiddleThick = m_staveDBHelper->getOmegaMiddleThick(geoIndex);
  m_OmegaTopWidth = m_staveDBHelper->getOmegaTopWidth(geoIndex);
  
  m_FoamThick = m_staveDBHelper->getFoamThick(geoIndex);
  
  m_StaveModuleOffset = 0;
  m_MechanicalStaveOffset = 0;
  m_omegaTotalThick = m_OmegaMiddleThick;
  m_flexTotalThick = 0.;
  
  MechanicalStaveWidth = m_FacePlateWidth;
  double halfMecStaveWidth=MechanicalStaveWidth*0.5;
  
  // SafetyMargin
  m_SafetyMargin=.001*CLHEP::mm;
  double xGblOffset=m_FacePlateThick+m_FacePlateGreaseThick+FacePlateGlueThick;
  double safetyMarginZ=.001*CLHEP::mm;
  m_FacePlateTotalThick = xGblOffset;
  
  // Compute approximated stave shape based on DB parameters
  double ratio = (m_FacePlateWidth-m_OmegaTopWidth)*.5/m_OmegaMiddleThick;
  ComputeStaveExternalShape(ratio,safetyMarginZ,xGblOffset);
  
  // //   // if no flex width is defined, take foam module side length as default value 
  // //   if (FlexWidth<0.001)
  // //     {
  // //        double xDelta=(m_FoamMiddleThick+m_OmegaMidStaveThick)-m_FoamEdgePointX;
  // //        double yDelta=-m_FoamEdgePointY;
  // //        FlexWidth = sqrt(xDelta*xDelta+yDelta*yDelta);
  // //     }
  
  msg(MSG::INFO)<<" MechanicalStaveWidth  = "<<MechanicalStaveWidth <<endreq;
  msg(MSG::INFO)<<" MechanicalStaveLength = "<<MechanicalStaveLength <<endreq;
  //  msg(MSG::INFO)<<" MechanicalStaveOffset = "<<MechanicalStaveOffset <<endreq;
  //  msg(MSG::INFO)<<" StaveModuleOffset     = "<<StaveModuleOffset <<endreq;
  //  msg(MSG::INFO)<<" MechanicalStaveEndBlockLength = "<<MechanicalStaveEndBlockLength <<endreq;
  msg(MSG::INFO)<<" FacePlateThick  = "<<m_FacePlateThick <<endreq;
  msg(MSG::INFO)<<" TubeOuterDiam   = "<< TubeOuterDiam <<endreq; 
  msg(MSG::INFO)<<" TubeInnerDiam   = "<< TubeInnerDiam <<endreq; 
  msg(MSG::INFO)<<" TubeMiddlePos   = "<< TubeMiddlePos <<" "<<TubeCentralPos<<endreq; 
  
  //  gmt_mgr->msg(MSG::INFO)<<" FlexBaseThick   = "<< FlexBaseThick <<endreq; 
  //  gmt_mgr->msg(MSG::INFO)<<" FlexLayerThick  = "<< FlexLayerThick <<endreq; 
  //  gmt_mgr->msg(MSG::INFO)<<" FlexWidth       = "<< FlexWidth <<endreq; 
  //  gmt_mgr->msg(MSG::INFO)<<" FlexOffset      = "<< FlexOffset <<endreq; 
  //  gmt_mgr->msg(MSG::INFO)<<" XOffset / ladder = "<< StaveModuleOffset <<endreq; 
  

  // Coordinate system
  //
  //    (0,0) = middle point of the face plate side near to the module
  //    x axis : perpendicular to face plate
  //    y axis : along face plate 

  // ------------------------------------------------------------------------------------------------------------
  // ------------------------------------------------------------------------------------------------------------
  // Foam shape - OK
  // ------------------------------------------------------------------------------------------------------------
  // ------------------------------------------------------------------------------------------------------------

  double foam1x = xGblOffset,         foam1y = -foamEndPoint.y();
  double foam2x = foamTopPoint.x(),   foam2y = -foamTopPoint.y();
  //  double foam3x = foamTopPoint.x(),   foam3y = 0.0;
  double foam4x = foamTopPoint.x(),   foam4y = foamTopPoint.y();
  double foam5x = xGblOffset,         foam5y = foamEndPoint.y();

  // ------------------------------------------------------------------------------------------------------------
  // ------------------------------------------------------------------------------------------------------------
  // Omega shape
  // ------------------------------------------------------------------------------------------------------------
  // ------------------------------------------------------------------------------------------------------------
  // other 5 points are common with Foam shape (foam 5-4-3-2-1)

  double omega1x, omega1y, omega2x, omega2y, omega3x, omega3y;
  double omega4x, omega4y, omega5x, omega5y;
  omega1x=omega1y=omega2x=omega2y=omega3x=omega3y=omega4x=omega4y=omega5x=omega5y=0.0;

  omega1x = xGblOffset;                             omega1y = -halfMecStaveWidth;
  omega2x = omegaTopPoint.x();                      omega2y = -omegaTopPoint.y();
  omega3x = omegaTopPoint.x();                      omega3y = 0.0;
  omega4x = omegaTopPoint.x();                      omega4y = omegaTopPoint.y();
  omega5x = xGblOffset;                             omega5y = halfMecStaveWidth;
  
  // ------------------------------------------------------------------------------------------------------------
  // ------------------------------------------------------------------------------------------------------------
  // Face plate shape
  //   other 2 points are common with Foam Shape (foam1x and foam5x)
  // ------------------------------------------------------------------------------------------------------------
  // ------------------------------------------------------------------------------------------------------------

  double plate1x  =0.0,             plate1y= foam1y;
  //  double plate2x  =FacePlateThick,  plate2y= foam1y;
  //  double plate3x  =FacePlateThick,  plate3y= foam5y;
  double plate4x  =0.0,             plate4y= foam5y;

  //  plate1y=omega1y; plate4y=omega5y;

//   GeoBox *box = new GeoBox(FacePlateThick*0.5,MechanicalStaveWidth*0.5,MechanicalStaveLength*.5);
//   CLHEP::Hep3Vector box_pos((plate1x+plate2x+plate3x+plate4x)*0.25,(plate1y+plate2y+plate3y+plate4y)*0.25,0.0);
//   GeoTransform* box_xform = new GeoTransform(HepGeom::Transform3D(CLHEP::HepRotation(),box_pos));

//   //  const GeoMaterial* box_material = mat_mgr->getMaterial("std::Air");
//   double staveSupportThickness=FacePlateThick;
//   m_thicknessP = staveSupportThickness*.5;
//   m_thicknessN = staveSupportThickness*.5;
//   m_width = MechanicalStaveWidth;
//   m_length = MechanicalStaveLength;

//   double stave_xOffset=thicknessN()+m_brlModule->ThicknessP()+0.1;
//   double stave_yOffset=0.;
//   gmt_mgr->msg(MSG::INFO)<<" stave xoffset = "<< stave_xOffset <<endreq; 
//   gmt_mgr->msg(MSG::INFO)<<" stave yoffset = "<< stave_yOffset <<endreq; 
//   m_transform = HepGeom::Translate3D(stave_xOffset,stave_yOffset,0);

//   return logVolPV_box;

  
  // ------------------------------------------------------------------------------------------------------------
  // ------------------------------------------------------------------------------------------------------------
  // Cableflex shape
  // vPerpDir perpendicular direction to (foam3,foam4)
  // ------------------------------------------------------------------------------------------------------------
  // ------------------------------------------------------------------------------------------------------------

//   double seg1x, seg2x, seg1y, seg2y;
//   seg1x=omega3x; seg1y=omega3y;
//   seg2x=omega4x; seg2y=omega4y;

//   double vDirX=(seg2x-seg1x), vDirY=(seg2y-seg1y);
//   double invNorm=1.0/sqrt(vDirX*vDirX+vDirY*vDirY);  
//   vDirX*=invNorm; vDirY*=invNorm; 
//   double vPerpDirX=vDirY, vPerpDirY=-vDirX;

//   double foamMidX=m_MidStaveSidePointX+xGblOffset, foamMidY=m_MidStaveSidePointY;
//   foamMidX+=(vPerpDirX*m_SafetyMargin);
//   foamMidY+=(vPerpDirY*m_SafetyMargin);
//   double flexHaWidth=FlexWidth*0.5;
//   double wingHalfWidth = WingWidth*.5;
  
//   double flexWingMaxR=0.;
//   double maxFlex1x = foamMidX-(FlexOffset+flexHalfWidth)*vDirX;
//   double maxFlex1y = foamMidY-(FlexOffset+flexHalfWidth)*vDirY;
//   double maxFlex2x = maxFlex1x+MaxFlexThickness*vPerpDirX;
//   double maxFlex2y = maxFlex1y+MaxFlexThickness*vPerpDirY;
//   double maxFlex4x = foamMidX+(-FlexOffset+flexHalfWidth)*vDirX;
//   double maxFlex4y = foamMidY+(-FlexOffset+flexHalfWidth)*vDirY;
//   //  double maxFlex3x = maxFlex4x+MaxFlexThickness*vPerpDirX;
//   //  double maxFlex3y = maxFlex4y+MaxFlexThickness*vPerpDirY;
//   double maxFlex4x_wings = foamMidX+(-FlexOffset+flexHalfWidth+2.*wingHalfWidth+.5)*vDirX;                  // SES - fixme
//   double maxFlex4y_wings = foamMidY+(-FlexOffset+flexHalfWidth+2.*wingHalfWidth+.5)*vDirY;
//   double maxFlex3x_wings = maxFlex4x_wings+MaxFlexThickness*vPerpDirX;
//   double maxFlex3y_wings = maxFlex4y_wings+MaxFlexThickness*vPerpDirY;

//   double flexWingMaxR= 0.;
//   double maxFlex1x =  0.;
//   double maxFlex1y =  0.;
//   double maxFlex2x =  0.;
//   double maxFlex2y =  0.;
//   double maxFlex4x =  0.;
//   double maxFlex4y =  0.;
//   //  double maxFlex3x = maxFlex4x+MaxFlexThickness*vPerpDirX;
//   //  double maxFlex3y = maxFlex4y+MaxFlexThickness*vPerpDirY;
//   double maxFlex4x_wings =  0.;
//   double maxFlex4y_wings =  0.;
//   double maxFlex3x_wings =  0.;
//   double maxFlex3y_wings =  0.;

  // ------------------------------------------------------------------------------------------------------------
  // ------------------------------------------------------------------------------------------------------------
  // Stave shape : face plate + foam + cableflex + omega
  //  corresponding  physical volume is returned at the end of Build()
  // ------------------------------------------------------------------------------------------------------------
  // ------------------------------------------------------------------------------------------------------------

  //  std::cout<<"StaveSupport : build envelope"<<std::endl;

  // create shape
  m_basicStaveEnvelopShape = new GeoSimplePolygonBrep(m_StaveLength*.5+safetyMarginZ);
  std::vector<double> xShape, yShape;
  std::vector<int>iShapeExcept;

  double flexThickness=0.;
  double flexHalfWidth=0.;
  if(bBarrelStave) 
    {
      int flexIndex = m_staveDBHelper->getStaveFlexIndex();
      if(flexIndex>-1)
	{
	  std::vector<double> flexThick = m_staveDBHelper->getStaveFlexSectionThick(flexIndex);
	  std::vector<int> transRegion = m_staveDBHelper->getStaveFlexSectionTransition(flexIndex);
	  int nbRegion = (int)transRegion.size()+1;
	  flexThickness=(nbRegion-2)*flexThick[0];

	  flexHalfWidth= m_staveDBHelper->getStaveFlexWingWidth(flexIndex)-omegaTopPoint.y();
	  m_flexTotalThick = flexThickness;
	}
    }
  
  xShape.push_back(plate1x);  yShape.push_back(plate1y); iShapeExcept.push_back(1);
  xShape.push_back(omega1x);   yShape.push_back(omega1y); iShapeExcept.push_back(0);
  xShape.push_back(omega2x+flexThickness);   yShape.push_back(std::max(omega2y,flexHalfWidth)); iShapeExcept.push_back(0);
  xShape.push_back(omega4x+flexThickness);   yShape.push_back(std::min(omega4y,-flexHalfWidth)); iShapeExcept.push_back(0);
  xShape.push_back(omega5x);   yShape.push_back(omega5y); iShapeExcept.push_back(0);
  
  xShape.push_back(plate4x);  yShape.push_back(plate4y); iShapeExcept.push_back(1);

  //  for(int i=0; i<(int)xShape.size(); i++) std::cout<<xShape[i]<<" "<<yShape[i]<<std::endl;

  // clean up shape and store points to Brep structure

  if(m_bVerbose)msg(MSG::DEBUG)<<"Remove points "<<endreq;
  GetSurroundingConvexShape(xShape, yShape, iShapeExcept);
  RemoveCoincidentAndColinearPointsFromShape( xShape, yShape);
  
  //  std::cout<<"FINAL SHAPE "<<std::endl;
  //  for(int i=0; i<(int)xShape.size(); i++) std::cout<<xShape[i]<<" "<<yShape[i]<<std::endl;

  for(int iPt=0; iPt<(int)xShape.size(); iPt++)
    m_basicStaveEnvelopShape->addVertex(xShape[iPt],yShape[iPt]);

  // create log and phys volumes
  //  const GeoMaterial* envelop_material = mat_mgr->getMaterial("std::Air");
  const GeoMaterial* envelop_material = matMgr()->getMaterial("special::Ether");
  GeoLogVol * logVol = new GeoLogVol("StaveSupport",m_basicStaveEnvelopShape,envelop_material);

  // Time to create the stave envelope physVol 
  if(bBarrelStave){
    m_physVol = new GeoPhysVol(logVol);
    if(m_bGeoComp)
      m_geoVol = new InDet::GeoComponent(m_physVol);
  }
  
  // ------------------------------------------------------------------------------------------------------------
  // ------------------------------------------------------------------------------------------------------------
  // Carbon Foam 
  // ------------------------------------------------------------------------------------------------------------
  // ------------------------------------------------------------------------------------------------------------
  //  std::cout<<"StaveSupport : build carbon foam"<<std::endl;

  GeoSimplePolygonBrep * foam_shape = new GeoSimplePolygonBrep(MechanicalStaveLength*.5);
  foam_shape->addVertex(foam1x,foam1y);
  foam_shape->addVertex(foam2x,foam2y);
  //  foam_shape->addVertex(foam3x,foam3y);
  foam_shape->addVertex(foam4x,foam4y);
  foam_shape->addVertex(foam5x,foam5y);

  const GeoMaterial* foam_material = matMgr()->getMaterial("pix::CarbonFoam");

  //  std::cout<<"CarbonFoam : "<<(foam_material==0)<<std::endl;

  GeoNameTag* foam_tag = new GeoNameTag("CarbonFoam");
  HepGeom::Transform3D foam_trf = trf*HepGeom::Transform3D(CLHEP::HepRotation(),CLHEP::Hep3Vector());
  GeoTransform* foam_xform = new GeoTransform(foam_trf);
  
  GeoLogVol * foam_logVol = new GeoLogVol("CarbonFoam",foam_shape,foam_material);
  GeoPhysVol * foam_logVolPV = new GeoPhysVol(foam_logVol);

  //  DumpVolumeAndMaterial(foam_logVol,MechanicalStaveLength);

  InDet::GeoComponent *foam_geocomp = 0;
  if(m_bGeoComp){
    foam_geocomp = new InDet::GeoComponent(foam_logVolPV);
  }

  // ------------------------------------------------------------------------------------------------------------
  // ------------------------------------------------------------------------------------------------------------
  // Omega 
  // ------------------------------------------------------------------------------------------------------------
  // ------------------------------------------------------------------------------------------------------------

  //  std::cout<<"StaveSupport : build omega"<<std::endl;

  GeoSimplePolygonBrep * omega_shape = new GeoSimplePolygonBrep(MechanicalStaveLength*.5);
  omega_shape->addVertex(omega1x,omega1y);
  omega_shape->addVertex(omega2x,omega2y);
  //  omega_shape->addVertex(omega3x,omega3y);
  omega_shape->addVertex(omega4x,omega4y);
  omega_shape->addVertex(omega5x,omega5y);

//   omega_shape->addVertex(foam5x,foam5y);
//   omega_shape->addVertex(foam4x,foam4y);
//   //  omega_shape->addVertex(foam3x,foam3y);
//   omega_shape->addVertex(foam2x,foam2y);
//   omega_shape->addVertex(foam1x,foam1y); 

  omega_shape->addVertex(omegaEndPoint_bottom.x(),omegaEndPoint_bottom.y());
  omega_shape->addVertex(omegaTopPoint_bottom.x(),omegaTopPoint_bottom.y());
  //  omega_shape->addVertex(omegaTopPoint_bottom3x,omegaTopPoint_bottom3y);
  omega_shape->addVertex(omegaTopPoint_bottom.x(),-omegaTopPoint_bottom.y());
  omega_shape->addVertex(omegaEndPoint_bottom.x(),-omegaEndPoint_bottom.y());


  msg(MSG::INFO)<<"** OMEGA : without Stycast "<<m_OmegaGlueThick<<endreq;
  const GeoMaterial* omega_material = matMgr()->getMaterial("pix::Omega");
  GeoLogVol* omega_logVol = new GeoLogVol("Omega",omega_shape,omega_material);

  GeoNameTag* omega_tag = new GeoNameTag("Omega");
  HepGeom::Transform3D omega_trf = trf*HepGeom::Transform3D(CLHEP::HepRotation(),CLHEP::Hep3Vector());      
  GeoTransform* omega_xform = new GeoTransform(omega_trf);

  //  DumpVolumeAndMaterial(omega_logVol,MechanicalStaveLength);

  GeoPhysVol * omega_logVolPV = new GeoPhysVol(omega_logVol);
  if(m_bGeoComp){
    InDet::GeoComponent *omega = new InDet::GeoComponent(omega_logVolPV);
    m_geoVol->placeAsChild( *omega, omega_trf, 0);
  }
  else {
    m_physVol->add(omega_tag);
    m_physVol->add(omega_xform);
    m_physVol->add(omega_logVolPV);
  }

    
  //  std::cout<<"StaveSupport : build omega glue"<<std::endl;

  GeoSimplePolygonBrep * omega_glue_shape = new GeoSimplePolygonBrep(MechanicalStaveLength*.5);
  omega_glue_shape->addVertex(glueEndPoint_top.x(),-glueEndPoint_top.y());
  omega_glue_shape->addVertex(glueTopPoint_top.x(),-glueTopPoint_top.y());
  omega_glue_shape->addVertex(glueTopPoint_top.x(),glueTopPoint_top.y());
  omega_glue_shape->addVertex(glueEndPoint_top.x(),glueEndPoint_top.y());

  omega_glue_shape->addVertex(glueEndPoint_bottom.x(),glueEndPoint_bottom.y());
  omega_glue_shape->addVertex(glueTopPoint_bottom.x(),glueTopPoint_bottom.y());
  omega_glue_shape->addVertex(glueTopPoint_bottom.x(),-glueTopPoint_bottom.y());
  omega_glue_shape->addVertex(glueEndPoint_bottom.x(),-glueEndPoint_bottom.y());


  const GeoMaterial* omega_glue_material = matMgr()->getMaterial("pix::EpoxyGlue");
  GeoLogVol* omega_glue_logVol = new GeoLogVol("OmegaGlue",omega_glue_shape,omega_glue_material);

  GeoNameTag* omega_glue_tag = new GeoNameTag("OmegaGlue");
  HepGeom::Transform3D omega_glue_trf = trf*HepGeom::Transform3D(CLHEP::HepRotation(),CLHEP::Hep3Vector());      
  GeoTransform* omega_glue_xform = new GeoTransform(omega_glue_trf);

  GeoPhysVol * omega_glue_logVolPV = new GeoPhysVol(omega_glue_logVol);
  if(m_bGeoComp){
    InDet::GeoComponent *omega_glue = new InDet::GeoComponent(omega_glue_logVolPV);
    m_geoVol->placeAsChild( *omega_glue, omega_glue_trf, 0);
  }
  else{
    m_physVol->add(omega_glue_tag);
    m_physVol->add(omega_glue_xform);
    m_physVol->add(omega_glue_logVolPV);
  }


  // ------------------------------------------------------------------------------------------------------------
  // ------------------------------------------------------------------------------------------------------------
  // Stave flex
  // ------------------------------------------------------------------------------------------------------------
  // ------------------------------------------------------------------------------------------------------------

  int flexIndex = m_staveDBHelper->getStaveFlexIndex();

  //  std::cout<<"STAVE FLEX : "<<m_layer<<" "<<flexIndex<<std::endl;

  if(bBarrelStave&&flexIndex>-1) 
    {
      int flexSections = m_staveDBHelper->getStaveFlexSectionNumber(flexIndex);
      std::vector<double> flexZmin = m_staveDBHelper->getStaveFlexSectionZmin(flexIndex);
      std::vector<double> flexZmax = m_staveDBHelper->getStaveFlexSectionZmax(flexIndex);
      std::vector<double> flexLength = m_staveDBHelper->getStaveFlexSectionLength(flexIndex);
      std::vector<double> flexThick = m_staveDBHelper->getStaveFlexSectionThick(flexIndex);
      std::vector<double> flexVol = m_staveDBHelper->getStaveFlexSectionVolume(flexIndex);

      double thickCu = m_staveDBHelper->getStaveCopperThick(flexIndex);
      std::vector<double> volCu = m_staveDBHelper->getStaveFlexSectionCopperVolume(flexIndex);
      std::vector<double> volAdh = m_staveDBHelper->getStaveFlexSectionAdhesiveVolume(flexIndex);
      std::vector<double> volPoly= m_staveDBHelper->getStaveFlexSectionPolyimideVolume(flexIndex);

      std::vector<double> wgCu = m_staveDBHelper->getStaveFlexSectionCopperWeight(flexIndex);
      std::vector<double> wgAdh = m_staveDBHelper->getStaveFlexSectionAdhesiveWeight(flexIndex);
      std::vector<double> wgPoly= m_staveDBHelper->getStaveFlexSectionPolyimideWeight(flexIndex);

      std::vector<std::string> flexMatName = m_staveDBHelper->getStaveFlexSectionMaterial(flexIndex);
      std::vector<std::string> flexMatNameTrans = m_staveDBHelper->getStaveFlexSectionMaterialTrans(flexIndex);

      std::vector<int> transRegion = m_staveDBHelper->getStaveFlexSectionTransition(flexIndex);
      int nbRegion = (int)transRegion.size()+1;
      double widthTrans = 0;
      std::vector<double> zPosTrans;
      std::vector<double> yWidthTrans;
      std::vector<double> copperVolTrans;
      std::vector<double> adhVolTrans;
      std::vector<double> polyVolTrans;
      //      double wgCuMin = 1.;
      //      double wgCuMax = 0.;

      double xFlexPos = omega3x;
      //      double flexZmin = 9999.;
      //      doubel flexZmax = 0;

      for(int iRegion=0; iRegion<nbRegion; iRegion++)
	{
	  int sec0 = (iRegion==0)? 0 : transRegion[iRegion-1];
	  int sec1 = (iRegion==nbRegion-1) ? flexSections: transRegion[iRegion];
	  double widthPrevious = 0;

// 	  double regVolCu=0.;
// 	  double regVolAdh=0.;
// 	  double regVolPoly=0.;

	  int iCmpt = 0;
	  for(int iSec=sec0; iSec<sec1; iSec++)
	    {
	      double zmin = flexZmin[iSec];
	      double zmax = flexZmax[iSec];
	      double length = flexLength[iSec];
	      double thick = flexThick[0];
	      double widthInit = volCu[iSec]/(length*thickCu);

	      //	      flexZmin = std::min(flexZmin,zmin);
	      //	      flexZmax = std::min(flexZmin,zmax);

	      if(iRegion==nbRegion-1)
 		{ widthPrevious=0.; } 

	      std::ostringstream lname;
	      lname << "R" << iRegion<<"S"<<iCmpt;
	      iCmpt++;
	      GeoNameTag * flex_tag = new GeoNameTag(lname.str());

	      double width = widthInit - widthTrans;
	      
	      if(iSec>sec0&&(iSec-sec0)%2==0) width=width-widthPrevious;
	      if((iSec-sec0)%2==1) widthPrevious=width;
	      
	      //	      std::cout<<"STAVE FLEX : width "<<iSec<<" "<<widthInit<<" -> "<<width<<"   "<<thick<<std::endl;
	      
	      if(width>0)
		{

		  GeoBox *box = new GeoBox(thick*.5-0.001, width*.5, length*.5-0.001);
		  HepGeom::Point3D<double> box_pos( xFlexPos+thick*.5, omega2y+width*.5, (zmin+zmax)*.5 );
		  GeoTransform* box_xform = new GeoTransform(HepGeom::Translate3D(box_pos));
		  
		  //std::string matName = "pix::"+flexMatName[iSec];
		  std::string matName = "std::Copper";
		  //		  std::cout<<"FLEX stave material "<<matName<<std::endl;
		  const GeoMaterial* flex_material = matMgr()->getMaterial(matName);
		  GeoLogVol* flex_logVol = new GeoLogVol("Flex",box,flex_material);
		  GeoPhysVol * flex_logVolPV = new GeoPhysVol(flex_logVol);
		  
		  m_physVol->add(flex_tag);
		  m_physVol->add(box_xform);
		  m_physVol->add(flex_logVolPV);
		  
		  HepGeom::Point3D<double> box_pos_C( xFlexPos+thick*.5, omega2y+width*.5, -(zmin+zmax)*.5 );
		  GeoTransform* box_xform_C = new GeoTransform(HepGeom::Translate3D(box_pos_C));
		  m_physVol->add(flex_tag);
		  m_physVol->add(box_xform_C);
		  m_physVol->add(flex_logVolPV);	 
		  
// 		  double scaleFactor=width/widthInit;
// 		  double vCopper = wgCu[iSec]*scaleFactor;
// 		  double vAdh = wgAdh[iSec]*scaleFactor;
// 		  double vPoly = wgPoly[iSec]*scaleFactor;
// 		  double vTotInv = 1./(vCopper+vAdh+vPoly);
// 		  double density = 1./((vCopper*vTotInv)/8.96+(vAdh*vTotInv)/1.14+(vPoly*vTotInv)/1.42);
// 		  std::cout<<"FLEX material "<<lname.str()<<" : "<<vCopper*vTotInv<<" "<<vAdh*vTotInv<<" "<<vPoly*vTotInv<<" -> "<<density<<std::endl;
		  
// 		  wgCuMin=std::min(wgCuMin,vCopper*vTotInv);
// 		  wgCuMax=std::max(wgCuMax,vCopper*vTotInv);

// 		  regVolCu += wgCu[iSec]*(1.-scaleFactor);
// 		  regVolAdh +=  wgAdh[iSec]*(1.-scaleFactor);
// 		  regVolPoly +=  wgPoly[iSec]*(1.-scaleFactor);

		}

	      if(iSec==sec1-1)
		{
		  zPosTrans.push_back(zmax);
		  yWidthTrans.push_back(width);
		  widthTrans+=width;

// 		  copperVolTrans.push_back(regVolCu);
// 		  adhVolTrans.push_back(regVolAdh);
// 		  polyVolTrans.push_back(regVolPoly);
		}
	    }
	}

//       volCuMin-=0.001;
//       volCuMax+=0.001;
//       std::cout<<"FLEX material Cu min max "<<volCuMin<<" "<<volCuMax<<std::endl;

//       widthTrans = 0;      
//       std::vector<std::string> matList;
//       for(int iRegion=0; iRegion<nbRegion; iRegion++)
// 	{
// 	  int sec0 = (iRegion==0)? 0 : transRegion[iRegion-1];
// 	  int sec1 = (iRegion==nbRegion-1) ? flexSections: transRegion[iRegion];
// 	  double widthPrevious = 0;

// 	  int iCmpt = 0;
// 	  for(int iSec=sec0; iSec<sec1; iSec++)
// 	    {
// 	      double zmin = flexZmin[iSec];
// 	      double zmax = flexZmax[iSec];
// 	      double length = flexLength[iSec];
// 	      double thick = flexThick[0];
// 	      double widthInit = wgCu[iSec]/(length*thickCu);

// 	      if(iRegion==nbRegion-1)
//  		{ widthPrevious=0.; } 

// 	      double width = widthInit - widthTrans;
	      
// 	      std::ostringstream lname;
// 	      lname << "R" << iRegion<<"S"<<iCmpt;
// 	      iCmpt++;

// 	      if(iSec>sec0&&(iSec-sec0)%2==0) width=width-widthPrevious;
// 	      if((iSec-sec0)%2==1) widthPrevious=width;
	      
// 	      std::cout<<"STAVE FLEX : width "<<iSec<<" "<<widthInit<<" -> "<<width<<"   "<<thick<<std::endl;
	      
// 	      if(width>0)
// 		{
		  
// 		  double scaleFactor=width/widthInit;
// 		  double vCopper = wgCu[iSec]*scaleFactor;
// 		  double vAdh = wgAdh[iSec]*scaleFactor;
// 		  double vPoly = wgPoly[iSec]*scaleFactor;
// 		  double vTotInv = 1./(vCopper+vAdh+vPoly);
// 		  double density = 1./((vCopper*vTotInv)/8.96+(vAdh*vTotInv)/1.14+(vPoly*vTotInv)/1.42);
// 		  std::cout<<"FLEX material "<<lname.str()<<" : "<<vCopper*vTotInv<<" "<<vAdh*vTotInv<<" "<<vPoly*vTotInv<<" -> "<<density<<std::endl;
		  
// 		  double tmp=vCopper*vTotInv;
// 		  int nbStep=5;
// 		  double delta=(wgCuMax-wgCuMin)/(float)nbStep;
// 		  int index=-1;
// 		  for(int i=0; i<nbStep&&index<0; i++)
// 		    if( (tmp-(wgCuMin+i*delta))*(tmp-(wgCuMin+(i+1)*delta))<0) index=i;

// 		  if(index<0)std::cout<<"FLEX material index issue : "<<tmp<<std::endl;

// 		  std::ostringstream matName;
// 		  matName << "L"<<m_layer<<"_FlexMat" <<index;
// 		  matList.push_back(matName.str());
// 		}
// 	      else
// 		  matList.push_back("none");

// 	      if(iSec==sec1-1)
// 		{
// 		  widthTrans+=width;
// 		}

// 	    }
// 	}

//       std::cout<<"FLEX material Cu min max "<<wgCuMin<<" "<<wgCuMax<<std::endl;

//       int nbStep=5;
//       double delta=(wgCuMax-wgCuMin)/(float)nbStep;

//       for(int iStep=0; iStep<nbStep; iStep++)
// 	{
// 	  double volCopper = volCuMin+(iStep+.5)*delta;
	  
// 	  double vMin=9999.;
// 	  int index=-1;
// 	  for(int i=0; i<flexSections; i++)
// 	    {
// 	      double vCopper = wgCu[i];
// 	      double vAdh = wgAdh[i];
// 	      double vPoly = wgPoly[i];
// 	      double vTotInv = 1./(vCopper+vAdh+vPoly);
	      
// 	      double tmp=vCopper*vTotInv;
// 	      if(fabs(volCopper-tmp)<vMin)
// 		{
// 		  vMin = fabs(volCopper-tmp);
// 		  index =i;
// 		}
// 	    }

// 	  double vCopper = wgCu[index];
// 	  double vAdh = wgAdh[index];
// 	  double vPoly = wgPoly[index];
// 	  double vTotInv = 1./(vCopper+vAdh+vPoly);
// 	  double density = 1./((vCopper*vTotInv)/8.96+(vAdh*vTotInv)/1.14+(vPoly*vTotInv)/1.42);
	  
// 	  std::cout<<"MATERIAL "<<iStep<<"  "<<vCopper*vTotInv<<" "<<vAdh*vTotInv+vPoly*vTotInv<<" -> "<<density<<std::endl;
	  
// 	}



//       for(int i=0; i<flexSections; i++)
// 	{
// 	  std::cout<<matList[i]<<" ";
// 	}
//       std::cout<<std::endl;

      for(int iRegion=0; iRegion<nbRegion-2; iRegion++)
	{
	  double zmin=zPosTrans[iRegion];
	  double zmax=zPosTrans[nbRegion-1];
	  double length=zmax-zmin;
	  double width=yWidthTrans[iRegion];
	  double thick = flexThick[0];

	  //	  std::cout<<"STAVE FLEX - additionnal : "<<iRegion<<" : "<<zmin<<" "<<zmax<<" "<<width<<std::endl;

	  GeoBox *box = new GeoBox(thick*.5, width*.5, length*.5);
	  HepGeom::Point3D<double> box_pos( xFlexPos+thick*(iRegion+1.5), omega2y+width*.5, (zmin+zmax)*.5 );
	  GeoTransform* box_xform = new GeoTransform(HepGeom::Translate3D(box_pos));
	  
	  GeoNameTag* flex_tag = new GeoNameTag("StaveFlex");

	  //	  std::string matName = "pix::"+flexMatNameTrans[iRegion];
	  std::string matName = "std::Copper";
	  //	  std::cout<<"FLEX stave material "<<matName<<std::endl;
	  const GeoMaterial* flex_material = matMgr()->getMaterial(matName);
	  GeoLogVol* flex_logVol = new GeoLogVol("Flex",box,flex_material);
	  GeoPhysVol * flex_logVolPV = new GeoPhysVol(flex_logVol);
	  
	  m_physVol->add(flex_tag);
	  m_physVol->add(box_xform);
	  m_physVol->add(flex_logVolPV);

	  HepGeom::Point3D<double> box_pos_C( xFlexPos+thick*(iRegion+1.5), omega2y+width*.5, -(zmin+zmax)*.5 );
	  GeoTransform* box_xform_C = new GeoTransform(HepGeom::Translate3D(box_pos_C));
	  m_physVol->add(flex_tag);
	  m_physVol->add(box_xform_C);
	  m_physVol->add(flex_logVolPV);	 

// 	  double scaleFactor=1.;
// 	  double vCopper = copperVolTrans[iRegion];
// 	  double vAdh = adhVolTrans[iRegion];
// 	  double vPoly = polyVolTrans[iRegion];
// 	  double vTotInv = 1./(vCopper+vAdh+vPoly);
// 	  double density = 1./((vCopper*vTotInv)/8.96+(vAdh*vTotInv)/1.14+(vPoly*vTotInv)/1.42);

// 	  std::cout<<"FLEX material - transition "<<iRegion<<" : "<<vCopper*vTotInv<<" "<<vAdh*vTotInv<<" "<<vPoly*vTotInv<<" -> "<<density<<std::endl;

// 	  double tmp=vCopper*vTotInv;
// 	  int nbStep=5;
// 	  double delta=(wgCuMax-wgCuMin)/(float)nbStep;
// 	  int index=-1;
// 	  for(int i=0; i<nbStep&&index<0; i++)
// 	    if( (tmp-(wgCuMin+i*delta))*(tmp-(wgCuMin+(i+1)*delta))<0) index=i;
	  
// 	  if(index<0)std::cout<<"FLEX material index issue : "<<tmp<<std::endl;
	  
// 	  std::ostringstream matName;
// 	  matName << "L"<<m_layer<<"_FlexMat" <<index;
// 	  matList.push_back(matName.str());
	  
// 	  std::cout<<"FLEX material - transition "<<iRegion<<" : "<<matName.str()<<std::endl;
	  
	}
    }

  
  // ------------------------------------------------------------------------------------------------------------
  // ------------------------------------------------------------------------------------------------------------
  // Face plate  
  // ------------------------------------------------------------------------------------------------------------
  // ------------------------------------------------------------------------------------------------------------

  if(bBarrelStave && m_FacePlateGreaseThick>0.001)
    {
      GeoBox *grease_box = new GeoBox(m_FacePlateGreaseThick*0.5-safetyMarginZ,MechanicalStaveWidth*0.5,MechanicalStaveLength*.5-safetyMarginZ);
      CLHEP::Hep3Vector grease_pos( m_FacePlateGreaseThick*.5, 0., 0.);
      HepGeom::Transform3D grease_trf = trf*HepGeom::Transform3D(CLHEP::HepRotation(),grease_pos);
      GeoTransform*grease_xform = new GeoTransform(grease_trf);

      GeoNameTag* grease_tag = new GeoNameTag("FacePlateGrease");
      const GeoMaterial* grease_material = matMgr()->getMaterial("pix::ThermGrease");
      GeoLogVol * grease_logVol = new GeoLogVol("FacePlateGrease",grease_box,grease_material);
      GeoPhysVol * grease_logVolPV = new GeoPhysVol(grease_logVol);

      if(m_bGeoComp){
	InDet::GeoComponent *grease = new InDet::GeoComponent(grease_logVolPV);
	m_geoVol->placeAsChild( *grease, grease_trf, 0);
      }
      else {
	m_physVol->add(grease_tag);
	m_physVol->add(grease_xform);
	m_physVol->add(grease_logVolPV);      
      }
    }

  if(bBarrelStave) {

    //    std::cout<<"BARRELSTAVE "<<m_layer<<std::endl;

    //    double realFacePlateThick = m_FacePlateThick-m_FacePlateGreaseThick-FacePlateGlueThick;
    
    GeoBox *faceplate_box = new GeoBox(m_FacePlateThick*0.5-safetyMarginZ,MechanicalStaveWidth*0.5,MechanicalStaveLength*.5-safetyMarginZ);
    CLHEP::Hep3Vector faceplate_pos( m_FacePlateGreaseThick+m_FacePlateThick*.5, 0., 0.);
    HepGeom::Transform3D faceplate_trf = trf*HepGeom::Transform3D(CLHEP::HepRotation(),faceplate_pos);
    GeoTransform*faceplate_xform = new GeoTransform(faceplate_trf);
    
    GeoNameTag* faceplate_tag = new GeoNameTag("FacePlate");
    const GeoMaterial* faceplate_material = matMgr()->getMaterial("pix::FacePlate");
    GeoLogVol * faceplate_logVol = new GeoLogVol("FacePlate",faceplate_box,faceplate_material);
    GeoPhysVol * faceplate_logVolPV = new GeoPhysVol(faceplate_logVol);

    //    DumpVolumeAndMaterial(faceplate_logVol,MechanicalStaveLength);
    
    if(m_bGeoComp){
      InDet::GeoComponent *faceplate = new InDet::GeoComponent(faceplate_logVolPV);
      m_geoVol->placeAsChild( *faceplate, faceplate_trf, 0);
    }
    else {
      m_physVol->add(faceplate_tag);
      m_physVol->add(faceplate_xform);
      m_physVol->add(faceplate_logVolPV);      
    }

    if(FacePlateGlueThick>0.001)
      {
	GeoBox *glue_box = new GeoBox(FacePlateGlueThick*0.5-safetyMarginZ,MechanicalStaveWidth*0.5,MechanicalStaveLength*.5-safetyMarginZ);
	CLHEP::Hep3Vector glue_pos( m_FacePlateGreaseThick+m_FacePlateThick+FacePlateGlueThick*.5, 0., 0.);
	HepGeom::Transform3D glue_trf = trf*HepGeom::Transform3D(CLHEP::HepRotation(),glue_pos);
	GeoTransform*glue_xform = new GeoTransform(glue_trf);
	
	GeoNameTag* glue_tag = new GeoNameTag("FacePlateGlue");
	const GeoMaterial* glue_material = matMgr()->getMaterial("pix::Stycast2850FT");
	GeoLogVol * glue_logVol = new GeoLogVol("FacePlateGlue",glue_box,glue_material);
	GeoPhysVol * glue_logVolPV = new GeoPhysVol(glue_logVol);

	if(m_bGeoComp){
	  InDet::GeoComponent *glue = new InDet::GeoComponent(glue_logVolPV);
	  m_geoVol->placeAsChild( *glue, glue_trf, 0);
	}
	else {
	  m_physVol->add(glue_tag);
	  m_physVol->add(glue_xform);
	  m_physVol->add(glue_logVolPV);      
	}
      }
   }
  
  else {    // endcap stave
    
    const GeoShape * lastShape = 0;
    const GeoShape * lastShape_glue = 0;
    
    double moduleLength = m_brlModule->Length()*.5;
    double zMargin = 0.0;
    double zPosPrevious = m_endcapModPos[0].z()+m_FoamBaseThick+.7;
    for(int iFoam=1; iFoam<(int)m_endcapModPos.size(); iFoam++){
      
      double zPosNew = m_endcapModPos[iFoam].z();
      double zModulePos = m_zEndOfBrlModulePos+moduleLength*.5;
      
      double yPos= 0.;
      double fpWidth = MechanicalStaveWidth*0.5;
      if(iFoam==1){
	yPos = -MechanicalStaveWidth*.25;
	fpWidth*=.5-0.01;
      }
      
      double zMin = zPosPrevious;
      double zMax = zPosNew-zMargin;
      if(iSide<0)
	{ double tmp = zMax; zMax= -zMin;  zMin=-tmp;  zModulePos*=-1; }
      //	{ zMin = -zPosNew; zMax=-zPosPrevious; zModulePos*=-1; }
      
      GeoShape *faceplate_box = new GeoBox(m_FacePlateThick*0.5-safetyMarginZ,fpWidth,(zMax-zMin)*.5);
      CLHEP::Hep3Vector faceplate_pos( 0., yPos, (zMin+zMax)*.5);
      HepGeom::Transform3D faceplate_trf = HepGeom::Transform3D(CLHEP::HepRotation(),faceplate_pos);
      lastShape = addShape(lastShape, faceplate_box, faceplate_trf); 
      
      if(iFoam==1){
	GeoShape *faceplate_box = new GeoBox(m_FacePlateThick*0.5-safetyMarginZ,fpWidth,moduleLength*.5-0.01);
	CLHEP::Hep3Vector faceplate_pos( 0., -yPos, zModulePos);
	HepGeom::Transform3D faceplate_trf = HepGeom::Transform3D(CLHEP::HepRotation(),faceplate_pos);
	lastShape = addShape(lastShape, faceplate_box, faceplate_trf); 
      }
      
      if(FacePlateGlueThick>0.001)
	{
	  GeoShape *faceplate_box = new GeoBox(FacePlateGlueThick*0.5-safetyMarginZ,fpWidth,(zMax-zMin)*.5);
	  CLHEP::Hep3Vector faceplate_pos( 0., yPos, (zMin+zMax)*.5);
	  HepGeom::Transform3D faceplate_trf = HepGeom::Transform3D(CLHEP::HepRotation(),faceplate_pos);	  
	  lastShape_glue = addShape(lastShape_glue, faceplate_box, faceplate_trf); 
	  
	  if(iFoam==1){
	    GeoShape *faceplate_box = new GeoBox(FacePlateGlueThick*0.5-safetyMarginZ,fpWidth,moduleLength*.5-0.01);
	    CLHEP::Hep3Vector faceplate_pos( 0., -yPos, zModulePos );
	    HepGeom::Transform3D faceplate_trf = HepGeom::Transform3D(CLHEP::HepRotation(),faceplate_pos);
	    lastShape_glue = addShape(lastShape_glue, faceplate_box, faceplate_trf); 
	  }
	}
      
      zPosPrevious = zPosNew+zMargin+m_FoamBaseThick+.7;
      
    }
    
    CLHEP::Hep3Vector faceplate_pos( m_FacePlateGreaseThick+m_FacePlateThick*.5, 0., 0.);
    HepGeom::Transform3D faceplate_trf = HepGeom::Transform3D(CLHEP::HepRotation(),faceplate_pos);
    GeoTransform*faceplate_xform = new GeoTransform(faceplate_trf);
    
    GeoNameTag* faceplate_tag = new GeoNameTag("FacePlate_EC");
    const GeoMaterial* faceplate_material = matMgr()->getMaterial("pix::FacePlate");
    GeoLogVol * faceplate_logVol = new GeoLogVol("FacePlate_EC",lastShape,faceplate_material);
    GeoPhysVol * faceplate_logVolPV = new GeoPhysVol(faceplate_logVol);
    
    if(m_bGeoComp){
      InDet::GeoComponent *faceplate = new InDet::GeoComponent(faceplate_logVolPV);
      m_geoVol->placeAsChild( *faceplate, faceplate_trf, 0);
    }
    else {
      m_physVol->add(faceplate_tag);
      m_physVol->add(faceplate_xform);
      m_physVol->add(faceplate_logVolPV);      
    }
    
    if(FacePlateGlueThick>0.001) {
      
      CLHEP::Hep3Vector faceplate_pos( m_FacePlateGreaseThick+m_FacePlateThick+FacePlateGlueThick*.5, 0., 0.);
      HepGeom::Transform3D faceplate_trf = HepGeom::Transform3D(CLHEP::HepRotation(),faceplate_pos);
      GeoTransform*faceplate_xform = new GeoTransform(faceplate_trf);
      
      GeoNameTag* faceplate_tag = new GeoNameTag("FacePlateGlue_EC");
      const GeoMaterial* faceplate_material = matMgr()->getMaterial("pix::Stycast2850FT");
      GeoLogVol * faceplate_logVol = new GeoLogVol("FacePlateGlue_EC",lastShape_glue,faceplate_material);
      GeoPhysVol * faceplate_logVolPV = new GeoPhysVol(faceplate_logVol);
      
      if(m_bGeoComp){
	InDet::GeoComponent *faceplate = new InDet::GeoComponent(faceplate_logVolPV);
	m_geoVol->placeAsChild( *faceplate, faceplate_trf, 0);
      }
      else {
	m_physVol->add(faceplate_tag);
	m_physVol->add(faceplate_xform);
	m_physVol->add(faceplate_logVolPV);      
      }
    }
  }  
  
  // ------------------------------------------------------------------------------------------------------------
  // ------------------------------------------------------------------------------------------------------------
  // Cooling pipe
  // ------------------------------------------------------------------------------------------------------------
  // ------------------------------------------------------------------------------------------------------------
  const GeoTube* coolingPipeGlue = new GeoTube(TubeOuterDiam*.5,TubeOuterDiam*.5+TubeGlueThick,MechanicalStaveLength*0.5);
  const GeoTube* coolingPipe = new GeoTube(TubeInnerDiam*.5,TubeOuterDiam*0.5-safetyMarginZ,MechanicalStaveLength*0.5);
  const GeoTube* coolingPipeInner = new GeoTube(0.0,TubeInnerDiam*0.5-safetyMarginZ,MechanicalStaveLength*0.5);


  msg(MSG::INFO)<<"** TUBE : colling pipe glue "<<TubeGlueThick<<endreq;
  const GeoMaterial* cp_glue_material = matMgr()->getMaterial("pix::Stycast2850FT");
  GeoLogVol* cp_glue_logVol = new GeoLogVol("CoolingPipeGlue",coolingPipeGlue,cp_glue_material);
  GeoPhysVol * cp_glue_logPV = new GeoPhysVol(cp_glue_logVol);

  GeoNameTag* cp_glue_tag = new GeoNameTag("CoolingPipeGlue");
  CLHEP::Hep3Vector cp_glue_pos(xGblOffset+TubeMiddlePos,TubeCentralPos,0.0);
  HepGeom::Transform3D cp_glue_trf = HepGeom::Transform3D(CLHEP::HepRotation(),cp_glue_pos);
  GeoTransform* cp_glue_xform = new GeoTransform(cp_glue_trf);

  if(m_bGeoComp){
    InDet::GeoComponent *cp_glue = new InDet::GeoComponent(cp_glue_logPV);
    foam_geocomp->placeAsChild_insert( *cp_glue, cp_glue_trf, 0);
  }
  else {
    foam_logVolPV->add(cp_glue_tag);
    foam_logVolPV->add(cp_glue_xform);
    foam_logVolPV->add(cp_glue_logPV);
  }
  
  msg(MSG::INFO)<<"** TUBE : without Stycast "<<TubeGlueThick<<endreq;
  const GeoMaterial* cp_material = matMgr()->getMaterial("pix::CoolingPipe");
  GeoLogVol* cp_logVol = new GeoLogVol("CoolingPipe",coolingPipe,cp_material);
  GeoPhysVol * cp_logPV = new GeoPhysVol(cp_logVol);
  
  //  DumpVolumeAndMaterial(cp_logVol,MechanicalStaveLength);

  GeoNameTag* cp_tag = new GeoNameTag("CoolingPipe");
  CLHEP::Hep3Vector cp_pos(0.0,0.0,0.0);
  //  GeoTransform* cp_xform = new GeoTransform(HepGeom::Transform3D(CLHEP::HepRotation(),cp_pos));
//   cp_glue_logPV->add(cp_tag);
//   cp_glue_logPV->add(cp_xform);
//   cp_glue_logPV->add(cp_logPV);

  if(m_bGeoComp){
    InDet::GeoComponent *cp = new InDet::GeoComponent(cp_logPV);
    foam_geocomp->placeAsChild_insert( *cp, cp_glue_trf, 0);
  }
  else {
    foam_logVolPV->add(cp_tag);
    foam_logVolPV->add(cp_glue_xform);
    foam_logVolPV->add(cp_logPV);
  }

  //  const GeoTube* coolingPipeInner = new GeoTube(0.0,TubeInnerDiam*0.5,MechanicalStaveLength*0.5);
  const GeoMaterial* cp_inner_material = matMgr()->getMaterial("pix::CoolingFluid");
  GeoLogVol * cp_inner_log = new GeoLogVol("CoolingPipeInner",coolingPipeInner,cp_inner_material);
  GeoPhysVol * cp_inner_logPV = new GeoPhysVol(cp_inner_log);

  //  DumpVolumeAndMaterial(cp_inner_log,MechanicalStaveLength);
  
  GeoNameTag* cp_inner_tag = new GeoNameTag("CoolingPipeInner");
  CLHEP::Hep3Vector cp_inner_pos(0.0,0.0,0.0);
  //  GeoTransform* cp_inner_xform = new GeoTransform(HepGeom::Transform3D(CLHEP::HepRotation(),cp_inner_pos));
//   cp_logPV->add(cp_inner_tag);
//   cp_logPV->add(cp_inner_xform);
//   cp_logPV->add(cp_inner_logPV);
  if(m_bGeoComp){
    InDet::GeoComponent *cp_inner = new InDet::GeoComponent(cp_inner_logPV);
    foam_geocomp->placeAsChild_insert( *cp_inner, cp_glue_trf, 0);
  }
  else {
    foam_logVolPV->add(cp_inner_tag);
    foam_logVolPV->add(cp_glue_xform);
    foam_logVolPV->add(cp_inner_logPV);
  }

  if(TubeCentralPos>0.01)
    {
      GeoNameTag* cp_glue_tag = new GeoNameTag("CoolingPipe");
      CLHEP::Hep3Vector cp_glue_pos(xGblOffset+TubeMiddlePos,-TubeCentralPos,0.0);
      HepGeom::Transform3D cp_glue_trf = HepGeom::Transform3D(CLHEP::HepRotation(),cp_glue_pos);
      GeoTransform* cp_glue_xform = new GeoTransform(cp_glue_trf);
      
      if(m_bGeoComp){
	InDet::GeoComponent *cp_glue = new InDet::GeoComponent(cp_glue_logPV);
	foam_geocomp->placeAsChild_insert( *cp_glue, cp_glue_trf, 0);
      }
      else {
	foam_logVolPV->add(cp_glue_tag);
	foam_logVolPV->add(cp_glue_xform);
	foam_logVolPV->add(cp_glue_logPV);
      }
      
      //     cp_glue_logPV->add(cp_tag);
      //     cp_glue_logPV->add(cp_xform);
      //     cp_glue_logPV->add(cp_logPV);
      if(m_bGeoComp){
	InDet::GeoComponent *cp = new InDet::GeoComponent(cp_logPV);
	foam_geocomp->placeAsChild_insert( *cp, cp_glue_trf, 0);
      }
      else {
	foam_logVolPV->add(cp_tag);
	foam_logVolPV->add(cp_glue_xform);
	foam_logVolPV->add(cp_logPV);
      }
      
      //     cp_logPV->add(cp_inner_tag);
      //     cp_logPV->add(cp_inner_xform);
      //     cp_logPV->add(cp_inner_logPV);
      if(m_bGeoComp){
	InDet::GeoComponent *cp_inner = new InDet::GeoComponent(cp_inner_logPV);
	foam_geocomp->placeAsChild_insert( *cp_inner, cp_glue_trf, 0);
      }
      else {
	foam_logVolPV->add(cp_inner_tag);
	foam_logVolPV->add(cp_glue_xform);
	foam_logVolPV->add(cp_inner_logPV);
      }
    }
  
  if(m_bGeoComp){
    m_geoVol->placeAsChild( *foam_geocomp, foam_trf, 0);
  }
  else {
    m_physVol->add(foam_tag);
    m_physVol->add(foam_xform);
    m_physVol->add(foam_logVolPV);
  }
  
  // ------------------------------------------------------------------------------------------------------------
  // ------------------------------------------------------------------------------------------------------------
  // Compute (stave + module) envelopp, stave HEPtransform and stave thickness N&P 
  // ------------------------------------------------------------------------------------------------------------
  // ------------------------------------------------------------------------------------------------------------
  double pmThicknessN = m_brlModule->ThicknessN();
  double pmThicknessP = m_brlModule->ThicknessP();
  double pmWidth = m_brlModule->Width();
  double pmThicknessN3D = m_maxTiltedModuleThick;
  double pmThicknessP3D = m_brlModule->ThicknessP();
  double pmShilftLateral3D = 0.;
  
  computeStaveEnvelopTransformAndSize(pmThicknessN,pmThicknessP,pmWidth,
				      pmThicknessN3D,pmThicknessP3D,pmShilftLateral3D);
  
  //  const GeoMaterial* box_material = matMgr()->getMaterial("std::Air");
  m_width = m_staveBarrelWidth;
  m_length = MechanicalStaveLength_total;
  
  
  //   if(m_bVerbose){
  //     msg(MSG::DEBUG)<<"Max thickness N/P : "<<m_thicknessN<<" "<<m_thicknessP<<endreq;
  //     msg(MSG::DEBUG)<<"Max thickness P : "<<m_thicknessP+layerRadius<<" "<<flexWingMaxR<<endreq;
  //     msg(MSG::DEBUG)<<"Max thickness N : "<<-m_thicknessN+layerRadius<<" "<<-m_thicknessN-radialShiftThickN+layerRadius<<" "<<radialShiftThickN<<endreq;
  //   }
  
  //   if(bFlexAndWing){
  //     if(flexWingMaxR>layerRadius+m_thicknessP)m_thicknessP=flexWingMaxR-layerRadius;
  //     m_thicknessP+=0.11;                        // SES - fixme
  //   }
  //  m_thicknessN+=radialShiftThickN+.11;                 // SES - fixme
  
  //  if(m_bVerbose)msg(MSG::DEBUG)<<"Max thickness -> output N/P : "<<m_thicknessN<<" "<<m_thicknessP<<endreq;
  
  //**********  return stave phys. vol. (= face plate + foam + cable flex + end blocks) *********************
      
      //  return logVolPV;
      
      }
  

void GeoPixelStaveSupportAlpineBuilder::computeStaveEnvelopTransformAndSize(double moduleThickN,double moduleThickP, double moduleWidth,
								     double moduleThickN3D,double /*moduleThickP3D*/, double pmShilftLateral3D)
{

  // **** Global envelop including module and stave *****************************************************
 
  // Offsets have to be defined at the begnning of Build process 
  //   -> these parameters are used to compute staveEnvelopShape

//   double OmegaThick = getDouble("PixelStaveSupport", layerIndex, "OmegaThick").c_str());
//   double OmegaGlueThick = getDouble("PixelStaveSupport", layerIndex, "OmegaGlueThick").c_str());
//   double OmegaMiddleThick = getDouble("PixelStaveSupport", layerIndex, "OmegaMiddleThick").c_str());

//   double MechanicalStaveOffset = IBLStaveMechanicalStaveOffset();
//   double StaveModuleOffset = gmt_mgr->IBLStaveMechanicalStaveModuleOffset();
//   double FacePlateThick = gmt_mgr->IBLStaveFacePlateThickness(); 
//   double m_FacePlateGreaseThick = gmt_mgr->IBLStavem_FacePlateGreaseThickness();
//   if(m_FacePlateGreaseThick>0.001) FacePlateThick+=m_FacePlateGreaseThick;

  double stave_xOffset=m_StaveModuleOffset+moduleThickP;
  double stave_yOffset=m_MechanicalStaveOffset;
  msg(MSG::INFO)<<" stave xoffset = "<< stave_xOffset <<endreq; 
  msg(MSG::INFO)<<" stave yoffset = "<< stave_yOffset <<endreq; 
  m_transform = HepGeom::Translate3D(stave_xOffset,stave_yOffset,0);

  double staveSupportThickness=m_FacePlateTotalThick+m_omegaTotalThick+m_flexTotalThick;
  m_thicknessP = staveSupportThickness;
  m_thicknessN = (moduleThickN3D>moduleThickN)?moduleThickN3D:moduleThickN;

  msg(MSG::INFO)<<" stave minmax = "<< m_FacePlateTotalThick<<"  "<<m_omegaTotalThick<<std::endl;
  msg(MSG::INFO)<<" stave minmax P/N : "<< m_thicknessP<<" "<<m_thicknessN<<std::endl;

  // stave envelop shape (translated vs stave offsets / module)
  int nbVertices=m_basicStaveEnvelopShape->getNVertices();
  std::vector<double> xVertices, yVertices;
  for(int iPt=0; iPt<nbVertices; iPt++)
    {
      xVertices.push_back(m_basicStaveEnvelopShape->getXVertex(iPt)+stave_xOffset);
      yVertices.push_back(m_basicStaveEnvelopShape->getYVertex(iPt)+stave_yOffset);
    }

 // module shape  (supposed to be rectangular)
  double moduleThicknessN=max(moduleThickN,moduleThickN3D);
  double moduleThicknessP=moduleThickP;

  //  msg(MSG::INFO)<<"Stave envelop : module thicknesses P/N "<<moduleThicknessP<<" "<<moduleThicknessN<<endreq;

  double module1x  =-moduleThicknessN,  module1y= -moduleWidth*0.5;
  double module2x  =moduleThicknessP,  module2y= -moduleWidth*0.5;
  double module3x  =moduleThicknessP,  module3y= moduleWidth*0.5;
  double module4x  =-moduleThicknessN,  module4y= moduleWidth*0.5;

  // in case 3D module is laterally shifted (in Y)
  if(pmShilftLateral3D<0){
    module1y+=pmShilftLateral3D;
    module2y+=pmShilftLateral3D;
  }
  else{
    module3y+=pmShilftLateral3D;
    module4y+=pmShilftLateral3D;
  }

  // stick module and stave envelop together
  double halfLadderLength=m_basicStaveEnvelopShape->getDZ();
  msg(MSG::INFO)<<"Stave envelop length : "<<m_StaveLength<<" ->  "<<halfLadderLength<<endreq;
  m_staveEnvelopShape = new GeoSimplePolygonBrep(halfLadderLength);

  m_staveEnvelopShape->addVertex(module1x, module1y);
  if (fabs(sqrt((xVertices[0]-module2x)*(xVertices[0]-module2x)+(yVertices[0]-module2y)*(yVertices[0]-module2y)))>0.01)
    m_staveEnvelopShape->addVertex(module2x, module2y);
  for(int iPt=0; iPt<nbVertices; iPt++)
    m_staveEnvelopShape->addVertex(xVertices[iPt],yVertices[iPt]);
  if (fabs(sqrt((xVertices[nbVertices-1]-module3x)*(xVertices[nbVertices-1]-module3x)+(yVertices[nbVertices-1]-module3y)*(yVertices[nbVertices-1]-module3y)))>0.01)
    m_staveEnvelopShape->addVertex(module3x, module3y);
  m_staveEnvelopShape->addVertex(module4x, module4y);


 //  std::cout<<"STAVE ENVELOPE STRANSFORM AND SIZE - SHAPE "<<std::endl;
//   nbVertices=m_staveEnvelopShape->getNVertices();
//   for(int iPt=0; iPt<nbVertices; iPt++)
//     {
//       std::cout<<"  "<<iPt<<" "<<m_staveEnvelopShape->getXVertex(iPt)<<" "<<m_staveEnvelopShape->getYVertex(iPt)<<std::endl;
//     }
  
}


GeoSimplePolygonBrep* GeoPixelStaveSupportAlpineBuilder::computeStaveEnvelopShape( double safetyMargin )
{

  if(m_bVerbose)msg(MSG::DEBUG)<<"GeoSimplePolygonBrep* GeoPixelStaveSupportAlpineBuilder::computeStaveEnvelopShape( double safetyMargin )"<<endreq;

  if(safetyMargin<0 || m_staveEnvelopShape==0 || m_staveEnvelopShape->getNVertices()<=0)
    return m_staveEnvelopShape;

  // stave envelop shape (translated vs stave offsets / module)
  int nbVertices=m_staveEnvelopShape->getNVertices();
  double shapeDZ=m_staveEnvelopShape->getDZ();
  std::vector<double> xVertices, yVertices;
  for(int iPt=0; iPt<nbVertices; iPt++)
    {
      xVertices.push_back(m_staveEnvelopShape->getXVertex(iPt));
      yVertices.push_back(m_staveEnvelopShape->getYVertex(iPt));

      if(m_bVerbose)msg(MSG::DEBUG)<<"  "<<iPt<<" "<<m_staveEnvelopShape->getXVertex(iPt)<<" "<<m_staveEnvelopShape->getYVertex(iPt)<<endreq;

    }

  // Extract convex shape from stave envelopp
  GetSurroundingConvexShape(xVertices, yVertices);
  // Clean up shape points 
  RemoveCoincidentAndColinearPointsFromShape(xVertices, yVertices);
  // Add margin
  //  AddSurroundingXYMargin(safetyMargin, safetyMargin, xVertices, yVertices);
  
  // Create and return stave envelop
  GeoSimplePolygonBrep* m_convexStaveEnvelopShape = new GeoSimplePolygonBrep(shapeDZ);
  for(unsigned int iPt=0; iPt<xVertices.size(); iPt++)
    {
      m_convexStaveEnvelopShape->addVertex(xVertices[iPt],yVertices[iPt]);
      if(m_bVerbose)msg(MSG::DEBUG)<<"  "<<xVertices[iPt]<<" "<<yVertices[iPt]<<endreq;
    }

  return m_convexStaveEnvelopShape;

}


void GeoPixelStaveSupportAlpineBuilder::RemoveCoincidentAndColinearPointsFromShape(std::vector<double> &xPoint, std::vector<double >&yPoint)
{

  bool bRemovedPoint=true;
  int iMaxWhileLoop=xPoint.size();
  int iWhileLoop=0;

  // Remove coincident points from shape  ( dist < 0.01 mm)
  while(bRemovedPoint&&iWhileLoop<iMaxWhileLoop)
    {
      bRemovedPoint=false;
      int nbPoint=xPoint.size();
      for(int iPt=0; iPt<nbPoint&&!bRemovedPoint; iPt++)
	{
	  int i1=iPt;
	  int i2=(iPt+1)%(nbPoint);
	  
	  double zDist=fabs(sqrt((xPoint[i1]-xPoint[i2])*(xPoint[i1]-xPoint[i2])+(yPoint[i1]-yPoint[i2])*(yPoint[i1]-yPoint[i2])));
	  if(zDist<0.01*CLHEP::mm){
	      xPoint.erase(xPoint.begin()+i1);
	      yPoint.erase(yPoint.begin()+i1);
	      bRemovedPoint=true;
	    }
	}
      iWhileLoop++;
    }

  // Remove colinear points  (scalar product < 0.0017  (~ 0.1 deg) )
  bRemovedPoint=true;
  iMaxWhileLoop=xPoint.size();
  iWhileLoop=0;
  while(bRemovedPoint&&iWhileLoop<iMaxWhileLoop)
    {
      bRemovedPoint=false;
      int nbPoint=xPoint.size();
      for(int iPt=0; iPt<nbPoint&&!bRemovedPoint; iPt++)
	{
	  int i1=(iPt-1); if(i1<0)i1=nbPoint-1;
	  int i=iPt;
	  int i2=(iPt+1)%(nbPoint);
	  
	  double zScalProd=(xPoint[i1]-xPoint[i])*(xPoint[i2]-xPoint[i])+(yPoint[i1]-yPoint[i])*(yPoint[i2]-yPoint[i]);
	  double vNorm1=sqrt((xPoint[i1]-xPoint[i])*(xPoint[i1]-xPoint[i])+(yPoint[i1]-yPoint[i])*(yPoint[i1]-yPoint[i]));
	  double vNorm2=sqrt((xPoint[i2]-xPoint[i])*(xPoint[i2]-xPoint[i])+(yPoint[i2]-yPoint[i])*(yPoint[i2]-yPoint[i]));
	  if(fabs(zScalProd/(vNorm1*vNorm2))>1.0-0.0017)
	    {
	      xPoint.erase(xPoint.begin()+i);
	      yPoint.erase(yPoint.begin()+i);
	      bRemovedPoint=true;
	    }
	}
      iWhileLoop++;
    }

}


void GeoPixelStaveSupportAlpineBuilder::GetSurroundingConvexShape(std::vector<double> &xVertices, std::vector<double >&yVertices,
							     std::vector<int> iException)
{

  // Get convex envelop = remove points that are "inside shape"
  bool bEndPointReached=false;
  int iMaxWhileLoop=xVertices.size();
  int iWhileLoop=0;

  if(m_bVerbose)msg(MSG::DEBUG)<<"Convex shape "<<iException.size()<<endreq;

  if(iException.size()==0)
    for(int i=0; i<(int)xVertices.size(); i++)iException.push_back(0);

  std::vector<int>iRemoved;
  for(int i=0; i<(int)xVertices.size(); i++)iRemoved.push_back(0);

  // removing process is based on the sign of Z component of vector product (Pi-1,Pi)x(Pi,Pi+1)
  //     for each set of successive points Pi-1, Pi, Pi+1 
  //   => if Z component >0 : point Pi is removed
  std::vector<double> xTmp, yTmp;
  std::vector<int> iTmp;
  while(!bEndPointReached&&iWhileLoop<iMaxWhileLoop)
    {
      bool bRemovedPoint=false;
      iWhileLoop++;

      xTmp.clear(); yTmp.clear(); iTmp.clear();
      for(int i=0; i<(int)xVertices.size(); i++)
	if(iRemoved[i]==0){
	  xTmp.push_back(xVertices[i]);
	  yTmp.push_back(yVertices[i]);
	  iTmp.push_back(i);
	}

      if(m_bVerbose)msg(MSG::DEBUG)<<"---> # points "<<xTmp.size()<<endreq;

      int nbTmp=xTmp.size();
      int iPt;
      for(iPt=0; iPt<nbTmp&&!bRemovedPoint; iPt++)
	if(iException[iTmp[iPt]]==0)
	{
	  int i1=(iPt-1); if(i1<0)i1=nbTmp-1;
	  int i=iPt;
	  int i2=(iPt+1)%(nbTmp);
	  
	  double zProdVect=(xTmp[i1]-xTmp[i])*(yTmp[i2]-yTmp[i])-(xTmp[i2]-xTmp[i])*(yTmp[i1]-yTmp[i]);
	  
	  if(m_bVerbose)msg(MSG::DEBUG)<<" => "<<iPt<<" "<<zProdVect<<endreq;

	  if(zProdVect>0)
	    {
	      bRemovedPoint=true;
	      iRemoved[iTmp[iPt]]++;
	      
// 	      xVertices.erase(xVertices.begin()+iPt);
// 	      yVertices.erase(yVertices.begin()+iPt);
// 	      iException.erase(iException.begin()+iPt);
	      if(m_bVerbose)msg(MSG::DEBUG)<<" -> remove point "<<iPt<<endreq;
	      
	    }
	}
      if(!bRemovedPoint)bEndPointReached=true;
      //      bEndPointReached=true;
    }  
  
  xVertices.clear(); yVertices.clear();
  for(int i=0; i<(int)xTmp.size(); i++){
    xVertices.push_back(xTmp[i]);
    yVertices.push_back(yTmp[i]);
  }

}


void GeoPixelStaveSupportAlpineBuilder::AddSurroundingXYMargin(double vMarginX, double vMarginY, std::vector<double> &xPoint, std::vector<double>&yPoint)
{

  // For each segment (Pi-1,Pi) define parallel segment (newPi-1,newPi)
  // Parallel direction is defined by the greatest compenent of the vector perpendicular   
  //            to (Pi-1,Pi)
  int nbPoint=xPoint.size();
  std::vector<double> xEndPoint1,yEndPoint1,xEndPoint2,yEndPoint2;
  for(int iPt=0; iPt<nbPoint; iPt++)
    {
      int i1=(iPt-1); if(i1<0) i1=nbPoint-1;
      int i2=iPt;

      // Perpendicular directrion
      double xPt1=xPoint[i1]; double yPt1=yPoint[i1];
      double xPt2=xPoint[i2]; double yPt2=yPoint[i2];
      double vDirX=(xPt2-xPt1), vDirY=(yPt2-yPt1);
      double invNorm=1.0/sqrt(vDirX*vDirX+vDirY*vDirY);  
      vDirX*=invNorm; vDirY*=invNorm; 
      double vPerpDirX=vDirY, vPerpDirY=-vDirX;

      // Compute parallel segment
      double xNewPt1, yNewPt1, xNewPt2, yNewPt2;
      if(fabs(vDirX)<fabs(vDirY))   // X direcion is valid
	{
	  double coeffDir=1.0;  if(vPerpDirX<0)coeffDir=-1.0;
	  xNewPt1=xPt1+coeffDir*vMarginX;
	  yNewPt1=yPt1;
	  xNewPt2=xPt2+coeffDir*vMarginX;
	  yNewPt2=yPt2;
	}
      else   // Y direction is valid
	{
	  double coeffDir=1.0;  if(vPerpDirY<0)coeffDir=-1.0;
	  xNewPt1=xPt1;
	  yNewPt1=yPt1+coeffDir*vMarginY;
	  xNewPt2=xPt2;
	  yNewPt2=yPt2+coeffDir*vMarginY;
	}
      
      xEndPoint1.push_back(xNewPt1); yEndPoint1.push_back(yNewPt1);
      xEndPoint2.push_back(xNewPt2); yEndPoint2.push_back(yNewPt2);

    }

  // Compute intersection points of succesive (newPi-1,newPi) segments
  //    (colinear points were previously removed from shape) 
  std::vector<double> xInter, yInter;
  for(int iPt=0; iPt<nbPoint; iPt++)
    {
      int i1=iPt;
      int i2=(iPt+1)%nbPoint;

      double Ax=xEndPoint1[i1], Ay=yEndPoint1[i1];
      double Bx=xEndPoint2[i1], By=yEndPoint2[i1];
      
      double Cx=xEndPoint1[i2], Cy=yEndPoint1[i2];
      double Dx=xEndPoint2[i2], Dy=yEndPoint2[i2];
      
      double tmp=(Bx-Ax)*(Dy-Cy)-(By-Ay)*(Dx-Cx);
      double invTmp=1.0/tmp;

      double r=((Ay-Cy)*(Dx-Cx)-(Ax-Cx)*(Dy-Cy))*invTmp;
      //      double s=((Ay-Cy)*(Bx-Ax)-(Ax-Cx)*(By-Ay))*invTmp;

      double xInter1=Ax+r*(Bx-Ax);
      double yInter1=Ay+r*(By-Ay);
      //      double xInter2=Cx+s*(Dx-Cx);
      //      double yInter2=Cy+s*(Dy-Cy);

      xInter.push_back(xInter1); yInter.push_back(yInter1);
    }

  
  // New shape is definied by the intersection points defined above
  for(int iPt=0; iPt<nbPoint; iPt++)
    {      
      xPoint[iPt]=xInter[iPt];
      yPoint[iPt]=yInter[iPt];
    }

}


void GeoPixelStaveSupportAlpineBuilder::ComputeStaveExternalShape(double ratio, double margin, double offset)
{

  double staveAngle = atan(ratio);
  HepGeom::Point3D<double> vDir( cos(staveAngle), -sin(staveAngle), 0.);
  HepGeom::Point3D<double> vPerpDir( -sin(staveAngle), -cos(staveAngle), 0.);

  //  std::cout<<"Intersection vector : "<<vDir.x()<<" "<<vDir.y()<<std::endl;
  //  std::cout<<"Intersection vector : "<<vPerpDir.x()<<" "<<vPerpDir.y()<<std::endl;

  double facePlateBase = offset;

  // Omega top - end point
  omegaTopPoint = IntersectionPoint( facePlateBase + m_OmegaMiddleThick, -100.,
				     facePlateBase + m_OmegaMiddleThick,  100.,
				     facePlateBase , m_FacePlateWidth*.5,
				     facePlateBase+100.*vDir.x(), m_FacePlateWidth*.5+100.*vDir.y()
				     );

  //  std::cout<<"Intersection point - omega : "<<omegaTopPoint.x()<<"  "<<omegaTopPoint.y()<<"  "<<omegaTopPoint.z()<<std::endl;

  double deltaThick = m_OmegaThick;
  double thick = m_OmegaMiddleThick - deltaThick;

  HepGeom::Point3D<double> p11( facePlateBase + deltaThick*vPerpDir.x(), 
				m_FacePlateWidth*.5 + deltaThick*vPerpDir.y(), 
				0.);
  HepGeom::Point3D<double> p21( facePlateBase + 50.*vDir.x() + deltaThick*vPerpDir.x(), 
				m_FacePlateWidth*.5+ 50.*vDir.y() + deltaThick*vPerpDir.y(), 
				0.);
  
  omegaTopPoint_bottom = IntersectionPoint( p11.x(), p11.y(), p21.x(), p21.y(),
					    facePlateBase+thick, -100.,
					    facePlateBase+thick, 100.
					    );
  
  omegaEndPoint_bottom = IntersectionPoint( p11.x(), p11.y(), p21.x(), p21.y(),
					    facePlateBase, -100.,
					    facePlateBase, 100.
					    );
  

  //  std::cout<<"Intersection point - omega bottom  : "<<omegaTopPoint_bottom.x()<<"  "<<omegaTopPoint_bottom.y()<<"  "<<omegaTopPoint_bottom.z()<<std::endl;
  //  std::cout<<"Intersection point - omega bottom  : "<<omegaEndPoint_bottom.x()<<"  "<<omegaEndPoint_bottom.y()<<"  "<<omegaEndPoint_bottom.z()<<std::endl;

  deltaThick +=margin;
  thick = m_OmegaMiddleThick-deltaThick;
  // Omega glue
  if(m_OmegaGlueThick>0.001){

    HepGeom::Point3D<double> p11( facePlateBase + deltaThick*vPerpDir.x(), 
				 m_FacePlateWidth*.5 + deltaThick*vPerpDir.y(), 
				 0.);
    HepGeom::Point3D<double> p21( facePlateBase + 50.*vDir.x() + deltaThick*vPerpDir.x(), 
				 m_FacePlateWidth*.5+ 50.*vDir.y() + deltaThick*vPerpDir.y(), 
				 0.);
    
    glueTopPoint_top = IntersectionPoint( p11.x(), p11.y(), p21.x(), p21.y(),
				      facePlateBase+thick, -100.,
				      facePlateBase+thick, 100.
				      );
    
    glueEndPoint_top = IntersectionPoint( p11.x(), p11.y(), p21.x(), p21.y(),
				      facePlateBase, -100.,
				      facePlateBase, 100.
				      );

    deltaThick += m_OmegaGlueThick;
    thick = m_OmegaMiddleThick-deltaThick;

    HepGeom::Point3D<double> p12( facePlateBase + deltaThick*vPerpDir.x(), 
				 m_FacePlateWidth*.5 + deltaThick*vPerpDir.y(), 
				 0.);
    HepGeom::Point3D<double> p22( facePlateBase + 50.*vDir.x() + deltaThick*vPerpDir.x(), 
				 m_FacePlateWidth*.5+ 50.*vDir.y() + deltaThick*vPerpDir.y(), 
				 0.);
    
    glueTopPoint_bottom = IntersectionPoint( p12.x(), p12.y(), p22.x(), p22.y(),
					     facePlateBase+thick, -100.,
					     facePlateBase+thick, 100.
					     );
    
    glueEndPoint_bottom = IntersectionPoint( p12.x(), p12.y(), p22.x(), p22.y(),
					     facePlateBase, -100.,
					     facePlateBase, 100.
					     );

    deltaThick += margin;
    thick = m_OmegaMiddleThick-deltaThick;
  }

				       
  // Foam top - end point
  HepGeom::Point3D<double> p1( facePlateBase + deltaThick*vPerpDir.x(), 
			       m_FacePlateWidth*.5 + deltaThick*vPerpDir.y(), 
			       0.);
  HepGeom::Point3D<double> p2( facePlateBase + 50.*vDir.x() + deltaThick*vPerpDir.x(), 
			       m_FacePlateWidth*.5+ 50.*vDir.y() + deltaThick*vPerpDir.y(), 
			       0.);

  foamTopPoint = IntersectionPoint( p1.x(), p1.y(), p2.x(), p2.y(),
				    facePlateBase+thick, -100.,
				    facePlateBase+thick, 100.
				    );

  //  std::cout<<"Intersection point - foam : "<<foamTopPoint.x()<<"  "<<foamTopPoint.y()<<"  "<<foamTopPoint.z()<<std::endl;

  foamEndPoint = IntersectionPoint( p1.x(), p1.y(), p2.x(), p2.y(),
				    facePlateBase, -100.,
				    facePlateBase, 100.
				    );

  //  std::cout<<"Intersection point - foam : "<<foamEndPoint.x()<<"  "<<foamEndPoint.y()<<"  "<<foamEndPoint.z()<<std::endl;

}


HepGeom::Point3D<double> GeoPixelStaveSupportAlpineBuilder::IntersectionPoint(double Ax,double Ay,double Bx,double By,double Cx,double Cy,double Dx,double Dy)
{
  double tmp=(Bx-Ax)*(Dy-Cy)-(By-Ay)*(Dx-Cx);
  double invTmp=1.0/tmp;
    
  double r=((Ay-Cy)*(Dx-Cx)-(Ax-Cx)*(Dy-Cy))*invTmp;
    
  HepGeom::Point3D<double> inter(Ax+r*(Bx-Ax),Ay+r*(By-Ay),0.0);
    
  return inter;
}


CLHEP::Hep3Vector GeoPixelStaveSupportAlpineBuilder::NormalizeDir(CLHEP::Hep3Vector v)
{
  double tmp=1.0/sqrt(v.x()*v.x()+v.y()*v.y());
  return CLHEP::Hep3Vector(v.x()*tmp,v.y()*tmp,0.0);
}


HepGeom::Point3D<double> GeoPixelStaveSupportAlpineBuilder::NeighbourPoint_Rad(HepGeom::Point3D<double> p, CLHEP::Hep3Vector v, double delta)
{
  CLHEP::Hep3Vector vNorm=NormalizeDir(v);
  double xnew=p.x()+delta*vNorm.x();
  double ynew=p.y()+delta*vNorm.y();
  return HepGeom::Point3D<double>(xnew,ynew,0.0);
}

HepGeom::Point3D<double> GeoPixelStaveSupportAlpineBuilder::NeighbourPoint_Perp(HepGeom::Point3D<double> p, CLHEP::Hep3Vector v, double delta, int iDir)
{
  CLHEP::Hep3Vector vNorm=NormalizeDir(v);
  double xnew=p.x()-iDir*delta*vNorm.y();
  double ynew=p.y()+iDir*delta*vNorm.x();
  return HepGeom::Point3D<double>(xnew,ynew,0.0);
}

double GeoPixelStaveSupportAlpineBuilder::ComputeAngle(double ux, double uy, double vx, double vy)
{
  
  CLHEP::Hep3Vector u(ux,uy,0.0);
  CLHEP::Hep3Vector v(vx,vy,0.0);

  CLHEP::Hep3Vector uNorm=NormalizeDir(u);
  CLHEP::Hep3Vector vNorm=NormalizeDir(v);

  double pScal=uNorm.x()*vNorm.x()+uNorm.y()*vNorm.y();
  double angle=acos(pScal);

  return angle;
}

double GeoPixelStaveSupportAlpineBuilder::ComputeDistance(HepGeom::Point3D<double> p, HepGeom::Point3D<double> q)
{
  return sqrt((q.x()-p.x())*(q.x()-p.x())+(q.y()-p.y())*(q.y()-p.y()));
}


const GeoShape* GeoPixelStaveSupportAlpineBuilder::addShape(const GeoShape * lastShape, const GeoShape * nextShape, const HepGeom::Transform3D & trans)
{
  const GeoShape * shiftedShape = &(*nextShape << trans);
  if (lastShape) {
    lastShape = &(lastShape->add(*shiftedShape));
  } else {
    lastShape = shiftedShape;
  }
  return lastShape;
}



void GeoPixelStaveSupportAlpineBuilder::DumpVolumeAndMaterial(const GeoLogVol* logVol, double normFactor)
{
  
  if(m_sector>0) return;

  const GeoMaterial* geoMat=logVol->getMaterial();

  double matA = 0;
  double matZ = 0.;
  for(unsigned int iElt=0; iElt<geoMat->getNumElements(); iElt++){
    double A = geoMat->getElement(iElt)->getA();
    double Z = geoMat->getElement(iElt)->getZ();
    double fracElt = geoMat->getFraction(iElt);
    matA += (A*fracElt);
    matZ += (Z*fracElt);
  }

  std::cout<<"DumpVolumeAndMaterial :   Layer "<<m_layer<<"  Volume : "<<logVol->getName()<<"  "<<geoMat->getName()<<std::endl;

  std::cout<<"Volume par unite de longueur : "<<(logVol->getShape()->volume()/normFactor)/CLHEP::cm3<<" cm3"<<std::endl;
  std::cout<<"Material : name      "<<geoMat->getName()<<std::endl;
  std::cout<<"           densite   "<<geoMat->getDensity()/(CLHEP::g/CLHEP::mm3)<<" g/mm3"<<std::endl;
  std::cout<<"           X0        "<<geoMat->getRadLength()/(CLHEP::mm)<<" mm"<<std::endl;
  std::cout<<"           L0        "<<geoMat->getIntLength()/(CLHEP::mm)<<" mm"<<std::endl;
  std::cout<<"           A         "<<matA/(CLHEP::g/CLHEP::mole)<<" g/mole"<<std::endl;
  std::cout<<"           Z         "<<matZ<<std::endl;

  std::cout<<"-------------------------------------------------------"<<std::endl;

}


