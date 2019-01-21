/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/


// Build detailed stave support : face plate + carbon foam + cable flex + cooling pipe + end blocks
// This is built one time per layer. 

#include "GeoPixelDetailedStaveSupport.h"

#include "GeoPixelSiCrystal.h"
#include "GeoPixelModule.h"

#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoTube.h"
#include "GeoModelKernel/GeoSimplePolygonBrep.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "GeoModelKernel/GeoNameTag.h"

#include "GeoModelKernel/GeoTransform.h"
#include "GeoModelKernel/GeoDefinitions.h"
#include "GaudiKernel/PhysicalConstants.h"

#include <algorithm>
using std::max;

GeoPixelDetailedStaveSupport::GeoPixelDetailedStaveSupport()
  : m_transform(GeoTrf::Transform3D::Identity())
{
  m_staveEnvelopShape=0;
  m_bVerbose = (m_gmt_mgr->msgLvl(MSG::DEBUG));
  m_physVol = Build();

}

GeoVPhysVol* GeoPixelDetailedStaveSupport::Build() {

  m_gmt_mgr->msg(MSG::INFO) <<"Build detailed stave support description :  layout "<<m_gmt_mgr->PixelStaveLayout()<<endmsg;

  bool isBLayer = false;
  if(m_gmt_mgr->GetLD() == 0) isBLayer = true;
  int staveLayout = m_gmt_mgr->PixelStaveLayout();
  
  // Module geometry
  GeoPixelSiCrystal theSensor(isBLayer);
  GeoPixelModule pm(theSensor);
  double pmThicknessN=pm.ThicknessN_noSvc();
  double pmThicknessP=pm.ThicknessP();
  double pmWidth=pm.Width();

  //  double tmp = pm.ThicknessN();
  m_thicknessN_svc = pm.ModuleServiceThickness();
  m_width_svc = pm.ModuleServiceWidth();

  // Module geometry - Si3D
  double pmThicknessN3D=-1;
  double pmThicknessP3D=-1.;
  double pmWidth3D=-1;
  double pmShilftLateral3D=0.;
  double radialShift=0.;
  double radialShiftThickN=0.;

  if(staveLayout==5||staveLayout==6||staveLayout==7)     // 75/25 or 50/50
    {
      GeoPixelSiCrystal theSensor3D(isBLayer,true);
      GeoPixelModule pm3D(theSensor3D);

      pmThicknessN3D=pm3D.ThicknessN();
      pmThicknessP3D=pm3D.ThicknessP();
      
      if(pmThicknessP3D>pmThicknessP) radialShift = -(pmThicknessP3D-pmThicknessP);
      m_gmt_mgr->msg(MSG::INFO)<<"Si3D/SiPl radial shift : "<<radialShift<<endmsg;

      pmThicknessN3D=pm3D.ThicknessN()-radialShift;   // /!\ thicknesses in SiPl referential 
      pmThicknessP3D=pm3D.ThicknessP()+radialShift;
      pmWidth3D=pm3D.Width();

      m_gmt_mgr->msg(MSG::INFO) <<"Module thicknesses :  SiPl P/N "<<pmThicknessP<<" "<<pmThicknessN<<" "<<pmWidth<<
	"  SiPl P/N "<<pmThicknessP3D<<" "<<pmThicknessN3D<<" "<<pmWidth3D<<
	"  //  "<<pm3D.ThicknessP()<<" "<<pm3D.ThicknessN()<<endmsg;

      double MechanicalStaveOffset = m_gmt_mgr->IBLStaveMechanicalStaveOffset();
      double MechanicalStaveOffset3D = m_gmt_mgr->IBLStaveMechanicalStaveOffset(true);
      pmShilftLateral3D= -(MechanicalStaveOffset3D-MechanicalStaveOffset);

      radialShiftThickN=fabs(radialShift)+(pmThicknessN3D-pmThicknessN)*.5;
    }

  int ModuleNumber= m_gmt_mgr->IBLStaveModuleNumber_AllPlanar(); 
  int ModuleNumber_allPlanar=ModuleNumber;
  double ModuleGap= m_gmt_mgr->IBLStaveModuleGap(); 
  double ModuleLength=pm.Length();
  double MechanicalStaveEndBlockLength = m_gmt_mgr->IBLStaveMechanicalStaveEndBlockLength();
  double totalStaveLength = m_gmt_mgr->IBLStaveLength();
  int Module3DNumber= 0;
  double Module3DLength=0;

  // ------------------------------------------------------------------------------------------------------------
  // ------------------------------------------------------------------------------------------------------------
  // Module 3D geometry
  // ------------------------------------------------------------------------------------------------------------
  // ------------------------------------------------------------------------------------------------------------
  if(staveLayout==5||staveLayout==6||staveLayout==7)     // 75/25 or 50/50 or 75/25 review
    {
      // Get the 3D module description
      bool isModule3D=true;
      GeoPixelSiCrystal theSensor3D(isBLayer,isModule3D);
      GeoPixelModule pm3D(theSensor3D);     
      Module3DLength=pm3D.Length();

      if(staveLayout==5||staveLayout==7)  // 75/25
	{
	  Module3DNumber=(ModuleNumber/4)*2;
	  ModuleNumber=ModuleNumber*0.75;
	}
      else if(staveLayout==6)    // 50/50
	{
	  ModuleNumber=ModuleNumber/2;
	  Module3DNumber=ModuleNumber*2;
	}
    }

  m_PlanarModuleNumber = ModuleNumber;
  m_3DModuleNumber = Module3DNumber;
  m_gmt_mgr->msg(MSG::INFO) <<"Pixel modules "<<ModuleNumber<<" -> planar/3D : "<<m_PlanarModuleNumber<<" "<<m_3DModuleNumber<<endmsg;

  // ------------------------------------------------------------------------------------------------------------
  // ------------------------------------------------------------------------------------------------------------
  // Compute stave length : is identical for all the configration ( = IBL one)
  // ------------------------------------------------------------------------------------------------------------
  // ------------------------------------------------------------------------------------------------------------
  double lgPlanar=ModuleLength*m_PlanarModuleNumber+ModuleGap*(m_PlanarModuleNumber-1);
  double lg3D=Module3DLength*m_3DModuleNumber+ModuleGap*m_3DModuleNumber;
  double lgEndBlock=2.0*MechanicalStaveEndBlockLength;

  m_StaveLength=totalStaveLength;
  double compStaveLength=lgPlanar+lg3D+lgEndBlock;
  double lgNAStave=(totalStaveLength-compStaveLength)*0.5;

//   int ModuleNumber_allplanar= m_gmt_mgr->IBLStaveModuleNumber_AllPlanar(); 
//   double ModuleGap_allplanar= m_gmt_mgr->IBLStaveModuleGap(); 
//   m_StaveLength=ModuleLength*ModuleNumber_allplanar+ModuleGap_allplanar*(ModuleNumber_allplanar-1)+2.0*MechanicalStaveEndBlockLength;


  m_gmt_mgr->msg(MSG::INFO) <<"Module length/gap : "<<ModuleLength<<" / "<<Module3DLength<<" "<<ModuleGap<<"  -> Stave length : "<<m_StaveLength<<endmsg;
  m_gmt_mgr->msg(MSG::INFO) <<"   planar/3D/endblock/NonActive lengths : "<<lgPlanar<<" "<<lg3D<<" "<<lgEndBlock<<" "<<lgNAStave<<endmsg;

  // ------------------------------------------------------------------------------------------------------------
  // ------------------------------------------------------------------------------------------------------------
  // Stave geometry
  // ------------------------------------------------------------------------------------------------------------
  // ------------------------------------------------------------------------------------------------------------
  double FacePlateThick = m_gmt_mgr->IBLStaveFacePlateThickness(); 
  double FacePlateGreaseThick = m_gmt_mgr->IBLStaveFacePlateGreaseThickness();
  if(FacePlateGreaseThick>0.001) FacePlateThick+=FacePlateGreaseThick;

  double MechanicalStaveWidth = m_gmt_mgr->IBLStaveMechanicalStaveWidth();
  double MechanicalStaveLength = m_StaveLength;
  double MechanicalStaveOffset = m_gmt_mgr->IBLStaveMechanicalStaveOffset();
  double StaveModuleOffset = m_gmt_mgr->IBLStaveMechanicalStaveModuleOffset();

  double TubeOuterDiam = m_gmt_mgr->IBLStaveTubeOuterDiameter();
  double TubeInnerDiam = m_gmt_mgr->IBLStaveTubeInnerDiameter();
  double TubeMiddlePos = m_gmt_mgr->IBLStaveTubeMiddlePos();

  double FlexLayerThick = m_gmt_mgr->IBLStaveFlexLayerThickness();
  double FlexBaseThick = m_gmt_mgr->IBLStaveFlexBaseThickness();
  double FlexWidth = m_gmt_mgr->IBLStaveFlexWidth();
  double FlexOffset = m_gmt_mgr->IBLStaveFlexOffset();
  double FlexStaveMiddleGap = m_gmt_mgr->IBLFlexMiddleGap();
  double FlexEOS_zpos = m_gmt_mgr->IBLFlexPP0Z(0);
  double FlexDogLeg_length = m_gmt_mgr->IBLFlexDoglegLength();
  double WingWidth = m_gmt_mgr->IBLStaveFlexWingWidth();
  double WingThick = m_gmt_mgr->IBLStaveFlexWingThick();
  bool bFlexAndWing = m_gmt_mgr->IBLFlexAndWingDefined();

  bool bFlexConstantThickness = true;
  if(!bFlexAndWing) bFlexConstantThickness = false;

  double layerRadius = m_gmt_mgr->PixelLayerRadius();

  double OmegaThick = m_gmt_mgr->IBLStaveOmegaThickness();

  double FacePlateGlueThick = m_gmt_mgr->IBLStaveFacePlateGlueThickness();
  double TubeGlueThick = m_gmt_mgr->IBLStaveTubeGlueThickness();
  double OmegaGlueThick = m_gmt_mgr->IBLStaveOmegaGlueThickness();

  double MaxFlexThickness;
  if (!bFlexConstantThickness){
    if(ModuleNumber_allPlanar%2==0)
      MaxFlexThickness=FlexBaseThick+(ModuleNumber_allPlanar/2-1)*FlexLayerThick;
    else
      MaxFlexThickness=FlexBaseThick+((ModuleNumber_allPlanar-1)/2)*FlexLayerThick;
  }
  else
    MaxFlexThickness=FlexBaseThick;

  m_gmt_mgr->msg(MSG::INFO) <<"Flex thickness (max) : "<<MaxFlexThickness<<" computed for "<<ModuleNumber_allPlanar<<" modules "<<endmsg;
  
  double halfMecStaveWidth=MechanicalStaveWidth*0.5;

  // SafetyMargin
  m_SafetyMargin=.001*Gaudi::Units::mm;
  double xGblOffset=FacePlateThick+m_SafetyMargin;
  double safetyMarginZ=.001*Gaudi::Units::mm;

  // Compute approximated stave shape based on DB parameters
  ComputeStaveExternalShape();

  // if no flex width is defined, take foam module side length as default value 
  if (FlexWidth<0.001)
    {
       double xDelta=(m_FoamMiddleThick+m_OmegaMidStaveThick)-m_FoamEdgePointX;
       double yDelta=-m_FoamEdgePointY;
       FlexWidth = sqrt(xDelta*xDelta+yDelta*yDelta);
    }

  m_gmt_mgr->msg(MSG::INFO)<<" MechanicalStaveWidth  = "<<MechanicalStaveWidth <<endmsg;
  m_gmt_mgr->msg(MSG::INFO)<<" MechanicalStaveLength = "<<MechanicalStaveLength <<endmsg;
  m_gmt_mgr->msg(MSG::INFO)<<" MechanicalStaveOffset = "<<MechanicalStaveOffset <<endmsg;
  m_gmt_mgr->msg(MSG::INFO)<<" StaveModuleOffset     = "<<StaveModuleOffset <<endmsg;
  m_gmt_mgr->msg(MSG::INFO)<<" MechanicalStaveEndBlockLength = "<<MechanicalStaveEndBlockLength <<endmsg;
  m_gmt_mgr->msg(MSG::INFO)<<" FacePlateThick  = "<<FacePlateThick <<endmsg;
  m_gmt_mgr->msg(MSG::INFO)<<" TubeOuterDiam   = "<< TubeOuterDiam <<endmsg; 
  m_gmt_mgr->msg(MSG::INFO)<<" TubeInnerDiam   = "<< TubeInnerDiam <<endmsg; 
  m_gmt_mgr->msg(MSG::INFO)<<" TubeMiddlePos   = "<< TubeMiddlePos <<endmsg; 
  m_gmt_mgr->msg(MSG::INFO)<<" FlexBaseThick   = "<< FlexBaseThick <<endmsg; 
  m_gmt_mgr->msg(MSG::INFO)<<" FlexLayerThick  = "<< FlexLayerThick <<endmsg; 
  m_gmt_mgr->msg(MSG::INFO)<<" FlexWidth       = "<< FlexWidth <<endmsg; 
  m_gmt_mgr->msg(MSG::INFO)<<" FlexOffset      = "<< FlexOffset <<endmsg; 
  m_gmt_mgr->msg(MSG::INFO)<<" XOffset / ladder = "<< StaveModuleOffset <<endmsg; 

  // Adjust length of face plate/foam  depending on end block
  double MiddleSectionLength=MechanicalStaveLength-2.0*MechanicalStaveEndBlockLength;

  // Coordinate system
  //
  //    (0,0) = middle point of the face plate side near to the module
  //    x axis : perpendicular to face plate
  //    y axis : along face plate 


  // ------------------------------------------------------------------------------------------------------------
  // ------------------------------------------------------------------------------------------------------------
  // Foam shape
  // ------------------------------------------------------------------------------------------------------------
  // ------------------------------------------------------------------------------------------------------------

  double foam1x = 0.0,              foam1y = -halfMecStaveWidth+m_OmegaEndStaveThick;
  double foam2x = m_FoamEdgePointX,   foam2y = -m_FoamEdgePointY;
  double foam3x = m_FoamMiddleThick,  foam3y = 0.0;
  double foam4x = m_FoamEdgePointX,   foam4y = m_FoamEdgePointY;
  double foam5x = 0.0,              foam5y = halfMecStaveWidth-m_OmegaEndStaveThick;

  foam1x += xGblOffset;
  foam2x += xGblOffset;
  foam3x += xGblOffset;
  foam4x += xGblOffset;
  foam5x += xGblOffset;

  double FoamBaseWidth=sqrt((foam5x-foam1x)*(foam5x-foam1x)+(foam5y-foam1y)*(foam5y-foam1y));

  // ------------------------------------------------------------------------------------------------------------
  // ------------------------------------------------------------------------------------------------------------
  // Omega shape
  // ------------------------------------------------------------------------------------------------------------
  // ------------------------------------------------------------------------------------------------------------
  // other 5 points are common with Foam shape (foam 5-4-3-2-1)

  double omega1x, omega1y, omega2x, omega2y, omega3x, omega3y;
  double omega4x, omega4y, omega5x, omega5y;
  omega1x=omega1y=omega2x=omega2y=omega3x=omega3y=omega4x=omega4y=omega5x=omega5y=0.0;

  omega1x = 0.0;                                    omega1y = -halfMecStaveWidth;
  omega2x = m_OmegaEndStavePointX;                  omega2y = -m_OmegaEndStavePointY;
  omega3x = m_FoamMiddleThick+m_OmegaMidStaveThick; omega3y = 0.0;
  omega4x = m_OmegaEndStavePointX;                  omega4y = m_OmegaEndStavePointY;
  omega5x = 0.0;                                    omega5y = halfMecStaveWidth;
  
  omega1x += xGblOffset;
  omega2x += xGblOffset;
  omega3x += xGblOffset;
  omega4x += xGblOffset;
  omega5x += xGblOffset;

  // ------------------------------------------------------------------------------------------------------------
  // ------------------------------------------------------------------------------------------------------------
  // Face plate shape
  //   other 2 points are common with Foam Shape (foam1x and foam5x)
  // ------------------------------------------------------------------------------------------------------------
  // ------------------------------------------------------------------------------------------------------------

  double plate1x  =0.0,             plate1y= foam1y;
  double plate2x  =FacePlateThick,  plate2y= foam1y;
  double plate3x  =FacePlateThick,  plate3y= foam5y;
  double plate4x  =0.0,             plate4y= foam5y;

  plate1y=omega1y; plate4y=omega5y;
  
  // ------------------------------------------------------------------------------------------------------------
  // ------------------------------------------------------------------------------------------------------------
  // Cableflex shape
  // vPerpDir perpendicular direction to (foam3,foam4)
  // ------------------------------------------------------------------------------------------------------------
  // ------------------------------------------------------------------------------------------------------------

  double seg1x, seg2x, seg1y, seg2y;
  seg1x=omega3x; seg1y=omega3y;
  seg2x=omega4x; seg2y=omega4y;

  double vDirX=(seg2x-seg1x), vDirY=(seg2y-seg1y);
  double invNorm=1.0/sqrt(vDirX*vDirX+vDirY*vDirY);  
  vDirX*=invNorm; vDirY*=invNorm; 
  double vPerpDirX=vDirY, vPerpDirY=-vDirX;

  double foamMidX=m_MidStaveSidePointX+xGblOffset, foamMidY=m_MidStaveSidePointY;
  foamMidX+=(vPerpDirX*m_SafetyMargin);
  foamMidY+=(vPerpDirY*m_SafetyMargin);
  double flexHalfWidth=FlexWidth*0.5;
  double wingHalfWidth = WingWidth*.5;
  
  double flexWingMaxR=0.;
  double maxFlex1x = foamMidX-(FlexOffset+flexHalfWidth)*vDirX;
  double maxFlex1y = foamMidY-(FlexOffset+flexHalfWidth)*vDirY;
  double maxFlex2x = maxFlex1x+MaxFlexThickness*vPerpDirX;
  double maxFlex2y = maxFlex1y+MaxFlexThickness*vPerpDirY;
  double maxFlex4x = foamMidX+(-FlexOffset+flexHalfWidth)*vDirX;
  double maxFlex4y = foamMidY+(-FlexOffset+flexHalfWidth)*vDirY;
  //  double maxFlex3x = maxFlex4x+MaxFlexThickness*vPerpDirX;
  //  double maxFlex3y = maxFlex4y+MaxFlexThickness*vPerpDirY;
  double maxFlex4x_wings = foamMidX+(-FlexOffset+flexHalfWidth+2.*wingHalfWidth+.5)*vDirX;                  
  double maxFlex4y_wings = foamMidY+(-FlexOffset+flexHalfWidth+2.*wingHalfWidth+.5)*vDirY;
  double maxFlex3x_wings = maxFlex4x_wings+MaxFlexThickness*vPerpDirX;
  double maxFlex3y_wings = maxFlex4y_wings+MaxFlexThickness*vPerpDirY;

  if(m_gmt_mgr->PixelStaveAxe()==1){
    wingHalfWidth = WingWidth*.5*.6;
    maxFlex4x_wings = foamMidX+(-FlexOffset+flexHalfWidth+2.*wingHalfWidth+.5)*vDirX;                  
    maxFlex4y_wings = foamMidY+(-FlexOffset+flexHalfWidth+2.*wingHalfWidth+.5)*vDirY;
    maxFlex3x_wings = maxFlex4x_wings+MaxFlexThickness*vPerpDirX;
    maxFlex3y_wings = maxFlex4y_wings+MaxFlexThickness*vPerpDirY;
  }

  // ------------------------------------------------------------------------------------------------------------
  // ------------------------------------------------------------------------------------------------------------
  // Stave shape : face plate + foam + cableflex + omega
  //  corresponding  physical volume is returned at the end of Build()
  // ------------------------------------------------------------------------------------------------------------
  // ------------------------------------------------------------------------------------------------------------

  // create shape
  m_basicStaveEnvelopShape = new GeoSimplePolygonBrep(MiddleSectionLength*.5+safetyMarginZ);
  std::vector<double> xShape, yShape;
  std::vector<int>iShapeExcept;

  double yStaveOffset = m_gmt_mgr->IBLStaveMechanicalStaveOffset();
  double plate1y_tmp = std::min(plate1y,-pm.Width()*.5-yStaveOffset);
  double omega1y_tmp = std::min(omega1y,-pm.Width()*.5-yStaveOffset);

  xShape.push_back(plate1x);  yShape.push_back(plate1y_tmp); iShapeExcept.push_back(1);
  xShape.push_back(omega1x);   yShape.push_back(omega1y_tmp); iShapeExcept.push_back(0);
  xShape.push_back(omega2x);   yShape.push_back(omega2y); iShapeExcept.push_back(0);
  xShape.push_back(omega3x);   yShape.push_back(omega3y); iShapeExcept.push_back(0);
  
  xShape.push_back(maxFlex1x);   yShape.push_back(maxFlex1y); iShapeExcept.push_back(0);
  xShape.push_back(maxFlex2x);   yShape.push_back(maxFlex2y); iShapeExcept.push_back(0);
  
  xShape.push_back(maxFlex3x_wings);   yShape.push_back(maxFlex3y_wings); iShapeExcept.push_back(0);
  xShape.push_back(maxFlex4x_wings);   yShape.push_back(maxFlex4y_wings); iShapeExcept.push_back(0);
  
  if(bFlexAndWing){ 
    xShape.push_back(maxFlex4x);   yShape.push_back(maxFlex4y); iShapeExcept.push_back(1);
    flexWingMaxR=sqrt((maxFlex3x_wings+layerRadius)*(maxFlex3x_wings+layerRadius)+maxFlex3y_wings*maxFlex3y_wings);
  }
  
  xShape.push_back(omega4x);   yShape.push_back(omega4y); iShapeExcept.push_back(0);
  xShape.push_back(omega5x);   yShape.push_back(omega5y); iShapeExcept.push_back(0);
  
  xShape.push_back(plate4x);  yShape.push_back(plate4y); iShapeExcept.push_back(1);

  // clean up shape and store points to Brep structure

  if(m_bVerbose)m_gmt_mgr->msg(MSG::DEBUG)<<"Remove points "<<endmsg;
  GetSurroundingConvexShape(xShape, yShape, iShapeExcept);
  RemoveCoincidentAndColinearPointsFromShape( xShape, yShape);

  for(unsigned int iPt=0; iPt<xShape.size(); iPt++)
      m_basicStaveEnvelopShape->addVertex(xShape[iPt],yShape[iPt]);

  // create log and phys volumes
  const GeoMaterial* envelop_material = m_mat_mgr->getMaterial("std::Air");
  GeoLogVol * logVol = new GeoLogVol("StaveSupport",m_basicStaveEnvelopShape,envelop_material);
  GeoPhysVol * logVolPV = new GeoPhysVol(logVol);
  
  
  // ------------------------------------------------------------------------------------------------------------
  // ------------------------------------------------------------------------------------------------------------
  // Carbon Foam 
  // ------------------------------------------------------------------------------------------------------------
  // ------------------------------------------------------------------------------------------------------------
  GeoSimplePolygonBrep * foam_shape = new GeoSimplePolygonBrep(MiddleSectionLength*.5);
  foam_shape->addVertex(foam1x,foam1y);
  foam_shape->addVertex(foam2x,foam2y);
  foam_shape->addVertex(foam3x,foam3y);
  foam_shape->addVertex(foam4x,foam4y);
  foam_shape->addVertex(foam5x,foam5y);

  const GeoMaterial* foam_material = m_mat_mgr->getMaterial(m_gmt_mgr->getMaterialName("CarbonFoam",0,0));
  GeoNameTag* foam_tag = new GeoNameTag("CarbonFoam");
  GeoTransform* foam_xform = new GeoTransform(GeoTrf::Transform3D::Identity());
  
  GeoLogVol * foam_logVol = new GeoLogVol("CarbonFoam",foam_shape,foam_material);
  GeoPhysVol * foam_logVolPV = new GeoPhysVol(foam_logVol);

  // ------------------------------------------------------------------------------------------------------------
  // ------------------------------------------------------------------------------------------------------------
  // Omega 
  // ------------------------------------------------------------------------------------------------------------
  // ------------------------------------------------------------------------------------------------------------

  GeoSimplePolygonBrep * omega_shape = new GeoSimplePolygonBrep(MiddleSectionLength*.5);
  omega_shape->addVertex(omega1x,omega1y);
  omega_shape->addVertex(omega2x,omega2y);
  omega_shape->addVertex(omega3x,omega3y);
  omega_shape->addVertex(omega4x,omega4y);
  omega_shape->addVertex(omega5x,omega5y);
  omega_shape->addVertex(foam5x,foam5y);
  omega_shape->addVertex(foam4x,foam4y);
  omega_shape->addVertex(foam3x,foam3y);
  omega_shape->addVertex(foam2x,foam2y);
  omega_shape->addVertex(foam1x,foam1y); 

  GeoLogVol * omega_logVol = 0;
  // Create composite material made of omega+glue if a thickness of glue is defined is DB
  if(OmegaGlueThick<0.0001)
    {
      m_gmt_mgr->msg(MSG::INFO)<<"** OMEGA : without Stycast "<<OmegaGlueThick<<endmsg;
      const GeoMaterial* omega_material = m_mat_mgr->getMaterial("pix::Omega_IBL");
      //      const GeoMaterial* omega_material = m_mat_mgr->getMaterial(m_gmt_mgr->getMaterialName("Omega",0,0));
      omega_logVol = new GeoLogVol("Omega",omega_shape,omega_material);
    }
  else
    {
      m_gmt_mgr->msg(MSG::INFO)<<"** OMEGA : with Stycast "<<OmegaGlueThick<<endmsg;
      double omegaVolume = omega_shape->volume();
      double glueVolume = omegaVolume*(OmegaGlueThick/OmegaThick);
      const std::string compMatName="OmegaGlue_IBL";
      const GeoMaterial* omega_material = m_mat_mgr->getCompositeMaterialForVolume(compMatName,
										 omegaVolume,
										 omegaVolume,"pix::Omega_IBL",
										 glueVolume,"pix::Stycast2850FT");
      m_gmt_mgr->msg(MSG::INFO)<<"***> new material : "<<omega_material->getName()<<" "<<omega_material->getDensity()/(GeoModelKernelUnits::gram/Gaudi::Units::cm3)<<endmsg;
      omega_logVol = new GeoLogVol("Omega",omega_shape,omega_material);
    }

  GeoNameTag* omega_tag = new GeoNameTag("Omega");
  GeoTransform* omega_xform = new GeoTransform(GeoTrf::Transform3D::Identity());

  //       const GeoMaterial* omega_material = m_mat_mgr->getMaterial(m_gmt_mgr->getMaterialName("Omega",0,0));
  //       GeoNameTag* omega_tag = new GeoNameTag("Omega");
  //       GeoTransform* omega_xform = new GeoTransform(GeoTrf::Transform3D(Gaudi::Units::HepRotation(),GeoTrf::Vector3D()));
  //      GeoLogVol * omega_logVol = new GeoLogVol("Omega",omega_shape,omega_material);

  GeoPhysVol * omega_logVolPV = new GeoPhysVol(omega_logVol);
  logVolPV->add(omega_tag);
  logVolPV->add(omega_xform);
  logVolPV->add(omega_logVolPV);

  
  // ------------------------------------------------------------------------------------------------------------
  // ------------------------------------------------------------------------------------------------------------
  // Face plate  
  // ------------------------------------------------------------------------------------------------------------
  // ------------------------------------------------------------------------------------------------------------

  GeoBox * faceplate_shape = new GeoBox(FacePlateThick*0.5,MechanicalStaveWidth*0.5,MiddleSectionLength*.5);
  GeoTrf::Translate3D faceplate_pos((plate1x+plate2x+plate3x+plate4x)*0.25,(plate1y+plate2y+plate3y+plate4y)*0.25,0.0);
  GeoTransform* faceplate_xform = new GeoTransform(faceplate_pos);
  
 std::string faceplateMatName;
 const GeoMaterial* faceplate_material;

  // Create composite material made of face plate +glue if a thickness of glue is defined is DB
  if(FacePlateGlueThick<0.0001)
    {
      m_gmt_mgr->msg(MSG::INFO)<<"** FacePlate : without Stycast "<<FacePlateGlueThick<<endmsg;
      faceplateMatName = "pix::FacePlate_IBL";
      faceplate_material = m_mat_mgr->getMaterial(faceplateMatName);
    }
  else
    {
      m_gmt_mgr->msg(MSG::INFO)<<" ** FacePlate : with Stycast "<<FacePlateGlueThick<<"  grease : "<<FacePlateGreaseThick<<endmsg;
      // /!\ : faceplate thickness is already set to faceplate thickness + grease thickness
      double glueVolume = FacePlateGlueThick*MechanicalStaveWidth*MiddleSectionLength; 
      double facePlateVolume = (FacePlateThick-FacePlateGreaseThick)*MechanicalStaveWidth*MiddleSectionLength; 

      faceplateMatName="FacePlateGlue_IBL";
      faceplate_material = m_mat_mgr->getCompositeMaterialForVolume(faceplateMatName,
								  facePlateVolume,
								  facePlateVolume,"pix::FacePlate_IBL",
								  glueVolume,"pix::Stycast2850FT");
      m_gmt_mgr->msg(MSG::INFO)<<"***> new material : "<<faceplate_material->getName()<<" "<<faceplate_material->getDensity()/(GeoModelKernelUnits::gram/Gaudi::Units::cm3)<<endmsg;
    }

  // Create composite material made of faceplate + grease if a thickness of grease is defined is DB
  GeoLogVol * faceplate_logVol = 0;
  if(FacePlateGreaseThick<0.0001)
    {
      m_gmt_mgr->msg(MSG::INFO)<<"** FacePlate : without grease"<<endmsg;
      //  const GeoMaterial* faceplate_material = m_mat_mgr->getMaterial(m_gmt_mgr->getMaterialName("FacePlate",0,0));
      faceplate_logVol = new GeoLogVol("FacePlate",faceplate_shape,faceplate_material);
    }
  else
    {
      m_gmt_mgr->msg(MSG::INFO)<<"** FacePlate : with grease"<<endmsg;
      // /!\ : faceplate thickness is already set to faceplate thickness + grease thickness
      double greaseVolume = FacePlateGreaseThick*MechanicalStaveWidth*MiddleSectionLength;
      double facePlateVolume = (FacePlateThick-FacePlateGreaseThick)*MechanicalStaveWidth*MiddleSectionLength; 

      std::string compMatName="FacePlateGrease_IBL";
      if(FacePlateGlueThick>0.0001) compMatName="FacePlateGlueGrease_IBL";
      const GeoMaterial* faceplate_material = m_mat_mgr->getCompositeMaterialForVolume(compMatName,
										     facePlateVolume+greaseVolume,
										     facePlateVolume,faceplateMatName,
										     greaseVolume,"pix::ThermGrease_IBL");
      faceplate_logVol = new GeoLogVol("FacePlate",faceplate_shape,faceplate_material);
      m_gmt_mgr->msg(MSG::INFO)<<"***> new material : "<<faceplate_material->getName()<<" "<<faceplate_material->getDensity()/(GeoModelKernelUnits::gram/Gaudi::Units::cm3)<<endmsg;
    }

  //  const GeoMaterial* faceplate_material = m_mat_mgr->getMaterial(m_gmt_mgr->getMaterialName("FacePlate",0,0));
  //   GeoLogVol * faceplate_logVol = new GeoLogVol("FacePlate",faceplate_shape,faceplate_material);
  GeoPhysVol * faceplate_logVolPV = new GeoPhysVol(faceplate_logVol);
  GeoNameTag* faceplate_tag = new GeoNameTag("FacePlate");
  logVolPV->add(faceplate_tag);
  logVolPV->add(faceplate_xform);
  logVolPV->add(faceplate_logVolPV);      


  // ------------------------------------------------------------------------------------------------------------
  // ------------------------------------------------------------------------------------------------------------
  // Stave Flex
  // ------------------------------------------------------------------------------------------------------------
  // ------------------------------------------------------------------------------------------------------------

  int ModuleNumber_flex= m_gmt_mgr->IBLStaveModuleNumber_AllPlanar(); 
  double ModuleGap_flex= m_gmt_mgr->IBLStaveModuleGap(); 
  double ModuleLength_flex=pm.Length();

  double ModuleTotLength=ModuleLength_flex+ModuleGap_flex;
  double ModulePosZ=ModuleTotLength*0.5;
  int NbModuleRL=ModuleNumber_flex/2;
  if(ModuleNumber_flex%2==1)
    {
      ModulePosZ=0.0;
      NbModuleRL+=1;
    }
  double FlexThicknessRL=FlexBaseThick;
  double flex_angle=((maxFlex1x-maxFlex4x)*(foam1x-foam5x)+(maxFlex1y-maxFlex4y)*(foam1y-foam5y))/(FlexWidth*FoamBaseWidth);
  flex_angle=acos(flex_angle);
  const GeoMaterial* cableflex_material = m_mat_mgr->getMaterial("pix::CableFlex_IBL");
  //double flexDensity=cableflex_material->getDensity();

  m_gmt_mgr->msg(MSG::INFO)<<"Flex angle : "<<flex_angle<<"  Flex pos. in Z : ";
  double EndblockFlexXpos=0.;
  double EndblockFlexYpos=0.;
  double EndblockFlexThickness=0.;

  double wingZmin=0., wingZmax=0.;
  int nbModuleSvc = m_gmt_mgr->PixelModuleServiceNumber();    
  GeoLogVol * wingflex_logVol = 0;
  double wingFlexPosX = 0.;
  double wingFlexPosY = 0.;

  for(int iModule=0; iModule<NbModuleRL; iModule++)
    {

      double flexGapZ=0.;

      // Add flex over stave end block
      if(iModule==NbModuleRL-1)
	{
	  wingZmax=ModulePosZ+ModuleTotLength*0.5;
	  double delta=MiddleSectionLength*0.5-(ModulePosZ+ModuleTotLength*0.5);
	  ModulePosZ+=delta*0.5;
	  ModuleTotLength+=delta;
	  //	  ModulePosZ+=(MechanicalStaveEndBlockLength+lgNAStave)*0.5;
	  //	  ModuleTotLength+=MechanicalStaveEndBlockLength+lgNAStave;
	}
      else if(iModule==0){
	flexGapZ = FlexStaveMiddleGap;
	wingZmin = flexGapZ;
      }

      // Box and coordinates
      GeoBox * cableflex_shape = new GeoBox(FlexThicknessRL*0.5,FlexWidth*0.5,(ModuleTotLength-m_SafetyMargin-flexGapZ)*.5);

      double flex1x = foamMidX-(FlexOffset+flexHalfWidth)*vDirX;
      double flex1y = foamMidY-(FlexOffset+flexHalfWidth)*vDirY;
      double flex2x = flex1x+FlexThicknessRL*vPerpDirX;
      double flex2y = flex1y+FlexThicknessRL*vPerpDirY;
      double flex4x = foamMidX+(-FlexOffset+flexHalfWidth)*vDirX;
      double flex4y = foamMidY+(-FlexOffset+flexHalfWidth)*vDirY;
      double flex3x = flex4x+FlexThicknessRL*vPerpDirX;
      double flex3y = flex4y+FlexThicknessRL*vPerpDirY;

      // flex name
      std::ostringstream lname;
      int iFlexModule=(ModuleNumber_flex%2==0)?(iModule+1):iModule;
      lname << "StaveFlex_"<<iFlexModule;
      if(!(ModuleNumber_flex%2==1&&iModule==0))lname<<"A";

      // scaled material
      const GeoMaterial* scaledFlexMaterial=0;
      if(bFlexAndWing){ 
	std::string flexMatName=m_gmt_mgr->IBLFlexMaterial(iModule+1,"staveA");
	scaledFlexMaterial= m_mat_mgr->getMaterial(flexMatName);
      }
//They are commented off due to the judgement in Line 177 about bFlexAndWing and bFlexConstantThickness.
//But we keep it here in case they are needed in the future.
//       else if(bFlexConstantThickness){
// 	std::ostringstream tmp;
// 	tmp<<"Staveflex"<<iModule+1<<"_IBL";
// 	scaledFlexMaterial= m_mat_mgr->getMaterial("pix::CableFlex_IBL", flexDensity*(iModule+1), tmp.str());
//       }

      // Add flex in 3D model : A component
      GeoTrf::Translation3D cableflex_pos((flex1x+flex2x+flex3x+flex4x)*0.25,(flex1y+flex2y+flex3y+flex4y)*0.25,ModulePosZ+flexGapZ*0.5);
      //      GeoTransform* cableflex_xform = new GeoTransform(GeoTrf::Transform3D(Gaudi::Units::HepRotation(0.0,0.0,-fabs(flex_angle)),cableflex_pos));
      GeoTransform* cableflex_xform = new GeoTransform(GeoTrf::Transform3D(cableflex_pos*GeoTrf::RotateZ3D(fabs(flex_angle))));

      GeoLogVol * cableflex_logVol = 0;
      if(bFlexAndWing||bFlexConstantThickness)
	cableflex_logVol= new GeoLogVol(lname.str(),cableflex_shape,scaledFlexMaterial);
      else
	cableflex_logVol= new GeoLogVol(lname.str(),cableflex_shape,cableflex_material);

      // Wing flex ( second version - not smeared )
      if(nbModuleSvc>0)
	{
	  std::ostringstream wingName;
	  wingName << "WingFlex";

	  if(iModule==0)
	    {
	      WingThick = 0.06;
	      double flex1x_tmp = flex4x;
	      double flex1y_tmp = flex4y;
	      double flex2x_tmp = flex4x+2.*wingHalfWidth*vDirX;
	      double flex2y_tmp = flex4y+2.*wingHalfWidth*vDirY;
	      double flex4x_tmp = flex1x_tmp+WingThick*vPerpDirX;
	      double flex4y_tmp = flex1y_tmp+WingThick*vPerpDirY;
	      double flex3x_tmp = flex2x_tmp+WingThick*vPerpDirX;
	      double flex3y_tmp = flex2y_tmp+WingThick*vPerpDirY;

	      maxFlex3x_wings = flex3x_tmp;
	      maxFlex3y_wings = flex3y_tmp;
	      maxFlex4x_wings = flex4x_tmp;
	      maxFlex4y_wings = flex4y_tmp;
	      wingFlexPosX = (flex1x_tmp+flex2x_tmp+flex3x_tmp+flex4x_tmp)*0.25;
	      wingFlexPosY = (flex1y_tmp+flex2y_tmp+flex3y_tmp+flex4y_tmp)*0.25;

	      double wingHalfLength = 12.15*.5;
	      GeoBox* wingflex_shape = new GeoBox(WingThick*0.5,wingHalfWidth,wingHalfLength);
	      
	      // flex name
	      const GeoMaterial* wingflex_material= m_mat_mgr->getMaterialForVolume("pix::WingFlexTop",wingflex_shape->volume()) ;
	      wingflex_logVol = new GeoLogVol(wingName.str(),wingflex_shape,wingflex_material);
	    }
	  

	  // Add flex in 3D model : A component

	  double zPos = (iModule+.5)*ModuleLength_flex+(iModule+.5)*ModuleGap_flex;
	  GeoTrf::Translation3D wingflex_posA(wingFlexPosX, wingFlexPosY , zPos-ModuleLength_flex*.25);
	  GeoTransform* wingflex_xformA = new GeoTransform(GeoTrf::Transform3D(wingflex_posA*GeoTrf::RotateZ3D(fabs(flex_angle))));
	  
	  GeoPhysVol * wingflex_logVolPV_1 = new GeoPhysVol(wingflex_logVol);
	  GeoNameTag* wingflex_tag = new GeoNameTag(wingName.str());
	  logVolPV->add(wingflex_tag);
	  logVolPV->add(wingflex_xformA);
	  logVolPV->add(wingflex_logVolPV_1);

	  GeoTrf::Translation3D wingflex_posA_2(wingFlexPosX, wingFlexPosY , zPos+ModuleLength_flex*.25);
	  GeoTransform* wingflex_xformA_2 = new GeoTransform(GeoTrf::Transform3D(wingflex_posA_2*GeoTrf::RotateZ3D(fabs(flex_angle))));
	  
	  GeoPhysVol * wingflex_logVolPV_2 = new GeoPhysVol(wingflex_logVol);
	  logVolPV->add(wingflex_tag);
	  logVolPV->add(wingflex_xformA_2);
	  logVolPV->add(wingflex_logVolPV_2);


	  // Add flex in 3D model : C component
	  GeoTrf::Translation3D wingflex_posC(wingFlexPosX, wingFlexPosY , -zPos-ModuleLength_flex*.25);
	  GeoTransform* wingflex_xformC = new GeoTransform(GeoTrf::Transform3D(wingflex_posC*GeoTrf::RotateZ3D(fabs(flex_angle))));
	  
	  GeoPhysVol * wingflex_logVolPV_C1 = new GeoPhysVol(wingflex_logVol);
	  logVolPV->add(wingflex_tag);
	  logVolPV->add(wingflex_xformC);
	  logVolPV->add(wingflex_logVolPV_C1);

	  GeoTrf::Translation3D wingflex_posC_2(wingFlexPosX, wingFlexPosY , -zPos+ModuleLength_flex*.25);
	  GeoTransform* wingflex_xformC_2 = new GeoTransform(GeoTrf::Transform3D(wingflex_posC_2*GeoTrf::RotateZ3D(fabs(flex_angle))));
	  
	  GeoPhysVol * wingflex_logVolPV_C2 = new GeoPhysVol(wingflex_logVol);
	  logVolPV->add(wingflex_tag);
	  logVolPV->add(wingflex_xformC_2);
	  logVolPV->add(wingflex_logVolPV_C2);

	}

      GeoPhysVol * cableflex_logVolPV = new GeoPhysVol(cableflex_logVol);
      GeoNameTag* cableflex_tag = new GeoNameTag(lname.str());
      logVolPV->add(cableflex_tag);
      logVolPV->add(cableflex_xform);
      logVolPV->add(cableflex_logVolPV);

      m_gmt_mgr->msg(MSG::INFO) 
	  << "(" << cableflex_pos.x() << "," << cableflex_pos.y() << "," << cableflex_pos.z() << ")"
	  << " " << FlexThicknessRL<<" / ";
     
      // Add flex in 3D model : C component
      if((ModuleNumber_flex%2==1&&iModule>0)||ModuleNumber_flex%2==0)
	{
	  std::ostringstream lname;
	  lname << "StaveFlex_"<<iFlexModule<<"C";

	  GeoTrf::Translation3D cableflex_pos2((flex1x+flex2x+flex3x+flex4x)*0.25,(flex1y+flex2y+flex3y+flex4y)*0.25,-ModulePosZ-flexGapZ*0.5);
	  GeoTransform* cableflex_xform2 = new GeoTransform(GeoTrf::Transform3D(cableflex_pos2*GeoTrf::RotateZ3D(fabs(flex_angle))));
	  GeoLogVol * cableflex_logVol = 0;

	  const GeoMaterial* scaledFlexMaterial=0;
	  if(bFlexAndWing){ 
	    std::string flexMatName=m_gmt_mgr->IBLFlexMaterial(iModule+1,"staveC");
	    scaledFlexMaterial= m_mat_mgr->getMaterial(flexMatName);
	  }
          //They are commented off due to the judgement in Line 177 about bFlexAndWing and bFlexConstantThickness.
          //But we keep it here in case they are needed in the future.
	  //else if(bFlexConstantThickness){
	  //  std::ostringstream tmp;
	  //  tmp<<"Staveflex"<<iModule+1<<"_IBL";
	  //  scaledFlexMaterial= m_mat_mgr->getMaterial("pix::CableFlex_IBL", flexDensity*(iModule+1), tmp.str());
	  //}

	  if(bFlexAndWing||bFlexConstantThickness)
	    cableflex_logVol= new GeoLogVol(lname.str(),cableflex_shape,scaledFlexMaterial);
	  else
	    cableflex_logVol= new GeoLogVol(lname.str(),cableflex_shape,cableflex_material);
	  GeoPhysVol * cableflex_logVolPV = new GeoPhysVol(cableflex_logVol);
	  GeoNameTag* cableflex_tag = new GeoNameTag(lname.str());
	  logVolPV->add(cableflex_tag);
	  logVolPV->add(cableflex_xform2);
	  logVolPV->add(cableflex_logVolPV);
	}

      // Save data for endblock flex
      if(iModule==NbModuleRL-1)
	{
	  EndblockFlexXpos=(flex1x+flex2x+flex3x+flex4x)*0.25;
	  EndblockFlexYpos=(flex1y+flex2y+flex3y+flex4y)*0.25;
	  EndblockFlexThickness=FlexThicknessRL;
	}

      ModulePosZ+=ModuleTotLength;
      if(!bFlexConstantThickness)
	FlexThicknessRL+=FlexLayerThick;
    }

  m_gmt_mgr->msg(MSG::INFO)<<endmsg;

  // ------------------------------------------------------------------------------------------------------------
  // ------------------------------------------------------------------------------------------------------------
  // Wings Flex
  // ------------------------------------------------------------------------------------------------------------
   // ------------------------------------------------------------------------------------------------------------

  if(bFlexAndWing&&nbModuleSvc==0)   // old wing flex definition
    {
      m_gmt_mgr->msg(MSG::INFO)<<"Wings : "<< FlexOffset<<" "<<flexHalfWidth<<endmsg;
      
      double wingPos = FlexOffset+flexHalfWidth+2.*m_SafetyMargin+0.5;               // SES - fixme
      double flex1x = foamMidX+(wingPos+wingHalfWidth)*vDirX;
      double flex1y = foamMidY+(wingPos+wingHalfWidth)*vDirY;
      double flex2x = flex1x+WingThick*vPerpDirX;
      double flex2y = flex1y+WingThick*vPerpDirY;
      double flex4x = foamMidX+(wingPos)*vDirX;
      double flex4y = foamMidY+(wingPos)*vDirY;
      double flex3x = flex4x+WingThick*vPerpDirX;
      double flex3y = flex4y+WingThick*vPerpDirY;
	
      GeoBox* wingflex_shape = new GeoBox(WingThick*0.5,wingHalfWidth-m_SafetyMargin,(wingZmax-wingZmin)*.5);
      
      // flex name
      std::ostringstream wingName;
      wingName << "WingFlex";
      
      // Add flex in 3D model : A component
      GeoTrf::Vector3D wingflex_posA((flex1x+flex2x+flex3x+flex4x)*0.25,(flex1y+flex2y+flex3y+flex4y)*0.25,(wingZmax-wingZmin)*.5+FlexStaveMiddleGap);
      GeoTransform* wingflex_xformA = new GeoTransform(GeoTrf::GeoTransformRT(GeoTrf::GeoRotation(0.0,0.0,-fabs(flex_angle)),wingflex_posA));
      
      const GeoMaterial* wingflex_material= m_mat_mgr->getMaterial("pix::WingFlexA");
      GeoLogVol * wingflex_logVol = new GeoLogVol(wingName.str(),wingflex_shape,wingflex_material);
      
      GeoPhysVol * wingflex_logVolPV = new GeoPhysVol(wingflex_logVol);
      GeoNameTag* wingflex_tag = new GeoNameTag(wingName.str());
      logVolPV->add(wingflex_tag);
      logVolPV->add(wingflex_xformA);
      logVolPV->add(wingflex_logVolPV);
      
      GeoTrf::Vector3D wingflex_posC((flex1x+flex2x+flex3x+flex4x)*0.25,(flex1y+flex2y+flex3y+flex4y)*0.25,-((wingZmax-wingZmin)*.5+FlexStaveMiddleGap));
      GeoTransform* wingflex_xformC = new GeoTransform(GeoTrf::GeoTransformRT(GeoTrf::GeoRotation(0.0,0.0,-fabs(flex_angle)),wingflex_posC));
      logVolPV->add(wingflex_tag);
      logVolPV->add(wingflex_xformC);
      logVolPV->add(wingflex_logVolPV);
    }

  // ------------------------------------------------------------------------------------------------------------
  // ------------------------------------------------------------------------------------------------------------
  // Cooling pipe
  // ------------------------------------------------------------------------------------------------------------
  // ------------------------------------------------------------------------------------------------------------
  const GeoTube* coolingPipe = new GeoTube(0.0,TubeOuterDiam*0.5,MiddleSectionLength*0.5);
  const GeoTube* coolingPipeInner = new GeoTube(0.0,TubeInnerDiam*0.5,MiddleSectionLength*0.5);

  GeoLogVol * cp_logVol = 0;
  // Create composite material made of omega+glue if a thickness of glue is defined is DB
  if(TubeGlueThick<0.0001)
    {
      m_gmt_mgr->msg(MSG::INFO)<<"** TUBE : without Stycast "<<TubeGlueThick<<endmsg;
      const GeoMaterial* cp_material = m_mat_mgr->getMaterial("pix::CoolingPipe_IBL");
      cp_logVol = new GeoLogVol("CoolingPipe",coolingPipe,cp_material);
    }
  else
    {
      m_gmt_mgr->msg(MSG::INFO)<<"** TUBE : with Stycast "<<TubeGlueThick<<"  diam "<<TubeOuterDiam*0.5<<" "<<TubeInnerDiam*0.5<<endmsg;
      double glueVolume = (TubeOuterDiam*0.5+TubeGlueThick)*(TubeOuterDiam*0.5+TubeGlueThick)*Gaudi::Units::pi*MiddleSectionLength;
      double tubeOuterVolume = TubeOuterDiam*TubeOuterDiam*0.25*Gaudi::Units::pi*MiddleSectionLength;
      double tubeInnerVolume = TubeInnerDiam*TubeInnerDiam*0.25*Gaudi::Units::pi*MiddleSectionLength;

      const std::string compMatName="CoolingPipeGlue_IBL";
      const GeoMaterial* cp_material = m_mat_mgr->getCompositeMaterialForVolume(compMatName,
									      tubeOuterVolume-tubeInnerVolume,
									      tubeOuterVolume-tubeInnerVolume,"pix::CoolingPipe_IBL",
									      glueVolume-tubeOuterVolume,"pix::Stycast2850FT");
      m_gmt_mgr->msg(MSG::INFO)<<"***> new material : "<<cp_material->getName()<<" "<<cp_material->getDensity()/(GeoModelKernelUnits::gram/Gaudi::Units::cm3)<<endmsg;
      cp_logVol = new GeoLogVol("CoolingPipe",coolingPipe,cp_material);
    }

  GeoPhysVol * cp_logPV = new GeoPhysVol(cp_logVol);

//   const GeoMaterial* cp_material = m_mat_mgr->getMaterial("std::Titanium");
//   GeoLogVol * cp_log = new GeoLogVol("CoolingPipe",coolingPipe,cp_material);
//   GeoPhysVol * cp_logPV = new GeoPhysVol(cp_log);
  
  GeoNameTag* cp_tag = new GeoNameTag("CoolingPipe");
  GeoTrf::Translate3D cp_pos(xGblOffset+TubeMiddlePos,0.0,0.0);
  GeoTransform* cp_xform = new GeoTransform(cp_pos);
  foam_logVolPV->add(cp_tag);
  foam_logVolPV->add(cp_xform);
  foam_logVolPV->add(cp_logPV);

  //  const GeoTube* coolingPipeInner = new GeoTube(0.0,TubeInnerDiam*0.5,MiddleSectionLength*0.5);
  const GeoMaterial* cp_inner_material = m_mat_mgr->getMaterial(m_gmt_mgr->getMaterialName("CoolingFluid",0,0));
  GeoLogVol * cp_inner_log = new GeoLogVol("CoolingPipeInner",coolingPipeInner,cp_inner_material);
  GeoPhysVol * cp_inner_logPV = new GeoPhysVol(cp_inner_log);
  
  GeoNameTag* cp_inner_tag = new GeoNameTag("CoolingPipeInner");
  GeoTransform* cp_inner_xform = new GeoTransform(GeoTrf::Transform3D::Identity());
  cp_logPV->add(cp_inner_tag);
  cp_logPV->add(cp_inner_xform);
  cp_logPV->add(cp_inner_logPV);
		  
  logVolPV->add(foam_tag);
  logVolPV->add(foam_xform);
  logVolPV->add(foam_logVolPV);



  // ------------------------------------------------------------------------------------------------------------
  // ------------------------------------------------------------------------------------------------------------
  // ------------------------------------------------------------------------------------------------------------
  // Stave end blocks  and services in stave ring support area ( GeoModel volume build in GeoPixelLayer class)
  // ------------------------------------------------------------------------------------------------------------
  // ------------------------------------------------------------------------------------------------------------
  // ------------------------------------------------------------------------------------------------------------

  m_endblockAPhysVol=0;
  m_endblockCPhysVol=0;
  m_endblockZpos=0.;
  m_endblockLength=0.;
  m_endblockFlexPhysVol=0;
  m_endblockFlexTrf=0;
  
  m_endblockLength=MechanicalStaveEndBlockLength-safetyMarginZ*4.;

  m_endblockSrvLength=0.;
  double doglegFlexLength=FlexDogLeg_length;
  if(bFlexAndWing)
      m_endblockSrvLength=FlexEOS_zpos-totalStaveLength*.5;

  if(m_bVerbose)m_gmt_mgr->msg(MSG::DEBUG)<<"FLEX PP0 srv : "<<  m_endblockSrvLength<<" +  "<<doglegFlexLength<<endmsg;


  double serviceZMidPoint=MechanicalStaveLength*0.5-MechanicalStaveEndBlockLength*.5;
  serviceZMidPoint+=(m_endblockLength+m_endblockSrvLength)*0.5;

  // -----------  End block : Carbon Foam
  // ------------------------------------------------------------------------------------------------------------

  GeoSimplePolygonBrep * endblock_shape = new GeoSimplePolygonBrep(m_endblockLength*.5+safetyMarginZ);
  endblock_shape->addVertex(plate1x,plate1y);
  endblock_shape->addVertex(omega1x,omega1y);
  endblock_shape->addVertex(omega2x,omega2y);
  endblock_shape->addVertex(omega3x,omega3y);
  endblock_shape->addVertex(omega4x,omega4y);
  endblock_shape->addVertex(omega5x,omega5y);
  endblock_shape->addVertex(plate4x,plate4y);
  
  const GeoMaterial* endblock_material_weight = m_mat_mgr->getMaterialForVolume("pix::EndblockA_IBLwght",endblock_shape->volume());
  GeoLogVol * endblock_logVol = 0;
  GeoLogVol * endblockA_logVol = 0;
  GeoLogVol * endblockC_logVol = 0;
  if(endblock_material_weight){
    double endblockOmegaOverlap=m_gmt_mgr->IBLStaveMechanicalStaveEndBlockOmegaOverlap();
    double omegaStaveVolume = omega_shape->volume();
    double omegaVolume = omegaStaveVolume*endblockOmegaOverlap/omegaStaveVolume;
    double endblockVolume = endblock_shape->volume();

    const std::string compMatNameA="EndblockOgA_IBL";
    const GeoMaterial* endblockA_omega_material = m_mat_mgr->getCompositeMaterialForVolume(compMatNameA,
											endblockVolume+omegaVolume,
											omegaVolume,"pix::Omega_IBL",
											endblockVolume,"pix::EndblockA_IBLwght");

    endblockA_logVol = new GeoLogVol("EndBlock",endblock_shape,endblockA_omega_material);
    m_endblockAPhysVol = new GeoPhysVol(endblockA_logVol);

    const std::string compMatNameC="EndblockOgC_IBL";
    const GeoMaterial* endblockC_omega_material = m_mat_mgr->getCompositeMaterialForVolume(compMatNameC,
											 endblockVolume+omegaVolume,
											 omegaVolume,"pix::Omega_IBL",
											 endblockVolume,"pix::EndblockC_IBLwght");
    endblockC_logVol = new GeoLogVol("EndBlock",endblock_shape,endblockC_omega_material);
    m_endblockCPhysVol = new GeoPhysVol(endblockC_logVol);

  }
  else {
    const GeoMaterial* endblock_material = m_mat_mgr->getMaterial("pix::EndBlock_IBL");
    endblock_logVol = new GeoLogVol("EndBlock",endblock_shape,endblock_material);
    m_endblockAPhysVol = new GeoPhysVol(endblock_logVol);
  }
    
  //      const GeoMaterial* endblock_material = m_mat_mgr->getMaterial(m_gmt_mgr->getMaterialName("EndBlock",0,0));
  //  GeoNameTag* endblock_tag = new GeoNameTag("EndBlock");
  //      GeoLogVol * endblock_logVol = new GeoLogVol("EndBlock",endblock_shape,endblock_material);
  //       GeoPhysVol * endblock_logVolPV = new GeoPhysVol(endblock_logVol);


  // -----------  End block : Cooling pipe 
  // ------------------------------------------------------------------------------------------------------------

  const GeoTube* endblock_coolingPipe = new GeoTube(0.0,TubeOuterDiam*0.5,m_endblockLength*.5);

  const GeoMaterial* cp_endblock_material = m_mat_mgr->getMaterial("pix::CoolingPipe_IBL");
  GeoLogVol * cp_endblock_log = new GeoLogVol("EndBlockCoolingPipe",endblock_coolingPipe,cp_endblock_material);
  GeoPhysVol * cp_endblock_logPV = new GeoPhysVol(cp_endblock_log);
  
  GeoNameTag* cp_endblock_tag = new GeoNameTag("EndBlockCoolingPipe");
  GeoTrf::Translate3D cp_endblock_pos(xGblOffset+TubeMiddlePos,0.0,0.0);
  GeoTransform* cp_endblock_xform = new GeoTransform(cp_endblock_pos);
  m_endblockAPhysVol->add(cp_endblock_tag);
  m_endblockAPhysVol->add(cp_endblock_xform);
  m_endblockAPhysVol->add(cp_endblock_logPV);
  if(m_endblockCPhysVol){
    m_endblockCPhysVol->add(cp_endblock_tag);
    m_endblockCPhysVol->add(cp_endblock_xform);
    m_endblockCPhysVol->add(cp_endblock_logPV);
  }

  const GeoTube* endblock_coolingPipeInner = new GeoTube(0.0,TubeInnerDiam*0.5,m_endblockLength*.5);
  const GeoMaterial* cp_endblock_inner_material = m_mat_mgr->getMaterial(m_gmt_mgr->getMaterialName("CoolingFluid",0,0));
  GeoLogVol * cp_endblock_inner_log = new GeoLogVol("EndBlockCoolingPipeInner",endblock_coolingPipeInner,cp_endblock_inner_material);
  GeoPhysVol * cp_endblock_inner_logPV = new GeoPhysVol(cp_endblock_inner_log);
  
  GeoNameTag* cp_endblock_inner_tag = new GeoNameTag("EndBlockCoolingPipeInner");
  GeoTransform* cp_endblock_inner_xform = new GeoTransform(GeoTrf::Transform3D::Identity());
  cp_endblock_logPV->add(cp_endblock_inner_tag);
  cp_endblock_logPV->add(cp_endblock_inner_xform);
  cp_endblock_logPV->add(cp_endblock_inner_logPV);
		  
  m_serviceZpos=MechanicalStaveLength*0.5-m_endblockLength+(m_endblockLength+m_endblockSrvLength)*0.5;
  m_endblockZpos=MechanicalStaveLength*0.5-m_endblockLength*0.5;

  m_gmt_mgr->msg(MSG::INFO)<<"IBL EOS : mechanical stave length : "<<MechanicalStaveLength*0.5<<endmsg;
  m_gmt_mgr->msg(MSG::INFO)<<"IBL EOS :            service Zpos : "<<m_serviceZpos<<endmsg;
  m_gmt_mgr->msg(MSG::INFO)<<"IBL EOS :           endblock Zpos : "<<m_endblockZpos<<endmsg;
  m_gmt_mgr->msg(MSG::INFO)<<"IBL EOS :         endblock length : "<<m_endblockLength<<endmsg;
  m_gmt_mgr->msg(MSG::INFO)<<"IBL EOS :     endblock srv length : "<<m_endblockSrvLength<<endmsg;


  // -----------  Flex running on the top of End block & service 
  // ------------------------------------------------------------------------------------------------------------

  // Box and coordinates
  GeoBox * cableflex_shape = new GeoBox(EndblockFlexThickness*0.5,FlexWidth*0.5,(m_endblockLength+m_endblockSrvLength)*.5);

  const GeoMaterial* scaledFlexMaterial=0;
  if(bFlexAndWing){ 
    std::string flexMatName=m_gmt_mgr->IBLFlexMaterial(1,"doglegA");
    scaledFlexMaterial= m_mat_mgr->getMaterial(flexMatName);
  }
  //They are commented off due to the judgement in Line 177 about bFlexAndWing and bFlexConstantThickness.
  //But we keep it here in case they are needed in the future.
  //else if(bFlexConstantThickness){
  //  std::ostringstream tmp;
  //  tmp<<"CableFlex_IBL";
  //  scaledFlexMaterial= m_mat_mgr->getMaterial("pix::CableFlex_IBL", flexDensity*(ModuleNumber_flex/2+1), tmp.str());
  //}
  else{
    scaledFlexMaterial= m_mat_mgr->getMaterial("pix::CableFlex_IBL");
  }

  // flex name
  std::ostringstream lname;
  lname << "SrvEndblockFlex";

  // Add flex in 3D model : A component
  //  GeoNameTag* cableflex_tag = new GeoNameTag(lname.str());
  GeoTrf::Vector3D cableflex_pos(EndblockFlexXpos,EndblockFlexYpos,0.);
  m_endblockFlexTrf = new GeoTransform(GeoTrf::GeoTransformRT(GeoTrf::GeoRotation(0.0,0.0,-fabs(flex_angle)),cableflex_pos));
  GeoLogVol * cableflex_logVol = new GeoLogVol(lname.str(),cableflex_shape,scaledFlexMaterial);
  m_endblockFlexPhysVol = new GeoPhysVol(cableflex_logVol);

  m_gmt_mgr->msg(MSG::INFO)<<"Flex half thickness srv : "<<EndblockFlexThickness<<endmsg;

  // -----------  service cooling pipe
  // ------------------------------------------------------------------------------------------------------------

  m_serviceCoolPipePhysVol = 0;
  m_serviceCoolPipeTrfA = 0;
  m_serviceCoolPipeTrfC = 0;

  if(bFlexAndWing){

    const GeoTube* service_coolingPipe = new GeoTube(0.0,TubeOuterDiam*0.5,m_endblockSrvLength*.5+doglegFlexLength*.5-2*safetyMarginZ);
    
    const GeoMaterial* cp_service_material = m_mat_mgr->getMaterial("pix::CoolingPipe_IBL");
    GeoLogVol * cp_service_log = new GeoLogVol("SrvCoolingPipe",service_coolingPipe,cp_service_material);
    m_serviceCoolPipePhysVol = new GeoPhysVol(cp_service_log);
  
//   GeoNameTag* cp_service_tag = new GeoNameTag("ServiceCoolingPipe");
//   GeoTrf::Vector3D cp_service_pos(xGblOffset+TubeMiddlePos,0.0,0.0);
//   GeoTransform* cp_service_xform = new GeoTransform(GeoTrf::Transform3D(Gaudi::Units::HepRotation(),cp_service_pos));
//   //       service_logVolPV->add(cp_service_tag);
//   //       service_logVolPV->add(cp_service_xform);
//   //       service_logVolPV->add(cp_service_logPV);
//   m_serviceCoolPipePhysVol->add(cp_service_tag);
//   m_serviceCoolPipePhysVol->add(cp_service_xform);
//   m_serviceCoolPipePhysVol->add(cp_service_logPV);

    const GeoTube* service_coolingPipeInner = new GeoTube(0.0,TubeInnerDiam*0.5,m_endblockSrvLength*.5+doglegFlexLength*.5-2*safetyMarginZ);
    const GeoMaterial* cp_service_inner_material = m_mat_mgr->getMaterial(m_gmt_mgr->getMaterialName("CoolingFluid",0,0));
    GeoLogVol * cp_service_inner_log = new GeoLogVol("SrvCoolingPipeInner",service_coolingPipeInner,cp_service_inner_material);
    GeoPhysVol * cp_service_inner_logPV = new GeoPhysVol(cp_service_inner_log);
    
    GeoNameTag* cp_service_inner_tag = new GeoNameTag("SrvCoolingPipeInner");
    GeoTransform* cp_service_inner_xform = new GeoTransform(GeoTrf::Transform3D::Identity());
    m_serviceCoolPipePhysVol->add(cp_service_inner_tag);
    m_serviceCoolPipePhysVol->add(cp_service_inner_xform);
    m_serviceCoolPipePhysVol->add(cp_service_inner_logPV);
    
    GeoTrf::Translate3D cpipe_posA(xGblOffset+TubeMiddlePos,0.0,(MechanicalStaveLength*0.5+m_endblockSrvLength*0.5)-m_serviceZpos);
    m_serviceCoolPipeTrfA = new GeoTransform(GeoTrf::Transform3D(cpipe_posA));
    
    GeoTrf::Translate3D cpipe_posC(xGblOffset+TubeMiddlePos,0.0,-((MechanicalStaveLength*0.5+m_endblockSrvLength*0.5)-m_serviceZpos));
    m_serviceCoolPipeTrfC = new GeoTransform(GeoTrf::Transform3D(cpipe_posC));
  }

  // ------------------------------------------------------------------------------------------------------------
  // ------------------------------------------------------------------------------------------------------------
  // Compute (stave + module) envelopp, stave HEPtransform and stave thickness N&P 
  // ------------------------------------------------------------------------------------------------------------
  // ------------------------------------------------------------------------------------------------------------
  computeStaveEnvelopTransformAndSize(pmThicknessN,pmThicknessP,pmWidth,
				      pmThicknessN3D,pmThicknessP3D,pmShilftLateral3D);

  if(m_bVerbose){
    m_gmt_mgr->msg(MSG::DEBUG)<<"Max thickness N/P : "<<m_thicknessN<<" "<<m_thicknessP<<endmsg;
    m_gmt_mgr->msg(MSG::DEBUG)<<"Max thickness P : "<<m_thicknessP+layerRadius<<" "<<flexWingMaxR<<endmsg;
    m_gmt_mgr->msg(MSG::DEBUG)<<"Max thickness N : "<<-m_thicknessN+layerRadius<<" "<<-m_thicknessN-radialShiftThickN+layerRadius<<" "<<radialShiftThickN<<endmsg;
  }

  if(bFlexAndWing){
    if(flexWingMaxR>layerRadius+m_thicknessP)m_thicknessP=flexWingMaxR-layerRadius;
    m_thicknessP+=0.11;                        // SES - fixme
  }
  m_thicknessN+=radialShiftThickN+.11;                 // SES - fixme

  if(m_bVerbose)m_gmt_mgr->msg(MSG::DEBUG)<<"Max thickness -> output N/P : "<<m_thicknessN<<" "<<m_thicknessP<<endmsg;

  //**********  return stave phys. vol. (= face plate + foam + cable flex + end blocks) *********************

  return logVolPV;

}

