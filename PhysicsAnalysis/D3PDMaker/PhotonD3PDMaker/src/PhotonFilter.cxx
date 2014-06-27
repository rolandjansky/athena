/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <algorithm>

#include "PhotonFilter.h"
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

#define PI 2.*asin(1.)

namespace D3PD{
PhotonFilter::PhotonFilter(const std::string& name, 
			   ISvcLocator* pSvcLocator) : 
  Algorithm(name, pSvcLocator), 
  m_storeGate(0),
  m_ph_getter(),
  m_PhotonNNTool("PhotonNeuralNetworkTool")
{ 
  declareProperty("OutputCollectionName"     , m_outputCollName   = "SelectedPhotons"      );
  declareProperty("photonGetter"             , m_ph_getter                                 );
  declareProperty("ptCut"                    , m_ptCut            = 20000.                 );
  declareProperty("etaCut"                   , m_etaCut           = 2.5                    );
  declareProperty("IsEM"                     , m_IsEM             = egammaPID::PhotonLoose );
  declareProperty("Indices"                  , m_indices                                   );
  declareProperty("NNCut"                    , m_NNCut            = true                   );

  declareProperty ("PhotonNNTool"            , m_PhotonNNTool,
		   "Photon neural-network helper tool.");
}

PhotonFilter::~PhotonFilter()
{ }

StatusCode PhotonFilter::initialize()
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
  CHECK( m_PhotonNNTool.retrieve() );

  return StatusCode::SUCCESS;
    
}


StatusCode PhotonFilter::finalize()
{

 return StatusCode::SUCCESS;
}

///////////////
StatusCode PhotonFilter::execute()
{
  MsgStream log( messageService(), name() );
  log << MSG::DEBUG << " in execute" << endreq;

  // Record the output tracks container
  egammaContainer* outputCont = new egammaContainer(SG::VIEW_ELEMENTS) ;
  StatusCode sc = m_storeGate->record(outputCont, m_outputCollName) ; 
  if(StatusCode::SUCCESS != sc ) {
    log<<MSG::ERROR<<" unable to record output container: "<< m_outputCollName <<endreq; 
    return sc;
  }


  // Get the photons
  if (m_ph_getter->reset().isFailure()) {
    REPORT_MESSAGE (MSG::ERROR) << "Can't reset input getter.";
    return 0;
  }

  // now, loop over all the photons, store the eta and phi positions of qualifying photons
  int numAcceptedObjects=0;
  while ( const Analysis::Photon* obj = m_ph_getter->next<Analysis::Photon>()) {
    if(accept(obj)){
      if(m_indices.size()==0 || 
	 std::find(m_indices.begin(),m_indices.end(),numAcceptedObjects)!=m_indices.end()){
	outputCont->push_back(const_cast<Analysis::Photon*> (obj)); 
      }
      ++numAcceptedObjects;
    }
  }

  return sc;
}


// Keep photons that satisfy all of the requirements.
bool PhotonFilter::accept(const Analysis::Photon* photon){
  if(!photon->cluster()) 
    return false;
  if(photon->cluster()->pt() < m_ptCut) 
    return false;
  if(fabs(photon->cluster()->etaBE(2)) > m_etaCut) 
    return false;
  
  if(m_IsEM != 0 && !photon->passID((egammaPID::egammaIDQuality)m_IsEM)) 
    return false;
  if(m_NNCut  && !m_PhotonNNTool->Passes(photon)) 
    return false;

  return true;
}

}
