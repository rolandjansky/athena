/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     egammaFinalizeClusters.cxx
PACKAGE:  offline/Reconstruction/egammaRec

********************************************************************/

// INCLUDE HEADER FILES:
#include "egammaRec/egammaFinalizeClusters.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "CaloUtils/CaloClusterStoreHelper.h"
    
egammaFinalizeClusters::egammaFinalizeClusters(const std::string& name, 
					       ISvcLocator* pSvcLocator): 
  AthAlgorithm(name, pSvcLocator){
  declareProperty ("outputClusterKey",m_outputClusterKey="egammaClusters");
  declareProperty ("outputForwardClusterKey",m_outputForwardClusterKey="ForwardElectronClusters");
  declareProperty ("outputTopoSeededClusterKey",m_outputTopoSeededClusterKey="egammaTopoSeededClusters");
}

// DESTRUCTOR:
egammaFinalizeClusters::~egammaFinalizeClusters(){  
}

// INITIALIZE METHOD:
StatusCode egammaFinalizeClusters::initialize(){
  ATH_MSG_INFO("Initializing egammaFinalizeClusters");
  return StatusCode::SUCCESS;
}

// FINALIZE METHOD:

StatusCode egammaFinalizeClusters::finalize() {
  return StatusCode::SUCCESS;
}

/////////////////////////////////////////////////////////////////
// ATHENA EXECUTE METHOD:
   
StatusCode egammaFinalizeClusters::execute() {
  
  ATH_MSG_DEBUG("Executing egammaFinalizeClusters");
  ATH_CHECK(impl(m_outputClusterKey));
  ATH_CHECK(impl(m_outputForwardClusterKey));
  ATH_CHECK(impl(m_outputTopoSeededClusterKey));
  //Return Gracefully
  return StatusCode::SUCCESS;
}

StatusCode egammaFinalizeClusters::impl(const std::string& name) const {

  if (evtStore()->contains<xAOD::CaloClusterContainer>(name)){
    xAOD::CaloClusterContainer* output=0;   
    ATH_CHECK(evtStore()->retrieve(output,name));
    ATH_CHECK( CaloClusterStoreHelper::finalizeClusters(&*evtStore(),
							output,
							name, 
							msg()) );
  }
  return StatusCode::SUCCESS;
}
