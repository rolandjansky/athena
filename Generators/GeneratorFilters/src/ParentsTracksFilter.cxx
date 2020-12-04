/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration 
*/
// -------------------------------------------------------------
// File: GeneratorFilters/ParentsTracksFilter.cxx
// Description:
//
//   Allows the user to search for a few particles (parents)
//   decaying to final states containing stable charged (e+-, mu+-, pi+-, K+-, p+-)
//   particles (tracks): Parent_1/2/.. -> tracks_1 + track_2 + track_... + X.
//   Up to 4 consequitive decays are considered.
//
//   The Parents are required to be listed in the vector PDGParent[],
//   which compactly opens for multiple OR'ed searches, 
//
//   Furthermore, setting PDGParent = [0] in the jobOption removes the 
//   PDG requirement on the Parent. This setting would test on 'decays' of the 
//   type  A(status 1) -> A(status 2). 
//   To mend this, Parent and  is also required to have different PDG. 
//
//   Eta and Pt cuts are imposed on both Parents and Childs. 
//   For childs, the cuts are imposed separately for Leptons and Hadrons
//
//   Numbers of selected Tracks (Leptons+Hadrons),
//   Leptons and Others (nu_e, nu_mu, pi0, n, K0, ...)
//   are requred to be in some ranges set by input commands.
//
//   There is no requirement on the status of the particles. Since typical Child particles 
//   like tau (and quarks) are not stable, such a requirement would be of limited use. 
//   In any case the order Parent -> Child should be sufficient for well-defined filtering. 
//
// AuthorList:         
//   B K Gjelsten   March 2006 (author of preceding ParentChildFilter)
//   L K Gladilin (gladilin@mail.cern.ch)  March 2016
//

// Header for this module:-

#include "GeneratorFilters/ParentsTracksFilter.h"

// Framework Related Headers:-
#include "GaudiKernel/MsgStream.h"


// Other classes used by this class:-
#include <math.h>

using HepMC::GenVertex;
using HepMC::GenParticle;

//--------------------------------------------------------------------------
ParentsTracksFilter::ParentsTracksFilter(const std::string& name, 
      ISvcLocator* pSvcLocator): GenFilter(name,pSvcLocator) {
  //--------------------------------------------------------------------------    
  declareProperty("PDGParent",m_PDGParent); 
  declareProperty("PtMinParent",m_PtMinParent = 0.); 
  declareProperty("PtMaxParent",m_PtMaxParent = 1e9);
  declareProperty("MassMinParent",m_MassMinParent = -1e9);
  declareProperty("MassMaxParent",m_MassMaxParent = 1e9); 
  declareProperty("EtaRangeParent",m_EtaRangeParent = 10.0); 
  declareProperty("PtMinLeptons",m_PtMinLeptons = 0.);  
  declareProperty("EtaRangeLeptons",m_EtaRangeLeptons = 10.0);
  declareProperty("PtMinHadrons",m_PtMinHadrons = 0.);  
  declareProperty("EtaRangeHadrons",m_EtaRangeHadrons = 10.0); 
  declareProperty("NumMinTracks",m_NumMinTracks = 0);  
  declareProperty("NumMaxTracks",m_NumMaxTracks = 999999);  
  declareProperty("NumMinLeptons",m_NumMinLeptons = 0);  
  declareProperty("NumMaxLeptons",m_NumMaxLeptons = 999999);  
  declareProperty("NumMinOthers",m_NumMinOthers = 0);
  declareProperty("NumMaxOthers",m_NumMaxOthers = 999999);  
}

//--------------------------------------------------------------------------
 ParentsTracksFilter::~ParentsTracksFilter(){
//--------------------------------------------------------------------------

}

