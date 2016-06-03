/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     egammaLockCollections.cxx
PACKAGE:  offline/Reconstruction/egammaRec

********************************************************************/

// INCLUDE HEADER FILES:
#include "egammaRec/egammaLockCollections.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/PhotonContainer.h"

//  END OF HEADER FILES INCLUDE
/////////////////////////////////////////////////////////////////
//  CONSTRUCTOR:
    
egammaLockCollections::egammaLockCollections(const std::string& name, 
				     ISvcLocator* pSvcLocator): 
  AthAlgorithm(name, pSvcLocator){
  // The following properties are specified at run-time
  // (declared in jobOptions file)  
  declareProperty ("outputElectronKey",m_outputElectronKey="Electrons");
  declareProperty ("outputPhotonKey",m_outputPhotonKey="Photons");
  declareProperty ("outputForwardElectronKey",m_outputForwardElectronKey="ForwardElectrons");
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
  ATH_CHECK(impl<xAOD::ElectronContainer>(m_outputElectronKey));
  ATH_CHECK(impl<xAOD::PhotonContainer>(m_outputPhotonKey));
  ATH_CHECK(impl<xAOD::ElectronContainer>(m_outputForwardElectronKey));
  //Return Gracefully
  return StatusCode::SUCCESS;
}

