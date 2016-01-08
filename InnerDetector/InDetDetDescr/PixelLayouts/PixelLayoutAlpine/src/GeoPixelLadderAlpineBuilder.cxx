/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelLayoutAlpine/GeoPixelLadderAlpineBuilder.h"
#include "PixelLayoutAlpine/GeoPixelStaveSupportAlpineBuilder.h"
#include "PixelLayoutAlpine/GeoPixelEndcapFoamModule.h"

#include "InDetReadoutGeometry/PixelModuleDesign.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"

#include "InDetReadoutGeometry/PixelDetectorManager.h"
#include "PixelGeoModel/PixelGeoBuilder.h"
/*#include "TrkDetDescrTools/TrkGeoTemplates.h"*/
#include "InDetTrackingGeometryXML/XMLGeoTemplates.h"

#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoTubs.h"
#include "GeoModelKernel/GeoTrap.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoNameTag.h"
#include "GeoModelKernel/GeoIdentifierTag.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelKernel/GeoVPhysVol.h"
#include "GeoModelKernel/GeoMaterial.h"

#include "GeoModelKernel/GeoTransform.h"
#include "GeoModelKernel/GeoAlignableTransform.h"
#include "GeoModelKernel/GeoShapeShift.h"
#include "GeoModelKernel/GeoShapeUnion.h"
#include "PathResolver/PathResolver.h"

#include <iostream>
#include <random>

using std::max;

GeoPixelLadderAlpineBuilder::GeoPixelLadderAlpineBuilder(const PixelGeoBuilderBasics* basics, 
							 const InDet::StaveTmp *staveTmp, 
							 int iLayer, HepGeom::Transform3D trf):
  PixelGeoBuilder(basics),
  m_staveTmp(staveTmp),
  m_layer(iLayer),
  m_localTrf(trf),
  m_sector(0),
  m_barrelModule(0),
  m_barrelModuleDesign(0),
  m_endcapModule(0),
  m_endcapModuleDesign(0),
  m_transitionModule(0),
  m_transitionModuleDesign(0),
  m_barrelModule_geocomp(0),
  m_endcapModule_geocomp(0),
  m_transitionModule_geocomp(0),
  m_staveSupport(0),
  m_ladderShape(0),
  m_pixelModuleSvc("PixelModuleSvc","PixelModuleSvc"),
  m_pixelDesignSvc("PixelDesignSvc","PixelDesignSvc")

{
  m_ladderGeoComp = 0;
  m_bGeoComp = basics->isGeoComponent();

  // Build stave support and module
  preBuild();
}


// The module has X axis (depth axis) out of the plane pointing
// towards the chips. Z axis is the eta direction.  
// For the module at global phi ~= 0 the module X,Y,Z axis are
// approximately aligned with the global X,Y,Z (ignoring the tilts)

// Looking from IP at global phi ~=0  
//
//
//      
//    Z ^
//      |
//      |
//  Y<--x
//
// X axis into page
//
// MCC side
// 
//            _ _ 0   F        ^z
//  pigtail  |_ _ .   .        |
//                .   .        |  
//                .   .    <-- x     Module ref system (Hit frame)
//                7   8    y
//
//


void GeoPixelLadderAlpineBuilder::preBuild( ) {


  // Check if the envelope is defined as a tool and available in ToolSvc
  std::cout<<"RETRIEVE PIXEL MODULE SVC  -  algTools " <<std::endl;
  StatusCode sc = m_pixelModuleSvc.retrieve();
  if(sc.isFailure())
    {
      std::cout << "Could not retrieve pixel module builder tool " <<  m_pixelModuleSvc << ",  some services will not be built." << std::endl;
    } 
  else 
    {
      std::cout << "Pixel module builder tool retrieved: " << m_pixelModuleSvc << std::endl;
    }


  std::string value;
  printf("************** BUILD LADDER for layer  %d\n", m_layer);

  std::string staveType = m_staveTmp->type;

  m_barrelModuleNumber = m_staveTmp->b_modn;
  m_barrelModuleType = m_staveTmp->b_type;
  m_barrelModuleGap =  m_staveTmp->b_gap;

  m_endcapModuleNumber = 0;
  m_endcapModuleType = "";
  m_endcapModulePos.clear();
  m_endcapModuleGap = 0;
  m_endcapTiltAngle = 0;
  m_transitionModuleType = "";
  m_transitionTiltAngle = 0;
  m_transModulePos.clear();
  if(staveType.compare("Alpine")==0){
    m_endcapModulePos = m_staveTmp->alp_pos;
    m_endcapModuleType = m_staveTmp->alp_type;
    m_endcapModuleGap = 0.;
    m_endcapTiltAngle = m_staveTmp->alp_angle;

    m_transModulePos = m_staveTmp->trans_pos;
    m_transitionModuleType = m_staveTmp->trans_type;
    m_transitionTiltAngle = m_staveTmp->trans_angle;

    m_endcapModPos = DecodeEndCapModulePositions(0., 0., m_transModulePos,m_endcapModulePos);
    m_endcapModuleNumber = (int)m_endcapModPos.size();

//     m_endcapModPos = DecodeEndCapModulePositions(0., 0., m_endcapModulePos);
//     m_endcapModuleNumber = (int)(m_endcapModPos.size());

      //    m_endcapModuleNumber = getInt("PixelStave", ladderIndex, "EndcapModules");
//     m_endcapModuleType = getChildValue("PixelStave", ladderIndex, "EndcapModuleType");
//     m_endcapModuleGap = getDouble("PixelStave", ladderIndex, "EndcapEndcapGap");
//     m_endcapModulePos = getString("PixelStave", ladderIndex, "EndcapPosOnStavePlane");
//     m_endcapTiltAngle = getDouble("PixelStave", ladderIndex, "EndcapSensorTilt");
//     m_transitionModuleType = getString("PixelStave", ladderIndex, "TransitionModuleType");
//     m_transitionTiltAngle = getDouble("PixelStave", ladderIndex, "TransitionSensorTilt");

//     m_endcapModPos = DecodeEndCapModulePositions(0., 0., m_endcapModulePos);
//     m_endcapModuleNumber = (int)(m_endcapModPos.size());

  }


  //  double phiOfSTaveZero = 0.;

  m_transitionTiltAngle = m_endcapTiltAngle;
  int layerModuleIndex = 0;
  std::cout<<"xxxxxxxxxxxxx Get barrel module from svc : "<<m_barrelModuleType<<" / "<<staveType<<std::endl;
  m_barrelModule = m_pixelModuleSvc->getModule(getBasics(),0,layerModuleIndex,m_barrelModuleType);
  m_barrelModuleDesign = m_pixelDesignSvc->getDesign(getBasics(),m_barrelModuleType);
  if(m_bGeoComp)
    m_barrelModule_geocomp = dynamic_cast<InDet::GeoDetModule*>(m_pixelModuleSvc->getModuleGeoComp(getBasics(),0,m_layer,m_barrelModuleType));

  std::cout<<"-- Barrel modules : "<<m_barrelModuleNumber<<" "<<m_barrelModuleType<<" / "<<staveType<<std::endl;

  double maxTiltedModuleThick=0.; 
  if(staveType.compare("Alpine")==0){
    std::cout<<"xxxxxxxxxxxxx Get endcap module from svc : "<<m_endcapModuleType<<" / "<<staveType<<std::endl;
    m_endcapModule =  m_pixelModuleSvc->getModule(getBasics(),0,layerModuleIndex,m_endcapModuleType);
    m_endcapModuleDesign = m_pixelDesignSvc->getDesign(getBasics(),m_endcapModuleType);
    m_transitionModule =  m_pixelModuleSvc->getModule(getBasics(),0,layerModuleIndex,m_transitionModuleType);
    m_transitionModuleDesign = m_pixelDesignSvc->getDesign(getBasics(),m_transitionModuleType);
    maxTiltedModuleThick=std::max(m_endcapModule->Length()*cos(m_endcapTiltAngle), 
				  m_transitionModule->Length()*cos(m_transitionTiltAngle));

    if(m_bGeoComp){
      m_endcapModule_geocomp = dynamic_cast<InDet::GeoDetModule*>(m_pixelModuleSvc->getModuleGeoComp(getBasics(),0,m_layer,m_endcapModuleType));
      m_transitionModule_geocomp =  dynamic_cast<InDet::GeoDetModule*>(m_pixelModuleSvc->getModuleGeoComp(getBasics(),0,m_layer,m_transitionModuleType));
    }
  }


  std::cout<<"xxxxxxxxxxxxx Ladder modules barrel/endcap "<<m_barrelModuleNumber<<" "<<m_endcapModuleNumber<<"   Total "<<m_barrelModuleNumber*.5+m_endcapModuleNumber<<std::endl;

  int iSector = 0;
  std::cout<<"xxxxxxxxxxxxx Build stave support for layer : "<<m_layer<<std::endl;
  double zEndOfNBarrelModulePos = (m_barrelModuleNumber*m_barrelModule->Length()+m_barrelModuleGap*(m_barrelModuleNumber-1))*.5;
  m_staveSupport = new GeoPixelStaveSupportAlpineBuilder( getBasics(),
							  m_layer, iSector, m_barrelModule, 
							  maxTiltedModuleThick, zEndOfNBarrelModulePos,
							  m_endcapModPos); 
  
  m_thicknessN = m_staveSupport->thicknessN() * 1.15;
  m_thicknessP = m_staveSupport->thicknessP() * 1.15;
  m_length = m_staveSupport->length()+5.;
  m_width = m_staveSupport->width()+.01;
  m_FacePlateThick = m_staveSupport->getFacePlateThickness();

  std::cout<<"** stave support thicknesses   : "<<m_thicknessN<<"  "<<m_thicknessP<<std::endl;
  std::cout<<"** stave support width/length  : "<<m_width<<"  "<<m_length<<std::endl;

  double halfThickness = 0.5*(m_thicknessP+m_thicknessN);
  double shift = 0.5*(m_thicknessP-m_thicknessN);
  GeoBox * box = new GeoBox(halfThickness, m_width/2., m_length/2.);
  const GeoShape & shiftedBox = (*box) << HepGeom::TranslateX3D(shift);
  m_ladderShape = &shiftedBox;  

  const GeoMaterial* air = matMgr()->getMaterial("special::Ether");
  m_theLadder = new GeoLogVol("Ladder",m_ladderShape,air);

  m_rmin = 999999.9;
  m_rmax = -999999.9;

  // Register the number of mopdules defined for a stave
  int nbTotModule = m_barrelModuleNumber;
  if(m_endcapModuleNumber>0) nbTotModule += 2*(m_endcapModuleNumber+1);
  getBasics()->getDetectorManager()->numerology().setNumEtaModulesForLayer(m_layer, nbTotModule);

//   //  calcRminmax();
  
//   if(m_layer==0){
//     m_rmin = m_staveSupport->thicknessN() +3.;
//     m_rmax = m_staveSupport->thicknessP() +3.;
//   }

//   std::cout<<"** compute Rminmax : "<<m_rmin<<"  "<<m_rmax<<std::endl;
}



