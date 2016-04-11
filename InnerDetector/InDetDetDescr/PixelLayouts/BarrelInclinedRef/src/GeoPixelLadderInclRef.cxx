/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "BarrelInclinedRef/GeoPixelLadderInclRef.h"
#include "BarrelInclinedRef/GeoPixelStaveSupportInclRef.h"
#include "BarrelInclinedRef/PixelInclRefStaveXMLHelper.h"
#include "BarrelInclinedRef/GeoPixelEndcapModuleSvcRef.h"

#include "PixelGeoModelModule/GeoDetModulePixel.h"
#include "InDetReadoutGeometry/PixelModuleDesign.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"

#include "InDetGeoModelUtils/InDetMaterialManager.h" 
#include "PixelGeoModel/PixelGeoBuilder.h"
#include "InDetTrackingGeometryXML/XMLGeoTemplates.h"

#include "InDetReadoutGeometry/PixelDetectorManager.h"
#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoTubs.h"
#include "GeoModelKernel/GeoTrap.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoNameTag.h"
#include "GeoModelKernel/GeoIdentifierTag.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelKernel/GeoMaterial.h"

#include "GeoModelKernel/GeoTransform.h"
#include "GeoModelKernel/GeoAlignableTransform.h"
#include "GeoModelKernel/GeoShapeShift.h"
#include "GeoModelKernel/GeoShapeUnion.h"
#include "PathResolver/PathResolver.h"

using std::max;

GeoPixelLadderInclRef::GeoPixelLadderInclRef(const PixelGeoBuilderBasics* basics, const InDet::StaveTmp *staveTmp, 
				     int iLayer, HepGeom::Transform3D trf):
  PixelGeoBuilder(basics),							
  m_staveTmp(staveTmp),
  m_layer(iLayer),
  m_localTrf(trf),
  m_sector(0),
  m_barrelModule(0),
  m_staveSupport(0),
  m_ladderShape(0),
  m_minmaxDefined(false),
  m_pixelModuleSvc("PixelModuleSvc","PixelModuleSvc"),
  m_pixelDesignSvc("PixelDesignSvc","PixelDesignSvc"),
  m_IDserviceTool("InDetservicesTool/PixelServicesTool")
{

  StatusCode sc = m_IDserviceTool.retrieve(); 
  if (sc.isFailure()){
    msg(MSG::ERROR) << "Could not retrieve " <<  m_IDserviceTool << ",  some services will not be built." << endreq;
  }
  else{
    msg(MSG::INFO) << "Service builder tool retrieved: " << m_IDserviceTool << endreq;
  }

  // Build stave support and module
  m_svcMaterialCmpt=0;
  preBuild();
}


