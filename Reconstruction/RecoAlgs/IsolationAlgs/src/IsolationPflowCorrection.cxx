///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// IsolationPflowCorrection.cxx 
// Implementation file for class IsolationPflowCorrection
/////////////////////////////////////////////////////////////////// 

// Isolation includes
#include "IsolationPflowCorrection.h"

// FrameWork includes
#include "GaudiKernel/Property.h"

#include "xAODPrimitives/IsolationConeSize.h"
#include "xAODPrimitives/IsolationHelpers.h"
#include "xAODEgamma/EgammaxAODHelpers.h"
#include "xAODEgamma/EgammaContainer.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/ElectronAuxContainer.h"
#include "xAODEgamma/Electron.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODEgamma/PhotonAuxContainer.h"
#include "xAODEgamma/Photon.h"

#include <set>
#include <utility>

template< class CONTAINER, class AUXSTORE > StatusCode IsolationPflowCorrection::deepCopyImp( const std::string& key ) const {
  
  // Retrieve the const container:
  ATH_MSG_VERBOSE( "Will retrieve " << key);
  const CONTAINER* c = nullptr;
  ATH_CHECK( evtStore()->retrieve( c, key ) );
  
  // Create the new container:
  ATH_MSG_VERBOSE( "Will create new containers" );
  //CONTAINER* copy = new CONTAINER();
  //AUXSTORE* copyAux = new AUXSTORE();
  //copy->setStore( copyAux );

  std::unique_ptr<CONTAINER> copy(new CONTAINER());
  std::unique_ptr<AUXSTORE> copyAux(new AUXSTORE());
  copy->setStore( copyAux.get() );

  // Do the deep copy:
  ATH_MSG_VERBOSE( "Will copy the object" );
  for( const auto &oldObj : *c ) {
    ATH_MSG_VERBOSE( "Now working on object " << oldObj);
    auto newObj = new typename CONTAINER::base_value_type();
    copy->push_back( newObj );
    *newObj = *oldObj;
  }
  
  // Do the overwrite:
  ATH_MSG_VERBOSE( "Will overwrite the container" );
  ATH_CHECK( evtStore()->overwrite( copy.release(), key, true, false ) );
  ATH_MSG_VERBOSE( "Will overwrite the aux container" );
  ATH_CHECK( evtStore()->overwrite( copyAux.release(), key + "Aux.", true, false ) );
  ATH_MSG_VERBOSE( "Done" );
  
  // Return gracefully:
  return StatusCode::SUCCESS;
}

template< class CONTAINER, class AUXSTORE > StatusCode IsolationPflowCorrection::deepCopy( const std::string& key ) const {
    
  // Let the user know what's happening:
  ATH_MSG_VERBOSE( "Running deepCopy on container: " << key );
  
  // Decide which implementation to call:
  if( evtStore()->template contains< AUXSTORE >( key + "Aux." ) ) {
    if( deepCopyImp< CONTAINER, AUXSTORE >( key ).isFailure() ) {
      ATH_MSG_FATAL( "Couldn't call deepCopyImp with concrete "
		     "auxiliary store" );
      return StatusCode::FAILURE;
    }
  } else if( evtStore()->template contains< xAOD::AuxContainerBase >( key +
								      "Aux." ) ) {
    if( deepCopyImp< CONTAINER,
	xAOD::AuxContainerBase >( key ).isFailure() ) {
      ATH_MSG_FATAL( "Couldn't call deepCopyImp with generic "
		     "auxiliary store" );
      return StatusCode::FAILURE;
    }
  } else {
    ATH_MSG_FATAL( "Couldn't discover auxiliary store type for container \""
		   << key << "\"" );
    return StatusCode::FAILURE;
  }
  
  // Return gracefully:
  return StatusCode::SUCCESS;
}

IsolationPflowCorrection::IsolationPflowCorrection( const std::string& name, 
			  ISvcLocator* pSvcLocator ) : 
  ::AthAlgorithm( name, pSvcLocator ),
    m_pflowObjectsInConeTool("")
{
  //
  // Property declaration
  // 
  declareProperty("ElectronCollectionContainerName",    m_ElectronContainerName    = "Electrons");
  declareProperty("PhotonCollectionContainerName",    m_PhotonContainerName    = "Photons");
  declareProperty("PFlowObjectsInConeTool",         m_pflowObjectsInConeTool);
}

StatusCode IsolationPflowCorrection::initialize()
{
  ATH_MSG_INFO ("Initializing " << name() << "...");
  if (!m_pflowObjectsInConeTool.empty())
    ATH_CHECK(m_pflowObjectsInConeTool.retrieve());
  return StatusCode::SUCCESS;
}

StatusCode IsolationPflowCorrection::finalize()
{
  ATH_MSG_INFO ("Finalizing " << name() << "...");
  return StatusCode::SUCCESS;
}

