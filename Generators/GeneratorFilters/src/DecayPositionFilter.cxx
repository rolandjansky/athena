/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Allows the user to search for particles with specified decay positions
// It will pass if there is an particle with c*t in the specified range
#include <fstream>

#include "GeneratorFilters/DecayPositionFilter.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/DataSvc.h"

#include "StoreGate/StoreGateSvc.h"

#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/Incident.h"

#include "HepPDT/TableBuilder.hh"

using HepMC::GenVertex;
using HepMC::GenParticle;

DecayPositionFilter::DecayPositionFilter(const std::string& name, ISvcLocator* pSvcLocator)
  : GenFilter(name, pSvcLocator)
{
  declareProperty("RCut",m_RCut = 400.);
  declareProperty("PDG",m_PDGID = 3000001);
  declareProperty("MinPass",m_MinPass=2);
}


StatusCode DecayPositionFilter::filterInitialize() {
  ATH_MSG_INFO("RCut=" << m_RCut);
  ATH_MSG_INFO("PDG=" << m_PDGID);
  return StatusCode::SUCCESS;
}

StatusCode DecayPositionFilter::filterEvent() {
  int nPass = 0;
  for (McEventCollection::const_iterator itr = events()->begin(); itr != events()->end(); ++itr) {
    const HepMC::GenEvent* genEvt = (*itr);
    for (HepMC::GenEvent::particle_const_iterator pitr = genEvt->particles_begin(); pitr != genEvt->particles_end(); ++pitr) {
      if (abs((*pitr)->pdg_id()) == m_PDGID) {
	    if ( fabs((*pitr)->momentum().pseudoRapidity()) ) {
	    
            // Count only particles not decaying to themselves
            bool notSelfDecay = true;
            if ((*pitr)->end_vertex()) {
              HepMC::GenVertex::particle_iterator child = (*pitr)->end_vertex()->particles_begin(HepMC::children);
              HepMC::GenVertex::particle_iterator childE = (*pitr)->end_vertex()->particles_end(HepMC::children);
              for (; child != childE; ++child) {
                if ( (*child)->pdg_id() == (*pitr)->pdg_id() && (*child)->barcode()!=(*pitr)->barcode() && (*child)->barcode() < 100000) {
                  notSelfDecay = false;
                  break;
                }
              }
              if(notSelfDecay){

	            HepMC::GenVertex* vtx = (*pitr)->end_vertex();
				float x = vtx->position().x();
				float y = vtx->position().y();
				float Rdecay = sqrt(x*x + y*y);
			    if(Rdecay < m_RCut ) nPass++;	
			  }
			}
	    }
      }
     }
    }
    setFilterPassed(nPass == m_MinPass);

  return StatusCode::SUCCESS;
}