//---------------------------------------------------------------------------
StatusCode ParentsTracksFilter::filterInitialize() {
//---------------------------------------------------------------------------

 MsgStream log(messageService(), name());   
 log << MSG:: INFO << "ParentsTracksFilter INITIALISING "  << endreq;   

 if(m_PDGParent.size()==0) 
   log << MSG:: ERROR << "PDGParent[] not set " << endreq; 
 // if(m_PDGChild.size()==0) 
 //   log << MSG:: ERROR << "PDGChild[] not set " << endreq; 

 for(int i=0;i<int(m_PDGParent.size());i++) 
   log << MSG:: INFO << "PDGParent["<<i<<"] = " << m_PDGParent[i] << endreq;
 log << MSG:: INFO << "PtMinParent = " << m_PtMinParent << endreq;
 log << MSG:: INFO << "PtMaxParent = " << m_PtMaxParent << endreq;
 log << MSG:: INFO << "MassMinParent = " << m_MassMinParent << endreq;
 log << MSG:: INFO << "MassMaxParent = " << m_MassMaxParent << endreq;
 log << MSG:: INFO << "EtaRangeParent = " << m_EtaRangeParent << endreq;
 log << MSG:: INFO << "NumMinTracks = " << m_NumMinTracks << endreq;
 log << MSG:: INFO << "NumMaxTracks = " << m_NumMaxTracks << endreq;
 log << MSG:: INFO << "NumMinLeptons = " << m_NumMinLeptons << endreq;
 log << MSG:: INFO << "NumMaxLeptons = " << m_NumMaxLeptons << endreq;
 log << MSG:: INFO << "NumMinOthers = " << m_NumMinOthers << endreq;
 log << MSG:: INFO << "NumMaxOthers = " << m_NumMaxOthers << endreq;
 // for(int i=0;i<int(m_PDGChild.size());i++) 
   //   log << MSG:: INFO << "PDGChild["<<i<<"] = " << m_PDGChild[i] << endreq;
 log << MSG:: INFO << "PtMinLeptons = " << m_PtMinLeptons << endreq;
 log << MSG:: INFO << "EtaRangeLeptons = " << m_EtaRangeLeptons << endreq;
 log << MSG:: INFO << "PtMinHadrons = " << m_PtMinHadrons << endreq;
 log << MSG:: INFO << "EtaRangeHadrons = " << m_EtaRangeHadrons << endreq;
 return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------------
StatusCode ParentsTracksFilter::filterFinalize() {
//---------------------------------------------------------------------------
 return StatusCode::SUCCESS;
}


//---------------------------------------------------------------------------
StatusCode ParentsTracksFilter::filterEvent() {
//---------------------------------------------------------------------------

// Loop over all events in McEventCollection 
  MsgStream log(messageService(), name());

  log << MSG:: INFO << " ParentsTracksFilter filtering "  << endreq;  
  McEventCollection::const_iterator itr;
  for (itr = events_const()->begin(); itr!=events_const()->end(); ++itr) {
    // Loop over all particles in the event
    const HepMC::GenEvent* genEvt = (*itr);
    for(HepMC::GenEvent::particle_const_iterator pitr=genEvt->particles_begin();
	pitr!=genEvt->particles_end(); ++pitr ){
      // Parent
      int okPDGParent=0;
      for(int i=0;i<int(m_PDGParent.size());i++) 
	if(abs((*pitr)->pdg_id()) == m_PDGParent[i]) okPDGParent=1;
      if( ( (m_PDGParent[0] == 0) || okPDGParent )
	  && ((*pitr)->momentum().perp() >= m_PtMinParent)
          && ((*pitr)->momentum().perp() < m_PtMaxParent)
          && ((*pitr)->momentum().m() >= m_MassMinParent)
          && ((*pitr)->momentum().m() < m_MassMaxParent) 
	  && (fabs((*pitr)->momentum().eta()) < m_EtaRangeParent) ) {
	//Check if has end_vertex (skips initial protons)
	if(!((*pitr)->end_vertex())) continue; 
	// Child
	HepMC::GenVertex::particle_iterator firstChild =
	  (*pitr)->end_vertex()->particles_begin(HepMC::children);
	HepMC::GenVertex::particle_iterator endChild =
	  (*pitr)->end_vertex()->particles_end(HepMC::children);
	HepMC::GenVertex::particle_iterator thisChild = firstChild;
	//
	int NumHadrons=0;
	int NumLeptons=0;
	int NumOthers=0;
	//
	for(; thisChild != endChild; ++thisChild){
	  //
	  if( (*thisChild)->pdg_id() == (*pitr)->pdg_id() ) continue; 
	  //
	  if ( ( abs((*thisChild)->pdg_id()) == 11 || abs((*thisChild)->pdg_id()) == 13 )
	       && ((*thisChild)->momentum().perp() > m_PtMinLeptons)
	       && (fabs((*thisChild)->momentum().eta()) < m_EtaRangeLeptons) ) {
	    ++NumLeptons;
	  }
	  else if ( ( abs((*thisChild)->pdg_id()) == 211 || abs((*thisChild)->pdg_id()) == 321
                                                         || abs((*thisChild)->pdg_id()) == 2212 )
                    && ((*thisChild)->momentum().perp() > m_PtMinHadrons)
	            && (fabs((*thisChild)->momentum().eta()) < m_EtaRangeHadrons) ) {
	    ++NumHadrons;
	  }
          else if (((*thisChild)->end_vertex())) {
	    //
 	    HepMC::GenVertex::particle_iterator firstChild1 =
	    (*thisChild)->end_vertex()->particles_begin(HepMC::children);
	    HepMC::GenVertex::particle_iterator endChild1 =
	    (*thisChild)->end_vertex()->particles_end(HepMC::children);
	    HepMC::GenVertex::particle_iterator thisChild1 = firstChild1;
	    //
	    for(; thisChild1 != endChild1; ++thisChild1){
	      //
	      if ( ( abs((*thisChild1)->pdg_id()) == 11 || abs((*thisChild1)->pdg_id()) == 13 )
	           && ((*thisChild1)->momentum().perp() > m_PtMinLeptons)
	           && (fabs((*thisChild1)->momentum().eta()) < m_EtaRangeLeptons) ) {
		++NumLeptons;
	      }
	      else if ( ( abs((*thisChild1)->pdg_id()) == 211 || abs((*thisChild1)->pdg_id()) == 321
                                                              || abs((*thisChild1)->pdg_id()) == 2212 )
	                && ((*thisChild1)->momentum().perp() > m_PtMinHadrons)
	                && (fabs((*thisChild1)->momentum().eta()) < m_EtaRangeHadrons) ) {
	        ++NumHadrons;
	      }
              else if (((*thisChild1)->end_vertex())) {
		//
   	        HepMC::GenVertex::particle_iterator firstChild2 =
	        (*thisChild1)->end_vertex()->particles_begin(HepMC::children);
	        HepMC::GenVertex::particle_iterator endChild2 =
	        (*thisChild1)->end_vertex()->particles_end(HepMC::children);
	        HepMC::GenVertex::particle_iterator thisChild2 = firstChild2;
		//
	        for(; thisChild2 != endChild2; ++thisChild2){
		  //
	          if ( ( abs((*thisChild2)->pdg_id()) == 11 || abs((*thisChild2)->pdg_id()) == 13 )
	               && ((*thisChild2)->momentum().perp() > m_PtMinLeptons)
	               && (fabs((*thisChild2)->momentum().eta()) < m_EtaRangeLeptons) ) {
		    ++NumLeptons;
	          }
	          else if ( ( abs((*thisChild2)->pdg_id()) == 211 || abs((*thisChild2)->pdg_id()) == 321
                                                                  || abs((*thisChild2)->pdg_id()) == 2212 )
	                    && ((*thisChild2)->momentum().perp() > m_PtMinHadrons)
	                    && (fabs((*thisChild2)->momentum().eta()) < m_EtaRangeHadrons) ) {
		    ++NumHadrons;
	          }
                  else if (((*thisChild2)->end_vertex())) {
		    //
   	            HepMC::GenVertex::particle_iterator firstChild3 =
	            (*thisChild2)->end_vertex()->particles_begin(HepMC::children);
	            HepMC::GenVertex::particle_iterator endChild3 =
	            (*thisChild2)->end_vertex()->particles_end(HepMC::children);
	            HepMC::GenVertex::particle_iterator thisChild3 = firstChild3;
		    //
	            for(; thisChild3 != endChild3; ++thisChild3){
		      //
	              if ( ( abs((*thisChild3)->pdg_id()) == 11 || abs((*thisChild3)->pdg_id()) == 13 )
	                   && ((*thisChild3)->momentum().perp() > m_PtMinLeptons)
	                   && (fabs((*thisChild3)->momentum().eta()) < m_EtaRangeLeptons) ) {
		        ++NumLeptons;
	              }
	              else if ( ( abs((*thisChild3)->pdg_id()) == 211 || abs((*thisChild3)->pdg_id()) == 321
                                                                      || abs((*thisChild3)->pdg_id()) == 2212 )
	                        && ((*thisChild3)->momentum().perp() > m_PtMinHadrons)
	                        && (fabs((*thisChild3)->momentum().eta()) < m_EtaRangeHadrons) ) {
		        ++NumHadrons;
	              }
	              else {
		        ++NumOthers;
		      }
		    } //for(; thisChild3 != endChild3; ++thisChild3){
		  }	          
                  else {
		    ++NumOthers;
		  }
		} //for(; thisChild2 != endChild2; ++thisChild2){
	      }
	      else {
	        ++NumOthers;
	      }
	    } //for(; thisChild1 != endChild1; ++thisChild1){
	  }
	  else {
	    ++NumOthers;
	  }
	} //for(; thisChild != endChild; ++thisChild){
	//
        if ( (NumLeptons + NumHadrons) >= m_NumMinTracks &&  (NumLeptons + NumHadrons) <= m_NumMaxTracks
           && NumLeptons >= m_NumMinLeptons && NumLeptons <= m_NumMaxLeptons
           && NumOthers >= m_NumMinOthers && NumOthers <= m_NumMaxOthers ) {
          setFilterPassed(true);
          return StatusCode::SUCCESS;
        }
	//
      } //for(int i=0;i<int(m_PDGParent.size());i++) 
    } //for(HepMC::GenEvent::particle_const_iterator pitr=genEvt->particles_begin();
  } //for (itr = events()->begin(); itr!=events()->end(); ++itr) {
  //
  // if we get here we have failed
  //
  setFilterPassed(false);
  return StatusCode::SUCCESS;
}