GeoVPhysVol* GeoPixelLadderAlpineBuilder::Build( ) {

  GeoPhysVol* ladderPhys = new GeoPhysVol(m_theLadder);
  if(m_bGeoComp)
    m_ladderGeoComp = new InDet::GeoStaveAlpine(ladderPhys);

  int iModuleCmpt = 0; 

  double radiusMin = 999999.9;
  double radiusMax = -999999.9;
  
  bool bVerbose = false;
  //    bVerbose = true;
  
  //  m_endcapTiltAngle=90.*CLHEP::deg-m_endcapTiltAngle;
  
  if(m_endcapModuleNumber>0)
    iModuleCmpt = -(m_endcapModuleNumber+1)-m_barrelModuleNumber/2 - (m_barrelModuleNumber%2);
  else
    iModuleCmpt = -m_barrelModuleNumber/2 - (m_barrelModuleNumber%2);

  if(bVerbose)std::cout<<"FIRST module number "<<m_barrelModuleNumber<<"  "<<m_endcapModuleNumber<<"  "<<iModuleCmpt<<std::endl;
  
  int alpineModTag = 500;

   //--------------------------------------------------------
   // Compute endcap module positions
   //--------------------------------------------------------
  std::vector<HepGeom::Point3D<double> > endcapModulePos;
   if(m_endcapModuleNumber>0)
     {
       HepGeom::Point3D<double> vDir_endcap(-cos(m_endcapTiltAngle), sin(m_endcapTiltAngle), 0.);
       HepGeom::Point3D<double> vDirPerp_endcap(sin(m_endcapTiltAngle), cos(m_endcapTiltAngle), 0.);
       HepGeom::Point3D<double> vDir_transition(-cos(m_transitionTiltAngle), sin(m_transitionTiltAngle), 0.);
       HepGeom::Point3D<double> vDirPerp_transition(sin(m_transitionTiltAngle), cos(m_transitionTiltAngle), 0.);

       double EcModThick_chip= m_endcapModule->ThicknessP();
       double EcModHalfLen = m_endcapModule->Length()*.5;
       
       for(int iPos=0; iPos<m_endcapModuleNumber; iPos++)
	 {
	   double xPos=m_barrelModule->ThicknessP();
	   double yPos=m_endcapModPos[iPos].y();
	   double zPos=m_endcapModPos[iPos].z();
	   
	   if(iPos>0) {
	     xPos=xPos+EcModHalfLen*vDir_endcap.x()+EcModThick_chip*vDirPerp_endcap.x();
	     zPos=zPos+EcModHalfLen*vDir_endcap.y()+EcModThick_chip*vDirPerp_endcap.y();
	     yPos=0.;
	     endcapModulePos.push_back(HepGeom::Point3D<double>(xPos,yPos,zPos));
	   }	      
	   else {
	     xPos=xPos+EcModHalfLen*vDir_transition.x()+EcModThick_chip*vDirPerp_transition.x();
	     zPos=zPos+EcModHalfLen*vDir_transition.y()+EcModThick_chip*vDirPerp_transition.y();
	     yPos-=m_transitionModule->Width()*.5;
	     endcapModulePos.push_back(HepGeom::Point3D<double>(xPos,yPos,zPos));
	   }
	 }
     }
       
   //------------------------------------------------------
   // Place the endcap modules (Z<0)
   //------------------------------------------------------
   if(m_endcapModuleNumber>0)
     {
      
       GeoPixelEndcapFoamModule foamBuilder(getBasics(),m_layer, m_endcapModule, m_transitionModule, m_endcapTiltAngle, m_transitionTiltAngle);
       GeoPhysVol* foamEndcap = dynamic_cast<GeoPhysVol*>(foamBuilder.getEndcapFoam());
       
       double zEndcapFoamShift = foamBuilder.getEndcapZshift()+.25;
       GeoPhysVol* foamTrans = dynamic_cast<GeoPhysVol*>(foamBuilder.getTransFoam());
       double zTransFoamShift = foamBuilder.getTransZshift()+.25;
       HepGeom::Transform3D trfFoam = HepGeom::RotateX3D(180.*CLHEP::deg)*HepGeom::RotateZ3D(270.*CLHEP::deg)*HepGeom::RotateY3D(90.*CLHEP::deg);
       
       
       HepGeom::Transform3D EcRot= HepGeom::RotateY3D(270.*CLHEP::deg-m_endcapTiltAngle)*HepGeom::RotateZ3D(180.*CLHEP::deg);
       
       int endcapIterator = m_endcapModuleNumber-1;
       for(int iNeg=0; iNeg<m_endcapModuleNumber; iNeg++)
	{
	  double xPos, yPos, zPos;

	  GeoFullPhysVol* modulePhys = 0;
	  Identifier idwafer;
	  if(iNeg<m_endcapModuleNumber-1) {
	    //	    m_endcapModule->setIdentifierFlags(0, m_layer, m_sector, iModuleCmpt);
	    std::ostringstream modName; 
	    modName<<"_"<<m_layer<<"_"<<m_sector<<"_"<<iModuleCmpt;
	    modulePhys = m_endcapModule->Build(0, m_layer, m_sector, iModuleCmpt, alpineModTag , modName.str());
	    xPos=endcapModulePos[endcapIterator].x();
	    yPos=endcapModulePos[endcapIterator].y();
	    zPos=-endcapModulePos[endcapIterator].z();
	    if(bVerbose)std::cout<<"ENDCAP MODULE - ec : "<<m_layer<<" "<<iNeg<<"  "<<zPos<<std::endl;
	    if(bVerbose)std::cout<<"wafer id : "<<0<<" "<< m_layer<<" "<< m_sector<<" "<< iModuleCmpt<<std::endl;
	    idwafer = getBasics()->getIdHelper()->wafer_id(0, m_layer, m_sector, iModuleCmpt);
	    InDetDD::SiDetectorElement* element = new InDetDD::SiDetectorElement( idwafer, m_endcapModuleDesign, 
										  modulePhys, getBasics()->getCommonItems());
	    //	    getBasics()->getDetectorManager()->addDesign(m_barrelModuleDesign);
	    getBasics()->getDetectorManager()->addDetectorElement(element);
	  }
	  else {
	    //m_transitionModule->setIdentifierFlags(0, m_layer, m_sector, iModuleCmpt);
	    std::ostringstream modName; 
	    modName<<"_"<<m_layer<<"_"<<m_sector<<"_"<<iModuleCmpt;
	    modulePhys = m_transitionModule->Build(0, m_layer, m_sector, iModuleCmpt, alpineModTag , modName.str());
	    EcRot= HepGeom::RotateY3D(270.*CLHEP::deg-m_transitionTiltAngle)*HepGeom::RotateZ3D(180.*CLHEP::deg);
	    xPos=endcapModulePos[endcapIterator].x();
	    yPos=endcapModulePos[endcapIterator].y();
	    zPos=-endcapModulePos[endcapIterator].z();
	    if(bVerbose)std::cout<<"ENDCAP MODULE - tr : "<<m_layer<<" "<<iNeg<<"  "<<zPos<<std::endl;

	    if(bVerbose)std::cout<<"wafer id : "<<0<<" "<< m_layer<<" "<< m_sector<<" "<< iModuleCmpt<<std::endl;
	    idwafer = getBasics()->getIdHelper()->wafer_id(0, m_layer, m_sector, iModuleCmpt);
	    InDetDD::SiDetectorElement* element = new InDetDD::SiDetectorElement( idwafer, m_transitionModuleDesign, 
										  modulePhys, getBasics()->getCommonItems());
	    //	    getBasics()->getDetectorManager()->addDesign(m_barrelModuleDesign);
	    getBasics()->getDetectorManager()->addDetectorElement(element);
	  }
	  
	  const HepGeom::Transform3D moduleTrans = HepGeom::Transform3D(EcRot.getRotation(),HepGeom::Vector3D<double> (xPos,yPos,zPos));
	  if(bVerbose)std::cout<<"*******************************************************************"<<std::endl;
	  if(bVerbose)std::cout<<"-> place endcap module : "<<iNeg<<" "<<endcapIterator<<"  "<<zPos<<std::endl;

	  std::ostringstream nameTag; 
	  nameTag << "ModuleBrl" << iModuleCmpt;
	  GeoNameTag * tag = new GeoNameTag(nameTag.str());
	  GeoAlignableTransform* xform = new GeoAlignableTransform(moduleTrans);
	  if(!m_bGeoComp){
	    ladderPhys->add(tag);
	    ladderPhys->add(new GeoIdentifierTag(iModuleCmpt) );
	    ladderPhys->add(xform);
	    ladderPhys->add(modulePhys);
	  }
	  else{
	    if(bVerbose)std::cout<<"-> placeEndcapModule : "<<m_endcapModuleNumber-1-endcapIterator<<" "<<std::endl;
	    if(iNeg<m_endcapModuleNumber-1)
	      m_ladderGeoComp->placeEndcapModule( *m_endcapModule_geocomp, moduleTrans, iModuleCmpt, -1, m_endcapModuleNumber-1-endcapIterator,iModuleCmpt);
	    else
	      m_ladderGeoComp->placeEndcapModule( *m_transitionModule_geocomp, moduleTrans, iModuleCmpt, -1, m_endcapModuleNumber-1-endcapIterator,iModuleCmpt);
	  }

	  if(m_sector==0){
	    if(iNeg==0)
	      computeRadiusMinMax(m_ladderTransform*moduleTrans, 
				  m_endcapModule->Length(),  m_endcapModule->Width(), m_endcapModule->Thickness(), radiusMin, radiusMax);
	    else if(iNeg==m_endcapModuleNumber-1)
	      computeRadiusMinMax(m_ladderTransform*moduleTrans, 
				  m_transitionModule->Length(),  m_transitionModule->Width(), m_transitionModule->Thickness(), radiusMin, radiusMax);
	  }
	  
	  // Now store the xform by identifier:
	  //	  Identifier id = m_endcapModule->getID();
	  //	  DDmgr->addAlignableTransform(0,id,xform,modulePhys);
	  getBasics()->getDetectorManager()->addAlignableTransform(0,idwafer,xform,modulePhys);

 	  if(iNeg<m_endcapModuleNumber-1) {
	    std::ostringstream nameTag; 
	    nameTag << "EndcapFoam_L" <<m_layer<<"M"<<iModuleCmpt;
	    GeoNameTag * tag = new GeoNameTag(nameTag.str());
	    const HepGeom::Transform3D foamTrf = HepGeom::TranslateX3D(m_barrelModule->ThicknessP()+m_FacePlateThick)*HepGeom::TranslateZ3D(zPos-zEndcapFoamShift)*trfFoam;
	    GeoAlignableTransform* xformFoam = new GeoAlignableTransform(foamTrf);
	    ladderPhys->add(tag);
	    ladderPhys->add(xformFoam);
	    ladderPhys->add(foamEndcap);
 	  }
 	  else {
	    std::ostringstream nameTag; 
	    nameTag << "EndcapFoam_L" <<m_layer<<"M"<<iModuleCmpt;
	    GeoNameTag * tag = new GeoNameTag(nameTag.str());
 	    const HepGeom::Transform3D foamTrf = HepGeom::TranslateY3D(yPos)*HepGeom::TranslateX3D(m_barrelModule->ThicknessP()+m_FacePlateThick)*HepGeom::TranslateZ3D(zPos-zTransFoamShift)*trfFoam;
 	    GeoAlignableTransform* xformFoam = new GeoAlignableTransform(foamTrf);
	    ladderPhys->add(tag);
 	    ladderPhys->add(xformFoam);
 	    ladderPhys->add(foamTrans);
 	  }

	  iModuleCmpt++;
	  endcapIterator--;
	}

    }


  //------------------------------------------------------
  // Place the barrel modules
  //------------------------------------------------------
   double zNegStavePos= (m_barrelModuleNumber*m_barrelModule->Length()+m_barrelModuleGap*(m_barrelModuleNumber-1))*.5;
   zNegStavePos *=-1.;

   double zpos = zNegStavePos;
   if(bVerbose)std::cout<<"BARREL BEGIN : "<<zpos<<std::endl;

   double zPosFinal;
   int iBarrelModuleCmpt = 0;
   for(int iBrl = 0; iBrl < m_barrelModuleNumber; iBrl++) 
     {

       if(iBrl==0&&m_endcapModuleNumber>0){

	 double xPosTrans = 0;
	 double yPosTrans = (m_transitionModule->Width()*.5);
	 double zPosTrans = zNegStavePos-m_barrelModuleGap-m_transitionModule->Length()*.5;
	 
	 CLHEP::Hep3Vector modulepos(xPosTrans, yPosTrans, zPosTrans);
	 CLHEP::HepRotation rm; rm.rotateY(0.);
	 
	 if(bVerbose)std::cout<<"*******************************************************************"<<std::endl;
	 if(bVerbose)std::cout<<"xx > module barrel transition Z<0 "<<iModuleCmpt<<" "<<iBrl<<"/"<<m_barrelModuleNumber<<std::endl;
	 //	 m_transitionModule->setIdentifierFlags(0, m_layer, m_sector, iModuleCmpt);
	 std::ostringstream modName; 
	 modName<<"_"<<m_layer<<"_"<<m_sector<<"_"<<iModuleCmpt;
	 GeoFullPhysVol* modulePhys = m_transitionModule->Build(0, m_layer, m_sector, iModuleCmpt, alpineModTag , modName.str());

	 if(bVerbose)std::cout<<"wafer id : "<<0<<" "<< m_layer<<" "<< m_sector<<" "<< iModuleCmpt<<std::endl;
	 Identifier idwafer = getBasics()->getIdHelper()->wafer_id(0, m_layer, m_sector, iModuleCmpt);
	 InDetDD::SiDetectorElement* element = new InDetDD::SiDetectorElement( idwafer, m_transitionModuleDesign, 
									       modulePhys, getBasics()->getCommonItems());
	 //	 getBasics()->getDetectorManager()->addDesign(m_barrelModuleDesign);
	 getBasics()->getDetectorManager()->addDetectorElement(element);

	 std::ostringstream nameTag; 
	 nameTag << "ModuleBrl" << iModuleCmpt;
	 GeoNameTag * tag = new GeoNameTag(nameTag.str());

	 HepGeom::Transform3D moduleTrans = HepGeom::Transform3D(rm,modulepos);
	 GeoAlignableTransform* xform = new GeoAlignableTransform(moduleTrans);
	 if(!m_bGeoComp){
	   ladderPhys->add(tag);
	   ladderPhys->add(new GeoIdentifierTag(iModuleCmpt));
	   ladderPhys->add(xform);
	   ladderPhys->add(modulePhys);
	 }
	 else{
	   if(bVerbose)std::cout<<"-> placeBarrelTransitionModule : "<<iBrl<<" "<<std::endl;
	   m_ladderGeoComp->placeBarrelModule( *m_transitionModule_geocomp, moduleTrans, iModuleCmpt,iBarrelModuleCmpt,iModuleCmpt);
	 }

	 // Now store the xform by identifier:
	 //	 Identifier id = m_barrelModule->getID();
	 //	 DDmgr->addAlignableTransform(0,id,xform,modulePhys);
	 getBasics()->getDetectorManager()->addAlignableTransform(0,idwafer,xform,modulePhys);

	 iModuleCmpt++;
	 iBarrelModuleCmpt++;
       }

       //       int etaModule = iBrl;
       //       double xpos=0.; /*, ypos=0.;*/
       zpos+=m_barrelModule->Length()*.5;

       if(bVerbose)std::cout<<"BARREL MODULE : "<<iBrl<<"  "<<zpos<<std::endl;
       
       //       CLHEP::Hep3Vector modulepos(xpos+xposShift,yposShift,zpos);
       CLHEP::Hep3Vector modulepos(0.,0.,zpos);
       //
       //  Rotate if module is inclined.
       //
       CLHEP::HepRotation rm;
       //       rm.rotateY(gmt_mgr->PixelModuleAngle()*gmt_mgr->PixelModuleAngleSign(etaModule) );
       rm.rotateY(0.);
       //
       // Place the Module
       //

       if(bVerbose)std::cout<<"*******************************************************************"<<std::endl;
       if(bVerbose)std::cout<<"xx > module barrel planar "<<iModuleCmpt<<" "<<iBrl<<"/"<<m_barrelModuleNumber<<std::endl;
       //       m_barrelModule->setIdentifierFlags(0, m_layer, m_sector, iModuleCmpt);
       std::ostringstream modName; 
       modName<<"_"<<m_layer<<"_"<<m_sector<<"_"<<iModuleCmpt;
       GeoFullPhysVol* modulePhys = m_barrelModule->Build(0, m_layer, m_sector, iModuleCmpt, alpineModTag , modName.str());
       
       if(bVerbose)std::cout<<"wafer id : "<<0<<" "<< m_layer<<" "<< m_sector<<" "<< iModuleCmpt<<std::endl;
       Identifier idwafer = getBasics()->getIdHelper()->wafer_id(0, m_layer, m_sector, iModuleCmpt);
       InDetDD::SiDetectorElement* element = new InDetDD::SiDetectorElement( idwafer, m_barrelModuleDesign, 
									     modulePhys, getBasics()->getCommonItems());
       //       getBasics()->getDetectorManager()->addDesign(m_barrelModuleDesign);
       getBasics()->getDetectorManager()->addDetectorElement(element);

       std::ostringstream nameTag; 
       nameTag << "ModuleBrl" << iModuleCmpt;
       GeoNameTag * tag = new GeoNameTag(nameTag.str());
       HepGeom::Transform3D barrelTrans = HepGeom::Transform3D(rm,modulepos);
       GeoAlignableTransform* xform = new GeoAlignableTransform(barrelTrans);
       if(!m_bGeoComp){
	 ladderPhys->add(tag);
	 ladderPhys->add(new GeoIdentifierTag(iModuleCmpt));
	 ladderPhys->add(xform);
	 ladderPhys->add(modulePhys);
       }
       else{
	 if(bVerbose)std::cout<<"-> placeBarrelTransitionModule : "<<iBrl<<" "<<std::endl;
	 m_ladderGeoComp->placeBarrelModule( *m_barrelModule_geocomp, barrelTrans, iModuleCmpt,iBarrelModuleCmpt,iModuleCmpt);
       }

       if(m_sector==0&&iBrl==0){
 	 computeRadiusMinMax(m_ladderTransform*HepGeom::Transform3D(rm,modulepos), 
 			     m_barrelModule->Length(),  m_staveSupport->width(), 2.*m_staveSupport->thickness(), radiusMin, radiusMax);
	 computeRadiusMinMax(m_ladderTransform*HepGeom::Transform3D(rm,modulepos), 
			     m_barrelModule->Length(),  m_barrelModule->Width(), m_barrelModule->Thickness(), radiusMin, radiusMax);
       }
       
       // Now store the xform by identifier:
       //       Identifier id = m_barrelModule->getID();
       //       DDmgr->addAlignableTransform(0,id,xform,modulePhys);
       getBasics()->getDetectorManager()->addAlignableTransform(0,idwafer,xform,modulePhys);

       zpos+=(m_barrelModule->Length()*.5+m_barrelModuleGap);
       iModuleCmpt++;
       iBarrelModuleCmpt++;
       if(iModuleCmpt==0) iModuleCmpt++;

       if((iBrl==m_barrelModuleNumber-1)&&m_endcapModuleNumber>0){

	 double xPosTrans = 0;
	 double yPosTrans = (m_transitionModule->Width()*.5);
	 double zPosTrans = -zNegStavePos+m_barrelModuleGap+m_transitionModule->Length()*.5;

	 CLHEP::Hep3Vector modulepos(xPosTrans, yPosTrans, zPosTrans);
	 CLHEP::HepRotation rm; rm.rotateY(0.);

	 if(bVerbose)std::cout<<"*******************************************************************"<<std::endl;
	 if(bVerbose)std::cout<<"xx > module barrel transition Z>0 "<<iModuleCmpt<<" "<<iBrl<<"/"<<m_barrelModuleNumber<<std::endl;
	 //	 m_transitionModule->setIdentifierFlags(0, m_layer, m_sector, iModuleCmpt);
	 std::ostringstream modName; 
	 modName<<"_"<<m_layer<<"_"<<m_sector<<"_"<<iModuleCmpt;
	 GeoFullPhysVol* modulePhys = m_transitionModule->Build(0, m_layer, m_sector, iModuleCmpt, alpineModTag , modName.str());
	 
	 if(bVerbose)std::cout<<"wafer id : "<<0<<" "<< m_layer<<" "<< m_sector<<" "<< iModuleCmpt<<std::endl;
	 Identifier idwafer = getBasics()->getIdHelper()->wafer_id(0, m_layer, m_sector, iModuleCmpt);
	 InDetDD::SiDetectorElement* element = new InDetDD::SiDetectorElement( idwafer, m_transitionModuleDesign, 
									       modulePhys, getBasics()->getCommonItems());
	 //	 getBasics()->getDetectorManager()->addDesign(m_barrelModuleDesign);
	 getBasics()->getDetectorManager()->addDetectorElement(element);

	 std::ostringstream nameTag; 
	 nameTag << "ModuleBrl" << iModuleCmpt;
	 GeoNameTag * tag = new GeoNameTag(nameTag.str());
	 HepGeom::Transform3D moduleTrans = HepGeom::Transform3D(rm,modulepos);
	 GeoAlignableTransform* xform = new GeoAlignableTransform(moduleTrans);
	 if(!m_bGeoComp){
	   ladderPhys->add(tag);
	   ladderPhys->add(new GeoIdentifierTag(iModuleCmpt));
	   ladderPhys->add(xform);
	   ladderPhys->add(modulePhys);
	 }
	 else{
	   if(bVerbose)std::cout<<"-> placeBarrelTransitionModule : "<<iBrl<<" "<<std::endl;
	   m_ladderGeoComp->placeBarrelModule( *m_transitionModule_geocomp, moduleTrans, iModuleCmpt,iBarrelModuleCmpt,iModuleCmpt);
	 }
	 
	 // Now store the xform by identifier:
	 //	 Identifier id = m_barrelModule->getID();
	 //	 DDmgr->addAlignableTransform(0,id,xform,modulePhys);
	 getBasics()->getDetectorManager()->addAlignableTransform(0,idwafer,xform,modulePhys);

	 iModuleCmpt++;
	 iBarrelModuleCmpt++;

	 zPosFinal = zPosTrans+m_transitionModule->Length()*.5;
       }
       else
	 zPosFinal = zpos+m_barrelModule->Length()*.5;
     }

   if(bVerbose)std::cout<<"BARREL END "<<m_layer<<" : "<<-zNegStavePos<<"  "<<zPosFinal<<std::endl;

  //------------------------------------------------------
  // Place the endcap modules (Z>0)
  //------------------------------------------------------
  if(m_endcapModuleNumber>0)
    {
      //      double xModPos=0.0;
      //      double yModPos=0.0;
      GeoPixelEndcapFoamModule foamBuilder(getBasics(), m_layer, m_endcapModule, m_transitionModule, m_endcapTiltAngle, m_transitionTiltAngle);
      GeoPhysVol* foamEndcap = dynamic_cast<GeoPhysVol*>(foamBuilder.getEndcapFoam());

      double zEndcapFoamShift = foamBuilder.getEndcapZshift()+.25; //m_endcapModule->ThicknessN()*sin(m_endcapTiltAngle);
      GeoPhysVol* foamTrans = dynamic_cast<GeoPhysVol*>(foamBuilder.getTransFoam());
      double zTransFoamShift = foamBuilder.getTransZshift()+.25;
      HepGeom::Transform3D trfFoam = HepGeom::RotateX3D(0.*CLHEP::deg)*HepGeom::RotateZ3D(270.*CLHEP::deg)*HepGeom::RotateY3D(90.*CLHEP::deg);

      //      double EcModThick_chip= m_endcapModule->ThicknessP();
      //      double EcModThick_hyb = m_endcapModule->ThicknessN();
      //      double EcModHalfLen = m_endcapModule->Length()*.5;
      //      double deltaZ = fabs(m_endcapModule->Thickness()*cos(m_endcapTiltAngle)) - fabs(EcModThick_chip*cos(m_endcapTiltAngle));
      //      double deltaZ_trans = fabs(m_endcapModule->Thickness()*cos(m_transitionTiltAngle)) - fabs(EcModThick_chip*cos(m_transitionTiltAngle));
      
      HepGeom::Transform3D EcRot_pos= HepGeom::RotateY3D(90.*CLHEP::deg-m_endcapTiltAngle)*HepGeom::RotateZ3D(180.*CLHEP::deg);

      HepGeom::Point3D<double> vDir_endcap(-cos(m_endcapTiltAngle), sin(m_endcapTiltAngle), 0.);
      HepGeom::Point3D<double> vDirPerp_endcap(sin(m_endcapTiltAngle), cos(m_endcapTiltAngle), 0.);
      HepGeom::Point3D<double> vDir_transition(-cos(m_transitionTiltAngle), sin(m_transitionTiltAngle), 0.);
      HepGeom::Point3D<double> vDirPerp_transition(sin(m_transitionTiltAngle), cos(m_transitionTiltAngle), 0.);

      int endcapIterator = 0;
      Identifier idwafer;
      for(int iPos=0; iPos<m_endcapModuleNumber; iPos++)
	{
	  double xPos=0., yPos=0., zPos=0.;
	  GeoFullPhysVol* modulePhys = 0;
	  if(iPos>0) {
	    //	    m_endcapModule->setIdentifierFlags(0, m_layer, m_sector, iModuleCmpt);
	    std::ostringstream modName; 
	    modName<<"_"<<m_layer<<"_"<<m_sector<<"_"<<iModuleCmpt;
	    modulePhys = m_endcapModule->Build(0, m_layer, m_sector, iModuleCmpt, alpineModTag , modName.str());
	    xPos=endcapModulePos[iPos].x();
	    yPos=endcapModulePos[iPos].y();
	    zPos=endcapModulePos[iPos].z();
	    EcRot_pos= HepGeom::RotateY3D(90.*CLHEP::deg+m_endcapTiltAngle)*HepGeom::RotateZ3D(180.*CLHEP::deg);

	    if(bVerbose)std::cout<<"ENDCAP MODULE : ec "<<m_layer<<" "<<iPos<<"  "<<zPos<<std::endl;

	    idwafer = getBasics()->getIdHelper()->wafer_id(0, m_layer, m_sector, iModuleCmpt);
	    InDetDD::SiDetectorElement* element = new InDetDD::SiDetectorElement( idwafer, m_endcapModuleDesign, 
										  modulePhys, getBasics()->getCommonItems());
	    //	    getBasics()->getDetectorManager()->addDesign(m_barrelModuleDesign);
	    getBasics()->getDetectorManager()->addDetectorElement(element);
	  }
	  else {
	    //	    m_transitionModule->setIdentifierFlags(0, m_layer, m_sector, iModuleCmpt);
	    std::ostringstream modName; 
	    modName<<"_"<<m_layer<<"_"<<m_sector<<"_"<<iModuleCmpt;
	    modulePhys = m_transitionModule->Build(0, m_layer, m_sector, iModuleCmpt, alpineModTag , modName.str());
	    EcRot_pos= HepGeom::RotateY3D(90.*CLHEP::deg+m_transitionTiltAngle)*HepGeom::RotateZ3D(180.*CLHEP::deg);
	    xPos=endcapModulePos[iPos].x();
	    yPos=endcapModulePos[iPos].y();
	    zPos=endcapModulePos[iPos].z();
	    if(bVerbose)std::cout<<"ENDCAP MODULE - tr "<<m_layer<<" "<<iPos<<"  "<<zPos<<std::endl;

	    idwafer = getBasics()->getIdHelper()->wafer_id(0, m_layer, m_sector, iModuleCmpt);
	    InDetDD::SiDetectorElement* element = new InDetDD::SiDetectorElement( idwafer, m_transitionModuleDesign, 
										  modulePhys, getBasics()->getCommonItems());
	    //	    getBasics()->getDetectorManager()->addDesign(m_barrelModuleDesign);
	    getBasics()->getDetectorManager()->addDetectorElement(element);

	  }

	  const HepGeom::Transform3D moduleTrans = HepGeom::Transform3D(EcRot_pos.getRotation(),HepGeom::Vector3D<double> (xPos,yPos,zPos));
	  if(bVerbose)std::cout<<"*******************************************************************"<<std::endl;
	  if(bVerbose)std::cout<<"-> place endcap module : "<<iPos<<" "<<endcapIterator<<"  "<<zPos<<std::endl;

	  std::ostringstream nameTag; 
	  nameTag << "ModuleBrl" << iModuleCmpt;
	  GeoNameTag * tag = new GeoNameTag(nameTag.str());
	  GeoAlignableTransform* xform = new GeoAlignableTransform(moduleTrans);
	  if(!m_bGeoComp){
	    ladderPhys->add(tag);
	    ladderPhys->add(new GeoIdentifierTag(iModuleCmpt) );
	    ladderPhys->add(xform);
	    ladderPhys->add(modulePhys);
	  }
	  else{
	    if(bVerbose)std::cout<<"-> placeEndcapModule : "<<endcapIterator<<" "<<std::endl;
	    if(iPos>0)
	      m_ladderGeoComp->placeEndcapModule( *m_endcapModule_geocomp, moduleTrans, iModuleCmpt, 1,endcapIterator,iModuleCmpt);
	    else
	      m_ladderGeoComp->placeEndcapModule( *m_transitionModule_geocomp, moduleTrans, iModuleCmpt, 1,endcapIterator,iModuleCmpt);
	  }
	  
	  // Now store the xform by identifier:
	  //	  Identifier id = m_endcapModule->getID();
	  //	  DDmgr->addAlignableTransform(0,id,xform,modulePhys);
	  getBasics()->getDetectorManager()->addAlignableTransform(0,idwafer,xform,modulePhys);

	  if(iPos>0) {
	    std::ostringstream nameTag; 
	    nameTag << "EndcapFoam_L" <<m_layer<<"M"<<iModuleCmpt;
	    GeoNameTag * tag = new GeoNameTag(nameTag.str());
	    const HepGeom::Transform3D foamTrf = HepGeom::TranslateX3D(m_barrelModule->ThicknessP()+m_FacePlateThick)*HepGeom::TranslateZ3D(zPos+zEndcapFoamShift)*trfFoam;
	    GeoAlignableTransform* xformFoam = new GeoAlignableTransform(foamTrf);
	    ladderPhys->add(tag);
	    ladderPhys->add(xformFoam);
	    ladderPhys->add(foamEndcap);
	  }
	  else {
	    std::ostringstream nameTag; 
	    nameTag << "EndcapFoam_L" <<m_layer<<"M"<<iModuleCmpt;
	    GeoNameTag * tag = new GeoNameTag(nameTag.str());
	    const HepGeom::Transform3D foamTrf = HepGeom::TranslateY3D(yPos)*HepGeom::TranslateX3D(m_barrelModule->ThicknessP()+m_FacePlateThick)*HepGeom::TranslateZ3D(zPos+zTransFoamShift)*trfFoam;
	    GeoAlignableTransform* xformFoam = new GeoAlignableTransform(foamTrf);
	    ladderPhys->add(tag);
	    ladderPhys->add(xformFoam);
	    ladderPhys->add(foamTrans);
	  }

	  iModuleCmpt++;
	  endcapIterator++;
	}

    }

  //  std::cout<<"ALPINE ladder module number : "<<iModuleCmpt<<std::endl;

   //Add the TMT or other stave support
   if (m_staveSupport) {
     ladderPhys->add(new GeoTransform(m_staveSupport->transform()));
     if(!m_bGeoComp){
       if(m_staveSupport->getPhysVol())
	 ladderPhys->add(m_staveSupport->getPhysVol()); 
     }
     else
       m_ladderGeoComp->placeBarrelStave( *(m_staveSupport->getGeoCompVol()), m_staveSupport->transform(), 0);
   }

   if(m_sector==0){
     m_rmin = radiusMin;
     m_rmax = radiusMax;
   }
   
   return ladderPhys;
}



