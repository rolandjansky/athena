/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeneratorFilters/FourLeptonInvMassFilter.h"

#include <math.h>


FourLeptonInvMassFilter::FourLeptonInvMassFilter(const std::string& name, ISvcLocator* pSvcLocator) : 
  GenFilter(name,pSvcLocator) {

  declareProperty("MinPt",            m_minPt           = 5000.);  
  declareProperty("MaxEta",           m_maxEta          = 5.0); 
  declareProperty("MinMass",          m_minMass         = 60000);
  declareProperty("MaxMass",          m_maxMass         = 14000000);
}

FourLeptonInvMassFilter::~FourLeptonInvMassFilter(){}

StatusCode FourLeptonInvMassFilter::filterInitialize() {
  ATH_MSG_DEBUG( "MinPt           "  << m_minPt);
  ATH_MSG_DEBUG( "MaxEta          "  << m_maxEta);
  ATH_MSG_DEBUG( "MinMass         "  << m_minMass);
  ATH_MSG_DEBUG( "MaxMass         "  << m_maxMass);
  
  return StatusCode::SUCCESS;
}

StatusCode FourLeptonInvMassFilter::filterFinalize() {
  return StatusCode::SUCCESS;
}

StatusCode FourLeptonInvMassFilter::filterEvent() {
   // Loop over all events in McEventCollection
  McEventCollection::const_iterator itr;
  for (itr = events()->begin(); itr!=events()->end(); ++itr) {
    const HepMC::GenEvent* genEvt = (*itr);
     auto genEvt_particles_begin = HepMC::begin(*genEvt);
     auto genEvt_particles_end = HepMC::end(*genEvt);
    // Loop over all particles in the event
    for (auto  pitr1 = genEvt_particles_begin; pitr1 != genEvt_particles_end; ++pitr1 ){
      if((*pitr1)->status()!=1) continue;
	  
      // Pick electrons or muons with Pt > m_inPt and |eta| < m_maxEta
      int pdgId1((*pitr1)->pdg_id());
      if (!(abs(pdgId1) == 11 || abs(pdgId1) == 13)) continue;	  
      if (!((*pitr1)->momentum().perp() >= m_minPt && std::abs((*pitr1)->momentum().pseudoRapidity()) <= m_maxEta)) continue;
      
      // Loop over all remaining particles in the event
      auto pitr2 = pitr1;
      pitr2++;
      
      for(; pitr2 != genEvt_particles_end; ++pitr2){
	if((*pitr2)->status()!=1 || pitr1 == pitr2) continue;
	
	// Pick electrons or muons with Pt > m_inPt and |eta| < m_maxEta	  
	int pdgId2((*pitr2)->pdg_id());
	if (!(abs(pdgId2) == 11 || abs(pdgId2) == 13)) continue;	     
	if (!((*pitr2)->momentum().perp() >= m_minPt && std::abs((*pitr2)->momentum().pseudoRapidity()) <= m_maxEta)) continue;
	
	// Loop over all remaining particles in the event
	auto pitr3 = pitr2;
	pitr3++;
	
	for(; pitr3 != genEvt_particles_end; ++pitr3){
	  if((*pitr3)->status()!=1 || pitr1 == pitr3 || pitr2 == pitr3 ) continue;
	  
	  // Pick electrons or muons with Pt > m_inPt and |eta| < m_maxEta	  	      
	  int pdgId3((*pitr3)->pdg_id());
	  if (!(abs(pdgId3) == 11 || abs(pdgId3) == 13)) continue;	    
	  if (!((*pitr3)->momentum().perp() >= m_minPt && std::abs((*pitr3)->momentum().pseudoRapidity()) <= m_maxEta)) continue;
	  
	  // Loop over all remaining particles in the event
	  auto pitr4 = pitr3;
	  pitr4++;
	  
	  for(; pitr4 != genEvt_particles_end; ++pitr4){
	    if((*pitr4)->status()!=1 || pitr1 == pitr4 || pitr2 == pitr4 || pitr3 == pitr4) continue;
	    
	    // Pick electrons or muons with Pt > m_inPt and |eta| < m_maxEta	  	      
	    int pdgId4((*pitr4)->pdg_id());
	    if (!(abs(pdgId4) == 11 || abs(pdgId4) == 13)) continue;	    
	    if (!((*pitr4)->momentum().perp() >= m_minPt && std::abs((*pitr4)->momentum().pseudoRapidity()) <= m_maxEta)) continue;
	    
	    // 4lepton vector
	    HepMC::FourVector vec((*pitr1)->momentum().px() + (*pitr2)->momentum().px() + (*pitr3)->momentum().px() + (*pitr4)->momentum().px(),
				  (*pitr1)->momentum().py() + (*pitr2)->momentum().py() + (*pitr3)->momentum().py() + (*pitr4)->momentum().py(),
				  (*pitr1)->momentum().pz() + (*pitr2)->momentum().pz() + (*pitr3)->momentum().pz() + (*pitr4)->momentum().pz(),
				  (*pitr1)->momentum().e()  + (*pitr2)->momentum().e()  + (*pitr3)->momentum().e()  + (*pitr4)->momentum().e());
	    double invMass = vec.m();
	    if(invMass > m_minMass && invMass < m_maxMass){
	      ATH_MSG_DEBUG( "PASSED FILTER " << invMass );
	      return StatusCode::SUCCESS;
	    }
	  }
	}
      }
    }
  }	
  setFilterPassed(false);
  return StatusCode::SUCCESS;
}

