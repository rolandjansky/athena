/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -------------------------------------------------------------
// File: GeneratorFilters/ParentChildFilter.cxx
// Description:
//
//   Allows the user to search for any given decay  Parent -> Child + X with particle status
//   The Parent/Child is required to be listed in the vector PDGParent[], StatusParent[] / PDGChild[], 
//   which compactly opens for multiple OR'ed searches, 
//   e.g. setting PDGParent = [23,24] with StatusParent = [23] and PDGChild = [11,13,15] in the jobOptions
//   accepts decay of W/Z with status=23 into any of the leptons. 
//   
//   The filter is effective for the generator which contains the particle of intermediate state. This filter is tested with Pythia8.
//   User should check if the filter works for other generator.
//
//   Furthermore, setting PDGParent = [0] / PDGChild = [0] in the jobOption removes the 
//   PDG requirement on the Parent/Child. This setting would test on 'decays' of the 
//   type  A(status 1) -> A(status 2). 
//   To mend this, Parent and Child is also required to have different PDG. 
//
//   Eta and Pt cuts are imposed on both Parent and Child. 
//
// AuthorList:         
//   T Masubuchi   April 2013
//

// Header for this module:-

#include "GeneratorFilters/ParentChildwStatusFilter.h"

// Other classes used by this class:-
#include <math.h>

//--------------------------------------------------------------------------
ParentChildwStatusFilter::ParentChildwStatusFilter(const std::string& name, 
      ISvcLocator* pSvcLocator): GenFilter(name,pSvcLocator) {
  //--------------------------------------------------------------------------    
  declareProperty("PDGParent",m_PDGParent); 
  declareProperty("StatusParent",m_StatusParent); 
  declareProperty("PtMinParent",m_PtMinParent = 0.); 
  declareProperty("PtMaxParent",m_PtMaxParent = 1e9);
  declareProperty("MassMinParent",m_MassMinParent = -1e9);
  declareProperty("MassMaxParent",m_MassMaxParent = 1e9); 
  declareProperty("EtaRangeParent",m_EtaRangeParent = 10.0); 
  declareProperty("PDGChild",m_PDGChild); 
  declareProperty("PtMinChild",m_PtMinChild = 0.);  
  declareProperty("EtaRangeChild",m_EtaRangeChild = 10.0); 

}

//--------------------------------------------------------------------------
 ParentChildwStatusFilter::~ParentChildwStatusFilter(){
//--------------------------------------------------------------------------

}

//---------------------------------------------------------------------------
StatusCode ParentChildwStatusFilter::filterInitialize() {
//---------------------------------------------------------------------------

  msg(MSG:: INFO) << "ParentChildwStatusFilter INITIALISING "  << endmsg;   

 if(m_PDGParent.size()==0) 
   msg(MSG:: ERROR) << "PDGParent[] not set " << endmsg; 
 if(m_StatusParent.size()==0) 
   msg( MSG:: ERROR) << "StatusParent[] not set " << endmsg; 
 if(m_PDGChild.size()==0) 
   msg(MSG:: ERROR) << "PDGChild[] not set " << endmsg; 

 for(int i=0;i<int(m_PDGParent.size());i++) 
   msg(MSG:: INFO) << "PDGParent["<<i<<"] = " << m_PDGParent[i] << endmsg;
 for(int i=0;i<int(m_StatusParent.size());i++) 
   msg( MSG:: INFO) << "StatusParent["<<i<<"] = " << m_StatusParent[i] << endmsg;
 msg( MSG:: INFO) << "PtMinParent = " << m_PtMinParent << endmsg;
 msg(MSG:: INFO) << "PtMaxParent = " << m_PtMaxParent << endmsg;
 msg(MSG:: INFO) << "MassMinParent = " << m_MassMinParent << endmsg;
 msg( MSG:: INFO) << "MassMaxParent = " << m_MassMaxParent << endmsg;
 msg( MSG:: INFO) << "EtaRangeParent = " << m_EtaRangeParent << endmsg;
 for(int i=0;i<int(m_PDGChild.size());i++) 
 msg( MSG:: INFO) << "PDGChild["<<i<<"] = " << m_PDGChild[i] << endmsg;
 msg( MSG:: INFO) << "PtMinChild = " << m_PtMinChild << endmsg;
 msg( MSG:: INFO) << "EtaRangeChild = " << m_EtaRangeChild << endmsg;
 return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------------
StatusCode ParentChildwStatusFilter::filterFinalize() {
//---------------------------------------------------------------------------
 return StatusCode::SUCCESS;
}


//---------------------------------------------------------------------------
StatusCode ParentChildwStatusFilter::filterEvent() {
//---------------------------------------------------------------------------

// Loop over all events in McEventCollection 

  msg(MSG:: INFO) << " ParentChildwStatusFilter filtering "  << endmsg;  
  McEventCollection::const_iterator itr;
  for (itr = events()->begin(); itr != events()->end(); ++itr) {
    // Loop over all particles in the event
    const HepMC::GenEvent* genEvt = (*itr);
    for(auto pitr: *genEvt){
      // Parent
      int okPDGParent=0;
      int okStatusParent=0;
      for(int i=0;i<int(m_PDGParent.size());i++) 
	if(std::abs(pitr->pdg_id()) == m_PDGParent[i]) okPDGParent=1;
      for(int i=0;i<int(m_StatusParent.size());i++)
	if(std::abs(pitr->status()) == m_StatusParent[i]) okStatusParent=1;
      if( ( (m_PDGParent[0] == 0) || (okPDGParent && okStatusParent))
	  && (pitr->momentum().perp() >= m_PtMinParent)
          && (pitr->momentum().perp() < m_PtMaxParent)
          && (pitr->momentum().m() >= m_MassMinParent)
          && (pitr->momentum().m() < m_MassMaxParent) 
	  && (std::abs(pitr->momentum().eta()) < m_EtaRangeParent) ) {
	//Check if has end_vertex (skips initial protons)
	if(!(pitr->end_vertex())) continue; 
	// Child
	for(auto thisChild: *(pitr->end_vertex())){
	  int okPDGChild=0;
	  for(int i=0;i<int(m_PDGChild.size());i++) 
	    if(std::abs(thisChild->pdg_id()) == m_PDGChild[i]) okPDGChild=1;
	  if( (thisChild->pdg_id() != pitr->pdg_id() ) 
	      && ( (m_PDGChild[0] == 0) || okPDGChild )
	      && (thisChild->momentum().perp() > m_PtMinChild)
	      && (std::abs(thisChild->momentum().eta()) < m_EtaRangeChild) ) {
	    // appropriate Child found
	    return StatusCode::SUCCESS;
	  }
	}
      }	
    }
  }
  // if we get here we have failed
  setFilterPassed(false);
  return StatusCode::SUCCESS;
}


