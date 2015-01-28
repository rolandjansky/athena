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

  gmt_mgr->msg(MSG::INFO) <<"Build IBL I-Flex services"<<endreq;

  double safety = 0.01*CLHEP::mm;

  double barrelZmax = gmt_mgr->PixelBarrelHalfLength();
  int nSectors = gmt_mgr->NPixelSectors();

  // check if sectors are properly defined
  if(nSectors==0) return 0;
  double angle=360./(double)nSectors*CLHEP::deg;

  double zmin=0., zmax=0.;
  if(m_section==0) {
    zmin = gmt_mgr->IBLFlexPP0Z(1);
    zmax=barrelZmax-0.001;
  }
  else if(m_section==1) {
    zmin = barrelZmax+0.001;
    zmax = gmt_mgr->IBLFlexPP0Z(2);
  }
  else if(m_section==2) {
    zmin = gmt_mgr->IBLFlexPP0Z(2);
    zmax = gmt_mgr->IBLFlexPP0Z(3);
  }
  else if(m_section==3) {
    zmin = gmt_mgr->IBLFlexPP0Z(3);
    zmax = gmt_mgr->IBLFlexPP0Z(4);
  }


  double innerRadius = gmt_mgr->IBLServiceGetMaxRadialPosition("IPT","simple",zmin,zmax)+safety;
  double outerRadius = gmt_mgr->IBLServiceGetMinRadialPosition("IST","simple",zmin,zmax)-safety;
  double phiOfModuleZero =  gmt_mgr->PhiOfModuleZero();  


  // Define IFlex section for side A
  const GeoTube* supportShape = new GeoTube(innerRadius,outerRadius,(zmax-zmin)*0.5);
  const GeoMaterial* ether = mat_mgr->getMaterial("special::Ether");
  GeoLogVol* supportLogVol_A = new GeoLogVol("Brl0A_FlexRingPP0",supportShape,ether);
  GeoLogVol* supportLogVol_C = new GeoLogVol("Brl0C_FlexRingPP0",supportShape,ether);

  m_supportPhysA = new GeoPhysVol(supportLogVol_A);
  m_supportPhysC = new GeoPhysVol(supportLogVol_C);


  // -------------- Creation du cooling pipe 

  double cooling_radius = 35.1;
  double TubeOuterDiam = gmt_mgr->IBLStaveTubeOuterDiameter();
  double TubeInnerDiam = gmt_mgr->IBLStaveTubeInnerDiameter();
  double cooling_angle = -2.154*CLHEP::deg;

  const GeoTube* service_coolingPipe = new GeoTube(0.0,TubeOuterDiam*0.5,(zmax-zmin)*0.5);

  const GeoMaterial* cp_service_material = mat_mgr->getMaterial("pix::CoolingPipe_IBL");
  GeoLogVol * cp_service_log = new GeoLogVol("PP0CoolingPipe",service_coolingPipe,cp_service_material);
  GeoPhysVol* cpPhysVol = new GeoPhysVol(cp_service_log);

  const GeoTube* service_coolingPipeInner = new GeoTube(0.0,TubeInnerDiam*0.5,(zmax-zmin)*0.5);
  const GeoMaterial* cp_service_inner_material = mat_mgr->getMaterial(gmt_mgr->getMaterialName("CoolingFluid",0,0));
  GeoLogVol * cp_service_inner_log = new GeoLogVol("PP0CoolingPipeInner",service_coolingPipeInner,cp_service_inner_material);
  GeoPhysVol * cp_service_inner_logPV = new GeoPhysVol(cp_service_inner_log);

  GeoNameTag* cp_service_inner_tag = new GeoNameTag("PP0CoolingPipeInner");
  CLHEP::Hep3Vector cp_service_inner_pos(0.0,0.0,0.0);
  GeoTransform* cp_service_inner_xform = new GeoTransform(HepGeom::Transform3D(CLHEP::HepRotation(),cp_service_inner_pos));
  cpPhysVol->add(cp_service_inner_tag);
  cpPhysVol->add(cp_service_inner_xform);
  cpPhysVol->add(cp_service_inner_logPV);


  GeoLogVol* flex_logVol = 0;

  double flex_angle = -15.001*CLHEP::deg;
  double flex_rot=0.30265;
  flex_rot=-0.30265*.5;
  double flex_rmin=0, flex_rmax=0;
  double flexYmidPos=0;
  double flex_width = gmt_mgr->IBLStaveFlexWidth();

  if(m_section!=2){

    // -------------- Creation du flex (GeoBox)

    int secIdx=(m_section==3)?3:1;
    flex_rmin = gmt_mgr->IBLFlexPP0Rmin(secIdx);
    flex_rmax = gmt_mgr->IBLFlexPP0Rmax(secIdx);
    
    
    std::string flexMatName="noFlexMatDefined";
    if(m_section==0)
      flexMatName = gmt_mgr->IBLFlexMaterial(2,"doglegA");
    else if(m_section==1)
      flexMatName = gmt_mgr->IBLFlexMaterial(3,"doglegA");
    else if(m_section==3)
      flexMatName = gmt_mgr->IBLFlexMaterial(1,"PP0FlexA");
    
    gmt_mgr->msg(MSG::DEBUG)<<"Dogleg flex material : "<<m_section<<" "<<flexMatName<<endreq;
    
    GeoBox * flex_shape = new GeoBox((flex_rmax-flex_rmin)*.5, flex_width*.5, (zmax-zmin)*0.5);
    CLHEP::Hep3Vector flex_pos(0.,0.,0.);
    //    GeoTransform* flex_xform = new GeoTransform(HepGeom::Transform3D(CLHEP::HepRotation(0.0,0.0,fabs(flex_rot)),flex_pos));
    const GeoMaterial* flex_material = mat_mgr->getMaterial(flexMatName);
    if(flex_material==0) gmt_mgr->msg(MSG::DEBUG)<<"-> error while reading material"<<flexMatName<<endreq;
    
    flex_logVol = new GeoLogVol("Flex",flex_shape,flex_material);
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
    
    std::string IflexMatName = gmt_mgr->IBLFlexMaterial(1, "IFlexA");
    gmt_mgr->msg(MSG::DEBUG)<<"IFlex material : "<<m_section<<" "<<IflexMatName<<endreq;
    const GeoMaterial* Iflex_material = mat_mgr->getMaterial(IflexMatName);
    
    if(Iflex_material==0) gmt_mgr->msg(MSG::DEBUG)<<"-> error while reading IFlex material"<<endreq;
    
    flex_logVol = new GeoLogVol("IFlex",Iflex_shape,Iflex_material);
  }

  GeoPhysVol * flexPhysVol = new GeoPhysVol(flex_logVol);


  // Loop over sectors
  for(int ii = 0; ii < nSectors; ii++) {

    gmt_mgr->SetPhi(ii);    
    
    // cooling transform 
    double phiOfCooling = phiOfModuleZero+ cooling_angle + ii*angle;
    std::ostringstream tmp1; 
    tmp1 << "fl" << ii;
    GeoNameTag * tag1 = new GeoNameTag(tmp1.str());
    GeoTransform* xform1 = new GeoTransform(HepGeom::RotateZ3D(phiOfCooling)*HepGeom::TranslateX3D(cooling_radius));
    m_supportPhysA->add(tag1);
    m_supportPhysA->add(xform1);
    m_supportPhysA->add(cpPhysVol);

    m_supportPhysC->add(tag1);
    m_supportPhysC->add(xform1);
    m_supportPhysC->add(cpPhysVol);


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
      m_supportPhysA->add(flexPhysVol);

      GeoTransform* xformC2 = new GeoTransform(HepGeom::RotateZ3D(phiOfFlex)*HepGeom::TranslateX3D(flexYmidPos)*HepGeom::RotateZ3D(-90.*CLHEP::deg)*HepGeom::RotateY3D(90.*CLHEP::deg)*HepGeom::RotateX3D(-flex_rot));
      m_supportPhysC->add(tag2);
      m_supportPhysC->add(xformC2);
      m_supportPhysC->add(flexPhysVol);

    }
    else {

      // dogleg and PP0 flexes

      GeoTransform* xform2 = new GeoTransform(HepGeom::RotateZ3D(phiOfFlex)*HepGeom::TranslateX3D((flex_rmin+flex_rmax)*.5)*HepGeom::RotateZ3D(flex_rot));

      m_supportPhysA->add(tag2);
      m_supportPhysA->add(xform2);
      m_supportPhysA->add(flexPhysVol);
      
      m_supportPhysC->add(tag2);
      m_supportPhysC->add(xform2);
      m_supportPhysC->add(flexPhysVol);
    }


  }


  HepGeom::Transform3D supportTrfA = HepGeom::TranslateZ3D((zmin+zmax)*0.5);
  m_xformSupportA = new GeoTransform(supportTrfA);
  
  HepGeom::Transform3D supportTrfC = HepGeom::TranslateZ3D(-(zmin+zmax)*0.5);
  m_xformSupportC = new GeoTransform(supportTrfC);

  return 0;

}
