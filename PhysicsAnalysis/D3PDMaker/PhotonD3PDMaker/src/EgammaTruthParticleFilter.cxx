/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <algorithm>

#include "EgammaTruthParticleFilter.h"
#include "egammaEvent/Photon.h"
#include "egammaEvent/Electron.h"
#include "McParticleEvent/TruthParticle.h"
#include "McParticleEvent/TruthParticleContainer.h"

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/StatusCode.h"
 
#include "GaudiKernel/MsgStream.h"

#include "AthenaKernel/errorcheck.h"

#include "StoreGate/StoreGateSvc.h" 

#define PI 2.*asin(1.)

namespace D3PD{
EgammaTruthParticleFilter::EgammaTruthParticleFilter(const std::string& name, 
			 ISvcLocator* pSvcLocator) : 
  AthAlgorithm(name, pSvcLocator), 
  m_outputTruthName("EgammaTruth"),
  m_ph_getter(this),
  m_el_getter(this),
  m_truth_getter(this)
{ 
  declareProperty("OutputTruthCollectionName",m_outputTruthName);
  declareProperty("PhotonGetter"             ,m_ph_getter);
  declareProperty("ElectronGetter"           ,m_el_getter);
  declareProperty("TruthGetter"              ,m_truth_getter);
  declareProperty ("PAUtruthTool", m_PAUtruthTool,
                   "PAU truth tool.");
  declareProperty("PhotonPtCut", m_ptCut=500);
}

EgammaTruthParticleFilter::~EgammaTruthParticleFilter()
{ }

StatusCode EgammaTruthParticleFilter::initialize()
{

  // Retrieve the getter tool and configure it, saying that we want
  // to get @c PAU::egamma objects from it.
  CHECK( m_ph_getter.retrieve() );
  CHECK( m_ph_getter->configureD3PD<Analysis::Photon>() );

  // Retrieve the getter tool and configure it, saying that we want
  // to get @c PAU::egamma objects from it.
  CHECK( m_el_getter.retrieve() );
  CHECK( m_el_getter->configureD3PD<Analysis::Electron>() );

  return StatusCode::SUCCESS;
    
}


StatusCode EgammaTruthParticleFilter::finalize()
{
 return StatusCode::SUCCESS;
}

StatusCode EgammaTruthParticleFilter::AddTruthParticle(const TruthParticle* tp, TruthParticleContainer* outputCont){
  if(tp && m_kept_truth.find(tp) == m_kept_truth.end()){
    outputCont->push_back(const_cast<TruthParticle*>(tp));
    m_kept_truth.insert(tp);
  }
  return StatusCode::SUCCESS;
}

///////////////
StatusCode EgammaTruthParticleFilter::execute()
{
  MsgStream log( messageService(), name() );
  log << MSG::DEBUG << " in execute" << endreq;

  m_kept_truth.clear();

  // Record the output tracks container
  TruthParticleContainer* outputCont = new TruthParticleContainer(SG::VIEW_ELEMENTS);
  CHECK ( evtStore()->record(outputCont, m_outputTruthName) );

  // now, loop over all the photons, store the eta and phi positions of qualifying photons
  CHECK ( m_ph_getter->reset() );
  while ( const Analysis::Photon* obj = m_ph_getter->next<Analysis::Photon>()) {
    CHECK( AddTruthParticle(m_PAUtruthTool->getTruthMatch(obj), outputCont) );
    if (obj->conversion()) { // in case of converted photon, we want to keep the 2 truth objects mathced to the conversions tracks
      for (unsigned int i=0 ; i<m_PAUtruthTool->getTruthConvMatch(obj).size() ; i++) {
	CHECK( AddTruthParticle(m_PAUtruthTool->getTruthConvMatch(obj).at(i), outputCont) );
      }
    }
  }

  // now, loop over all the electrons, store the eta and phi positions of qualifying electrons
  CHECK ( m_el_getter->reset() );
  while ( const Analysis::Electron* obj = m_el_getter->next<Analysis::Electron>()) {
    CHECK ( AddTruthParticle(m_PAUtruthTool->getTruthMatch(obj), outputCont) );
  }

  // finally, check the rest of the truth particles for "interesting" candidates
  CHECK ( m_truth_getter->reset() );
  while ( const TruthParticle* tp = m_truth_getter->next<TruthParticle>()) {
    
    if ( tp == 0 || 
	 tp->genParticle() == 0 ||
	 tp->pdgId() != 22 || 
	 !(m_PAUtruthTool->isFinalStatePhotonMC(tp)) ||
	 (tp->pdgId()==22 && tp->pt()<m_ptCut) )  
      continue;

    CHECK ( AddTruthParticle(tp, outputCont) );
  }

  return StatusCode::SUCCESS;
}

}