void GeoPixelDetailedStaveSupport::computeStaveEnvelopTransformAndSize(double moduleThickN,double moduleThickP, double moduleWidth,
								       double moduleThickN3D,double /*moduleThickP3D*/, double pmShilftLateral3D)
{

  // **** Global envelop including module and stave *****************************************************
 
  // Offsets have to be defined at the begnning of Build process 
  //   -> these parameters are used to compute staveEnvelopShape

  double MechanicalStaveOffset = m_gmt_mgr->IBLStaveMechanicalStaveOffset();
  double StaveModuleOffset = m_gmt_mgr->IBLStaveMechanicalStaveModuleOffset();
  double FacePlateThick = m_gmt_mgr->IBLStaveFacePlateThickness(); 
  double FacePlateGreaseThick = m_gmt_mgr->IBLStaveFacePlateGreaseThickness();
  if(FacePlateGreaseThick>0.001) FacePlateThick+=FacePlateGreaseThick;

  double stave_xOffset=StaveModuleOffset+moduleThickP, stave_yOffset=MechanicalStaveOffset;
  m_gmt_mgr->msg(MSG::INFO)<<" stave xoffset = "<< stave_xOffset <<endmsg; 
  m_gmt_mgr->msg(MSG::INFO)<<" stave yoffset = "<< stave_yOffset <<endmsg; 
  m_transform = GeoTrf::Translate3D(stave_xOffset,stave_yOffset,0);

  double staveSupportThickness=FacePlateThick+m_FoamMiddleThick;
  m_thicknessP = staveSupportThickness;
  m_thicknessN = (moduleThickN3D>moduleThickN)?moduleThickN3D:moduleThickN;

//   double innerRadius = m_gmt_mgr->IBLSupportRingInnerRadius();
//   double outerRadius = m_gmt_mgr->IBLSupportMidRingOuterRadius();  
//   if(m_thicknessP<outerRadius)m_thicknessP=outerRadius;
//   if(m_thicknessN>innerRadius)m_thicknessN=innerRadius;

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

  //  m_gmt_mgr->msg(MSG::INFO)<<"Stave envelop : module thicknesses P/N "<<moduleThicknessP<<" "<<moduleThicknessN<<endmsg;

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
  m_gmt_mgr->msg(MSG::INFO)<<"Stave envelop length : "<<m_StaveLength<<" ->  "<<halfLadderLength<<endmsg;
  m_staveEnvelopShape = new GeoSimplePolygonBrep(halfLadderLength);

  bool bFlex=m_gmt_mgr->IBLFlexAndWingDefined();
  //  bFlex=false;
  if(!bFlex){
    m_staveEnvelopShape->addVertex(module1x, module1y);
    if (fabs(sqrt((xVertices[0]-module2x)*(xVertices[0]-module2x)+(yVertices[0]-module2y)*(yVertices[0]-module2y)))>0.01)
      m_staveEnvelopShape->addVertex(module2x, module2y);
    for(int iPt=0; iPt<nbVertices; iPt++)
      m_staveEnvelopShape->addVertex(xVertices[iPt],yVertices[iPt]);
    if (fabs(sqrt((xVertices[nbVertices-1]-module3x)*(xVertices[nbVertices-1]-module3x)+(yVertices[nbVertices-1]-module3y)*(yVertices[nbVertices-1]-module3y)))>0.01)
      m_staveEnvelopShape->addVertex(module3x, module3y);
    m_staveEnvelopShape->addVertex(module4x, module4y);
  }
  else{
    for(int iPt=0; iPt<nbVertices; iPt++)
      if(iPt==0||iPt==nbVertices-1)
	m_staveEnvelopShape->addVertex(module1x,yVertices[iPt]);
      else
	m_staveEnvelopShape->addVertex(xVertices[iPt],yVertices[iPt]);
 }
  
}