void GeoPixelLadderInclRef::preBuild( ) {


  // Check if the envelope is defined as a tool and available in ToolSvc
  StatusCode sc = m_pixelModuleSvc.retrieve();
  if(sc.isFailure())
    {
      msg(MSG::INFO) << "Could not retrieve pixel module builder tool " <<  m_pixelModuleSvc << ",  some services will not be built." << endreq;
    } 
  else 
    {
      msg(MSG::INFO) << "Pixel module builder tool retrieved: " << m_pixelModuleSvc << endreq;
    }


  std::string value;
  msg(MSG::INFO)<<endreq;
  msg(MSG::INFO)<<"************** BUILD LADDER for layer "<< m_layer<<"  "<<m_staveTmp->name<<" --"<<endreq;

  std::string staveType = m_staveTmp->type;

  m_barrelModuleNumber = m_staveTmp->b_modn;
  m_barrelModuleType = m_staveTmp->b_type;
  m_barrelModuleGap =  m_staveTmp->b_gap;
  m_barrelModuleTilt =  m_staveTmp->b_tilt;

  m_endcapModuleNumber = 0;
  m_endcapModuleType = "";
  m_endcapModulePos.clear();
  m_endcapModuleGap = 0;
  m_endcapInclAngle = 0;
  m_transitionModuleNumber = 0;
  m_transitionModuleType = "";
  m_transitionTiltAngle = 0;
  m_transModulePos.clear();
  if(staveType.compare("Alpine")==0){
    m_endcapModulePos = m_staveTmp->alp_pos;
    m_endcapModuleType = m_staveTmp->alp_type;
    m_endcapModuleGap = 0.;
    m_endcapInclAngle = m_staveTmp->alp_angle;
    m_endcapModuleRshift = m_staveTmp->alp_rshift;

    m_transModulePos = m_staveTmp->trans_pos;
    if(m_transModulePos.size()>0){
      m_transitionModuleNumber = (int)m_transModulePos.size();
      m_transitionModuleType = m_staveTmp->trans_type;
      m_transitionTiltAngle = m_staveTmp->trans_angle;
    }

    m_endcapModPos = DecodeEndCapModulePositions(0., 0., m_transModulePos,m_endcapModulePos);
    m_endcapModuleNumber = (int)m_endcapModPos.size();
  }

  int layerModuleIndex = 0;
  msg(MSG::INFO)<<"xxxxxxxxxxxxx Get barrel module from svc : "<<m_barrelModuleType<<" / "<<staveType<<endreq;
  m_barrelModule = m_pixelModuleSvc->getModule(getBasics(),0,layerModuleIndex,m_barrelModuleType);
  m_barrelModuleDesign = m_pixelDesignSvc->getDesign(getBasics(),m_barrelModuleType);

  msg(MSG::INFO)<<"-- Barrel modules : "<<m_barrelModuleNumber<<" "<<m_barrelModuleType<<" / "<<staveType<<endreq;

  double maxTiltedModuleThick=0.; 
  // get the transition and endcap modules from moduelSvc
  if(staveType.compare("Alpine")==0){
    msg(MSG::INFO)<<"xxxxxxxxxxxxx Get endcap module from svc : "<<m_endcapModuleType<<" / "<<staveType<<" "<<m_endcapModuleType<<"&"<<m_transitionModuleType<<endreq;
    m_endcapModule =  m_pixelModuleSvc->getModule(getBasics(),0,layerModuleIndex,m_endcapModuleType);
    m_endcapModuleDesign = m_pixelDesignSvc->getDesign(getBasics(),m_endcapModuleType);
    maxTiltedModuleThick=m_endcapModule->Length()*cos(m_endcapInclAngle); 
    if(m_transitionModuleNumber>0){
      m_transitionModule =  m_pixelModuleSvc->getModule(getBasics(),0,layerModuleIndex,m_transitionModuleType);
      m_transitionModuleDesign = m_pixelDesignSvc->getDesign(getBasics(),m_transitionModuleType);
      maxTiltedModuleThick=std::max(maxTiltedModuleThick,
				    m_transitionModule->Length()*cos(m_transitionTiltAngle));
    }

  }

  msg(MSG::INFO)<<"xxxxxxxxxxxxx Ladder modules barrel/endcap/transition "<<m_barrelModuleNumber<<" "<<m_endcapModuleNumber<<" "<<m_transitionModuleNumber<<"   Total "<<m_barrelModuleNumber*.5+m_endcapModuleNumber<<endreq;

  // ----------------------------------------------------------------------------
  // Stave module service thickness
  // ----------------------------------------------------------------------------
  m_moduleSvcThickness = .25 * CLHEP::mm;

  // ----------------------------------------------------------------------------
  // Build stave support
  // ----------------------------------------------------------------------------
  // Gap between last barrel module and zpos that correspond to the radial escape of the stvae & services
  m_gapPlanarStave = 10.;
  if(m_layer>1) m_gapPlanarStave = 4.;
  msg(MSG::INFO)<<"xxxxxxxxxxxxx Build stave support for layer : "<<m_layer<<endreq;
  double zEndOfNBarrelModulePos = (m_barrelModuleNumber*m_barrelModule->Length()+m_barrelModuleGap*(m_barrelModuleNumber-1))*.5;
  m_staveSupport = new GeoPixelStaveSupportInclRef( getBasics(),
						    m_layer, *m_barrelModule,
						    m_barrelModuleTilt, 0.,
						    m_gapPlanarStave,m_endcapModuleRshift-maxTiltedModuleThick,m_endcapModuleRshift,
						    zEndOfNBarrelModulePos);
						    
  m_thicknessN = m_staveSupport->thicknessN();
  m_thicknessP = m_staveSupport->thicknessP();
  m_length = m_staveSupport->length()+0.01;
  m_width = m_staveSupport->width()+.01;

  msg(MSG::INFO)<<"** stave support thicknesses   : "<<m_thicknessN<<"  "<<m_thicknessP<<endreq;
  msg(MSG::INFO)<<"** stave support width/length  : "<<m_width<<"  "<<m_length<<endreq;

//   double halfThickness = 0.5*(m_thicknessP+m_thicknessN);
//   double shift = 0.5*(m_thicknessP-m_thicknessN);
//   GeoBox * box = new GeoBox(halfThickness, m_width/2., m_length/2.);
//   const GeoShape & shiftedBox = (*box) << HepGeom::TranslateX3D(shift);
//   m_ladderShape = &shiftedBox;  

//   const GeoMaterial* air = matMgr()->getMaterial("special::Ether");
//   m_theLadder = new GeoLogVol("Ladder",m_ladderShape,air);

  // ----------------------------------------------------------------------------
  // MinMax parameters
  // ----------------------------------------------------------------------------
  m_envLength = m_length;
  m_rmin = m_thicknessP;
  m_rmax = m_thicknessN;

  // Register the number of mopdules defined for a stave
  int nbTotModule = m_barrelModuleNumber;
  if(m_endcapModuleNumber>0) nbTotModule += 2*(m_endcapModuleNumber);
  getBasics()->getDetectorManager()->numerology().setNumEtaModulesForLayer(m_layer, nbTotModule);

//   msg(MSG::INFO)<<"** compute Rminmax : "<<m_rmin<<"  "<<m_rmax<<endreq;
}



