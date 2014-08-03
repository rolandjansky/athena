/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Author : Ketevi A. Assamagan
// Date :   February 2008

#include "PileUpTruthExample.h"

#include "GeneratorObjects/McEventCollection.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/DataSvc.h"
#include "GaudiKernel/ISvcLocator.h"

#include "StoreGate/StoreGateSvc.h"

#include "TruthHelper/IsGenStable.h"
#include "TruthHelper/PileUpType.h"

using namespace TruthHelper;

PileUpTruthExample::PileUpTruthExample(const std::string& name, ISvcLocator* pSvcLocator) :
  Algorithm(name, pSvcLocator)
{
  declareProperty("McEventKey",     m_key="TruthEvent");
}

PileUpTruthExample::~PileUpTruthExample() {}

StatusCode PileUpTruthExample::initialize(){

  MsgStream log(messageService(), name());
  log << MSG::INFO << "in initialize" << endreq;

  StatusCode sc = service("StoreGateSvc", m_sgSvc);
  if (sc.isFailure()) {
    log << MSG::ERROR << "Could not find StoreGateSvc" << endreq;
    return sc;
  }
  // Initialization terminated
  return StatusCode::SUCCESS;
}

StatusCode PileUpTruthExample::finalize(){

  MsgStream log(messageService(), name());
  log << MSG::INFO << "in finalize" << endreq;

  return StatusCode::SUCCESS;
}

StatusCode PileUpTruthExample::execute() {

  MsgStream log(messageService(), name());

  const McEventCollection* mcCollptr;
  if ( m_sgSvc->retrieve(mcCollptr, m_key).isFailure() ) {
    log << MSG::ERROR << "Could not retrieve McEventCollection"
	<< endreq;
    return StatusCode::FAILURE;
  }

  /** initialize a pileup type helper object */ 
  PileUpType pileupType( mcCollptr );

  // signal
  const HepMC::GenEvent * signal = pileupType.signal_mc_event();
  const HepMC::GenVertex* prodVtx = signal->signal_process_vertex();
  if ( prodVtx ) {
     float xi = (prodVtx->position()).x();
     float yi = (prodVtx->position()).y();
     float zi = (prodVtx->position()).z();

     log << MSG::INFO << "signal vertex is " << xi << " " << yi << " " << zi
         << " process ID = " << signal->signal_process_id()
         << " McEvent index = " << signal->event_number() << endreq;
  }

  // in-time minbias
  McEventCollection::const_iterator ibeg = pileupType.in_time_minimum_bias_event_begin();
  McEventCollection::const_iterator iend = pileupType.in_time_minimum_bias_event_end();

  for (; ibeg!=iend; ++ibeg ) {
    const HepMC::GenVertex* prodVtx = (*ibeg)->signal_process_vertex();
    if ( prodVtx ) {
       float xi = (prodVtx->position()).x();
       float yi = (prodVtx->position()).y();
       float zi = (prodVtx->position()).z();
       log << MSG::INFO << "pileup vertex is " << xi << " " << yi << " " << zi
           << " process ID = " << (*ibeg)->signal_process_id()
           << " McEvent index = " << (*ibeg)->event_number() << endreq;
    }
  }

  /** retrive the list of in-time generator stable particles - signal and in-time minimum bias 
      in-time meas particles in Bunch crossing = 0 */
  IsGenStable isStable;
  std::vector<const HepMC::GenParticle*> particleList;
  pileupType.in_time_particles( particleList, isStable );

  /** now do something with the list */
  std::cout << " " << std::endl;
  log << MSG::INFO << "Number of stable in-time pileup particles = " << particleList.size();

  /** As another example, retrieve the in-time McEvents and print their contents 
      note that no selection is made in this case */
  McEventCollection::const_iterator ibegin = pileupType.in_time_event_begin();
  McEventCollection::const_iterator itend  = pileupType.in_time_event_end();
  for (; ibegin != itend; ++ibegin ) {
    (*ibegin)->print();
  }
  return StatusCode::SUCCESS;
}

