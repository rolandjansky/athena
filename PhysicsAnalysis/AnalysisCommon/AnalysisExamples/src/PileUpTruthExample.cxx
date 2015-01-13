/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Author : Ketevi A. Assamagan
// Date :   February 2008

#include "PileUpTruthExample.h"

#include "GeneratorObjects/McEventCollection.h"

#include "GaudiKernel/DataSvc.h"
#include "GaudiKernel/ISvcLocator.h"

#include "StoreGate/StoreGateSvc.h"

#include "TruthHelper/IsGenStable.h"
#include "TruthHelper/PileUpType.h"

using namespace TruthHelper;

PileUpTruthExample::PileUpTruthExample(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator)
{
  declareProperty("McEventKey",     m_key="TruthEvent");
}

PileUpTruthExample::~PileUpTruthExample() {}

StatusCode PileUpTruthExample::initialize()
{
  return StatusCode::SUCCESS;
}

StatusCode PileUpTruthExample::finalize()
{
  return StatusCode::SUCCESS;
}

StatusCode PileUpTruthExample::execute() {

  const McEventCollection* mcCollptr = 0;
  ATH_CHECK (evtStore()->retrieve(mcCollptr, m_key));

  /** initialize a pileup type helper object */ 
  PileUpType pileupType( mcCollptr );

  // signal
  const HepMC::GenEvent * signal = pileupType.signal_mc_event();
  const HepMC::GenVertex* prodVtx = signal->signal_process_vertex();
  if ( prodVtx ) {
     float xi = (prodVtx->position()).x();
     float yi = (prodVtx->position()).y();
     float zi = (prodVtx->position()).z();

     ATH_MSG_INFO ("signal vertex is " << xi << " " << yi << " " << zi
                   << " process ID = " << signal->signal_process_id()
                   << " McEvent index = " << signal->event_number());
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
       ATH_MSG_INFO ("pileup vertex is " << xi << " " << yi << " " << zi
                     << " process ID = " << (*ibeg)->signal_process_id()
                     << " McEvent index = " << (*ibeg)->event_number());
    }
  }

  /** retrive the list of in-time generator stable particles - signal and in-time minimum bias 
      in-time meas particles in Bunch crossing = 0 */
  IsGenStable isStable;
  std::vector<const HepMC::GenParticle*> particleList;
  pileupType.in_time_particles( particleList, isStable );

  /** now do something with the list */
  ATH_MSG_INFO ("Number of stable in-time pileup particles = " << particleList.size());

  /** As another example, retrieve the in-time McEvents and print their contents 
      note that no selection is made in this case */
  McEventCollection::const_iterator ibegin = pileupType.in_time_event_begin();
  McEventCollection::const_iterator itend  = pileupType.in_time_event_end();
  for (; ibegin != itend; ++ibegin ) {
    (*ibegin)->print();
  }
  return StatusCode::SUCCESS;
}

