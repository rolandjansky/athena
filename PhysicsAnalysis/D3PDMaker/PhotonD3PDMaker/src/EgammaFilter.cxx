/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <algorithm>

#include "EgammaFilter.h"
#include "Particle/TrackParticle.h"
#include "TrkTrackSummary/TrackSummary.h"
#include "Particle/TrackParticleContainer.h"
#include "egammaEvent/PhotonContainer.h"
#include "egammaEvent/ElectronContainer.h"
#include "egammaEvent/egammaPIDdefs.h"
#include "muonEvent/MuonContainer.h"

#include "TrkTrack/LinkToTrack.h"
#include "TrkParticleBase/TrackParticleBaseCollection.h"
#include "TrkParticleBase/TrackParticleBase.h"
#include "TrkParticleBase/LinkToTrackParticleBase.h"
#include "VxVertex/VxTrackAtVertex.h"

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/StatusCode.h"
 
#include "GaudiKernel/MsgStream.h"

#include "AthenaKernel/errorcheck.h"

#include "StoreGate/StoreGateSvc.h" 

namespace D3PD{
EgammaFilter::EgammaFilter(const std::string& name, 
			   ISvcLocator* pSvcLocator) : 
  Algorithm(name, pSvcLocator), 
  m_storeGate(0),
  m_ph_getter(),
  m_el_getter(),
  m_PhotonNNTool("PhotonNeuralNetworkTool")
{ 
  declareProperty("OutputPhotonCollectionName"     , m_outputPhotonCollName   = "SelectedPhotons"       );
  declareProperty("PhotonGetter"                   , m_ph_getter                                        );
  declareProperty("PhotonPtCut"                    , m_photon_ptCut           = 20000.                  );
  declareProperty("PhotonEtaCut"                   , m_photon_etaCut          = 2.5                     );
  declareProperty("PhotonIsEM"                     , m_photon_IsEM            = egammaPID::PhotonIDLoose);
  declareProperty("PhotonNNCut"                    , m_photon_NNCut           = true                   );

  declareProperty("OutputElectronCollectionName"   , m_outputElectronCollName = "SelectedElectrons"     );
  declareProperty("ElectronGetter"                 , m_el_getter                                        );
  declareProperty("ElectronPtCut"                  , m_electron_ptCut         = 20000.                  );
  declareProperty("ElectronEtaCut"                 , m_electron_etaCut        = 2.5                     );
  declareProperty("ElectronIsEM"                   , m_electron_IsEM          = egammaPID::ElectronIDMediumPP );

  declareProperty ("PhotonNNTool"                  , m_PhotonNNTool,
		   "Photon neural-network helper tool.");
}

EgammaFilter::~EgammaFilter()
{ }

StatusCode EgammaFilter::initialize()
{

  MsgStream log(msgSvc(), name());


  StatusCode sc;
   
  // get StoreGate 
  sc = service("StoreGateSvc", m_storeGate);
  if ( sc == StatusCode::FAILURE ) {
    log<<MSG::ERROR << "   could not access StoreGate " << endreq;
    return StatusCode::FAILURE;
  }
 
  // Retrieve the getter tool and configure it, saying that we want
  // to get @c PAU::egamma objects from it.
  CHECK( m_ph_getter.retrieve() );
  CHECK( m_ph_getter->configureD3PD<Analysis::Photon>() );

  CHECK( m_el_getter.retrieve() );
  CHECK( m_el_getter->configureD3PD<Analysis::Electron>() );

  CHECK( m_PhotonNNTool.retrieve() );

  return StatusCode::SUCCESS;
    
}


StatusCode EgammaFilter::finalize()
{

 return StatusCode::SUCCESS;
}

///////////////
StatusCode EgammaFilter::execute()
{
  MsgStream log( messageService(), name() );
  log << MSG::DEBUG << " in execute" << endreq;

  // Record the output photon container
  egammaContainer* outputPhotonCont = new egammaContainer(SG::VIEW_ELEMENTS) ;
  CHECK( m_storeGate->record(outputPhotonCont, m_outputPhotonCollName) ); 


  // Get the photons
  if (m_ph_getter->reset().isFailure()) {
    REPORT_MESSAGE (MSG::ERROR) << "Can't reset input getter.";
    return StatusCode::FAILURE;
  }

  // loop over all the photons, keep the ones that pass
  while ( const Analysis::Photon* obj = m_ph_getter->next<Analysis::Photon>() ) {
    if(acceptPhoton(obj)){
      outputPhotonCont->push_back(const_cast<Analysis::Photon*> (obj)); 
    }
  }


  // now, loop over all the electrons, keep the ones that pass and that don't overlap with photons
  // we've already accepted
  egammaContainer* outputElectronCont = new egammaContainer(SG::VIEW_ELEMENTS) ;
  CHECK( m_storeGate->record(outputElectronCont, m_outputElectronCollName) ); 

  if (m_el_getter->reset().isFailure()) {
    REPORT_MESSAGE (MSG::ERROR) << "Can't reset input getter.";
    return StatusCode::FAILURE;
  }

  while ( const Analysis::Electron* obj = m_el_getter->next<Analysis::Electron>()) {
    if(acceptElectron(obj)){
      bool isOverlap = false;
      for(egammaContainer::const_iterator photonIter = outputPhotonCont->begin();
	  photonIter != outputPhotonCont->end();
	  ++photonIter){
	if(IsOverlap(obj, *photonIter)){
	  isOverlap = true;
	  break;
	}
      }
      if(!isOverlap)
	outputElectronCont->push_back(const_cast<Analysis::Electron*> (obj)); 
    }
  }

  return StatusCode::SUCCESS;
}


// Keep photons that satisfy all of the requirements.
bool EgammaFilter::acceptPhoton(const Analysis::Photon* photon){
  if(!photon->cluster()) 
    return false;
  if(photon->cluster()->pt() < m_photon_ptCut) 
    return false;
  if(fabs(photon->cluster()->etaBE(2)) > m_photon_etaCut) 
    return false;
  
  if(m_photon_IsEM != 0 && !photon->passID((egammaPID::egammaIDQuality)m_photon_IsEM)) 
    return false;
  if(m_photon_NNCut  && !m_PhotonNNTool->Passes(photon)) 
    return false;
  
  return true;
}

// Keep electrons that satisfy all of the requirements.
bool EgammaFilter::acceptElectron(const Analysis::Electron* electron){
  if(!electron->cluster()) 
    return false;
  if(electron->cluster()->pt() < m_electron_ptCut) 
    return false;
  if(fabs(electron->cluster()->etaBE(2)) > m_electron_etaCut) 
    return false;
  
  if(m_electron_IsEM != 0 && !electron->passID((egammaPID::egammaIDQuality)m_electron_IsEM)) 
    return false;
  
  return true;
}


bool EgammaFilter::IsOverlap(const egamma* el, const egamma* ph){
  return el->hasSameAthenaBarCodeExceptVersion(*ph);
}

}
