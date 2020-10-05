/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -------------------------------------------------------------
// File: GeneratorFilters/MultiHiggsFilter.cxx
// Description:
//   
//  The main purpose of this EF is to select event with N different Higgses
// it cannot handle H-> HH
//
// AuthorList:
// 
//  James Ferrando
//

// Header for this module:-

#include "GeneratorFilters/MultiHiggsFilter.h"


// Other classes used by this class:-
#include <math.h>

//--------------------------------------------------------------------------
MultiHiggsFilter::MultiHiggsFilter(const std::string& name, 
			 ISvcLocator* pSvcLocator): GenFilter(name,pSvcLocator) {
  //--------------------------------------------------------------------------    

  declareProperty("NHiggs",m_NHiggs = 1);
  declareProperty("Status",m_Status = 2);
  declareProperty("UseStatus",m_UseStatus = false);
  // if property "Exclusive" is true, thean require exactly "NHiggs" Higgs particles. If false, require >= NHiggs.
  declareProperty("Exclusive",m_Exclusive = true);
  m_nMultiHiggs = 0;
  m_nGoodMultiHiggs = 0;
}

//--------------------------------------------------------------------------
MultiHiggsFilter::~MultiHiggsFilter(){
//--------------------------------------------------------------------------

}

//---------------------------------------------------------------------------
StatusCode MultiHiggsFilter::filterInitialize() {
//---------------------------------------------------------------------------
  ATH_MSG_INFO("NHiggs = " << m_NHiggs);
  ATH_MSG_INFO("Exclusive NHiggs? " << m_Exclusive);
  // init
  m_nMultiHiggs = 0;
  m_nGoodMultiHiggs = 0;
  return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------------
StatusCode MultiHiggsFilter::filterFinalize() {
//---------------------------------------------------------------------------
  ATH_MSG_INFO("Statistics of N Higgs ");
  ATH_MSG_INFO("  ALL  events " << m_nMultiHiggs);
  ATH_MSG_INFO("  Good N Higgs " << m_nGoodMultiHiggs);
  if (m_nMultiHiggs != 0) 
    ATH_MSG_INFO( "  Fraction   " << double(m_nGoodMultiHiggs)/double(m_nMultiHiggs));
  return StatusCode::SUCCESS;
}


//---------------------------------------------------------------------------
StatusCode MultiHiggsFilter::filterEvent() {
//---------------------------------------------------------------------------

  // Loop over all events in McEventCollection
  McEventCollection::const_iterator itr;
  int nGoodParent = 0;
  for (itr = events()->begin(); itr!=events()->end(); ++itr) {
    // Loop over all particles in the event
    const HepMC::GenEvent* genEvt = (*itr);
#ifdef HEPMC3
    for(auto pitr: *genEvt) {
      if (!( std::abs(pitr->pdg_id()) == 25 && ( pitr->status()==m_Status || !m_UseStatus))) continue;
	bool hasHiggsParent = false;
	for(auto thisMother: pitr->production_vertex()->particles_in()){
	  ATH_MSG_DEBUG(" Parent " << pitr->pdg_id() << " barcode = "   << HepMC::barcode(pitr) << " status = "  << pitr->status());
	  ATH_MSG_DEBUG(" a Parent mother "  << thisMother->pdg_id()<< " barc = " << HepMC::barcode(thisMother));
	  if( thisMother->pdg_id() == 25 ) hasHiggsParent = true;
	}
      ATH_MSG_DEBUG(" has Higgs parent? " << hasHiggsParent);
	if (hasHiggsParent) continue;
	++nGoodParent;
      }
#else
    for(HepMC::GenEvent::particle_const_iterator pitr=genEvt->particles_begin();
	pitr!=genEvt->particles_end(); ++pitr ) {
      if( std::abs((*pitr)->pdg_id()) == 25 && ( (*pitr)->status()==m_Status || !m_UseStatus)){
	HepMC::GenVertex::particle_iterator firstMother =
	  (*pitr)->production_vertex()->particles_begin(HepMC::parents);
	HepMC::GenVertex::particle_iterator endMother =
	  (*pitr)->production_vertex()->particles_end(HepMC::parents);
	HepMC::GenVertex::particle_iterator thisMother = firstMother;
	bool hasHiggsParent = false;
	for(; thisMother != endMother; ++thisMother){
	  ATH_MSG_DEBUG(" Parent " << (*pitr)->pdg_id() << " barcode = "   << (*pitr)->barcode() << " status = "  << (*pitr)->status());
	  ATH_MSG_DEBUG(" a Parent mother "  << (*thisMother)->pdg_id()<< " barc = " << (*thisMother)->barcode());
	  if( (*thisMother)->pdg_id() == 25 ) hasHiggsParent = true;
	}
      ATH_MSG_DEBUG(" has Higgs parent? " << hasHiggsParent);
	if (hasHiggsParent) continue;
	++nGoodParent;
      }
    }
#endif    
  }
  
  ATH_MSG_INFO("Result " << nGoodParent << " Higgs ");

   ++m_nMultiHiggs;

 if (m_Exclusive)
     {
       if (nGoodParent == m_NHiggs )  {
       ++m_nGoodMultiHiggs;
       return StatusCode::SUCCESS;
       }
     }
   else
     {
       if (nGoodParent >= m_NHiggs )  {
       ++m_nGoodMultiHiggs;
       return StatusCode::SUCCESS;
       }
     } 

  // if we get here we have failed
  setFilterPassed(false);
  return StatusCode::SUCCESS;
}
