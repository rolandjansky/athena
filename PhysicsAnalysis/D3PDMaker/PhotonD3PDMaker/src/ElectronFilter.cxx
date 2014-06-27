/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <algorithm>

#include "ElectronFilter.h"
#include "Particle/TrackParticle.h"
#include "TrkTrackSummary/TrackSummary.h"
#include "Particle/TrackParticleContainer.h"
#include "egammaEvent/ElectronContainer.h"
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
ElectronFilter::ElectronFilter(const std::string& name, 
			   ISvcLocator* pSvcLocator) : 
  Algorithm(name, pSvcLocator), 
  m_storeGate(0),
  m_el_getter()
{ 
  declareProperty("OutputCollectionName"     , m_outputCollName   = "SelectedElectrons"      );
  declareProperty("electronGetter"           , m_el_getter                                   );
  declareProperty("ptCut"                    , m_ptCut            = 20000.                   );
  declareProperty("etaCut"                   , m_etaCut           = 2.5                      );
  declareProperty("IsEM"                     , m_IsEM             = egammaPID::ElectronLoose );
  declareProperty("Indices"                  , m_indices                                     );
  declareProperty("RejectAll"                , m_RejectAll        = false                    );
  declareProperty("AcceptAll"                , m_AcceptAll        = false                    );
}

ElectronFilter::~ElectronFilter()
{ }

StatusCode ElectronFilter::initialize()
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
  CHECK( m_el_getter.retrieve() );
  CHECK( m_el_getter->configureD3PD<Analysis::Electron>() );

  return StatusCode::SUCCESS;
    
}


StatusCode ElectronFilter::finalize()
{

 return StatusCode::SUCCESS;
}

///////////////
StatusCode ElectronFilter::execute()
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


  // Get the electrons
  if (m_el_getter->reset().isFailure()) {
    REPORT_MESSAGE (MSG::ERROR) << "Can't reset input getter.";
    return 0;
  }

  // now, loop over all the electrons, store the eta and phi positions of qualifying electrons
  int numAcceptedObjects=0;
  while ( const Analysis::Electron* obj = m_el_getter->next<Analysis::Electron>()) {
    if(accept(obj)){
      if(m_indices.size()==0 || 
	 std::find(m_indices.begin(),m_indices.end(),numAcceptedObjects)!=m_indices.end()){
	outputCont->push_back(const_cast<Analysis::Electron*> (obj)); 
      }
      ++numAcceptedObjects;
    }
  }

  return sc;
}


// Keep electrons that satisfy all of the requirements.
bool ElectronFilter::accept(const Analysis::Electron* electron){
  if(m_RejectAll)
    return false;
  if(m_AcceptAll)
    return true;

  if(!electron->cluster()) 
    return false;
  if(electron->cluster()->pt() < m_ptCut) 
    return false;
  
  if(m_IsEM != 0 && !electron->passID((egammaPID::egammaIDQuality)m_IsEM)) 
    return false;

  return true;
}

}
