/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FastCaloSimGeometryHelper.h"
#include "CaloDetDescr/CaloDetDescrElement.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "GeoModelInterfaces/IGeoModelSvc.h"

using namespace std;

/** Constructor **/
FastCaloSimGeometryHelper::FastCaloSimGeometryHelper(const std::string& t, const std::string& n, const IInterface* p) : AthAlgTool(t,n,p), CaloGeometry(), m_geoModel(0),m_caloMgr(0)
{
  declareInterface<IFastCaloSimGeometryHelper>(this);

  //declareProperty("XXX"        , XXX     );
}

FastCaloSimGeometryHelper::~FastCaloSimGeometryHelper()
{
}

// Athena algtool's Hooks
StatusCode FastCaloSimGeometryHelper::initialize()
{
  ATH_MSG_INFO("Initializing FastCaloSimGeometryHelper");
  
  if(service("GeoModelSvc", m_geoModel).isFailure()) {
    ATH_MSG_ERROR( "Could not locate GeoModelSvc" );
    return StatusCode::FAILURE;
  }

  if(detStore()->retrieve(m_caloMgr, "CaloMgr").isFailure()) {
    ATH_MSG_ERROR("Unable to retrieve CaloDetDescrManager from DetectorStore");
    return StatusCode::FAILURE;
  }  

  if (m_geoModel->geoInitialized()) {
    // dummy parameters for the callback:
    int dummyInt=0;
    std::list<std::string> dummyList;
    
    if(geoInit(dummyInt,dummyList).isFailure()) {
      ATH_MSG_ERROR( "Call to geoInit failed" );
      return StatusCode::FAILURE;
    }
  } else {
    if(detStore()->regFcn(&IGeoModelSvc::geoInit, m_geoModel, &FastCaloSimGeometryHelper::geoInit,this).isFailure()) {
      ATH_MSG_ERROR( "Could not register geoInit callback" );
      return StatusCode::FAILURE;
    }
  }  

  return StatusCode::SUCCESS;
}

StatusCode FastCaloSimGeometryHelper::geoInit(IOVSVC_CALLBACK_ARGS)
{
  ATH_MSG_INFO("geoInit for " << m_geoModel->atlasVersion() );

  LoadGeometryFromCaloDDM();
  
  return StatusCode::SUCCESS;
}

StatusCode FastCaloSimGeometryHelper::finalize()
{
  ATH_MSG_INFO("Finalizing FastCaloSimGeometryHelper");

  return StatusCode::SUCCESS;
}

bool FastCaloSimGeometryHelper::LoadGeometryFromCaloDDM()
{
  ATH_MSG_INFO("Start LoadGeometryFromCaloDDM()");
  int jentry=0;
  for(CaloDetDescrManager::calo_element_const_iterator calo_iter=m_caloMgr->element_begin();calo_iter<m_caloMgr->element_end();++calo_iter) {
    const CaloDetDescrElement* pcell=*calo_iter;
    addcell(pcell);

    if(jentry%10000==0) {
      ATH_MSG_DEBUG("Load calo cell "<<jentry<<" : "<<pcell->getSampling()<<", "<<pcell->identify());
    }
    ++jentry;
  }

  bool ok=PostProcessGeometry();

  ATH_MSG_INFO("Done LoadGeometryFromCaloDDM()");

  return ok;
}