GeoPhysVol* GeoPixelDetailedStaveSupport::getEndblockEnvelopShape(int iObject) const 
{
  if(iObject==1)
    return m_endblockFlexPhysVol;
  else if(iObject==2)
    return m_endblockAPhysVol;
  else if(iObject==-2){
    if(m_endblockCPhysVol) return m_endblockCPhysVol;
    return m_endblockAPhysVol;
  }    

  return m_serviceCoolPipePhysVol;
}

GeoTransform* GeoPixelDetailedStaveSupport::getEndblockEnvelopShapeTrf(int iObject) const
{
  if(iObject==0)
    return 0;
  else if(iObject==1)
    return m_endblockFlexTrf;
  else if(iObject==2)
    return m_serviceCoolPipeTrfA;

  return m_serviceCoolPipeTrfC;
    
}



GeoSimplePolygonBrep* GeoPixelDetailedStaveSupport::computeStaveEnvelopShape( double safetyMargin )
{

  if(m_bVerbose)m_gmt_mgr->msg(MSG::DEBUG)<<"GeoSimplePolygonBrep* GeoPixelDetailedStaveSupport::computeStaveEnvelopShape( double safetyMargin )"<<endmsg;

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

      if(m_bVerbose)m_gmt_mgr->msg(MSG::DEBUG)<<"  "<<iPt<<" "<<m_staveEnvelopShape->getXVertex(iPt)<<" "<<m_staveEnvelopShape->getYVertex(iPt)<<endmsg;

    }

  // Extract convex shape from stave envelopp
  GetSurroundingConvexShape(xVertices, yVertices);
  // Clean up shape points 
  RemoveCoincidentAndColinearPointsFromShape(xVertices, yVertices);
  // Add margin
  //  AddSurroundingXYMargin(safetyMargin, safetyMargin, xVertices, yVertices);
  
  // Create and return stave envelop
  GeoSimplePolygonBrep* convexStaveEnvelopShape = new GeoSimplePolygonBrep(shapeDZ);

  if(m_thicknessN_svc<.01)
    for(unsigned int iPt=0; iPt<xVertices.size(); iPt++)
      {
	convexStaveEnvelopShape->addVertex(xVertices[iPt],yVertices[iPt]);
	if(m_bVerbose)m_gmt_mgr->msg(MSG::DEBUG)<<"  "<<xVertices[iPt]<<" "<<yVertices[iPt]<<endmsg;
      }
  else {
    for(unsigned int iPt=1; iPt<xVertices.size()-1; iPt++)
      {
	convexStaveEnvelopShape->addVertex(xVertices[iPt],yVertices[iPt]);
	if(m_bVerbose)m_gmt_mgr->msg(MSG::DEBUG)<<"  "<<xVertices[iPt]<<" "<<yVertices[iPt]<<endmsg;
      }

    xVertices[0]+=m_thicknessN_svc;
    double delta = 0.4;
    unsigned int iPoint = xVertices.size()-1;
    //    double xMid = xVertices[iPoint]+(xVertices[0]-xVertices[iPoint])*.45;
    //    double yMid = yVertices[iPoint]+(yVertices[0]-yVertices[iPoint])*.45;
    
    double xDir = xVertices[0]-xVertices[iPoint];
    double yDir = yVertices[0]-yVertices[iPoint];
    double tmp = 1./sqrt(xDir*xDir+yDir*yDir);
    xDir*=tmp; yDir*=tmp;

    double xMid = xVertices[0]-m_width_svc*xDir;
    double yMid = yVertices[0]-m_width_svc*yDir+1.;
    convexStaveEnvelopShape->addVertex(xMid, yMid);
    convexStaveEnvelopShape->addVertex(xMid-2.*delta*m_thicknessN_svc, yMid);

    xMid = xVertices[0]-m_width_svc*xDir;
    yMid = yVertices[0]-m_width_svc*.25*yDir+1.;
    convexStaveEnvelopShape->addVertex(xMid-2.*delta*m_thicknessN_svc, yMid);
    convexStaveEnvelopShape->addVertex(xMid-delta*m_thicknessN_svc*.5, yMid);
    convexStaveEnvelopShape->addVertex(xMid-delta*m_thicknessN_svc*.5, yVertices[0]);

    //    convexStaveEnvelopShape->addVertex(xVertices[0], yVertices[0]);
  }

  return convexStaveEnvelopShape;

}