StatusCode IsolationPflowCorrection::execute()
{  
  ATH_MSG_DEBUG ("Executing " << name() << "...");

  // If AODFix, first deep copy
  if (m_ElectronContainerName.size()) {
    if (!evtStore()->tryRetrieve<xAOD::ElectronContainer>(m_ElectronContainerName)) {
      if( deepCopy<xAOD::ElectronContainer,xAOD::ElectronAuxContainer>(m_ElectronContainerName).isFailure()) {
        ATH_MSG_FATAL( "Couldn't deep copy electrons" );
        return StatusCode::FAILURE;
      }
    }
  }
  if (m_PhotonContainerName.size()) {
    if (!evtStore()->tryRetrieve<xAOD::PhotonContainer>(m_PhotonContainerName)) {
      if( deepCopy<xAOD::PhotonContainer,xAOD::PhotonAuxContainer>(m_PhotonContainerName).isFailure()) {
        ATH_MSG_FATAL( "Couldn't deep copy electrons" );
        return StatusCode::FAILURE;
      }
    }
  }

  xAOD::ElectronContainer *elC = nullptr;
  if (evtStore()->contains<xAOD::ElectronContainer>(m_ElectronContainerName)) {
    if (evtStore()->retrieve(elC,m_ElectronContainerName).isFailure()) {
      ATH_MSG_FATAL("Cannot retrieve electron container " << m_ElectronContainerName);
      return StatusCode::FAILURE;
    }
  } else {
    ATH_MSG_DEBUG("ElectronContainer " << m_ElectronContainerName << " not available");
    return StatusCode::SUCCESS;
  }

  xAOD::PhotonContainer *phC = nullptr;
  if (evtStore()->contains<xAOD::PhotonContainer>(m_PhotonContainerName)) {
    if (evtStore()->retrieve(phC,m_PhotonContainerName).isFailure()) {
      ATH_MSG_FATAL("Cannot retrieve electron container " << m_PhotonContainerName);
      return StatusCode::FAILURE;
    }
  } else {
    ATH_MSG_DEBUG("PhotonContainer " << m_PhotonContainerName << " not available");
    return StatusCode::SUCCESS;
  }

  xAOD::ElectronContainer::iterator it = elC->begin(), itE = elC->end();
  for (; it != itE; it++) {
    xAOD::Electron *eg = *it; 
    float phi = eg->caloCluster()->phi();
    float eta = eg->caloCluster()->eta();
    std::vector<const xAOD::PFO*> clusts;    
    if (m_pflowObjectsInConeTool) {
      m_pflowObjectsInConeTool->particlesInCone(eta,phi,0.2,clusts);
    } else {
      ATH_MSG_WARNING("No PFlowObjectsInConeTool available");      
    }
    float tg3correction = 0;
    for(const auto& cl : clusts) {
      float dphi = fabs(cl->phi()-phi);
      if (dphi>TMath::Pi()) dphi = 2*TMath::Pi()-dphi;
      float deta = cl->eta()-eta;
      if (dphi*dphi+deta*deta > 0.01) continue;
      if (cl->ptEM() <= 0 || fabs(cl->eta()) > 7.0) continue;

      float st = 1./cosh(cl->etaEM());
      float tilegap3_e = 0;
      cl->attribute<float>(xAOD::PFODetails::eflowRec_LAYERENERGY_TileGap3,tilegap3_e);
      float tilegap3_et = tilegap3_e*st;
      tg3correction += tilegap3_et;
    }
    eg->auxdata<float>("neflowisol20_old") = eg->auxdata<float>("neflowisol20");
    eg->auxdata<float>("neflowisol20") = eg->auxdata<float>("neflowisol20")-tg3correction;
  }

  xAOD::PhotonContainer::iterator itph = phC->begin(), itphE = phC->end();
  for (; itph != itphE; itph++) {
    xAOD::Photon *eg = *itph; 
    float phi = eg->caloCluster()->phi();
    float eta = eg->caloCluster()->eta();
    std::vector<const xAOD::PFO*> clusts;    
    if (m_pflowObjectsInConeTool) {
      m_pflowObjectsInConeTool->particlesInCone(eta,phi,0.2,clusts);
    } else {
      ATH_MSG_WARNING("No PFlowObjectsInConeTool available");      
    }
    float tg3correction = 0;
    for(const auto& cl : clusts) {
      float dphi = fabs(cl->phi()-phi);
      if (dphi>TMath::Pi()) dphi = 2*TMath::Pi()-dphi;
      float deta = cl->eta()-eta;
      if (dphi*dphi+deta*deta > 0.01) continue;
      if (cl->ptEM() <= 0 || fabs(cl->eta()) > 7.0) continue;

      float st = 1./cosh(cl->etaEM());
      float tilegap3_e = 0;
      cl->attribute<float>(xAOD::PFODetails::eflowRec_LAYERENERGY_TileGap3,tilegap3_e);
      float tilegap3_et = tilegap3_e*st;
      tg3correction += tilegap3_et;
    }
    eg->auxdata<float>("neflowisol20_old") = eg->auxdata<float>("neflowisol20");
    eg->auxdata<float>("neflowisol20") = eg->auxdata<float>("neflowisol20")-tg3correction;
  }

  return StatusCode::SUCCESS;
} 

  