void GeoPixelLadderAlpineBuilder::calcRminmax() {


  //  std::cout<<"void GeoPixelLadderAlpineBuilder::calcRminmax()"<<std::endl;

  double rMin = 9999.;
  double rMax = -9999.;

  HepGeom::Transform3D trfId=HepGeom::Transform3D();
  std::vector<HepGeom::Point3D<double> > outline=getVertexShape(m_theLadder, trfId);
  for (std::vector<HepGeom::Point3D<double> >::const_iterator iPt=outline.begin(); iPt!=outline.end(); iPt++)
    {
      HepGeom::Point3D<double> pTrf=m_localTrf*(*iPt);
      //      std::cout<<pTrf.x()<<" "<<pTrf.y()<<std::endl;
      double radius=sqrt(pTrf.x()*pTrf.x()+pTrf.y()*pTrf.y());
      if(radius<rMin)rMin=radius;
      if(radius>rMax)rMax=radius;
    }  

  m_rmin = rMin;
  m_rmax = rMax;

}

void GeoPixelLadderAlpineBuilder::calcXMinMax(const GeoLogVol *logVol, HepGeom::Transform3D trf, double& xmin, double &xmax)
{

  double vMin=9999.;
  double vMax=-9999.;

  std::vector<HepGeom::Point3D<double> > outline=getVertexShape(logVol, trf);
  for (std::vector<HepGeom::Point3D<double> >::const_iterator iPt=outline.begin(); iPt!=outline.end(); iPt++)
    {
      HepGeom::Point3D<double> pTrf=trf*(*iPt);
      double tmp=pTrf.x();
      if(tmp<vMin)vMin=tmp;
      if(tmp>vMax)vMax=tmp;
    }  

  xmin=vMin;  xmax=vMax;
}

