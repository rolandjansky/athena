/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/


// Build detailed stave support : face plate + carbon foam + cable flex + cooling pipe + end blocks
// This is built one time per layer. 

#include "GeoPixelStaveRingServices.h"

#include "GeoPixelStaveRing.h"

#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoTube.h"
#include "GeoModelKernel/GeoSimplePolygonBrep.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "GeoModelKernel/GeoNameTag.h"

#include "GeoModelKernel/GeoTransform.h"
#include "GaudiKernel/SystemOfUnits.h"

#include <algorithm>
using std::max;

GeoPixelStaveRingServices::GeoPixelStaveRingServices(GeoPixelLadder& ladder, GeoPixelStaveSupport& staveSupport):
  m_ladder(ladder),m_staveSupport(staveSupport),
  m_supportPhysA(0),m_supportPhysC(0),m_supportMidRing(0),
  m_xformSupportA(0), m_xformSupportC(0), m_xformSupportMidRing(0)
{

}

GeoVPhysVol* GeoPixelStaveRingServices::Build()
{

  m_gmt_mgr->msg(MSG::INFO) <<"Build IBL stave ring services"<<endmsg;


  double layerRadius = m_gmt_mgr->PixelLayerRadius();
  double ladderTilt   = m_gmt_mgr->PixelLadderTilt();
  int nSectors = m_gmt_mgr->NPixelSectors();

  double ladderHalfThickN = m_ladder.thicknessN();
  double ladderHalfThickP = m_ladder.thicknessP();
  double safetyMargin = 0.001*Gaudi::Units::mm;

  // Get endblock from m_staveSupport
  GeoPhysVol* endblockA=dynamic_cast<GeoPhysVol*>(m_staveSupport.getEndblockEnvelopShape(2));
  GeoPhysVol* endblockC=dynamic_cast<GeoPhysVol*>(m_staveSupport.getEndblockEnvelopShape(-2));
  double endblockZpos=m_staveSupport.getEndblockZpos();
  double serviceZpos=m_staveSupport.getServiceZpos();
  double endblockLength=m_staveSupport.getEndblockLength();
  GeoPhysVol* endblockFlex=dynamic_cast<GeoPhysVol*>(m_staveSupport.getEndblockEnvelopShape(1));
  GeoTransform* endblockFlexTrf=m_staveSupport.getEndblockEnvelopShapeTrf(1);
  GeoPhysVol* serviceCoolPipe=dynamic_cast<GeoPhysVol*>(m_staveSupport.getEndblockEnvelopShape(3));
  GeoTransform* serviceCoolPipeTrfA=m_staveSupport.getEndblockEnvelopShapeTrf(2);
  GeoTransform* serviceCoolPipeTrfC=m_staveSupport.getEndblockEnvelopShapeTrf(-2);
  const GeoTrf::Transform3D ladderLocalTrf=m_staveSupport.transform();
  double dogLegStaveLength=17.87;
  double halfSupportLength=endblockLength*0.5+dogLegStaveLength*0.5;
  
  m_gmt_mgr->msg(MSG::DEBUG)<<"IBL EOS : "<<endblockZpos<<"  "<<serviceZpos<<"   "<<endblockLength<<endmsg;
  if((!endblockA) || (!endblockC) || (!endblockFlex) || (!serviceCoolPipe))
  {
          m_gmt_mgr->msg(MSG::ERROR) <<"dynamic_cast failure in "<<__FILE__<< ":"<< __LINE__<<endmsg;
          exit(EXIT_FAILURE);
  } 
  
  // Define staveRing for side A
  GeoPixelStaveRing staveRing;
  std::ostringstream lnameA;
  lnameA << "Brl0A_StaveRing";
  GeoVPhysVol* ringphysA = staveRing.SetParametersAndBuild(lnameA.str(),"AC");
  GeoNameTag *tagA = new GeoNameTag(lnameA.str());         
  
  // Define staveRing for side C
  std::ostringstream lnameC;
  lnameC << "Brl0C_StaveRing";
  GeoVPhysVol* ringphysC = staveRing.SetParametersAndBuild(lnameC.str(),"AC");
  GeoNameTag *tagC = new GeoNameTag(lnameC.str());         
  
  // Get assembly min and max radius
  double innerRadius = staveRing.GetInnerRadius()-safetyMargin;

  double zmin = serviceZpos-(endblockLength*0.5+dogLegStaveLength*0.5);
  double zmax = serviceZpos+(endblockLength*0.5+dogLegStaveLength*0.5);
  double outerRadius = m_gmt_mgr->IBLServiceGetMinRadialPosition("IST","simple",zmin, zmax);

    //layerRadius+ladderHalfThickP+m_staveSupport.thicknessP();
  
  m_gmt_mgr->msg(MSG::DEBUG)<<"Support assembly : "<<innerRadius<<" "<<outerRadius<<" ladderThickP  "<<ladderHalfThickP<<" "<<ladderHalfThickN<<
    " supportThickP "<<m_staveSupport.thicknessP()<<" "<<m_staveSupport.thicknessN()<<endmsg;
  
  
  const GeoTube* supportShape = new GeoTube(innerRadius,outerRadius,endblockLength*0.5+dogLegStaveLength*0.5);
  const GeoMaterial* ether = m_mat_mgr->getMaterial("special::Ether");
  GeoLogVol* supportLogVol_A = new GeoLogVol("StaveRingAndEndblocks_A",supportShape,ether);
  GeoLogVol* supportLogVol_C = new GeoLogVol("StaveRingAndEndblocks_C",supportShape,ether);
  
  //	  GeoPhysVol *supportPhysA = new GeoPhysVol(supportLogVol_A);
  //	  GeoPhysVol *supportPhysC = new GeoPhysVol(supportLogVol_C);
  
  m_supportPhysA = new GeoPhysVol(supportLogVol_A);
  m_supportPhysC = new GeoPhysVol(supportLogVol_C);
  
  if(nSectors==0){
    m_gmt_mgr->msg(MSG::ERROR)<<"No phi sectors defined"<<std::endl;
  }
  else {

    double angle=360./nSectors*Gaudi::Units::deg;
    GeoTrf::Transform3D transRadiusAndTilt = GeoTrf::TranslateX3D(layerRadius)*GeoTrf::RotateZ3D(ladderTilt);
    double phiOfModuleZero =  m_gmt_mgr->PhiOfModuleZero();

    if(m_gmt_mgr->PixelStaveAxe()==1)   
      {
	//  Point that defines the center of the cooling pipe
	GeoTrf::Vector3D centerCoolingPipe = m_gmt_mgr->IBLStaveRotationAxis() ;
	
	// Transforms
	GeoTrf::Transform3D staveTrf = GeoTrf::RotateZ3D(ladderTilt)*GeoTrf::Translate3D(-centerCoolingPipe.x(),-centerCoolingPipe.y(),-centerCoolingPipe.z());
	double staveRadius = m_gmt_mgr->IBLStaveRadius() ;
	
	transRadiusAndTilt = GeoTrf::TranslateX3D(staveRadius)*staveTrf;
      }
    
    for(int ii = 0; ii < nSectors; ii++) {
      m_gmt_mgr->SetPhi(ii);
      
      double phiOfSector = phiOfModuleZero + ii*angle;
      
      // transform equiv  RotZ(phiOfSector)*TransZ(layerRadius)*RotZ(tilt)
      GeoTrf::Transform3D ladderTransform = GeoTrf::RotateZ3D(phiOfSector) * transRadiusAndTilt ;
      
      //
      // Place endblock and endblock flex
      //
      std::ostringstream nameTag; 
      nameTag << "Endblock" << ii;
      GeoNameTag * tag = new GeoNameTag(nameTag.str());
      GeoTransform* xformA = new GeoTransform(ladderTransform*ladderLocalTrf*GeoTrf::TranslateZ3D(endblockZpos-serviceZpos-dogLegStaveLength*0.5));
      m_supportPhysA->add(tag);
      m_supportPhysA->add(xformA);
      m_supportPhysA->add(endblockA);

      GeoTransform* xformC = new GeoTransform(ladderTransform*ladderLocalTrf*GeoTrf::TranslateZ3D(-endblockZpos+serviceZpos+dogLegStaveLength*0.5));
      m_supportPhysC->add(tag);
      m_supportPhysC->add(xformC);
      m_supportPhysC->add(endblockC);

      //
      // Add endblock flex section
      //
      std::ostringstream nameTagFlex; 
      nameTagFlex << "EndblockFlex" << ii;
      GeoNameTag * tagFlex = new GeoNameTag(nameTagFlex.str());
      GeoTransform* xformFlexA = new GeoTransform(ladderTransform*ladderLocalTrf*(endblockFlexTrf->getTransform())*GeoTrf::TranslateZ3D(-dogLegStaveLength*0.5));
      GeoTransform* xformFlexB = new GeoTransform(ladderTransform*ladderLocalTrf*(endblockFlexTrf->getTransform())*GeoTrf::TranslateZ3D(dogLegStaveLength*0.5));
      
      m_supportPhysA->add(tagFlex);
      m_supportPhysA->add(xformFlexA);
      m_supportPhysA->add(endblockFlex);
   
      m_supportPhysC->add(tagFlex);
      m_supportPhysC->add(xformFlexB);
      m_supportPhysC->add(endblockFlex);
      
      //
      // Add cooling pipe service
      //
      if(serviceCoolPipe){
	std::ostringstream nameTagPipe; 
	nameTagPipe << "ServicePipe" << ii;
	GeoNameTag * tagPipe = new GeoNameTag(nameTagPipe.str());
	GeoTransform* xformPipeA = new GeoTransform(ladderTransform*ladderLocalTrf*(serviceCoolPipeTrfA->getTransform())) ;
	m_supportPhysA->add(tagPipe);
	m_supportPhysA->add(xformPipeA);
	m_supportPhysA->add(serviceCoolPipe);
	
	GeoTransform* xformPipeC = new GeoTransform(ladderTransform*ladderLocalTrf*(serviceCoolPipeTrfC->getTransform()));
	m_supportPhysC->add(tagPipe);
	m_supportPhysC->add(xformPipeC);
	m_supportPhysC->add(serviceCoolPipe);
	
//	GeoTrf::Vector3D p=ladderTransform*ladderLocalTrf*(serviceCoolPipeTrfA->getTransform())*GeoTrf::Vector3D(0.,0.,0.);
	//       std::cout<<"Cooling pipe srv : "<<p.x()<<" "<<p.y()<<" "<<p.z();
	//       std::cout<<"   r : "<<sqrt(p.x()*p.x()+p.y()*p.y())<<" ";
	//       std::cout<<"  ang : "<<acos(p.x()/sqrt(p.x()*p.x()+p.y()*p.y()))*180./3.1415<<" "; 
	//       std::cout<<std::endl;
	
	
	//
	//  flex dogleg
	//
	
	double length=m_gmt_mgr->IBLFlexDoglegLength();
	double eoStave=halfSupportLength-dogLegStaveLength;
	
	double height1=m_gmt_mgr->IBLFlexDoglegHeight(1);
	double height2=m_gmt_mgr->IBLFlexDoglegHeight(2);
	double part1=m_gmt_mgr->IBLFlexDoglegRatio();
	
	double alpha1=atan(height1/(length*part1));
	double alpha2=atan(height2/(length*(1.-part1)));
	
	double dimX_lin=length*part1;
	double dimX=dimX_lin/cos(alpha1)-.15;
	double dimY=m_gmt_mgr->IBLStaveFlexWidth();
	double dimZ=m_gmt_mgr->IBLStaveFlexBaseThickness();
	double angle=90.*Gaudi::Units::deg-alpha1;   //90.-27.99;
	double delta=m_gmt_mgr->IBLFlexDoglegDY();
	double trX=-dimX_lin*tan(alpha1)*0.5;   //-3.28;
	double trZ=eoStave+dimX_lin*0.5;
	
	std::vector<double> xShape, yShape;
	xShape.push_back(dimX*0.5);  yShape.push_back(dimY*0.5);
	xShape.push_back(-dimX*0.5);  yShape.push_back(dimY*0.5+delta);
	xShape.push_back(-dimX*0.5); yShape.push_back(-dimY*0.5+delta);
	xShape.push_back(dimX*0.5);  yShape.push_back(-dimY*0.5);
	GeoSimplePolygonBrep* tmp_shape = new GeoSimplePolygonBrep(dimZ*0.5);
	for(unsigned int iPt=0; iPt<xShape.size(); iPt++) tmp_shape->addVertex(xShape[iPt],yShape[iPt]);
	
	
	//	GeoPara * tmp_shape = new GeoPara(0.47,5.5,9.,0.*Gaudi::Units::deg,55.*Gaudi::Units::deg,0.);
	std::string flexMatName = m_gmt_mgr->IBLFlexMaterial(1,"doglegA");
	const GeoMaterial* tmp_material = m_mat_mgr->getMaterial(flexMatName);
	GeoLogVol* tmp_logVol = new GeoLogVol("FlexDogLeg1",tmp_shape,tmp_material);
	GeoPhysVol * tmp_logVolPV = new GeoPhysVol(tmp_logVol);
	GeoNameTag* tmp_tag = new GeoNameTag("FlexDogLeg1");
	GeoTrf::Transform3D trfGblA=ladderTransform*ladderLocalTrf*GeoTrf::TranslateX3D(trX)*GeoTrf::TranslateZ3D(trZ)*(endblockFlexTrf->getTransform())*GeoTrf::RotateY3D(angle);
	GeoTransform* tmp_xformA = new GeoTransform(trfGblA);
	
	m_supportPhysA->add(tmp_tag);
	m_supportPhysA->add(tmp_xformA);
	m_supportPhysA->add(tmp_logVolPV);
	
	trX=-dimX_lin*tan(alpha1)*0.5;   //-3.28;
	trZ=-(eoStave+dimX_lin*0.5);
	GeoTrf::Transform3D trfGblC=ladderTransform*ladderLocalTrf*GeoTrf::TranslateX3D(trX)*GeoTrf::TranslateZ3D(trZ)*(endblockFlexTrf->getTransform())*GeoTrf::RotateY3D(-angle);
	GeoTransform* tmp_xformC = new GeoTransform(trfGblC);
	
	m_supportPhysC->add(tmp_tag);
	m_supportPhysC->add(tmp_xformC);
	m_supportPhysC->add(tmp_logVolPV);
	
	//
	//  flex : from dogleg to longitudinal 
	//
	double dimX2_lin=length*(1.-part1);
	double dimX2=dimX2_lin/cos(alpha2)-.15;
	double dimY2=m_gmt_mgr->IBLStaveFlexWidth();
	double trX2=trX*2.-dimX2_lin*tan(alpha2)*0.5;   //-3.28;
	double trZ2=eoStave+dimX_lin+dimX2_lin*0.5;
	xShape.clear(); yShape.clear();
	angle=90.*Gaudi::Units::deg-alpha2;
	xShape.push_back(dimX2*0.5);  yShape.push_back(dimY2*0.5);
	xShape.push_back(-dimX2*0.5);  yShape.push_back(dimY2*0.5);
	xShape.push_back(-dimX2*0.5); yShape.push_back(-dimY2*0.5);
	xShape.push_back(dimX2*0.5);  yShape.push_back(-dimY2*0.5);
	GeoSimplePolygonBrep* tmp2_shape = new GeoSimplePolygonBrep(dimZ*0.5);
	for(unsigned int iPt=0; iPt<xShape.size(); iPt++) tmp2_shape->addVertex(xShape[iPt],yShape[iPt]);
	
	const GeoMaterial* tmp2_material = m_mat_mgr->getMaterial(flexMatName);
	GeoLogVol* tmp2_logVol = new GeoLogVol("FlexDogLeg2",tmp2_shape,tmp2_material);
	GeoPhysVol * tmp2_logVolPV = new GeoPhysVol(tmp2_logVol);
	GeoNameTag* tmp2_tag = new GeoNameTag("FlexDogLeg2");
	
	GeoTransform* tmp2_xformA = new GeoTransform(ladderTransform*ladderLocalTrf*GeoTrf::TranslateX3D(trX2)*GeoTrf::TranslateZ3D(trZ2)*(endblockFlexTrf->getTransform())*GeoTrf::RotateY3D(angle)*GeoTrf::TranslateY3D(delta*.75));
	m_supportPhysA->add(tmp2_tag);
	m_supportPhysA->add(tmp2_xformA);
	m_supportPhysA->add(tmp2_logVolPV);
	
	trZ2=-(eoStave+dimX_lin+dimX2_lin*0.5);
	GeoTransform* tmp2_xformC = new GeoTransform(ladderTransform*ladderLocalTrf*GeoTrf::TranslateX3D(trX2)*GeoTrf::TranslateZ3D(trZ2)*(endblockFlexTrf->getTransform())*GeoTrf::RotateY3D(-angle)*GeoTrf::TranslateY3D(delta*.75));
	m_supportPhysC->add(tmp2_tag);
	m_supportPhysC->add(tmp2_xformC);
	m_supportPhysC->add(tmp2_logVolPV);
      }
    }
  }
  
  // IBL layer shift ( 2mm shift issue )
  double layerZshift = m_gmt_mgr->PixelLayerGlobalShift();

  GeoTrf::Translate3D ring_posA(0.0,0.0,staveRing.GetPositionAlongZAxis()-serviceZpos-dogLegStaveLength*0.5+layerZshift);
  GeoTransform* xformA  = new GeoTransform(ring_posA);
  m_supportPhysA->add(tagA);
  m_supportPhysA->add(xformA);
  m_supportPhysA->add(ringphysA);
  
  GeoTrf::Translate3D ring_posC(0.0,0.0,-staveRing.GetPositionAlongZAxis()+serviceZpos+dogLegStaveLength*0.5-layerZshift);
  GeoTransform* xformC  = new GeoTransform(ring_posC);
  m_supportPhysC->add(tagC);
  m_supportPhysC->add(xformC);
  m_supportPhysC->add(ringphysC);
  
  m_gmt_mgr->msg(MSG::DEBUG)<<"IBL EOS : zpos "<<serviceZpos<<endmsg;
  GeoTrf::Transform3D supportTrfA = GeoTrf::TranslateZ3D(serviceZpos+dogLegStaveLength*0.5+layerZshift);
  m_xformSupportA = new GeoTransform(supportTrfA);
  
  GeoTrf::Transform3D supportTrfC = GeoTrf::TranslateZ3D(-serviceZpos-dogLegStaveLength*0.5+layerZshift);
  m_xformSupportC = new GeoTransform(supportTrfC);
  
  std::ostringstream lnameM;
  lnameM << "Brl0M_StaveRing";
  m_supportMidRing = staveRing.SetParametersAndBuild(lnameM.str(),"Mid");
  GeoTrf::Translate3D ring_posM(0.0,0.0,0.0+layerZshift);
  m_xformSupportMidRing = new GeoTransform(ring_posM);


  return 0;

}
