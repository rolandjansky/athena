/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     egammaLockCollections.cxx
PACKAGE:  offline/Reconstruction/egammaRec

********************************************************************/

// INCLUDE HEADER FILES:
#include "egammaRec/egammaLockCollections.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "CaloUtils/CaloClusterStoreHelper.h"

//  END OF HEADER FILES INCLUDE
/////////////////////////////////////////////////////////////////
//  CONSTRUCTOR:
    
egammaLockCollections::egammaLockCollections(const std::string& name, 
				     ISvcLocator* pSvcLocator): 
  AthAlgorithm(name, pSvcLocator){
  // The following properties are specified at run-time
  // (declared in jobOptions file)  
  declareProperty ("outputClusterKey",m_outputClusterKey="egammaClusters");
  declareProperty ("outputForwardClusterKey",m_outputForwardClusterKey="ForwardElectronClusters");
  declareProperty ("outputTopoSeededClusterKey",m_outputTopoSeededClusterKey="egammaTopoSeededClusters");

}

// DESTRUCTOR:
egammaLockCollections::~egammaLockCollections(){  
}

/////////////////////////////////////////////////////////////////

// INITIALIZE METHOD:
StatusCode egammaLockCollections::initialize(){
  ATH_MSG_INFO("Initializing egammaLockCollections");


  return StatusCode::SUCCESS;
}

// FINALIZE METHOD:

StatusCode egammaLockCollections::finalize() {
  return StatusCode::SUCCESS;
}

/////////////////////////////////////////////////////////////////
// ATHENA EXECUTE METHOD:
   
StatusCode egammaLockCollections::execute() {
  
  ATH_MSG_DEBUG("Executing egammaLockCollections");

  ATH_CHECK(impl(m_outputClusterKey));
  ATH_CHECK(impl(m_outputForwardClusterKey));
  ATH_CHECK(impl(m_outputTopoSeededClusterKey));

  //Return Gracefully
  return StatusCode::SUCCESS;
}

StatusCode egammaLockCollections::impl(const std::string& name) const {
  
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