void GeoPixelLadderAlpineBuilder::calcYMinMax(const GeoLogVol *logVol, HepGeom::Transform3D trf, double& xmin, double &xmax)
{

  double vMin=9999.;
  double vMax=-9999.;

  std::vector<HepGeom::Point3D<double> > outline=getVertexShape(logVol, trf);
  for (std::vector<HepGeom::Point3D<double> >::const_iterator iPt=outline.begin(); iPt!=outline.end(); iPt++)
    {
      HepGeom::Point3D<double> pTrf=trf*(*iPt);
      double tmp=pTrf.y();
      if(tmp<vMin)vMin=tmp;
      if(tmp>vMax)vMax=tmp;
    }  

  xmin=vMin;  xmax=vMax;
}

void GeoPixelLadderAlpineBuilder::calcZMinMax(const GeoLogVol *logVol, HepGeom::Transform3D trf, double& xmin, double &xmax)
{

  double vMin=9999.;
  double vMax=-9999.;

  std::vector<HepGeom::Point3D<double> > outline=getVertexShape(logVol, trf);
  for (std::vector<HepGeom::Point3D<double> >::const_iterator iPt=outline.begin(); iPt!=outline.end(); iPt++)
    {
      HepGeom::Point3D<double> pTrf=trf*(*iPt);
      double tmp=pTrf.z();
      if(tmp<vMin)vMin=tmp;
      if(tmp>vMax)vMax=tmp;
    }  

  xmin=vMin;  xmax=vMax;
}


