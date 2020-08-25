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
  for (itr = events()->begin(); itr!=events()->end(); ++itr) {

    // Loop over all particles in the event
    const HepMC::GenEvent* genEvt = (*itr);

    for(auto pitr: *genEvt){
        if( std::abs(pitr->pdg_id()) != 25 ) continue;
	N_Higgs_all++;	
	auto decayVtx = pitr->end_vertex();
	// verify if we got a valid pointer and retrieve the number of daughters
	if (!decayVtx ) continue;
#ifdef HEPMC3
	int n_daughters =  decayVtx->particles_out().size();
#else
        int n_daughters =  decayVtx->particles_out_size();
#endif
	if( n_daughters < 2 ) continue;
	for ( auto child_mcpart: *decayVtx) {
	      if ( std::abs(child_mcpart->pdg_id()) != 5 ) continue;
	      if ( pitr->pdg_id() ==  25 ) {
		N_Higgs++;
	      }
	      if( (pitr->momentum().perp() >=m_Ptmin && pitr->momentum().perp() <m_Ptmax ) ){
		N_pt_above_cut++;
	      }
      }//child loop
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
