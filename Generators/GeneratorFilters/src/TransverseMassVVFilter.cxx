/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration 
*/
#include "GeneratorFilters/TransverseMassVVFilter.h"
#include "HepMC/SimpleVector.h"
#include "TLorentzVector.h"
#include "CLHEP/Vector/LorentzVector.h"

TransverseMassVVFilter::TransverseMassVVFilter(const std::string& name, ISvcLocator* pSvcLocator)
  : GenFilter(name,pSvcLocator)
{
  declareProperty("MinMass",          m_minMass         = 400000);      // To avoid fsr etc
  declareProperty("MaxMass",          m_maxMass         = 14000000);
}


StatusCode TransverseMassVVFilter::filterInitialize() {
  ATH_MSG_DEBUG("MinMass         " << m_minMass);
  ATH_MSG_DEBUG("MaxMass         " << m_maxMass);
  return StatusCode::SUCCESS;
}


StatusCode TransverseMassVVFilter::filterEvent() {
  float MZ = 91187.6;
  McEventCollection::const_iterator itr;
  for (itr = events_const()->begin(); itr!=events_const()->end(); ++itr) {
    const HepMC::GenEvent* genEvt = (*itr);
    CLHEP::HepLorentzVector Boson1;
    CLHEP::HepLorentzVector Boson2;
  
    // Loop over all particles in the event
    for (HepMC::GenEvent::particle_const_iterator pitr1 = genEvt->particles_begin(); pitr1 != genEvt->particles_end(); ++pitr1 ){
      int pdgId1((*pitr1)->pdg_id());
      if( (pdgId1 == 23 && (*pitr1)->status() == 62) || (pdgId1 == 24 && (*pitr1)->status() == 62) ) {
        //==============================================
	Boson1.setPx((*pitr1)->momentum().px());
	Boson1.setPy((*pitr1)->momentum().py());
	Boson1.setPz((*pitr1)->momentum().pz());
	Boson1.setE((*pitr1)->momentum().e());
	
	// Loop over all remaining particles in the event
	HepMC::GenEvent::particle_const_iterator pitr2 = pitr1;
	pitr2++;
	for(; pitr2 != genEvt->particles_end(); ++pitr2){
	  int pdgId2((*pitr2)->pdg_id());
	  if( (pdgId2 == 23 && (*pitr2)->status() == 62) || (pdgId2 == 24 && (*pitr1)->status() == 62) ) {
	    //Repeat the children search for this Z boson
	    Boson2.setPx((*pitr2)->momentum().px());
	    Boson2.setPy((*pitr2)->momentum().py());
	    Boson2.setPz((*pitr2)->momentum().pz());
	    Boson2.setE((*pitr2)->momentum().e()); 
		  
	    ATH_MSG_DEBUG(" Z1 : Pt = " << (*pitr1)->momentum().perp() << ", Eta = " << (*pitr1)->momentum().pseudoRapidity() <<
			  ", pdgid = " << pdgId1 << ", status = " << (*pitr1)->status() << ", barcode = " << (*pitr1)->barcode() <<
			  " Z2 : Pt = " << (*pitr2)->momentum().perp() << ", Eta = " << (*pitr2)->momentum().pseudoRapidity() <<
			  ", pdgid = " << pdgId2 << ", status = " << (*pitr2)->status() << ", barcode = " << (*pitr2)->barcode());
	    
	    float firstTerm = pow(MZ,2)+pow(Boson1.perp(),2);
	    float secondTerm = pow(MZ,2)+pow(Boson2.perp(),2);
	    float thirdTerm = sqrt( (Boson1.px()+Boson2.px())*(Boson1.px()+Boson2.px()) + (Boson1.py()+Boson2.py())*(Boson1.py()+Boson2.py()) );
	    float mT_truth = sqrt( pow(sqrt(firstTerm)+sqrt(secondTerm), 2)-pow(thirdTerm,2));
	    ATH_MSG_DEBUG("MT = "<<mT_truth<<" Boson1 Pt, Boson2 Pt ="<<Boson1.perp()<<" "<<Boson2.perp());
	    
	    //filter on MT
	    if (m_minMass < mT_truth && mT_truth < m_maxMass) {
              //std::cout<<"Filter passed"<<std::endl;
	      ATH_MSG_DEBUG("PASSED FILTER");
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