void GeoPixelDetailedStaveSupport::RemoveCoincidentAndColinearPointsFromShape(std::vector<double> &xPoint, std::vector<double >&yPoint)
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
	  if(zDist<0.01*Gaudi::Units::mm){
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


void GeoPixelDetailedStaveSupport::GetSurroundingConvexShape(std::vector<double> &xVertices, std::vector<double >&yVertices,
							     std::vector<int> iException)
{

  // Get convex envelop = remove points that are "inside shape"
  bool bEndPointReached=false;
  int iMaxWhileLoop=xVertices.size();
  int iWhileLoop=0;

  if(m_bVerbose)m_gmt_mgr->msg(MSG::DEBUG)<<"Convex shape "<<iException.size()<<endmsg;

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

      if(m_bVerbose)m_gmt_mgr->msg(MSG::DEBUG)<<"---> # points "<<xTmp.size()<<endmsg;

      int nbTmp=xTmp.size();
      int iPt;
      for(iPt=0; iPt<nbTmp&&!bRemovedPoint; iPt++)
	if(iException[iTmp[iPt]]==0)
	{
	  int i1=(iPt-1); if(i1<0)i1=nbTmp-1;
	  int i=iPt;
	  int i2=(iPt+1)%(nbTmp);
	  
	  double zProdVect=(xTmp[i1]-xTmp[i])*(yTmp[i2]-yTmp[i])-(xTmp[i2]-xTmp[i])*(yTmp[i1]-yTmp[i]);
	  
	  if(m_bVerbose)m_gmt_mgr->msg(MSG::DEBUG)<<" => "<<iPt<<" "<<zProdVect<<endmsg;

	  if(zProdVect>0)
	    {
	      bRemovedPoint=true;
	      iRemoved[iTmp[iPt]]++;
	      
// 	      xVertices.erase(xVertices.begin()+iPt);
// 	      yVertices.erase(yVertices.begin()+iPt);
// 	      iException.erase(iException.begin()+iPt);
	      if(m_bVerbose)m_gmt_mgr->msg(MSG::DEBUG)<<" -> remove point "<<iPt<<endmsg;
	      
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


void GeoPixelDetailedStaveSupport::AddSurroundingXYMargin(double vMarginX, double vMarginY, std::vector<double> &xPoint, std::vector<double>&yPoint)
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


void GeoPixelDetailedStaveSupport::ComputeStaveExternalShape()
{

  GeoTrf::Vector3D midStaveCenter(m_gmt_mgr->IBLStaveOmegaMidCenterX(),0.0,0.0);
  double midStaveRadius=m_gmt_mgr->IBLStaveOmegaMidRadius();
  double midStaveAngle=90.0*Gaudi::Units::deg-m_gmt_mgr->IBLStaveOmegaMidAngle();

  GeoTrf::Vector3D endStaveCenter(m_gmt_mgr->IBLStaveOmegaEndCenterX(),m_gmt_mgr->IBLStaveOmegaEndCenterY(),0.0);
  double endStaveRadius=m_gmt_mgr->IBLStaveOmegaEndRadius();
  double endStaveAngle=90.0*Gaudi::Units::deg+m_gmt_mgr->IBLStaveOmegaEndAngle();

  double omegaThick = m_gmt_mgr->IBLStaveOmegaThickness();
  double omegaEndStavePointY = m_gmt_mgr->IBLStaveMechanicalStaveWidth()*0.5;

  m_gmt_mgr->msg(MSG::INFO)<<" Omega MidStaveCenterX  = "<<midStaveCenter.x()<<endmsg;
  m_gmt_mgr->msg(MSG::INFO)<<" Omega MidStaveAngle  = "<<midStaveAngle<<endmsg;
  m_gmt_mgr->msg(MSG::INFO)<<" Omega MidStaveRadius  = "<<midStaveRadius<<endmsg;
  m_gmt_mgr->msg(MSG::INFO)<<" Omega EndStaveCenterX  = "<<endStaveCenter.x()<<endmsg;
  m_gmt_mgr->msg(MSG::INFO)<<" Omega EndStaveCenterY  = "<<endStaveCenter.y()<<endmsg;
  m_gmt_mgr->msg(MSG::INFO)<<" Omega EndStaveAngle  = "<<endStaveAngle<<endmsg;
  m_gmt_mgr->msg(MSG::INFO)<<" Omega EndStaveRadius  = "<<endStaveRadius<<endmsg;
  m_gmt_mgr->msg(MSG::INFO)<<" Omega Thickness  = "<<omegaThick<<endmsg;

  // Sines and cosines
  double midCos=cos(midStaveAngle);
  double midSin=sin(midStaveAngle);

//   double endCos=cos(endStaveAngle);
//   double endSin=sin(endStaveAngle);
  
  // End stave thickness
  double endStavePt1=sqrt(endStaveRadius*endStaveRadius-endStaveCenter.x()*endStaveCenter.x())+endStaveCenter.y();
  double endStavePt2=-sqrt(endStaveRadius*endStaveRadius-endStaveCenter.x()*endStaveCenter.x())+endStaveCenter.y();
  double endStavePt=endStavePt1;
  if(fabs(endStavePt2-omegaEndStavePointY)<fabs(endStavePt1-omegaEndStavePointY))endStavePt=endStavePt2;
  GeoTrf::Vector3D endStavePoint(0.0,endStavePt,0.0);
  m_OmegaEndStaveThick=omegaEndStavePointY-endStavePt;
  m_gmt_mgr->msg(MSG::DEBUG)<<"End stave Y point : "<<endStavePt<<" "<<omegaEndStavePointY<<" -> "<<m_OmegaEndStaveThick<<endmsg;
  m_gmt_mgr->msg(MSG::DEBUG)<<"Angles : "<<midStaveAngle<<" "<<endStaveAngle<<endmsg;
  
  // ***** compute points coordinates defining stave long side
  // midSidePt : point next to cooling pipe 

  GeoTrf::Vector3D midSidePt(midStaveCenter.x()+midStaveRadius*midCos, midStaveCenter.y()+midStaveRadius*midSin, 0.0);
  m_gmt_mgr->msg(MSG::DEBUG)<< "midSidePt : "<<midSidePt.x()<<" "<<midSidePt.y()<<endmsg;
  m_gmt_mgr->msg(MSG::DEBUG)<< "-> verif : "<<(midSidePt.x()-midStaveCenter.x())*(midSidePt.x()-midStaveCenter.x())+(midSidePt.y()-midStaveCenter.y())*(midSidePt.y()-midStaveCenter.y())-midStaveRadius*midStaveRadius<<"  ps : "<<(midSidePt.x()-midStaveCenter.x())*midSin+(midSidePt.y()-midStaveCenter.y())*midCos<<endmsg;

  GeoTrf::Vector3D midSidePt_omega(0.0,0.0,0.0);
  GeoTrf::Vector3D midSideDir(midSidePt.x()-midStaveCenter.x(),midSidePt.y()-midStaveCenter.y(),0.0);
  midSidePt_omega=NeighbourPoint_Rad(midSidePt,midSideDir,omegaThick);

  // endSidePt : point next to end of stave
  GeoTrf::Vector3D endSidePt(0.0,0.0,0.0);
  endSidePt=GeoTrf::Vector3D(endStaveCenter.x()+endStaveRadius*midCos, endStaveCenter.y()+endStaveRadius*midSin, 0.0);
  m_gmt_mgr->msg(MSG::DEBUG)<< "endSidePt : "<<endSidePt.x()<<" "<<endSidePt.y()<<endmsg;
  m_gmt_mgr->msg(MSG::DEBUG)<< "-> verif : "<<(endSidePt.x()-endStaveCenter.x())*(endSidePt.x()-endStaveCenter.x())+(endSidePt.y()-endStaveCenter.y())*(endSidePt.y()-endStaveCenter.y())-endStaveRadius*endStaveRadius<<"  ps : "<<(endSidePt.x()-endStaveCenter.x())*midSin-(endSidePt.y()-endStaveCenter.y())*midCos<<endmsg;

  GeoTrf::Vector3D endSidePt_omega(0.0,0.0,0.0);
  GeoTrf::Vector3D endSideDir(endSidePt.x()-endStaveCenter.x(),endSidePt.y()-endStaveCenter.y(),0.0);
  endSidePt_omega=NeighbourPoint_Rad(endSidePt,endSideDir,omegaThick);


  // ***** Points defining the vertex of foam module and omega module
  
  // Foam module
  GeoTrf::Vector3D midStavePoint(0.0,0.0,0.0);
  midStavePoint=IntersectionPoint(midSidePt.x(),midSidePt.y(),endSidePt.x(),endSidePt.y(),
				  midSidePt.x(),-midSidePt.y(),endSidePt.x(),-endSidePt.y());
  m_FoamMiddleThick=midStavePoint.x();
  m_gmt_mgr->msg(MSG::DEBUG)<< "Foam thickness (middle) "<<m_FoamMiddleThick<<" ("<<midStavePoint.y()<<") "<<endmsg;

  // Omega module
  GeoTrf::Vector3D midStavePoint_omega(0.0,0.0,0.0);
  midStavePoint_omega=IntersectionPoint(midSidePt_omega.x(),midSidePt_omega.y(),endSidePt_omega.x(),endSidePt_omega.y(),
					midSidePt_omega.x(),-midSidePt_omega.y(),endSidePt_omega.x(),-endSidePt_omega.y());
  m_OmegaMidStaveThick=midStavePoint_omega.x()-midStavePoint.x();
  m_gmt_mgr->msg(MSG::DEBUG)<< "Omega thickness (middle) "<<m_OmegaMidStaveThick<<" ("<<midStavePoint_omega.y()<<") "<<endmsg;


  // ***** Points defining the rounded shape at the edge of the stave

  // Neighbour endstave point (without omega)
  GeoTrf::Vector3D endStavePoint_omega(0.0,0.0,0.0);
  GeoTrf::Vector3D endStaveDir(endStavePoint.x()-endStaveCenter.x(),endStavePoint.y()-endStaveCenter.y(),0.0);
  endStavePoint_omega=NeighbourPoint_Rad(endStavePoint,endStaveDir,omegaThick);

  GeoTrf::Vector3D endSidePt_omega2(0.0,0.0,0.0);
  endSidePt_omega2=IntersectionPoint(midSidePt_omega.x(),midSidePt_omega.y(),endSidePt_omega.x(),endSidePt_omega.y(),
				     endStavePoint_omega.x(),endStavePoint_omega.y(),endStavePoint.x(),endStavePoint.y()+m_OmegaEndStaveThick);
  m_gmt_mgr->msg(MSG::DEBUG)<< "EndStave + omega par intersection X/Y "<<endSidePt_omega2.x()<<" "<<endSidePt_omega2.y()<<endmsg;
  m_OmegaEndStavePointX=endSidePt_omega2.x();
  m_OmegaEndStavePointY=endSidePt_omega2.y();

  GeoTrf::Vector3D endSidePt_inner(0.0,0.0,0.0);
  GeoTrf::Vector3D endSidePt_vec(endSidePt_omega2.x()-endStavePoint.x(),endSidePt_omega2.y()-(endStavePoint.y()+m_OmegaEndStaveThick),0.0);
  endSidePt_inner=NeighbourPoint_Perp(endSidePt_omega2,endSidePt_vec,omegaThick,-1);

  // Compute Rx3,Ry3 coordinates
  GeoTrf::Vector3D endSidePt_inter(0.0,0.0,0.0);
  endSidePt_inter=IntersectionPoint(midSidePt.x(),midSidePt.y(),endSidePt.x(),endSidePt.y(),
				    endSidePt_inner.x(),endSidePt_inner.y(),endStavePoint.x(),endStavePoint.y());
  m_FoamEdgePointX=endSidePt_inter.x();
  m_FoamEdgePointY=endSidePt_inter.y();
  m_gmt_mgr->msg(MSG::DEBUG)<< "EndStave sans omega X/Y "<<m_FoamEdgePointX<<" "<<m_FoamEdgePointY<<endmsg;


    // Check angles
  double angle1=ComputeAngle(0.0,1.0,endSidePt.x()-midSidePt.x(),endSidePt.y()-midSidePt.y());
  double angle2=ComputeAngle(0.0,1.0,m_OmegaEndStavePointX-midSidePt_omega.x(),m_OmegaEndStavePointY-midSidePt_omega.y());
  double angle3=ComputeAngle(0.0,1.0,m_FoamEdgePointX-midSidePt.x(),m_FoamEdgePointY-midSidePt.y());

  m_gmt_mgr->msg(MSG::DEBUG)<< "Verfi angles : "<<angle1<<" "<<angle2<<" "<<angle3<<"  / "<<angle1*180.0/3.1415<<" "<<angle2*180.0/3.1415<<endmsg;

  // Compute mid stave side point
  GeoTrf::Vector3D midSidePoint(0.0,0.0,0.0);
  midSidePoint=IntersectionPoint(midSidePt_omega.x(),midSidePt_omega.y(),endSidePt_omega.x(),endSidePt_omega.y(),
				 0.0,(endStavePoint.y()+m_OmegaEndStaveThick)*0.5,10.0,(endStavePoint.y()+m_OmegaEndStaveThick)*0.5);
  m_MidStaveSidePointX=midSidePoint.x();
  m_MidStaveSidePointY=midSidePoint.y();

}


GeoTrf::Vector3D GeoPixelDetailedStaveSupport::IntersectionPoint(double Ax,double Ay,double Bx,double By,double Cx,double Cy,double Dx,double Dy)
{
  double tmp=(Bx-Ax)*(Dy-Cy)-(By-Ay)*(Dx-Cx);
  double invTmp=1.0/tmp;
    
  double r=((Ay-Cy)*(Dx-Cx)-(Ax-Cx)*(Dy-Cy))*invTmp;
    
  GeoTrf::Vector3D inter(Ax+r*(Bx-Ax),Ay+r*(By-Ay),0.0);
    
  return inter;
}


GeoTrf::Vector3D GeoPixelDetailedStaveSupport::NormalizeDir(GeoTrf::Vector3D v)
{
  double tmp=1.0/sqrt(v.x()*v.x()+v.y()*v.y());
  return GeoTrf::Vector3D(v.x()*tmp,v.y()*tmp,0.0);
}


GeoTrf::Vector3D GeoPixelDetailedStaveSupport::NeighbourPoint_Rad(GeoTrf::Vector3D p, GeoTrf::Vector3D v, double delta)
{
  GeoTrf::Vector3D vNorm=NormalizeDir(v);
  double xnew=p.x()+delta*vNorm.x();
  double ynew=p.y()+delta*vNorm.y();
  return GeoTrf::Vector3D(xnew,ynew,0.0);
}

GeoTrf::Vector3D GeoPixelDetailedStaveSupport::NeighbourPoint_Perp(GeoTrf::Vector3D p, GeoTrf::Vector3D v, double delta, int iDir)
{
  GeoTrf::Vector3D vNorm=NormalizeDir(v);
  double xnew=p.x()-iDir*delta*vNorm.y();
  double ynew=p.y()+iDir*delta*vNorm.x();
  return GeoTrf::Vector3D(xnew,ynew,0.0);
}

double GeoPixelDetailedStaveSupport::ComputeAngle(double ux, double uy, double vx, double vy)
{
  
  GeoTrf::Vector3D u(ux,uy,0.0);
  GeoTrf::Vector3D v(vx,vy,0.0);

  GeoTrf::Vector3D uNorm=NormalizeDir(u);
  GeoTrf::Vector3D vNorm=NormalizeDir(v);

  double pScal=uNorm.x()*vNorm.x()+uNorm.y()*vNorm.y();
  double angle=acos(pScal);

  return angle;
}

double GeoPixelDetailedStaveSupport::ComputeDistance(GeoTrf::Vector3D p, GeoTrf::Vector3D q)
{
  return sqrt((q.x()-p.x())*(q.x()-p.x())+(q.y()-p.y())*(q.y()-p.y()));
}