GeoVPhysVol* GeoPixelLadderInclRef::Build( ) {

  GeoPhysVol* ladderPhys = 0;
  //  GeoPhysVol* ladderPhys = new GeoPhysVol(m_theLadder);

  int iModuleCmpt = 0; 
  m_svcMaterialCmpt = 0;

  double radiusMin = 999999.9;
  double radiusMax = -999999.9;
  
  bool bVerbose = false;
  
  if(m_endcapModuleNumber>0)
    iModuleCmpt = -(m_endcapModuleNumber+m_transitionModuleNumber)-m_barrelModuleNumber/2;
  else
    iModuleCmpt = -m_barrelModuleNumber/2 - (m_barrelModuleNumber%2);

  if(bVerbose)std::cout<<"FIRST module number "<<m_barrelModuleNumber<<"  "<<m_endcapModuleNumber<<"  "<<iModuleCmpt<<std::endl;
  
  int inclinedModTag = 500;   // new sensor identifier tag for inclined modules
  double ecSvcRadialPos = (m_staveSupport->getSvcRoutingRadiaPos()=="inner")?-1:1;

  //--------------------------------------------------------
  // Compute endcap module positions
  //--------------------------------------------------------
  std::vector<HepGeom::Point3D<double> > endcapModulePos;
  std::vector<HepGeom::Point3D<double> > endcapModulePosBottom;
  std::vector<HepGeom::Point3D<double> > endcapModulePosTop;

  // Default values correspond to Outer position : top of the module aligned with the barrel modules
  //                                             :  zpos is the top corner of the endcap module
  if(m_endcapModuleNumber>0)
    {
      HepGeom::Point3D<double> vDir_endcap(-cos(m_endcapInclAngle), sin(m_endcapInclAngle), 0.);
      HepGeom::Point3D<double> vDirPerp_endcap(-sin(m_endcapInclAngle), -cos(m_endcapInclAngle), 0.);
      HepGeom::Point3D<double> vDir_transition(-cos(m_transitionTiltAngle), sin(m_transitionTiltAngle), 0.);
      HepGeom::Point3D<double> vDirPerp_transition(sin(m_transitionTiltAngle), cos(m_transitionTiltAngle), 0.);
      
      double EcModThick_chip= m_endcapModule->getModuleSensorThick()*.5;
      double EcModHalfLen = m_endcapModule->getModuleSensorLength()*.5;

      for(int iPos=0; iPos<m_endcapModuleNumber; iPos++)
	{
	  double xPos=m_barrelModule->ThicknessP();
	  double yPos=m_endcapModPos[iPos].y();
	  double zPos=m_endcapModPos[iPos].z();
	  
	  if(iPos==0&&m_transitionModuleNumber>0) {
	    xPos=xPos+EcModHalfLen*vDir_transition.x()+EcModThick_chip*vDirPerp_transition.x();
	    xPos+=m_endcapModuleRshift;
	    zPos=zPos+EcModHalfLen*vDir_transition.y()+EcModThick_chip*vDirPerp_transition.y();
	    yPos-=m_transitionModule->Width()*.5;
	    endcapModulePos.push_back(HepGeom::Point3D<double>(xPos,yPos,zPos));
	    endcapModulePosTop.push_back(HepGeom::Point3D<double>(xPos,yPos,zPos));
	    endcapModulePosBottom.push_back(HepGeom::Point3D<double>(xPos,yPos,zPos));
	  }	      
	  else {
	    xPos=xPos+EcModHalfLen*vDir_endcap.x()+EcModThick_chip*vDirPerp_endcap.x();
	    xPos+=m_endcapModuleRshift;
	    zPos=zPos+EcModHalfLen*vDir_endcap.y()+EcModThick_chip*vDirPerp_endcap.y();
	    yPos=0.;
	    endcapModulePos.push_back(HepGeom::Point3D<double>(xPos,yPos,zPos));
	    endcapModulePosTop.push_back(HepGeom::Point3D<double>(xPos,yPos,zPos));
	    endcapModulePosBottom.push_back(HepGeom::Point3D<double>(xPos,yPos,zPos));
	  }
	}
    }
       
  // Number of module services to take into account for each module
  std::vector<int> nbModuleSvc;
  int vecSize=(m_transitionModuleNumber==0)?2:3;
  nbModuleSvc.resize(vecSize);
  int brlModuleIndex=0;
  int transModuleIndex=(m_transitionModuleNumber>0)?1:-1;
  int endcapModuleIndex=(m_transitionModuleNumber>0)?2:1;

  int brlModuleCmpt=-1;
  nbModuleSvc[brlModuleIndex]=m_barrelModuleNumber/2+m_barrelModuleNumber%2;
  if(transModuleIndex>0)nbModuleSvc[transModuleIndex]=m_transitionModuleNumber/2;
  nbModuleSvc[endcapModuleIndex]=m_endcapModuleNumber;  

  if(bVerbose){
    std::cout<<"MODULE SERVICES init for layer  "<<m_layer <<" : ";  for(int i=0; i<(int)nbModuleSvc.size(); i++) std::cout<<nbModuleSvc[i]<<" "; std::cout<<std::endl;
  }

   //------------------------------------------------------
   // Place the endcap modules (Z<0)
   //------------------------------------------------------
   if(m_endcapModuleNumber>0)
     {
       // Build the service structure of the endcap module
       GeoPixelEndcapModuleSvcRef foamBuilder(getBasics(),m_layer, m_endcapModule, m_transitionModule, m_endcapInclAngle, m_transitionTiltAngle);
       GeoPhysVol* foamEndcap = dynamic_cast<GeoPhysVol*>(foamBuilder.getEndcapFoam());
       
       // Endcap servcie transforms
       double zEndcapFoamShift = foamBuilder.getEndcapZshift()+.25;
       GeoPhysVol* foamTrans = dynamic_cast<GeoPhysVol*>(foamBuilder.getTransFoam());
       double zTransFoamShift = foamBuilder.getTransZshift()+.25;
       HepGeom::Transform3D trfFoam = HepGeom::RotateX3D(180.*CLHEP::deg)*HepGeom::RotateZ3D(270.*CLHEP::deg)*HepGeom::RotateY3D(90.*CLHEP::deg);       
       HepGeom::Transform3D EcRot= HepGeom::RotateY3D(270.*CLHEP::deg-m_endcapInclAngle)*HepGeom::RotateZ3D(180.*CLHEP::deg);

       // Loop over the endcap modules
       int endcapIterator = m_endcapModuleNumber-1;
       for(int iNeg=0; iNeg<m_endcapModuleNumber; iNeg++)
	 {
	   double xPos=0., yPos=0., zPos=0.;
	   
	   GeoFullPhysVol* modulePhys = 0;
	   Identifier idwafer;
	   // Inclined module
	   if(iNeg<m_endcapModuleNumber-m_transitionModuleNumber) {
	     std::ostringstream modName; 
	     modName<<"_"<<m_layer<<"_"<<m_sector<<"_"<<iModuleCmpt;
	     modulePhys = m_endcapModule->Build(0, m_layer, m_sector, iModuleCmpt, inclinedModTag , modName.str());
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
	   // Transition module
	   else if(m_transitionModuleNumber>0){         
	     std::ostringstream modName; 
	     modName<<"_"<<m_layer<<"_"<<m_sector<<"_"<<iModuleCmpt;
	     modulePhys = m_transitionModule->Build(0, m_layer, m_sector, iModuleCmpt, inclinedModTag , modName.str());
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
	   
	   // Compute radial minmax sizes 
	   if(!m_minmaxDefined){
	     if(iNeg==0)
	       computeRadiusMinMax(m_localTrf*moduleTrans, 
				   m_endcapModule->Length(),  m_endcapModule->Width(), m_endcapModule->Thickness(), radiusMin, radiusMax);
	     else if(m_transitionModuleNumber>0&&iNeg==m_endcapModuleNumber-1)
	       computeRadiusMinMax(m_localTrf*moduleTrans, 
				   m_transitionModule->Length(),  m_transitionModule->Width(), m_transitionModule->Thickness(), radiusMin, radiusMax);
	     radiusMin-=.5;
	     radiusMax+=.5;

	   }

	   if(ladderPhys==0){

	     if(m_minmaxDefined){
	       radiusMin = m_rmin; 
	       radiusMax = m_rmax; 
	     }

	     double halfThickness = 0.5*(radiusMax-radiusMin);
	     double shift = 0.0;   //5*(radiusMax-radiusMin)-halfThickness;
	     GeoBox * box = new GeoBox(halfThickness, m_width/2., m_length/2.);
	     const GeoShape & shiftedBox = (*box) << HepGeom::TranslateX3D(shift);
	     m_ladderShape = &shiftedBox;  
	     
	     const GeoMaterial* air = matMgr()->getMaterial("special::Ether");
	     m_theLadder = new GeoLogVol("Ladder",m_ladderShape,air);
	     
	     ladderPhys = new GeoPhysVol(m_theLadder);
	   }

	   ladderPhys->add(tag);
	   ladderPhys->add(new GeoIdentifierTag(iModuleCmpt) );
	   ladderPhys->add(xform);
	   ladderPhys->add(modulePhys);
	   
	   // Service box that containes all the module servcies running on the top of the stave
 	   if(m_IDserviceTool->svcRouteAuto()){
 	     double zInit=(iNeg==0)?-m_length*.5:-endcapModulePosTop[endcapIterator+1].z();
 	     double zFinal=-endcapModulePosTop[endcapIterator].z();
 	     BuildAndPlaceModuleService(nbModuleSvc, zInit, zFinal, 0., ladderPhys, "endcap",ecSvcRadialPos);
 	   }

	   // Now store the xform by identifier:
	   getBasics()->getDetectorManager()->addAlignableTransform(0,idwafer,xform,modulePhys);
	   
	   // Place the endcap services (behind the module)
	   if(foamEndcap&&iNeg<m_endcapModuleNumber-1) {
	     std::ostringstream nameTag; 
	     nameTag << "EndcapFoam_L" <<m_layer<<"M"<<iModuleCmpt;
	     GeoNameTag * tag = new GeoNameTag(nameTag.str());
	     const HepGeom::Transform3D foamTrf = HepGeom::TranslateX3D(m_barrelModule->ThicknessP())*HepGeom::TranslateZ3D(zPos-zEndcapFoamShift)*trfFoam;
	     GeoAlignableTransform* xformFoam = new GeoAlignableTransform(foamTrf);
	     ladderPhys->add(tag);
	     ladderPhys->add(xformFoam);
	     ladderPhys->add(foamEndcap);
	   }
	   else if(foamTrans){
	     std::ostringstream nameTag; 
	     nameTag << "EndcapFoam_L" <<m_layer<<"M"<<iModuleCmpt;
	     GeoNameTag * tag = new GeoNameTag(nameTag.str());
	     const HepGeom::Transform3D foamTrf = HepGeom::TranslateY3D(yPos)*HepGeom::TranslateX3D(m_barrelModule->ThicknessP())*HepGeom::TranslateZ3D(zPos-zTransFoamShift)*trfFoam;
	     GeoAlignableTransform* xformFoam = new GeoAlignableTransform(foamTrf);
	     ladderPhys->add(tag);
	     ladderPhys->add(xformFoam);
	     ladderPhys->add(foamTrans);
	   }
	   
	   if(bVerbose){
	     std::cout<<"MODULE SERVICES ec neg  "<<iNeg <<" : ";  for(int i=0; i<(int)nbModuleSvc.size(); i++) std::cout<<nbModuleSvc[i]<<" "; std::cout<<std::endl;
	   }

	   // Decrement the number of module services 
	   if(iNeg<m_endcapModuleNumber-m_transitionModuleNumber) 
	     nbModuleSvc[endcapModuleIndex]--;
	   else if(m_transitionModuleNumber>0)
	     nbModuleSvc[transModuleIndex]--;
	     
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

       // --------------------------------------------------------------------------
       // BARREL TRANSITION REGION
       // --------------------------------------------------------------------------
       if(iBrl==0&&m_transitionModuleNumber>0){

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
	 GeoFullPhysVol* modulePhys = m_transitionModule->Build(0, m_layer, m_sector, iModuleCmpt, inclinedModTag , modName.str());

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
	 ladderPhys->add(tag);
	 ladderPhys->add(new GeoIdentifierTag(iModuleCmpt));
	 ladderPhys->add(xform);
	 ladderPhys->add(modulePhys);

	 // Now store the xform by identifier:
	 //	 Identifier id = m_barrelModule->getID();
	 //	 DDmgr->addAlignableTransform(0,id,xform,modulePhys);
	 getBasics()->getDetectorManager()->addAlignableTransform(0,idwafer,xform,modulePhys);

	 // Increment module counting parametr
	 iModuleCmpt++;
	 iBarrelModuleCmpt++;
       }

       // --------------------------------------------------------------------------
       // BARREL CENTRAL REGION
       // --------------------------------------------------------------------------

       zpos+=m_barrelModule->Length()*.5;

       if(bVerbose)std::cout<<"BARREL MODULE : "<<iBrl<<"  "<<zpos<<std::endl;
       
       //       CLHEP::Hep3Vector modulepos(xpos+xposShift,yposShift,zpos);
       CLHEP::Hep3Vector modulepos(0.,0.,zpos);
       //
       //  Rotate if module is inclined.
       //
       CLHEP::HepRotation rm;
       //       rm.rotateY(gmt_mgr->PixelModuleAngle()*gmt_mgr->PixelModuleAngleSign(etaModule) );
       rm.rotateZ(m_barrelModuleTilt);

       //
       // Place the Module
       //

       if(bVerbose)std::cout<<"*******************************************************************"<<std::endl;
       if(bVerbose)std::cout<<"xx > module barrel planar "<<iModuleCmpt<<" "<<iBrl<<"/"<<m_barrelModuleNumber<<std::endl;
       //       m_barrelModule->setIdentifierFlags(0, m_layer, m_sector, iModuleCmpt);
       std::ostringstream modName; 
       modName<<"_"<<m_layer<<"_"<<m_sector<<"_"<<iModuleCmpt;
       GeoFullPhysVol* modulePhys = m_barrelModule->Build(0, m_layer, m_sector, iModuleCmpt, inclinedModTag , modName.str());
       
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
       ladderPhys->add(tag);
       ladderPhys->add(new GeoIdentifierTag(iModuleCmpt));
       ladderPhys->add(xform);
       ladderPhys->add(modulePhys);

       if(bVerbose){
	 std::cout<<"MODULE SERVICES brl "<<iBrl <<" : ";  for(int i=0; i<(int)nbModuleSvc.size(); i++) std::cout<<nbModuleSvc[i]<<" "; std::cout<<std::endl;
       }

       // Service box that containes all the module servcies running on the top of the stave
       if(m_IDserviceTool->svcRouteAuto()){
	 // Barrel module services
	 double z0 = zpos-m_barrelModule->Length()*.5;
	 double z1 = zpos+m_barrelModule->Length()*.5;
	 if(iBrl==0) z0 -= m_gapPlanarStave;
	 if(iBrl==m_barrelModuleNumber-1) z1 += m_gapPlanarStave;
	 BuildAndPlaceModuleService(nbModuleSvc, z0, z1 ,m_barrelModuleTilt, ladderPhys, "barrel");

	 // Barrel module to the first endcap module
	 if(m_transitionModuleNumber>0) {
	   if((iBrl==0||iBrl==m_barrelModuleNumber-1)&&m_endcapModuleNumber>0){
	     double z0 = 0., z1=0.;
	     if(iBrl==0) { z0 = -endcapModulePos[0].z(); z1 = zpos - m_barrelModule->Length()*.5- m_gapPlanarStave; }
	     else if(iBrl==m_barrelModuleNumber-1) { z0 = zpos + m_barrelModule->Length()*.5+ m_gapPlanarStave; z1= endcapModulePos[0].z(); }
	     BuildAndPlaceModuleService(nbModuleSvc, z0, z1 , m_barrelModuleTilt, ladderPhys, "endcap",ecSvcRadialPos);
	     
	     // In case endcap modules are shifted in R
	     if(m_endcapModuleRshift){
	       double z0 = 0., z1=0.;
	       if(iBrl==0) { z0 = zpos - m_barrelModule->Length()*.5- m_gapPlanarStave; z1 = z0+m_moduleSvcThickness; }
	       else if(iBrl==m_barrelModuleNumber-1) { z1 = zpos + m_barrelModule->Length()*.5 + m_gapPlanarStave; z0 = z1-m_moduleSvcThickness; }
	       BuildAndPlaceModuleService(nbModuleSvc, z0, z1 , m_barrelModuleTilt, ladderPhys, "radial");
	     }
	   }
	 }
       }
       
       // Increment/decrement the number of module which services run on the top of the stave
       nbModuleSvc[brlModuleIndex]+=brlModuleCmpt;
       if(nbModuleSvc[brlModuleIndex]==0)
	 { nbModuleSvc[brlModuleIndex]=1; brlModuleCmpt=1; }
       
       if(!m_minmaxDefined&&iBrl==0){
	 //	 computeRadiusMinMax(m_localTrf*HepGeom::Transform3D(rm,modulepos), 
	 // 			     m_barrelModule->Length(),  m_staveSupport->width(), 2.*m_staveSupport->thickness(), radiusMin, radiusMax);
	 computeRadiusMinMax(m_localTrf*HepGeom::Transform3D(rm,modulepos), 
			     m_barrelModule->Length(),  m_barrelModule->Width(), m_barrelModule->Thickness(), radiusMin, radiusMax);
       }
       
       if(iBrl==0&&ladderPhys==0){

	 if(m_minmaxDefined>0){
	   radiusMin = m_rmin; 
	   radiusMax = m_rmax; 
	 }
	 
	 double halfThickness = 0.5*(radiusMax-radiusMin);
	 double shift = 0.5*(radiusMax-radiusMin)-halfThickness;
	 GeoBox * box = new GeoBox(halfThickness, m_width/2., m_length/2.);
	 const GeoShape & shiftedBox = (*box) << HepGeom::TranslateX3D(shift);
	 m_ladderShape = &shiftedBox;  
	 
	 const GeoMaterial* air = matMgr()->getMaterial("special::Ether");
	 m_theLadder = new GeoLogVol("Ladder",m_ladderShape,air);
	 
	 ladderPhys = new GeoPhysVol(m_theLadder);
       }

       // Now store the xform by identifier:
       getBasics()->getDetectorManager()->addAlignableTransform(0,idwafer,xform,modulePhys);

       zpos+=(m_barrelModule->Length()*.5+m_barrelModuleGap);
       iModuleCmpt++;
       iBarrelModuleCmpt++;
       if(iModuleCmpt==0&&m_barrelModuleNumber%2==0) iModuleCmpt++;

       // --------------------------------------------------------------------------
       // BARREL TRANSITION REGION
       // --------------------------------------------------------------------------
       if((iBrl==m_barrelModuleNumber-1)&&m_transitionModuleNumber>0){

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
	 GeoFullPhysVol* modulePhys = m_transitionModule->Build(0, m_layer, m_sector, iModuleCmpt, inclinedModTag , modName.str());
	 
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
	 ladderPhys->add(tag);
	 ladderPhys->add(new GeoIdentifierTag(iModuleCmpt));
	 ladderPhys->add(xform);
	 ladderPhys->add(modulePhys);
	 
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
      // Increment/decrement the number of module which services run on the top of the stave
      nbModuleSvc[brlModuleIndex]--;

      //      double xModPos=0.0;
      //      double yModPos=0.0;
      GeoPixelEndcapModuleSvcRef foamBuilder(getBasics(), m_layer, m_endcapModule, m_transitionModule, m_endcapInclAngle, m_transitionTiltAngle);
      GeoPhysVol* foamEndcap = dynamic_cast<GeoPhysVol*>(foamBuilder.getEndcapFoam());

      double zEndcapFoamShift = foamBuilder.getEndcapZshift()+.25; //m_endcapModule->ThicknessN()*sin(m_endcapInclAngle);
      GeoPhysVol* foamTrans = dynamic_cast<GeoPhysVol*>(foamBuilder.getTransFoam());
      double zTransFoamShift = foamBuilder.getTransZshift()+.25;
      HepGeom::Transform3D trfFoam = HepGeom::RotateX3D(0.*CLHEP::deg)*HepGeom::RotateZ3D(270.*CLHEP::deg)*HepGeom::RotateY3D(90.*CLHEP::deg);
      HepGeom::Transform3D EcRot_pos= HepGeom::RotateY3D(90.*CLHEP::deg-m_endcapInclAngle)*HepGeom::RotateZ3D(180.*CLHEP::deg);

      int endcapIterator = 0;
      Identifier idwafer;
      for(int iPos=0; iPos<m_endcapModuleNumber; iPos++)
	{

	  double xPos=0., yPos=0., zPos=0.;
	  GeoFullPhysVol* modulePhys = 0;
	  if(iPos>=m_transitionModuleNumber) {
	    //	    m_endcapModule->setIdentifierFlags(0, m_layer, m_sector, iModuleCmpt);
	    std::ostringstream modName; 
	    modName<<"_"<<m_layer<<"_"<<m_sector<<"_"<<iModuleCmpt;
	    modulePhys = m_endcapModule->Build(0, m_layer, m_sector, iModuleCmpt, inclinedModTag , modName.str());
	    xPos=endcapModulePos[iPos].x();
	    yPos=endcapModulePos[iPos].y();
	    zPos=endcapModulePos[iPos].z();
	    EcRot_pos= HepGeom::RotateY3D(90.*CLHEP::deg+m_endcapInclAngle)*HepGeom::RotateZ3D(180.*CLHEP::deg);

	    if(bVerbose)std::cout<<"ENDCAP MODULE : ec "<<m_layer<<" "<<iPos<<"  "<<zPos<<std::endl;
	    if(bVerbose)std::cout<<"wafer id : "<<0<<" "<< m_layer<<" "<< m_sector<<" "<< iModuleCmpt<<std::endl;

	    idwafer = getBasics()->getIdHelper()->wafer_id(0, m_layer, m_sector, iModuleCmpt);
	    InDetDD::SiDetectorElement* element = new InDetDD::SiDetectorElement( idwafer, m_endcapModuleDesign, 
										  modulePhys, getBasics()->getCommonItems());
	    //	    getBasics()->getDetectorManager()->addDesign(m_barrelModuleDesign);
	    getBasics()->getDetectorManager()->addDetectorElement(element);
	  }
	  else if(m_transitionModuleNumber>0) {
	    //	    m_transitionModule->setIdentifierFlags(0, m_layer, m_sector, iModuleCmpt);
	    std::ostringstream modName; 
	    modName<<"_"<<m_layer<<"_"<<m_sector<<"_"<<iModuleCmpt;
	    modulePhys = m_transitionModule->Build(0, m_layer, m_sector, iModuleCmpt, inclinedModTag , modName.str());
	    EcRot_pos= HepGeom::RotateY3D(90.*CLHEP::deg+m_transitionTiltAngle)*HepGeom::RotateZ3D(180.*CLHEP::deg);
	    xPos=endcapModulePos[iPos].x();
	    yPos=endcapModulePos[iPos].y();
	    zPos=endcapModulePos[iPos].z();
	    if(bVerbose)std::cout<<"ENDCAP MODULE - tr "<<m_layer<<" "<<iPos<<"  "<<zPos<<std::endl;
	    if(bVerbose)std::cout<<"wafer id : "<<0<<" "<< m_layer<<" "<< m_sector<<" "<< iModuleCmpt<<std::endl;

	    idwafer = getBasics()->getIdHelper()->wafer_id(0, m_layer, m_sector, iModuleCmpt);
	    InDetDD::SiDetectorElement* element = new InDetDD::SiDetectorElement( idwafer, m_transitionModuleDesign, 
										  modulePhys, getBasics()->getCommonItems());
	    //	    getBasics()->getDetectorManager()->addDesign(m_barrelModuleDesign);
	    getBasics()->getDetectorManager()->addDetectorElement(element);

	  }

	  // Decrement the number of module services 
	  if(iPos>=m_transitionModuleNumber)
	    nbModuleSvc[endcapModuleIndex]++;
	  else if(m_transitionModuleNumber>0)
	    nbModuleSvc[transModuleIndex]++;

	  const HepGeom::Transform3D moduleTrans = HepGeom::Transform3D(EcRot_pos.getRotation(),HepGeom::Vector3D<double> (xPos,yPos,zPos));
	  if(bVerbose)std::cout<<"*******************************************************************"<<std::endl;
	  if(bVerbose)std::cout<<"-> place endcap module : "<<iPos<<" "<<endcapIterator<<"  "<<zPos<<std::endl;

	  std::ostringstream nameTag; 
	  nameTag << "ModuleBrl" << iModuleCmpt;
	  GeoNameTag * tag = new GeoNameTag(nameTag.str());
	  GeoAlignableTransform* xform = new GeoAlignableTransform(moduleTrans);
	  ladderPhys->add(tag);
	  ladderPhys->add(new GeoIdentifierTag(iModuleCmpt) );
	  ladderPhys->add(xform);
	  ladderPhys->add(modulePhys);

	  if(bVerbose){
	    std::cout<<"MODULE SERVICES ec pos  "<<iPos <<" : ";  for(int i=0; i<(int)nbModuleSvc.size(); i++) std::cout<<nbModuleSvc[i]<<" "; std::cout<<std::endl;
	  }

	  // Now store the xform by identifier:
	  getBasics()->getDetectorManager()->addAlignableTransform(0,idwafer,xform,modulePhys);

	  // Service box that containes all the module servcies running on the top of the stave
 	  if(m_IDserviceTool->svcRouteAuto()){
 	    double zInit=endcapModulePosTop[iPos].z();
 	    double zFinal=(iPos<m_endcapModuleNumber-1)?endcapModulePosTop[iPos+1].z():m_length*.5;
 	    BuildAndPlaceModuleService(nbModuleSvc, zInit, zFinal , 0., ladderPhys, "endcap",ecSvcRadialPos);
 	  }

	  if(foamEndcap&&iPos>0) {
	    std::ostringstream nameTag; 
	    nameTag << "EndcapFoam_L" <<m_layer<<"M"<<iModuleCmpt;
	    GeoNameTag * tag = new GeoNameTag(nameTag.str());
	    const HepGeom::Transform3D foamTrf = HepGeom::TranslateX3D(m_barrelModule->ThicknessP())*HepGeom::TranslateZ3D(zPos+zEndcapFoamShift)*trfFoam;
	    GeoAlignableTransform* xformFoam = new GeoAlignableTransform(foamTrf);
	    ladderPhys->add(tag);
	    ladderPhys->add(xformFoam);
	    ladderPhys->add(foamEndcap);
	  }
	  else if(foamTrans){
	    std::ostringstream nameTag; 
	    nameTag << "EndcapFoam_L" <<m_layer<<"M"<<iModuleCmpt;
	    GeoNameTag * tag = new GeoNameTag(nameTag.str());
	    const HepGeom::Transform3D foamTrf = HepGeom::TranslateY3D(yPos)*HepGeom::TranslateX3D(m_barrelModule->ThicknessP())*HepGeom::TranslateZ3D(zPos+zTransFoamShift)*trfFoam;
	    GeoAlignableTransform* xformFoam = new GeoAlignableTransform(foamTrf);
	    ladderPhys->add(tag);
	    ladderPhys->add(xformFoam);
	    ladderPhys->add(foamTrans);
	  }

	  iModuleCmpt++;
	  endcapIterator++;
	}
    }
  
  std::cout<<"ALPINE ladder module number : "<<iModuleCmpt<<std::endl;
  
  //Add the TMT or other stave support
  if (m_staveSupport) {
    ladderPhys->add(new GeoTransform(m_staveSupport->transform()));
    if(m_staveSupport->getPhysVol())
      ladderPhys->add(m_staveSupport->getPhysVol()); 
  }
  
  if(!m_minmaxDefined){
    m_rmin = radiusMin;
    m_rmax = radiusMax;
    m_minmaxDefined = true;
   }
  
  return ladderPhys;

}

std::vector<HepGeom::Point3D<double> > GeoPixelLadderInclRef::DecodeEndCapModulePositions(double xPos, double yPos, 
											  std::vector<double> sValues0, std::vector<double> sValues1)
{
  std::vector<HepGeom::Point3D<double> > vecPos;

  for(int i=0; i<(int)sValues0.size(); i++)
      vecPos.push_back(HepGeom::Point3D<double> (xPos,yPos,sValues0[i]));
  for(int i=0; i<(int)sValues1.size(); i++)
      vecPos.push_back(HepGeom::Point3D<double> (xPos,yPos,sValues1[i]));

  return vecPos;
}



void GeoPixelLadderInclRef::computeRadiusMinMax(HepGeom::Transform3D trf, double length, double width, double thick, double &rMin, double &rMax)
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

//   std::default_random_engine generator;
//   std::uniform_int_distribution<int> distrib_int(0,7);
//   std::uniform_real_distribution<double> distrib_float(0.0,1.0);
  for(int i=0; i<(int)corners.size(); i++)
    {
      HepGeom::Point3D<double> p = corners_trf[i];
      double r = (p.x()*p.x()+p.y()*p.y());
      vMin = std::min(vMin,r);
      vMax = std::max(vMax,r);

    }

  vMin=sqrt(vMin); vMax=sqrt(vMax);
  if(vMin<rMin) rMin=vMin;
  if(vMax>rMax) rMax=vMax;

  std::cout<<"Compute ladder rminmax : "<<rMin<<" "<<rMax<<std::endl;

}



GeoPhysVol* GeoPixelLadderInclRef::createServiceVolume(double length, double thick, double width, std::vector<int> nModuleSvc)

{
  // Place the stave module service (on the top of the stave support...)
  GeoBox * svcBox = new GeoBox(thick, width, length);

  std::cout<<"Barrel module service material for layer  "<<m_layer <<" :  size WxTxL "<<width<<" "<<thick<<" "<<length<<"     # modules ";  
  for(int i=0; i<(int)nModuleSvc.size(); i++) std::cout<<nModuleSvc[i]<<" "; std::cout<<std::endl;
  std::string matName = m_IDserviceTool->getLayerModuleMaterialName(m_layer ,nModuleSvc);   // material name stored in PixelServicesTool (material are built there)
  std::cout<<"Barrel module service material  : "<<matName<<"  "<<std::endl;
  
  std::ostringstream wg_matName;  
  wg_matName<<matName<<"_L"<<m_layer<<"_"<<m_svcMaterialCmpt;
  
  std::cout<<"Barrel module weighted service material : "<<matName<<"  "<<wg_matName.str()<<"   / sector : "<<m_sector<<std::endl;
  if(matName=="None") return 0;

  GeoMaterial* svcMat = 0;  // do not redefine material if already done for sector 0
  //  if(m_sector==0)    //!m_minmaxDefined
  //    svcMat = const_cast<GeoMaterial*>(matMgr()->getMaterialForVolumeLength(matName, svcBox->volume(), m_barrelModule->Length(),wg_matName.str()));  // define material
  //  else
  //    svcMat = const_cast<GeoMaterial*>(matMgr()->getMaterial(wg_matName.str()));   // material already defined

  svcMat = const_cast<GeoMaterial*>(matMgr()->getMaterial(wg_matName.str()));   // material already defined
  if(svcMat==0)
    svcMat = const_cast<GeoMaterial*>(matMgr()->getMaterialForVolumeLength(matName, svcBox->volume(), m_barrelModule->Length(),wg_matName.str()));  // define material

//  svcMat = const_cast<GeoMaterial*>(matMgr()->getMaterial("std::Copper"));
  
  std::cout<<"Material : "<<svcMat->getDensity()<<std::endl;

  // Material not defined - FIXME SES
  //  if(svcMat==0)return 0;

  GeoLogVol* svcLog = new GeoLogVol("ModuleSvc",svcBox,svcMat);
  GeoPhysVol* svcPhys = new GeoPhysVol(svcLog);

  m_svcMaterialCmpt++;  
  return svcPhys;
}
 

void GeoPixelLadderInclRef:: BuildAndPlaceModuleService(std::vector<int> moduleNumber, double zInit, double zFinal, double locTilt, GeoPhysVol* volPhys, std::string type, double ecSide)
{
  
  double delta = .75;
  double xshift = .5;
  double ec_xshift = 0.;
  if(m_layer==1) 
    { delta=.5; xshift = 2.; ec_xshift = 2.25; }
  else if(m_layer>1)
    { xshift = 1.25; }

  double svcLength=zFinal-zInit;
  if(svcLength>0){
    double svcHalfThick = m_moduleSvcThickness*.5-0.001;
    double svcHalfWidth = m_barrelModule->Width()*delta*.5;
    double svcOffset = m_barrelModule->Width()*.5-svcHalfWidth;
    if(locTilt<0) svcOffset = -svcOffset;
    double xPos_svc = 0.;
    if(type=="radial"){
      xPos_svc = (m_staveSupport->thicknessP_barrel()+m_staveSupport->thicknessP_endcap())*.5;
      svcHalfThick = fabs(m_staveSupport->thicknessP_barrel()-m_staveSupport->thicknessP_endcap())*.5;
    }
    else {
      xPos_svc = (type=="barrel")?m_staveSupport->thicknessP_barrel():m_staveSupport->thicknessP_endcap(); 
      xPos_svc+=m_moduleSvcThickness*.5+xshift;
      if(type=="endcap"&&ecSide<0) xPos_svc = m_staveSupport->thicknessN_endcap()-m_moduleSvcThickness*.5-ec_xshift; 
    }
    GeoPhysVol* svcPhys = createServiceVolume(svcLength*.5, svcHalfThick, svcHalfWidth, moduleNumber);

    if(svcPhys){
      GeoTransform* xform_svc = new GeoTransform(HepGeom::TranslateY3D(-svcOffset)*HepGeom::TranslateX3D(xPos_svc)*HepGeom::TranslateZ3D((zInit+zFinal)*.5)*HepGeom::RotateZ3D(locTilt));
      volPhys->add(xform_svc);
      volPhys->add(svcPhys);	   
    }
  }
}