std::vector<HepGeom::Point3D<double>> GeoPixelLadderAlpineBuilder::getVertexShape(const GeoLogVol *logVol, HepGeom::Transform3D trf) const 
{
  return getVertexShape(logVol->getShape(), trf);
}

std::vector<HepGeom::Point3D<double>> GeoPixelLadderAlpineBuilder::getVertexShape(const GeoShape* shape, HepGeom::Transform3D /*trf*/) const 
{
  std::vector<HepGeom::Point3D<double> > vVertices;

  if(shape->type()=="Box")
    {
      //      if(bVerbose)std::cout<<"component is a box "<<std::endl;
      const GeoBox* bmBox = dynamic_cast< const GeoBox*>( shape);
      double xHalfSize=bmBox->getXHalfLength();
      double yHalfSize=bmBox->getYHalfLength();
      double zHalfSize=bmBox->getZHalfLength();
      vVertices.push_back(HepGeom::Point3D<double>(xHalfSize,yHalfSize,-zHalfSize));
      vVertices.push_back(HepGeom::Point3D<double>(-xHalfSize,yHalfSize,-zHalfSize));
      vVertices.push_back(HepGeom::Point3D<double>(-xHalfSize,-yHalfSize,-zHalfSize));
      vVertices.push_back(HepGeom::Point3D<double>(xHalfSize,-yHalfSize,-zHalfSize));
      vVertices.push_back(HepGeom::Point3D<double>(xHalfSize,yHalfSize,zHalfSize));
      vVertices.push_back(HepGeom::Point3D<double>(-xHalfSize,yHalfSize,zHalfSize));
      vVertices.push_back(HepGeom::Point3D<double>(-xHalfSize,-yHalfSize,zHalfSize));
      vVertices.push_back(HepGeom::Point3D<double>(xHalfSize,-yHalfSize,zHalfSize));
      return vVertices;

    }
  else if(shape->type()=="SimplePolygonBrep")
    {
      //      if(bVerbose)std::cout<<"component is a SimplePolygonBrep "<<std::endl;
      const GeoSimplePolygonBrep* bmBox = dynamic_cast< const GeoSimplePolygonBrep*>( shape);
      double zHalfSize=bmBox->getDZ();

      unsigned int nVertices=bmBox->getNVertices();
      for(unsigned int i=0; i<nVertices; i++)
	{
	  HepGeom::Point3D<double> p=HepGeom::Point3D<double>(bmBox->getXVertex(i),bmBox->getYVertex(i),zHalfSize);
	  vVertices.push_back(p);
	}
      for(unsigned int i=0; i<nVertices; i++)
	{
	  HepGeom::Point3D<double> p=HepGeom::Point3D<double>(bmBox->getXVertex(i),bmBox->getYVertex(i),-zHalfSize);
	  vVertices.push_back(p);
	}
      return vVertices;
    }
  else if(shape->type()=="Shift")
    {
      const GeoShapeShift* bmBox = dynamic_cast< const GeoShapeShift*>(shape);
      const GeoShape* shapeA = bmBox->getOp();
      const HepGeom::Transform3D trfShift = bmBox->getX();
      return getVertexShape(shapeA, trfShift);
    }
  else
    {
      std::cout<<"ERROR : UNDEFINED ladder shape "<<shape->type()<<std::endl;

    }

  return vVertices;

 }


