/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


// Build IBL fwd services (wavy shape)
// This is built one time per layer. 

#include "PixelGeoModel/GeoPixelIBLFwdServices.h"

#include "PixelGeoModel/GeoPixelIBLFwdSvcCADModel.h"
#include "PixelGeoModel/GeoPixelIBLFwdSvcModel1.h"

#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoMaterial.h"


#include <algorithm>
#include <iostream> 
#include <iomanip> 
using std::max;

GeoPixelIBLFwdServices::GeoPixelIBLFwdServices(int section): 
  m_bFwdComplexGeometry_CAD(false),
  m_bFwdComplexGeometry_Mod1(false),
  m_section(section),
  m_supportPhysA(0),
  m_supportPhysC(0),
  m_xformSupportA(0),
  m_xformSupportC(0)
{
}


bool GeoPixelIBLFwdServices::isComplexShapeDefined()
{
  // check if the wavy shape option is defined in the DB (based on the definition of the material)
  const GeoMaterial* cableMat_wavy = mat_mgr->getMaterialForVolume("pix::IBL_Fwd02_Cable_Wvy",1.,"pix::DummyMaterial");
  const GeoMaterial* cableMat_wavy_model1 = mat_mgr->getMaterialForVolume("pix::IBL_Fwd02_Cable_Wvy_M1",1.,"pix::DummyMaterial1");

  bool bFwd02Defined=false;
  int nbBarrelSvc = gmt_mgr->PixelServiceElements("barrel");
  for(int i=0; i<nbBarrelSvc; i++){
    std::string volName = gmt_mgr->PixelServiceName("barrel",i);
    if(volName.substr(0,9)=="IBL_Fwd02")bFwd02Defined=true;
  }
  int nbSimpleSvc = gmt_mgr->PixelServiceElements("simple");
  for(int i=0; i<nbSimpleSvc; i++){
    std::string volName = gmt_mgr->PixelServiceName("simple",i);
    if(volName.substr(0,9)=="IBL_Fwd02")bFwd02Defined=true;
  }

  if(cableMat_wavy&&!bFwd02Defined) m_bFwdComplexGeometry_CAD = true;
  if(cableMat_wavy_model1&&!bFwd02Defined) m_bFwdComplexGeometry_Mod1 = true;

  gmt_mgr->msg(MSG::INFO) <<"Build IBL fwd services - CAD tool design / S. model1 : "<<  m_bFwdComplexGeometry_CAD<<"/"<<m_bFwdComplexGeometry_Mod1<<endreq;


  return m_bFwdComplexGeometry_CAD||m_bFwdComplexGeometry_Mod1;

}


GeoVPhysVol* GeoPixelIBLFwdServices::Build()
{

  gmt_mgr->msg(MSG::INFO) <<"GeoPixelIBLFwdService - CAD tool design / S. model1 : "<<  m_bFwdComplexGeometry_CAD<<"/"<<m_bFwdComplexGeometry_Mod1<<endreq;
  
  if(m_bFwdComplexGeometry_CAD) {

    gmt_mgr->msg(MSG::INFO) <<"Build IBL fwd services - CAD tool design"<<endreq;
    GeoPixelIBLFwdSvcCADModel fwdSvc = GeoPixelIBLFwdSvcCADModel(m_section);
    
    fwdSvc.Build();
    m_supportPhysA = fwdSvc.getSupportA();
    m_supportPhysC = fwdSvc.getSupportC();
    m_xformSupportA = fwdSvc.getSupportTrfA();
    m_xformSupportC = fwdSvc.getSupportTrfC();

    if(m_supportPhysA==0) {
      gmt_mgr->msg(MSG::ERROR) <<"Build IBL fwd services - CAD tool design : the PixelGeoModel package is not meant to be used to build this design"<<endreq;
      gmt_mgr->msg(MSG::ERROR) <<"                                         : check that the GeoTorus object is defined in GeoModelKernel/GeoModel"<<endreq;
    }
    return 0;
  }


  if(m_bFwdComplexGeometry_Mod1) {

    gmt_mgr->msg(MSG::INFO) <<"Build IBL fwd services - S. Menke  design"<<endreq;
    GeoPixelIBLFwdSvcModel1 fwdSvc = GeoPixelIBLFwdSvcModel1(m_section);
    
    fwdSvc.Build();
    m_supportPhysA = fwdSvc.getSupportA();
    m_supportPhysC = fwdSvc.getSupportC();
    m_xformSupportA = fwdSvc.getSupportTrfA();
    m_xformSupportC = fwdSvc.getSupportTrfC();

    return 0;
  }
    
  return 0;

}
