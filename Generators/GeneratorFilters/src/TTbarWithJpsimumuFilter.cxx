/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration 
*/
#include "GeneratorFilters/TTbarWithJpsimumuFilter.h"

#include "GaudiKernel/MsgStream.h"

//--------------------------------------------------------------------------
TTbarWithJpsimumuFilter::TTbarWithJpsimumuFilter(const std::string& fname, 
						 ISvcLocator* pSvcLocator)
  : GenFilter(fname,pSvcLocator)
   
{
  declareProperty("SelectJpsi",m_selectJpsi=true);
  declareProperty("JpsipTMinCut",m_JpsiPtMinCut=0.); /// MeV
  declareProperty("JpsietaMaxCut",m_JpsiEtaMaxCut=5.);
}

//--------------------------------------------------------------------------
TTbarWithJpsimumuFilter::~TTbarWithJpsimumuFilter(){
  /////
}

//---------------------------------------------------------------------------
StatusCode TTbarWithJpsimumuFilter::filterInitialize() {
  ATH_MSG_INFO("Initialized");
  return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------------
StatusCode TTbarWithJpsimumuFilter::filterFinalize() {
  ATH_MSG_INFO(" Events out of " << m_nPass+m_nFail << " passed the filter");
  return StatusCode::SUCCESS;
}


//---------------------------------------------------------------------------
StatusCode TTbarWithJpsimumuFilter::filterEvent() {
  //---------------------------------------------------------------------------

  bool pass = false;
  bool isjpsi = false;

  McEventCollection::const_iterator itr;
  for (itr = events_const()->begin(); itr!=events_const()->end(); ++itr) {
    
    const HepMC::GenEvent* genEvt = (*itr);
    
    // Loop over all truth particles in the event
    // ===========================================
    for(HepMC::GenEvent::particle_const_iterator pitr=genEvt->particles_begin();pitr!=genEvt->particles_end(); ++pitr ) {

      const HepMC::GenParticle* part = (*pitr);
      
      if(part->barcode() > 200000) break;
      
      if ( !passJpsiSelection(part) ) continue;
      isjpsi=true;

    } /// loop on particles

  } // loop on events (only one at evgen - no PU)
  
  
  if (m_selectJpsi && isjpsi) pass=true;
  
  setFilterPassed(pass);
  return StatusCode::SUCCESS;

}

// ========================================================
bool TTbarWithJpsimumuFilter::isLeptonDecay(const HepMC::GenParticle* part, int type) const{

  HepMC::GenVertex* end = part->end_vertex();
  if(end){
    HepMC::GenVertex::particle_iterator firstChild = end->particles_begin(HepMC::children);
    HepMC::GenVertex::particle_iterator endChild = end->particles_end(HepMC::children);
    for(;firstChild!=endChild; ++firstChild){
      if( part->barcode() > (*firstChild)->barcode() ) continue; /// protection for sherpa
      int childtype = abs((*firstChild)->pdg_id());

      // debugging
      //const HepMC::FourVector& p4 = (*firstChild)->momentum();
      //double pt = p4.perp();
      //double eta = fabs(p4.eta());
      //std::cout << " childtype = " << childtype 
      //<< " " << pt
      //<< " " << eta
      //<< std::endl;

      if( childtype != type ){
	return false;
      }
    }

  }

  return true;

}

// ========================================================
bool TTbarWithJpsimumuFilter::passJpsiSelection(const HepMC::GenParticle* part) const{

  const HepMC::FourVector& p4 = part->momentum();
  double pt = p4.perp();
  double eta = fabs(p4.eta());
  
  if(pt<m_JpsiPtMinCut) return false;
  if(eta>m_JpsiEtaMaxCut) return false;

  return true;
  
}