std::vector<HepGeom::Point3D<double> > GeoPixelLadderAlpineBuilder::DecodeEndCapModulePositions(double xPos, double yPos, 
												std::vector<double> sValues0, std::vector<double> sValues1)
{
  std::vector<HepGeom::Point3D<double> > vecPos;

  for(int i=0; i<(int)sValues0.size(); i++)
      vecPos.push_back(HepGeom::Point3D<double> (xPos,yPos,sValues0[i]));
  for(int i=0; i<(int)sValues1.size(); i++)
      vecPos.push_back(HepGeom::Point3D<double> (xPos,yPos,sValues1[i]));

  return vecPos;
}



void GeoPixelLadderAlpineBuilder::computeRadiusMinMax(HepGeom::Transform3D trf, double length, double width, double thick, double &rMin, double &rMax)
{
  
  std::vector<HepGeom::Point3D<double> >corners;
  std::vector<HepGeom::Point3D<double> >corners_trf;
  
  double halfLength=length*.5;
  double halfWidth=width*.5;
  double halfThick=thick*.5;
       
  corners.push_back( HepGeom::Point3D<double> (-halfThick, -halfWidth, -halfLength));
  corners.push_back( HepGeom::Point3D<double> (-halfThick, halfWidth, -halfLength));
  corners.push_back( HepGeom::Point3D<double> (halfThick, halfWidth, -halfLength));
  corners.push_back( HepGeom::Point3D<double> (halfThick, -halfWidth, -halfLength));

  corners.push_back( HepGeom::Point3D<double> (-halfThick, -halfWidth, halfLength));
  corners.push_back( HepGeom::Point3D<double> (-halfThick, halfWidth, halfLength));
  corners.push_back( HepGeom::Point3D<double> (halfThick, halfWidth, halfLength));
  corners.push_back( HepGeom::Point3D<double> (halfThick, -halfWidth, halfLength));

  double vMin=999999.;
  double vMax=-999999.;
  
  for(unsigned int i=0; i<corners.size(); i++)
    {
      HepGeom::Point3D<double> p = trf*corners[i];
      corners_trf.push_back(p);
    }

  std::default_random_engine generator;
  std::uniform_int_distribution<int> distrib_int(0,7);
  std::uniform_real_distribution<double> distrib_float(0.0,1.0);
  for(int i=0; i<(int)corners.size(); i++)
    {
      HepGeom::Point3D<double> p = corners_trf[i];
      double r = (p.x()*p.x()+p.y()*p.y());
      vMin = std::min(vMin,r);
      vMax = std::max(vMax,r);

    }


//   for(int i=0; i<100; i++)
//     {
//       /* generate secret number between 1 and 10: */
//       int iPoint = distrib_int(generator);

//       int iPrevious = (iPoint>0) ? (iPoint-1) : 3;
//       int iNext = (iPoint+1)%8;
//       int iCorner = (iPoint+4)%8;
//       int iOpposite = (iPoint<4)?( iPoint+2)%4 : 4+(iPoint+2)%4;
//       double alpha = distrib_float(generator);

//       //      std::cout<<"POINT "<<iPoint<<" "<<iPrevious<<" "<<iNext<<" "<<iCorner<<" "<<iOpposite<<"   "<<alpha<<std::endl;
      
//       HepGeom::Point3D<double> p = computePoint( corners_trf[iPoint], corners_trf[iNext], alpha);
//       double r = (p.x()*p.x()+p.y()*p.y());
//       vMin = std::min(vMin,r);
//       vMax = std::max(vMax,r);

//       p = computePoint( corners_trf[iPoint], corners_trf[iPrevious], alpha);
//       r = (p.x()*p.x()+p.y()*p.y());
//       vMin = std::min(vMin,r);
//       vMax = std::max(vMax,r);

//       p = computePoint( corners_trf[iPoint], corners_trf[iCorner], alpha);
//       r = (p.x()*p.x()+p.y()*p.y());
//       vMin = std::min(vMin,r);
//       vMax = std::max(vMax,r);

//       p = computePoint( corners_trf[iPoint], corners_trf[iOpposite], alpha);
//       r = (p.x()*p.x()+p.y()*p.y());
//       vMin = std::min(vMin,r);
//       vMax = std::max(vMax,r);

//     }

  vMin=sqrt(vMin); vMax=sqrt(vMax);
  if(vMin<rMin) rMin=vMin;
  if(vMax>rMax) rMax=vMax;

}

