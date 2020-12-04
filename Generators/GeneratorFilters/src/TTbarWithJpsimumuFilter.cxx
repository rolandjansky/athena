/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration 
*/
#include "GeneratorFilters/TTbarWithJpsimumuFilter.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"

//--------------------------------------------------------------------------
TTbarWithJpsimumuFilter::TTbarWithJpsimumuFilter(const std::string& fname, 
						 ISvcLocator* pSvcLocator)
  : GenFilter(fname,pSvcLocator),
    m_log(MsgStream(messageService(), name()))
{
  declareProperty("SelectJpsi",m_selectJpsi=true);
  //declareProperty("UseMuonDecay",m_useMuonDecay=true);
  //declareProperty("UseElectronDecay",m_useElectronDecay=false);
  declareProperty("JpsipTMinCut",m_JpsiPtMinCut=0.); /// MeV
  declareProperty("JpsietaMaxCut",m_JpsiEtaMaxCut=5.);
}

//--------------------------------------------------------------------------
TTbarWithJpsimumuFilter::~TTbarWithJpsimumuFilter(){
  /////
}

//---------------------------------------------------------------------------
StatusCode TTbarWithJpsimumuFilter::filterInitialize() {
  m_log << MSG::INFO << "Initialized" << endreq;
  return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------------
StatusCode TTbarWithJpsimumuFilter::filterFinalize() {
  m_log << MSG::INFO << m_nPass  << " Events out of " << m_nPass+m_nFail << " passed the filter" << endreq;
  return StatusCode::SUCCESS;
}


//---------------------------------------------------------------------------
StatusCode TTbarWithJpsimumuFilter::filterEvent() {
  //---------------------------------------------------------------------------

  bool pass = false;
  bool isjpsi = false;

  //std::cout << " ====================================== " << std::endl;;
  //std::cout << " ====================================== " << std::endl;;
  //std::cout << " TTbarWithJpsimumuFilter::filterEvent() " << std::endl;;
  //std::cout << " ====================================== " << std::endl;;
  //std::cout << " ====================================== " << std::endl;;

  McEventCollection::const_iterator itr;
  for (itr = events_const()->begin(); itr!=events_const()->end(); ++itr) {
    
    const HepMC::GenEvent* genEvt = (*itr);
    
    // Loop over all truth particles in the event
    // ===========================================
    for(HepMC::GenEvent::particle_const_iterator pitr=genEvt->particles_begin();pitr!=genEvt->particles_end(); ++pitr ) {

      const HepMC::GenParticle* part = (*pitr);
      
      if(part->barcode() > 200000) break;
      
      
      int pdgid = abs(part->pdg_id());
      
      //std::cout << "pdgid = " << pdgid << std::endl;
      // don't loose time checking all if one found
      if (pdgid == 443) {
	//std::cout << "Good pdgid = " << pdgid << std::endl;
	// check if it decays into muons
	if(!isLeptonDecay(part,13)) continue;
	//if(!isLeptonDecay(part,13) && m_useMuonDecay) continue;
	// check if it decays into electrons
	//if(!isLeptonDecay(part,11) && m_useElectronDecay) continue;
	//std::cout << "Good children " << std::endl;
 
      } else {
	continue;
      }
      
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
    //int type = hadronType(part->pdg_id());
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
