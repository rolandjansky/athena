/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


// Build detailed stave support : face plate + carbon foam + cable flex + cooling pipe + end blocks
// This is built one time per layer. 

#include "PixelGeoModel/GeoPixelIFlexServices.h"

#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoTube.h"
#include "GeoModelKernel/GeoSimplePolygonBrep.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "GeoModelKernel/GeoNameTag.h"

#include "GeoModelKernel/GeoTransform.h"

#include <algorithm>
using std::max;

GeoPixelIFlexServices::GeoPixelIFlexServices(int iSection): 
  m_section(iSection),
  m_supportPhysA(0),
  m_supportPhysC(0),
  m_xformSupportA(0),
  m_xformSupportC(0)
{
}

GeoVPhysVol* GeoPixelIFlexServices::Build()
{

  m_gmt_mgr->msg(MSG::INFO) <<"Build IBL I-Flex services"<<endmsg;

  double safety = 0.01*CLHEP::mm;

  // IBL layer shift ( 2mm shift issue )
  double layerZshift = m_gmt_mgr->PixelLayerGlobalShift();

  double barrelZmax = m_gmt_mgr->PixelBarrelHalfLength();
  int nSectors = m_gmt_mgr->NPixelSectors();

  // check if sectors are properly defined
  if(nSectors==0) return 0;
  double angle=360./(double)nSectors*CLHEP::deg;

  double zmin=0., zmax=0.;
  double deltaLength = 0.;
  if(m_section==0) {
    zmin = m_gmt_mgr->IBLFlexPP0Z(1);
    zmax=barrelZmax-0.001;
    deltaLength=layerZshift;
  }
  else if(m_section==1) {
    zmin = barrelZmax+0.001;
    zmax = m_gmt_mgr->IBLFlexPP0Z(2);
    deltaLength=-layerZshift;
  }
  else if(m_section==2) {
    zmin = m_gmt_mgr->IBLFlexPP0Z(2);
    zmax = m_gmt_mgr->IBLFlexPP0Z(3);
  }
  else if(m_section==3) {
    zmin = m_gmt_mgr->IBLFlexPP0Z(3);
    zmax = m_gmt_mgr->IBLFlexPP0Z(4);
  }

  double zStartPosA = zmin+layerZshift;
  double zStartPosC = -zmax+layerZshift;
  // Caution : section 0 is in thebarrel volume that is already globally shifted
  if(m_section==0){    
    zStartPosC=-zmax; 
  }
  if(m_section==1) zStartPosA=zmin; 

  double innerRadius = m_gmt_mgr->IBLServiceGetMaxRadialPosition("IPT","simple",zmin,zmax)+safety;
  double outerRadius = m_gmt_mgr->IBLServiceGetMinRadialPosition("IST","simple",zmin,zmax)-safety;
  double phiOfModuleZero =  m_gmt_mgr->PhiOfModuleZero();  
  double layerRadius = m_gmt_mgr->PixelLayerRadius();

  double halfLengthA = (zmax-zmin)*0.5-deltaLength*.5;
  double halfLengthC = (zmax-zmin)*0.5+deltaLength*.5;

  // Define IFlex section for side A and C
  std::ostringstream lnameA;
  lnameA<<"Brl0A_FlexRingPP0_S"<<m_section;
  std::ostringstream lnameC;
  lnameC<<"Brl0C_FlexRingPP0_S"<<m_section;

  // Define IFlex section for side A
  const GeoTube* supportShapeA = new GeoTube(innerRadius,outerRadius,halfLengthA);
  const GeoTube* supportShapeC = new GeoTube(innerRadius,outerRadius,halfLengthC);
  const GeoMaterial* ether = m_mat_mgr->getMaterial("special::Ether");
//   GeoLogVol* supportLogVol_A = new GeoLogVol("Brl0A_FlexRingPP0",supportShapeA,ether);
//   GeoLogVol* supportLogVol_C = new GeoLogVol("Brl0C_FlexRingPP0",supportShapeC,ether);
  GeoLogVol* supportLogVol_A = new GeoLogVol(lnameA.str(),supportShapeA,ether);
  GeoLogVol* supportLogVol_C = new GeoLogVol(lnameC.str(),supportShapeC,ether);

  m_supportPhysA = new GeoPhysVol(supportLogVol_A);
  m_supportPhysC = new GeoPhysVol(supportLogVol_C);


  // -------------- Creation du cooling pipe 

  double cooling_radius = 35.1;
  double TubeOuterDiam = m_gmt_mgr->IBLStaveTubeOuterDiameter();
  double TubeInnerDiam = m_gmt_mgr->IBLStaveTubeInnerDiameter();
  double cooling_angle = -2.154*CLHEP::deg;

  if(m_gmt_mgr->PixelStaveAxe()==1)   
    {
      cooling_radius = 34.7 + layerRadius-33.25;
      cooling_angle = -.1*CLHEP::deg;
    }

  const GeoTube* service_coolingPipeA = new GeoTube(0.0,TubeOuterDiam*0.5,halfLengthA);
  const GeoTube* service_coolingPipeC = new GeoTube(0.0,TubeOuterDiam*0.5,halfLengthC);

  const GeoMaterial* cp_service_material = m_mat_mgr->getMaterial("pix::CoolingPipe_IBL");
  GeoLogVol * cp_service_logA = new GeoLogVol("PP0CoolingPipe",service_coolingPipeA,cp_service_material);
  GeoLogVol * cp_service_logC = new GeoLogVol("PP0CoolingPipe",service_coolingPipeC,cp_service_material);
  GeoPhysVol* cpPhysVolA = new GeoPhysVol(cp_service_logA);
  GeoPhysVol* cpPhysVolC = new GeoPhysVol(cp_service_logC);

  const GeoTube* service_coolingPipeInnerA = new GeoTube(0.0,TubeInnerDiam*0.5,halfLengthA);
  const GeoTube* service_coolingPipeInnerC = new GeoTube(0.0,TubeInnerDiam*0.5,halfLengthC);
  const GeoMaterial* cp_service_inner_material = m_mat_mgr->getMaterial(m_gmt_mgr->getMaterialName("CoolingFluid",0,0));
  GeoLogVol * cp_service_inner_logA = new GeoLogVol("PP0CoolingPipeInner",service_coolingPipeInnerA,cp_service_inner_material);
  GeoLogVol * cp_service_inner_logC = new GeoLogVol("PP0CoolingPipeInner",service_coolingPipeInnerC,cp_service_inner_material);
  GeoPhysVol * cp_service_inner_logPVA = new GeoPhysVol(cp_service_inner_logA);
  GeoPhysVol * cp_service_inner_logPVC = new GeoPhysVol(cp_service_inner_logC);

  GeoNameTag* cp_service_inner_tag = new GeoNameTag("PP0CoolingPipeInner");
  CLHEP::Hep3Vector cp_service_inner_pos(0.0,0.0,0.0);
  GeoTransform* cp_service_inner_xform = new GeoTransform(HepGeom::Transform3D(CLHEP::HepRotation(),cp_service_inner_pos));
  cpPhysVolA->add(cp_service_inner_tag);
  cpPhysVolA->add(cp_service_inner_xform);
  cpPhysVolA->add(cp_service_inner_logPVA);
  cpPhysVolC->add(cp_service_inner_tag);
  cpPhysVolC->add(cp_service_inner_xform);
  cpPhysVolC->add(cp_service_inner_logPVC);

  GeoLogVol* flex_logVolA = 0;
  GeoLogVol* flex_logVolC = 0;

  double flex_angle = -15.001*CLHEP::deg;
  if(m_gmt_mgr->PixelStaveAxe()==1)   
    flex_angle += 2.14*CLHEP::deg;

  double flex_rot=0.30265;
  flex_rot=-0.30265*.5;
  double flex_rmin=0, flex_rmax=0;
  double flexYmidPos=0;
  double flex_width = m_gmt_mgr->IBLStaveFlexWidth();

  if(m_section!=2){

    // -------------- Creation du flex (GeoBox)

    int secIdx=(m_section==3)?3:1;
    flex_rmin = m_gmt_mgr->IBLFlexPP0Rmin(secIdx);
    flex_rmax = m_gmt_mgr->IBLFlexPP0Rmax(secIdx);
    
    std::string flexMatName="noFlexMatDefined";
    if(m_section==0)
      flexMatName = m_gmt_mgr->IBLFlexMaterial(2,"doglegA");
    else if(m_section==1)
      flexMatName = m_gmt_mgr->IBLFlexMaterial(3,"doglegA");
    else if(m_section==3)
      flexMatName = m_gmt_mgr->IBLFlexMaterial(1,"PP0FlexA");
    
    GeoBox * flex_shapeA = new GeoBox((flex_rmax-flex_rmin)*.5, flex_width*.5, halfLengthA);
    GeoBox * flex_shapeC = new GeoBox((flex_rmax-flex_rmin)*.5, flex_width*.5, halfLengthC);
    CLHEP::Hep3Vector flex_pos(0.,0.,0.);
    //    GeoTransform* flex_xform = new GeoTransform(HepGeom::Transform3D(CLHEP::HepRotation(0.0,0.0,fabs(flex_rot)),flex_pos));
    const GeoMaterial* flex_material = m_mat_mgr->getMaterial(flexMatName);
    if(flex_material==0)
    {
      m_gmt_mgr->msg(MSG::ERROR)<<"-> error while reading material"<<flexMatName<<endmsg;
      return 0;
    } else{
    flex_logVolA = new GeoLogVol("Flex",flex_shapeA,flex_material);
    flex_logVolC = new GeoLogVol("Flex",flex_shapeC,flex_material);
    }
  }
  else {
    
    // -------------- Creation du flex (GeoSimplePolygonBRep)

    unsigned int nbFlexPoint=59;
    double xFlex[59]={-18.029,-15.546,-14.108,-12.832,-12.530,-11.770,-10.522,-9.285,-7.727,-6.132,-4.856,-3.906,-3.183,-1.643,-0.371,1.163,2.590,3.591,4.489,4.933,5.691,6.912,8.018,9.560,10.830,11.159,12.048,12.768,13.982,15.317,18.029,18.029,15.191,13.449,12.742,11.838,10.729,9.181,7.643,6.087,4.643,3.877,3.027,1.953,1.480,-0.071,-1.925,-3.102,-3.864,-5.065,-6.524,-7.999,-9.447,-10.867,-12.305,-13.289,-13.816,-15.377,-18.029};
    double yFlex[59]={32.768,32.749,32.997,34.143,34.552,35.888,36.844,37.204,37.260,36.858,35.875,34.396,32.984,31.517,31.110,31.364,32.348,33.602,35.204,36.151,37.422,38.466,38.761,38.453,37.521,37.182,35.860,34.828,33.877,33.653,33.668,34.589,34.566,35.449,36.501,37.805,38.814,39.571,39.665,39.034,37.597,36.057,34.440,33.014,32.612,32.021,32.858,34.847,36.074,37.383,37.962,38.205,38.111,37.698,36.831,35.075,34.406,33.655,33.689};
    
    flexYmidPos=(yFlex[0]+yFlex[nbFlexPoint-1])*.5;
    
    std::vector<double> xShape, yShape;
    xShape.clear(); yShape.clear();
    GeoSimplePolygonBrep* Iflex_shape = new GeoSimplePolygonBrep(flex_width*0.5);
    for(unsigned int iPt=0; iPt<nbFlexPoint; iPt++) Iflex_shape->addVertex(xFlex[iPt],yFlex[iPt]-flexYmidPos);
    
    std::string IflexMatName = m_gmt_mgr->IBLFlexMaterial(1, "IFlexA");
    m_gmt_mgr->msg(MSG::DEBUG)<<"IFlex material : "<<m_section<<" "<<IflexMatName<<endmsg;
    const GeoMaterial* Iflex_material = m_mat_mgr->getMaterial(IflexMatName);
    
    if(Iflex_material==0)
    {
      m_gmt_mgr->msg(MSG::ERROR)<<"-> error while reading IFlex material"<<endmsg;
      return 0;
    } else{
    flex_logVolA = new GeoLogVol("IFlex",Iflex_shape,Iflex_material);
    flex_logVolC = new GeoLogVol("IFlex",Iflex_shape,Iflex_material);
    }
  }

  GeoPhysVol * flexPhysVolA = new GeoPhysVol(flex_logVolA);
  GeoPhysVol * flexPhysVolC = new GeoPhysVol(flex_logVolC);

  // Loop over sectors
  for(int ii = 0; ii < nSectors; ii++) {

    m_gmt_mgr->SetPhi(ii);    
    
    // cooling transform 
    double phiOfCooling = phiOfModuleZero+ cooling_angle + ii*angle;
    std::ostringstream tmp1; 
    tmp1 << "fl" << ii;
    GeoNameTag * tag1 = new GeoNameTag(tmp1.str());
    GeoTransform* xform1 = new GeoTransform(HepGeom::RotateZ3D(phiOfCooling)*HepGeom::TranslateX3D(cooling_radius));
    m_supportPhysA->add(tag1);
    m_supportPhysA->add(xform1);
    m_supportPhysA->add(cpPhysVolA);

    m_supportPhysC->add(tag1);
    m_supportPhysC->add(xform1);
    m_supportPhysC->add(cpPhysVolC);


    // flex transform 
    double phiOfFlex = phiOfModuleZero+ flex_angle + ii*angle;
    std::ostringstream tmp2; 
    tmp2 << "fl" << ii;
    GeoNameTag * tag2 = new GeoNameTag(tmp2.str());

    if(m_section==2){

      // Intermediate flex
      GeoTransform* xformA2 = new GeoTransform(HepGeom::RotateZ3D(phiOfFlex)*HepGeom::TranslateX3D(flexYmidPos)*HepGeom::RotateZ3D(-90.*CLHEP::deg)*HepGeom::RotateY3D(-90.*CLHEP::deg)*HepGeom::RotateX3D(flex_rot));
      m_supportPhysA->add(tag2);
      m_supportPhysA->add(xformA2);
      m_supportPhysA->add(flexPhysVolA);

      GeoTransform* xformC2 = new GeoTransform(HepGeom::RotateZ3D(phiOfFlex)*HepGeom::TranslateX3D(flexYmidPos)*HepGeom::RotateZ3D(-90.*CLHEP::deg)*HepGeom::RotateY3D(90.*CLHEP::deg)*HepGeom::RotateX3D(-flex_rot));
      m_supportPhysC->add(tag2);
      m_supportPhysC->add(xformC2);
      m_supportPhysC->add(flexPhysVolC);

    }
    else {

      // dogleg and PP0 flexes

      GeoTransform* xform2 = new GeoTransform(HepGeom::RotateZ3D(phiOfFlex)*HepGeom::TranslateX3D((flex_rmin+flex_rmax)*.5)*HepGeom::RotateZ3D(flex_rot));

      m_supportPhysA->add(tag2);
      m_supportPhysA->add(xform2);
      m_supportPhysA->add(flexPhysVolA);
      
      m_supportPhysC->add(tag2);
      m_supportPhysC->add(xform2);
      m_supportPhysC->add(flexPhysVolC);
    }


  }

  double middleA = zStartPosA+halfLengthA;
  double middleC = zStartPosC+halfLengthC;

  HepGeom::Transform3D supportTrfA = HepGeom::TranslateZ3D(middleA);   //(zmin+zmax)*0.5+layerZshift);
  m_xformSupportA = new GeoTransform(supportTrfA);
  
  //  std::cout<<"Section final A  "<<m_section<<" - "<<middleA-halfLengthA<<" "<<middleA+halfLengthA<<"    "<<middleA<<std::endl;

  HepGeom::Transform3D supportTrfC = HepGeom::TranslateZ3D(middleC);   //(zmin+zmax)*0.5+layerZshift);
  m_xformSupportC = new GeoTransform(supportTrfC);

  //  std::cout<<"Section final C  "<<m_section<<" - "<<middleC-halfLengthC<<" "<<middleC+halfLengthC<<"    "<<middleC<<std::endl;
  //  std::cout<<"Section ---"<<std::endl;

  return 0;
}