HepGeom::Point3D<double> GeoPixelLadderAlpineBuilder::computePoint( HepGeom::Point3D<double>p1, HepGeom::Point3D<double>p2, double alpha)
{
  return HepGeom::Point3D<double> ( p1.x()*alpha + p2.x()*(1.-alpha),
				    p1.y()*alpha + p2.y()*(1.-alpha),
				    p1.z()*alpha + p2.z()*(1.-alpha));
}


void GeoPixelLadderAlpineBuilder::showTransform(HepGeom::Transform3D trf ) const
{

  HepGeom::Point3D<double> p1=HepGeom::Point3D<double>(1.00,0.0,0.0);
  HepGeom::Point3D<double> p2=HepGeom::Point3D<double>(0.0,1.00,0.0);
  HepGeom::Point3D<double> p3=HepGeom::Point3D<double>(0.0,0.0,1.00);

  HepGeom::Rotate3D trfRot = trf.getRotation();
  HepGeom::Vector3D<double> trfTrans = trf.getTranslation();

  HepGeom::Point3D<double> p1_trf=trfRot*p1;
  HepGeom::Point3D<double> p2_trf=trfRot*p2;
  HepGeom::Point3D<double> p3_trf=trfRot*p3;

  if(fabs(p1_trf.x())<0.001)p1_trf.setX(0.0); if(fabs(p1_trf.y())<0.001)p1_trf.setY(0.0); if(fabs(p1_trf.z())<0.001)p1_trf.setZ(0.0);
  if(fabs(p2_trf.x())<0.001)p2_trf.setX(0.0); if(fabs(p2_trf.y())<0.001)p2_trf.setY(0.0); if(fabs(p2_trf.z())<0.001)p2_trf.setZ(0.0);
  if(fabs(p3_trf.x())<0.001)p3_trf.setX(0.0); if(fabs(p3_trf.y())<0.001)p3_trf.setY(0.0); if(fabs(p3_trf.z())<0.001)p3_trf.setZ(0.0);

  std::cout<<std::setprecision(3)<<p1_trf.x()<<"  "<<p2_trf.x()<<"  "<<p3_trf.x()<<"  -  "<<trfTrans.x()<<std::endl;
  std::cout<<std::setprecision(3)<<p1_trf.y()<<"  "<<p2_trf.y()<<"  "<<p3_trf.y()<<"  -  "<<trfTrans.y()<<std::endl;
  std::cout<<std::setprecision(3)<<p1_trf.z()<<"  "<<p2_trf.z()<<"  "<<p3_trf.z()<<"  -  "<<trfTrans.z()<<std::endl;

}
