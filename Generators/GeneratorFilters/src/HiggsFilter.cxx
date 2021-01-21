/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -------------------------------------------------------------
//
//  File: GeneratorFilters/HiggsFilter.cxx
//
//  Description:
//
//  This filter requires the event to contain at least 
//  one Higgs with pt at or above Ptcut.
//
//  Events that do not contain H are rejected.
//
//  Only H decaying to b bbar are analyzed and counted in this algorithm. 
//
//  Originally created using GeneratorFilters templates for DC3 purposes.
//
//  AuthorList:
// 
//  M. Casolino: February 5, 2014
//

// Header for this module:-

#include "GeneratorFilters/HiggsFilter.h"
#include "CLHEP/Units/SystemOfUnits.h"

// Framework Related Headers:-
//#include "GaudiKernel/MsgStream.h"
//#include "GaudiKernel/AlgFactory.h"

// Other classes used by this class:-
//#include <math.h>

//using HepMC::GenVertex;
//using HepMC::GenParticle;
//using HepMC::GenVertex;


//--------------------------------------------------------------------------
HiggsFilter::HiggsFilter(const std::string& name, 
      ISvcLocator* pSvcLocator): GenFilter(name,pSvcLocator) {
  //--------------------------------------------------------------------------    
  declareProperty("Ptcut",m_Ptmin = 0.); 
  declareProperty("Ptmaxcut",m_Ptmax = 200000000.); 
}


//---------------------------------------------------------------------------
StatusCode HiggsFilter::filterEvent() {
//---------------------------------------------------------------------------


// Loop over all events in McEventCollection

  McEventCollection::const_iterator itr;

  int N_Higgs    = 0;

  int N_Higgs_all    = 0;

  int N_pt_above_cut = 0;


  //  Loop over all events in MCCollection
  for (itr = events_const()->begin(); itr!=events_const()->end(); ++itr) {

    // Loop over all particles in the event
    const HepMC::GenEvent* genEvt = (*itr);

    HepMC::GenEvent::particle_const_iterator pitr=genEvt->particles_begin();
    for(; pitr!=genEvt->particles_end(); ++pitr ){

      // Work only with tops
      if( fabs((*pitr)->pdg_id()) == 25 ){
	
	N_Higgs_all++;
	
	int n_daughters = 0;

	HepMC::GenParticle * mcpart = (*pitr);
	const HepMC::GenVertex * decayVtx = mcpart->end_vertex();

	// verify if we got a valid pointer and retrieve the number of daughters
	if ( decayVtx != 0 ) {
	  n_daughters = decayVtx->particles_out_size();
	}
	
	//  For this analysis we are not interested in t->t MC structures, only in decays
	if( n_daughters >= 2 ){

	  HepMC::GenVertex::particles_in_const_iterator child_mcpartItr  = decayVtx->particles_out_const_begin();
	  HepMC::GenVertex::particles_in_const_iterator child_mcpartItrE = decayVtx->particles_out_const_end();
	  
	  for (; child_mcpartItr != child_mcpartItrE; ++child_mcpartItr) {
	    
	    HepMC::GenParticle * child_mcpart = (*child_mcpartItr);
	    
	    //  Implicitly, I assume that tops always decay to W X
	    if ( fabs(child_mcpart->pdg_id()) == 5 ){

	      if ( (*pitr)->pdg_id() ==  25 ) {
		N_Higgs++;
	      }

	      if( ((*pitr)->momentum().perp() >=m_Ptmin && (*pitr)->momentum().perp() <m_Ptmax ) ){
		N_pt_above_cut++;
	      }

	    }//b bbar decay
	  }//child loop
	}// only decaying Higgs
      }//Higgs
    }
  }
  
  ATH_MSG_INFO( "Found " << N_Higgs_all    << " Higgs in event record" );

  ATH_MSG_INFO("Found " << N_Higgs    << " H    -> b bbar decays" );

  ATH_MSG_INFO( "The number of Higgs with pt above " << m_Ptmin/1000.0 << " GeV/c is " << N_pt_above_cut );

  if ( N_Higgs_all <= 0 ){
    ATH_MSG_ERROR( "No Higgs was found in a (presumably) Higgs event! Event is rejected." );
  }

  if ( N_Higgs >= 2 ){
    ATH_MSG_WARNING( "More than one H -> b b. Event is accepted.");
  }

  if( N_pt_above_cut >= 1 && N_Higgs >= 1)  {
    return StatusCode::SUCCESS;
  }

  // if we get here we have failed
  setFilterPassed(false);
  return StatusCode::SUCCESS;
}
