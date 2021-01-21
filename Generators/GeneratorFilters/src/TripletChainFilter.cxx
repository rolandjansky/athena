/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration 
*/
// -------------------------------------------------------------
// File: GeneratorFilters/TripletChainFilter.cxx
// Description:
//   Based on MassFilter.cxx
//
//   Allows the user to search for the events where 2 particles of user defined type that
// will pass if they have p_t, eta, and invariant mass in the specified range. 
// If there are more than 1 combinations which pass the eta and pT
// selection in the event, take the pair which has the largest invariant mass and then
// apply the invariant mass selection in order to avoid duplication.(Original MassFilter 
// takes the events where at least one pair passes the invariant mass selection.)
// default is 2 muons with p_t>=10 GeV, |eta| <= 10, and Mass between 0-14TeV.
// Always allow same sign events.
//
// AuthorList:
//         
//  Jeremy Love and Akimasa Ishikawa Sep 2010 

// Header for this module:-

#include "GeneratorFilters/TripletChainFilter.h"

// Framework Related Headers:-
#include "GaudiKernel/MsgStream.h"


// Other classes used by this class:-
#include <math.h>

using HepMC::GenVertex;
using HepMC::GenParticle;

//--------------------------------------------------------------------------
TripletChainFilter::TripletChainFilter(const std::string& name, 
                                       ISvcLocator* pSvcLocator): GenFilter(name,pSvcLocator) {
  //----------------------------    
  declareProperty("NTriplet",m_NTriplet=1);
  declareProperty("PdgId1",m_PdgId1=13);
  declareProperty("PdgId2",m_PdgId2=-13);
  declareProperty("PdgId3",m_PdgId3=13);
  declareProperty("NStep1",m_NStep1=1);
  declareProperty("NStep2",m_NStep2=1);
  declareProperty("NStep3",m_NStep3=1);
  declareProperty("PtMin1",m_PtMin1 = 2500.);  
  declareProperty("PtMin2",m_PtMin2 = 2500.);  
  declareProperty("PtMin3",m_PtMin3 = 2500.);  
  declareProperty("EtaMax1",m_EtaMax1 = 4.5); 
  declareProperty("EtaMax2",m_EtaMax2 = 4.5);
  declareProperty("EtaMax3",m_EtaMax3 = 4.5);
  declareProperty("TripletPdgId",m_TripletPdgId = 15 );  
  declareProperty("TripletPtMin",m_TripletPtMin = 10000.);  
  declareProperty("TripletEtaMax",m_TripletEtaMax = 102.5);  
  declareProperty("TripletMassMin",m_TripletMassMin = 0.);  
  declareProperty("TripletMassMax",m_TripletMassMax = 100000.);  
  declareProperty("ThirdParentPdgId",m_ThirdParentPdgId = 0 );  
  declareProperty("ThirdParentPtMin",m_ThirdParentPtMin = 0);  
  declareProperty("ThirdParentEtaMax",m_ThirdParentEtaMax = 102.5);  
  declareProperty("DoubletPdgId",m_DoubletPdgId = 0 );  
  declareProperty("DoubletPtMin",m_DoubletPtMin = 0);  
  declareProperty("DoubletEtaMax",m_DoubletEtaMax = 102.5);  
  declareProperty("DoubletMassMin",m_DoubletMassMin = 0.);  
  declareProperty("DoubletMassMax",m_DoubletMassMax = 100000.);  
  declareProperty("ParentPdgId",m_ParentPdgId = 0 );  
  declareProperty("ParentPtMin",m_ParentPtMin = 0);  
  declareProperty("ParentEtaMax",m_ParentEtaMax = 102.5);  
  declareProperty("ParentMassMin",m_ParentMassMin = 0.);  
  declareProperty("ParentMassMax",m_ParentMassMax = 100000.);  
}

//--------------------------------------------------------------------------
TripletChainFilter::~TripletChainFilter(){
  //--------------------------------------------------------------------------

}

//---------------------------------------------------------------------------
StatusCode TripletChainFilter::filterInitialize() {
  //---------------------------------------------------------------------------
  MsgStream log(messageService(), name());

  m_SamePdgId12   = ( abs(m_PdgId1)==abs(m_PdgId2) );
  m_SamePdgId13   = ( abs(m_PdgId1)==abs(m_PdgId3) );
  m_SamePdgId23   = ( abs(m_PdgId2)==abs(m_PdgId3) );
  m_SamePdgId123  = ( m_SamePdgId12 && m_SamePdgId13 && m_SamePdgId23 );

  log << MSG::INFO
      << "TripletChainFilter setup"  << endreq
      << "       NTriplet " << m_NTriplet << endreq
      << "         PdgId1 " << m_PdgId1 << endreq
      << "         PdgId2 " << m_PdgId2 << endreq
      << "         PdgId3 " << m_PdgId3 << endreq
      << "         NStep1 " << m_NStep1 << endreq
      << "         NStep2 " << m_NStep2 << endreq
      << "         NStep3 " << m_NStep3 << endreq
      << "         PtMin1 " << m_PtMin1 << " MeV" << endreq
      << "         PtMin2 " << m_PtMin2 << " MeV" << endreq
      << "         PtMin3 " << m_PtMin3 << " MeV" << endreq
      << "        EtaMax1 " << m_EtaMax1 << endreq
      << "        EtaMax2 " << m_EtaMax2 << endreq
      << "        EtaMax3 " << m_EtaMax3 << endreq
      << "    SamePdgId12 " << m_SamePdgId12 << endreq
      << "    SamePdgId13 " << m_SamePdgId13 << endreq
      << "    SamePdgId23 " << m_SamePdgId23 << endreq
      << "   TripletPdgId " << m_TripletPdgId << endreq
      << "   TripletPtMin " << m_TripletPtMin << " MeV" << endreq
      << "  TripletEtaMax " << m_TripletEtaMax << endreq
      << " TripletMassMin " << m_TripletMassMin << " MeV" << endreq
      << " TripletMassMax " << m_TripletMassMax << " MeV" << endreq
      << "   DoubletPdgId " << m_DoubletPdgId << endreq
      << "   DoubletPtMin " << m_DoubletPtMin << " MeV" << endreq
      << "  DoubletEtaMax " << m_DoubletEtaMax << endreq
      << " DoubletMassMin " << m_DoubletMassMin << " MeV" << endreq
      << " DoubletMassMax " << m_DoubletMassMax << " MeV" << endreq
      << " ThirdParentPdgId " << m_ThirdParentPdgId << endreq
      << " ThirdParentPtMin " << m_ThirdParentPtMin << " MeV" << endreq
      << " ThirdParentEtaMax " << m_ThirdParentEtaMax << endreq
      << "    ParentPdgId " << m_ParentPdgId << endreq
      << "    ParentPtMin " << m_ParentPtMin << " MeV" << endreq
      << "   ParentEtaMax " << m_ParentEtaMax << endreq
      << "  ParentMassMin " << m_ParentMassMin << " MeV" << endreq
      << "  ParentMassMax " << m_ParentMassMax << " MeV" << endreq;

  return StatusCode::SUCCESS;
 }

//---------------------------------------------------------------------------
StatusCode TripletChainFilter::filterFinalize() {
  //---------------------------------------------------------------------------
  return StatusCode::SUCCESS;
}


//---------------------------------------------------------------------------
StatusCode TripletChainFilter::filterEvent() {
//---------------------------------------------------------------------------

  MsgStream log(messageService(), name());

  bool isNoStep = ( m_NStep1==0 && m_NStep2==0 && m_NStep3==0 );
  bool isOneStep = ( m_NStep1==1 && m_NStep2==1 && m_NStep3==1 );
  int ntriplet  = 0;
  int nloose    = 0;

  // Loop over all events in McEventCollection
  McEventCollection::const_iterator itr;
  std::vector< HepMC::GenParticle* > tripvec; // for veto the same combinations

  for (McEventCollection::const_iterator itr = events_const()->begin(); itr != events_const()->end(); ++itr) {

    // Loop over all particles in the event
    const HepMC::GenEvent* genEvt = (*itr);
    
    // 1st particle
    for(HepMC::GenEvent::particle_const_iterator pitr1 = genEvt->particles_begin();
	    pitr1!=genEvt->particles_end(); ++pitr1 ){
      
      if( (*pitr1)->pdg_id() != m_PdgId1 ) continue;
      // parents search
      if( m_NStep1>0 ){
        HepMC::GenVertex* begvx1 = (*pitr1)->production_vertex();
        if( !begvx1 )continue;
        HepMC::GenVertex::particle_iterator ppitr1 = begvx1->particles_begin(HepMC::parents);
        if( !(*ppitr1) ) continue;
        if( m_NStep1==1 && (*ppitr1)->pdg_id()!=m_TripletPdgId ) continue;
        if( m_NStep1>1 ){
          HepMC::GenVertex* gbegvx1 = (*ppitr1)->production_vertex();
          if( !gbegvx1 )continue;
          HepMC::GenVertex::particle_iterator gppitr1 = gbegvx1->particles_begin(HepMC::parents);
          if( !(*gppitr1) ) continue;
          if( m_NStep1==2 && (*ppitr1)->pdg_id()!=m_DoubletPdgId ) continue;
          if( m_NStep1==2 && (*gppitr1)->pdg_id()!=m_TripletPdgId ) continue;
        }
      }
      // 2nd particle
      for(HepMC::GenEvent::particle_const_iterator pitr2 = genEvt->particles_begin(); 
          pitr2!=genEvt->particles_end(); ++pitr2 ){

        if( pitr1 == pitr2 ) continue;//if the pointers are the same. 
        if( (*pitr2)->pdg_id() !=m_PdgId2 ) continue;
        // parents search
        if( m_NStep2>0 ){
          HepMC::GenVertex* begvx2 = (*pitr2)->production_vertex();
          if( !begvx2 )continue;
          HepMC::GenVertex::particle_iterator ppitr2 = begvx2->particles_begin(HepMC::parents);
          if( !(*ppitr2) ) continue;
          if( m_NStep2==1 && (*ppitr2)->pdg_id()!=m_TripletPdgId ) continue;
          if( m_NStep2>1 ){
            HepMC::GenVertex* gbegvx2 = (*ppitr2)->production_vertex();
            if( !gbegvx2 )continue;
            HepMC::GenVertex::particle_iterator gppitr2 = gbegvx2->particles_begin(HepMC::parents);
            if( !(*gppitr2) ) continue;
            if( m_NStep2==2 && (*ppitr2)->pdg_id()!=m_DoubletPdgId ) continue;
            if( m_NStep2==2 && (*gppitr2)->pdg_id()!=m_TripletPdgId ) continue;
          }
        }
        // 3rd particle 
        for(HepMC::GenEvent::particle_const_iterator pitr3 = genEvt->particles_begin(); 
            pitr3!=genEvt->particles_end(); ++pitr3 ){

          if( pitr1 == pitr3 || pitr2 == pitr3 ) continue;//if the pointers are the same. 
          if( (*pitr3)->pdg_id() != m_PdgId3 ) continue;
          // parents search
          if( m_NStep3>0 ){
            HepMC::GenVertex* begvx3 = (*pitr3)->production_vertex();
            if( !begvx3 )continue;
            HepMC::GenVertex::particle_iterator ppitr3 = begvx3->particles_begin(HepMC::parents);
            if( !(*ppitr3) ) continue;
            if( m_NStep3==1 && (*ppitr3)->pdg_id()!=m_TripletPdgId ) continue;
            if( m_NStep3>1 ){
	      HepMC::GenVertex* gbegvx3 = (*ppitr3)->production_vertex();
	      if( !gbegvx3 )continue;
              HepMC::GenVertex::particle_iterator gppitr3 = gbegvx3->particles_begin(HepMC::parents);
              if( !(*gppitr3) ) continue;
              if( m_NStep3==2 && (*ppitr3)->pdg_id()!=m_DoubletPdgId && (*ppitr3)->pdg_id()!=m_ThirdParentPdgId ) continue;
              if( m_NStep3==2 && (*gppitr3)->pdg_id()!=m_TripletPdgId ) continue;
            }
          }
          // from the same vertex
          HepMC::GenParticle* tmp_triplet=0;
          HepMC::GenParticle* tmp_doublet=0;
	  HepMC::GenParticle* tmp_third_parent = 0;
          if( isNoStep ) {
            HepMC::GenVertex* begvx1 = (*pitr1)->production_vertex();
            HepMC::GenVertex* begvx2 = (*pitr2)->production_vertex();
            HepMC::GenVertex* begvx3 = (*pitr3)->production_vertex();
            if( !begvx1 || !begvx2 || !begvx3 ) continue;
          }
          else if( isOneStep ) {
            HepMC::GenVertex* begvx1 = (*pitr1)->production_vertex();
            HepMC::GenVertex* begvx2 = (*pitr2)->production_vertex();
            HepMC::GenVertex* begvx3 = (*pitr3)->production_vertex();
            if( begvx1!=begvx2 || begvx1!=begvx3 || begvx2!=begvx3 ) continue;
            tmp_triplet = *( begvx1->particles_begin(HepMC::parents) );     
          }
          else if( m_NStep1==2 && m_NStep2==2 && m_NStep3==2) { 
            HepMC::GenVertex* begvx1 = (*pitr1)->production_vertex();
            HepMC::GenVertex* begvx2 = (*pitr2)->production_vertex();
            HepMC::GenVertex* begvx3 = (*pitr3)->production_vertex();
            if( begvx1!=begvx2 ) continue;
            HepMC::GenVertex::particle_iterator ppitr = begvx1->particles_begin(HepMC::parents);
            HepMC::GenVertex* gbegvx = (*ppitr)->production_vertex();
	    HepMC::GenVertex* gbegvx2 = (*(begvx3->particles_begin(HepMC::parents)))->production_vertex();
            if( gbegvx!=gbegvx2 ) continue;
            tmp_triplet = *( gbegvx->particles_begin(HepMC::parents) );     
            tmp_doublet = *( ppitr );
	    tmp_third_parent = *(begvx3->particles_begin(HepMC::parents));
          }
          else if( m_NStep1==2 && m_NStep2==2 ) { 
            HepMC::GenVertex* begvx1 = (*pitr1)->production_vertex();
            HepMC::GenVertex* begvx2 = (*pitr2)->production_vertex();
            HepMC::GenVertex* begvx3 = (*pitr3)->production_vertex();
            if( begvx1!=begvx2 ) continue;
            HepMC::GenVertex::particle_iterator ppitr = begvx1->particles_begin(HepMC::parents);
            HepMC::GenVertex* gbegvx = (*ppitr)->production_vertex();
            if( gbegvx!=begvx3 ) continue;
            tmp_triplet = *( begvx3->particles_begin(HepMC::parents) );     
            tmp_doublet = *( ppitr );	   
          }
          else if( m_NStep1==2 && m_NStep3==2 ) {
            HepMC::GenVertex* begvx1 = (*pitr1)->production_vertex();
            HepMC::GenVertex* begvx2 = (*pitr2)->production_vertex();
            HepMC::GenVertex* begvx3 = (*pitr3)->production_vertex();
            if( begvx1!=begvx3 ) continue;
            HepMC::GenVertex::particle_iterator ppitr = begvx1->particles_begin(HepMC::parents);
            HepMC::GenVertex* gbegvx = (*ppitr)->production_vertex();
            if( gbegvx!=begvx2 ) continue;
            tmp_triplet = *( begvx2->particles_begin(HepMC::parents) );     
            tmp_doublet = *( ppitr );     
          }
          else if( m_NStep2==2 && m_NStep3==2 ) {
            HepMC::GenVertex* begvx1 = (*pitr1)->production_vertex();
            HepMC::GenVertex* begvx2 = (*pitr2)->production_vertex();
            HepMC::GenVertex* begvx3 = (*pitr3)->production_vertex();
            if( begvx2!=begvx3 ) continue;
            HepMC::GenVertex::particle_iterator ppitr = begvx2->particles_begin(HepMC::parents);
            HepMC::GenVertex* gbegvx = (*ppitr)->production_vertex();
            if( gbegvx!=begvx1 ) continue;
            tmp_triplet = *( begvx1->particles_begin(HepMC::parents) );     
            tmp_doublet = *( ppitr );     
          }
          else continue;
          
          // duplication veto
          if( tmp_triplet ) {
            const int n_triplet = tripvec.size();
            bool isDuplicated = false;
            for( int i=0; i<n_triplet; i++ ) {
              if( tripvec[i] == tmp_triplet ) isDuplicated = true;
            }
            if( isDuplicated ) continue;
            tripvec.push_back( tmp_triplet );
          }
          
          nloose++;
          // too many loose event veto
          if( m_NTriplet>=0 && nloose>(m_NTriplet) ) {
            log << MSG::INFO << "= TripletChainFilter get too many number of triplets.(FAIL)" << endreq;;
            setFilterPassed(false);
            return StatusCode::SUCCESS;
          }
         
          // loose event check
          log << MSG::INFO << "=== 1st particle ===" << endreq
              << "= PID: " << (*pitr1)->pdg_id()
              << ", Pt: " << (*pitr1)->momentum().perp()
              << ", Eta: " << (*pitr1)->momentum().pseudoRapidity() << endreq
              << "=== 2nd particle === " << endreq
              << "= PID: " << (*pitr2)->pdg_id()
              << ", Pt: " << (*pitr2)->momentum().perp()
              << ", Eta: " << (*pitr2)->momentum().pseudoRapidity() << endreq
              << "=== 3rd particle === " << endreq
              << "= PID: " << (*pitr3)->pdg_id()
              << ", Pt: " << (*pitr3)->momentum().perp()
              << ", Eta: " << (*pitr3)->momentum().pseudoRapidity() << endreq;
          
          // selection for tracks
          if( ( (*pitr1)->pdg_id() != m_PdgId1 ) || //PDG ID selection
              (*pitr1)->status() != 1    //status of the particle 
            ) continue;
          if( ( (*pitr2)->pdg_id() != m_PdgId2 ) || //PDG ID selection
              (*pitr2)->status() != 1    //status of the particle 
            ) continue;
	  if( ( (*pitr3)->pdg_id() != m_PdgId3 ) || //PDG ID selection
              (*pitr3)->status() != 1    //status of the particle 
            ) continue;
	  // all pdg IDs are defferent
          if( !m_SamePdgId12 && !m_SamePdgId13 && !m_SamePdgId23 ) { 
            if( ! ( (*pitr1)->momentum().perp() > m_PtMin1 ) || // pT cut
                ! ( fabs( (*pitr1)->momentum().pseudoRapidity() ) < m_EtaMax1 ) // eta cut
              ) continue;
            if( ! ( (*pitr2)->momentum().perp() > m_PtMin2 ) || // pT cut
                ! ( fabs( (*pitr2)->momentum().pseudoRapidity() ) < m_EtaMax2 ) // eta cut
              ) continue;
            if( ! ( (*pitr3)->momentum().perp() > m_PtMin3 ) || // pT cut
                ! ( fabs( (*pitr3)->momentum().pseudoRapidity() ) < m_EtaMax3 ) // eta cut
              ) continue;
          }
          // first 2 pdg IDs are same
          if ( m_SamePdgId12 && !m_SamePdgId13 && !m_SamePdgId23 ) {
            bool ORselection = 
              ( ( (*pitr1)->momentum().perp() > m_PtMin1  && // pT cut
                  fabs( (*pitr1)->momentum().pseudoRapidity() ) < m_EtaMax1 ) && // eta cut
                ( (*pitr2)->momentum().perp() > m_PtMin2  && // pT cut
                  fabs( (*pitr2)->momentum().pseudoRapidity() ) < m_EtaMax2 ) ) ||// eta cut
              ( ( (*pitr1)->momentum().perp() > m_PtMin2  && // pT cut
                  fabs( (*pitr1)->momentum().pseudoRapidity() ) < m_EtaMax2 ) && // eta cut
                ( (*pitr2)->momentum().perp() > m_PtMin1  && // pT cut
                  fabs( (*pitr2)->momentum().pseudoRapidity() ) < m_EtaMax1 ) );// eta cut
            if( !ORselection ) continue;
            if( ! ( (*pitr3)->momentum().perp() > m_PtMin3 ) || // pT cut
                ! ( fabs( (*pitr3)->momentum().pseudoRapidity() ) < m_EtaMax3 ) // eta cut
              ) continue;
	  }
          // first all 3 pdg IDs are same
          if ( m_SamePdgId123 ) {
            bool ORselection = 
              ( ( (*pitr1)->momentum().perp() > m_PtMin1  &&
                  fabs( (*pitr1)->momentum().pseudoRapidity() ) < m_EtaMax1 ) &&
                ( (*pitr2)->momentum().perp() > m_PtMin2  &&
                  fabs( (*pitr2)->momentum().pseudoRapidity() ) < m_EtaMax2 ) && 
                ( (*pitr3)->momentum().perp() > m_PtMin3  &&
                  fabs( (*pitr3)->momentum().pseudoRapidity() ) < m_EtaMax3 ) ) || // 123
              ( ( (*pitr1)->momentum().perp() > m_PtMin1  &&
                  fabs( (*pitr1)->momentum().pseudoRapidity() ) < m_EtaMax1 ) && 
                ( (*pitr2)->momentum().perp() > m_PtMin3  &&
                  fabs( (*pitr2)->momentum().pseudoRapidity() ) < m_EtaMax3 ) &&
                ( (*pitr3)->momentum().perp() > m_PtMin2  &&
                  fabs( (*pitr3)->momentum().pseudoRapidity() ) < m_EtaMax2 ) ) || // 132
              ( ( (*pitr1)->momentum().perp() > m_PtMin2  && 
                  fabs( (*pitr1)->momentum().pseudoRapidity() ) < m_EtaMax2 ) && 
                ( (*pitr2)->momentum().perp() > m_PtMin1  && 
                  fabs( (*pitr2)->momentum().pseudoRapidity() ) < m_EtaMax1 ) &&
                ( (*pitr3)->momentum().perp() > m_PtMin3  && 
                  fabs( (*pitr3)->momentum().pseudoRapidity() ) < m_EtaMax3 ) ) || // 213
              ( ( (*pitr1)->momentum().perp() > m_PtMin2  &&
                  fabs( (*pitr1)->momentum().pseudoRapidity() ) < m_EtaMax2 ) && 
                ( (*pitr2)->momentum().perp() > m_PtMin3  &&
                  fabs( (*pitr2)->momentum().pseudoRapidity() ) < m_EtaMax3 ) &&
                ( (*pitr3)->momentum().perp() > m_PtMin1  &&
                  fabs( (*pitr3)->momentum().pseudoRapidity() ) < m_EtaMax1 ) ) || // 231
              ( ( (*pitr1)->momentum().perp() > m_PtMin3  &&
                  fabs( (*pitr1)->momentum().pseudoRapidity() ) < m_EtaMax3 ) && 
                ( (*pitr2)->momentum().perp() > m_PtMin1  &&
                  fabs( (*pitr2)->momentum().pseudoRapidity() ) < m_EtaMax1 ) && 
                ( (*pitr3)->momentum().perp() > m_PtMin2  && 
                  fabs( (*pitr3)->momentum().pseudoRapidity() ) < m_EtaMax2 ) ) || //312
              ( ( (*pitr1)->momentum().perp() > m_PtMin3  && 
                  fabs( (*pitr1)->momentum().pseudoRapidity() ) < m_EtaMax3 ) && 
                ( (*pitr2)->momentum().perp() > m_PtMin2  &&
                  fabs( (*pitr2)->momentum().pseudoRapidity() ) < m_EtaMax2 ) &&
                ( (*pitr3)->momentum().perp() > m_PtMin1  && 
                  fabs( (*pitr3)->momentum().pseudoRapidity() ) < m_EtaMax1 ) );//321
            if( !ORselection ) continue;
          }
          
          if( isNoStep ) {
	    // doublet variable selection
            if( m_NStep1==2 && m_NStep2==2 ) {
              HepMC::FourVector doublet(
                (*pitr1)->momentum().px() + (*pitr2)->momentum().px(),
                (*pitr1)->momentum().py() + (*pitr2)->momentum().py(),
                (*pitr1)->momentum().pz() + (*pitr2)->momentum().pz(),
                (*pitr1)->momentum().e() + (*pitr2)->momentum().e()
              );
              double doubletPt    = doublet.perp();
              double doubletEta   = doublet.eta();
              double doubletMass  = doublet.m();
              if( m_DoubletPtMin>doubletPt ) continue;
              if( fabs(doubletEta)>m_DoubletEtaMax ) continue;
              if( doubletMass<m_DoubletMassMin || m_DoubletMassMax<doubletMass ) continue;
	      log << MSG::INFO << "pass IsNoStep" << endreq;
            }
            // triplet variable selection
            HepMC::FourVector triplet(
              (*pitr1)->momentum().px() + (*pitr2)->momentum().px() + (*pitr3)->momentum().px(),
              (*pitr1)->momentum().py() + (*pitr2)->momentum().py() + (*pitr3)->momentum().py(),
              (*pitr1)->momentum().pz() + (*pitr2)->momentum().pz() + (*pitr3)->momentum().pz(),
              (*pitr1)->momentum().e() + (*pitr2)->momentum().e() + (*pitr3)->momentum().e()
            );
            double tripletPt    = triplet.perp();
            double tripletEta   = triplet.eta();
            double tripletMass  = triplet.m();
            log << MSG::INFO << "=== loose triplet ===" << endreq
                << "= PID: no" 
                << ", Pt: " << tripletPt
                << ", Eta: " << tripletEta
                << ", Mass: " << tripletMass << endreq;
            if( (*pitr1)->production_vertex() ) {
              if( *((*pitr1)->production_vertex()->particles_begin(HepMC::parents)) ) 
              log << MSG::INFO << "= Par1 PID: " << (*((*pitr1)->production_vertex()->particles_begin(HepMC::parents)))->pdg_id() << endreq;
            }
            if( (*pitr2)->production_vertex() ) {
              if( *((*pitr2)->production_vertex()->particles_begin(HepMC::parents)) ) 
              log << MSG::INFO << "= Par2 PID: " << (*((*pitr2)->production_vertex()->particles_begin(HepMC::parents)))->pdg_id() << endreq;
            }
            if( (*pitr3)->production_vertex() ) {
              if( *((*pitr3)->production_vertex()->particles_begin(HepMC::parents)) ) 
              log << MSG::INFO << "= Par3 PID: " << (*((*pitr3)->production_vertex()->particles_begin(HepMC::parents)))->pdg_id() << endreq;
            }
            if( m_TripletPtMin>tripletPt ) continue;
            if( fabs(tripletEta)>m_TripletEtaMax ) continue;
            if( tripletMass<m_TripletMassMin || m_TripletMassMax<tripletMass ) continue;
          }
          else {
            // doublet variable selection
            if( tmp_doublet ) {
              log << MSG::INFO << "=== loose doublet ===" << endreq
                  << "= PID: " << tmp_doublet->pdg_id()
                  << ", Pt: " << tmp_doublet->momentum().perp()
                  << ", Eta: " << tmp_doublet->momentum().pseudoRapidity()
                  << ", ParPID: " << tmp_doublet->pdg_id()
                  << ", ParPt: " << tmp_doublet->momentum().perp() << endreq;
              if( tmp_doublet->momentum().perp() < m_DoubletPtMin ) continue;
              if( fabs( tmp_doublet->momentum().pseudoRapidity() ) > m_DoubletEtaMax ) continue;
              if( tmp_doublet->momentum().m() < m_DoubletMassMin || m_DoubletMassMax<tmp_doublet->momentum().m() ) continue;
            }
	    if(tmp_third_parent){
	       log << MSG::INFO << "=== third parent ===" << endreq
                  << "= PID: " << tmp_third_parent->pdg_id()
                  << ", Pt: " << tmp_third_parent->momentum().perp()
                  << ", Eta: " << tmp_third_parent->momentum().pseudoRapidity()
                  << ", ParPID: " << tmp_third_parent->pdg_id()
                  << ", ParPt: " << tmp_third_parent->momentum().perp() << endreq;
              if( tmp_third_parent->momentum().perp() < m_ThirdParentPtMin ) continue;
              if( fabs( tmp_third_parent->momentum().pseudoRapidity() ) > m_ThirdParentEtaMax ) continue;
	    }
            // triplet variable selection
            if( tmp_triplet ) {
              log << MSG::INFO << "=== loose triplet ===" << endreq
                  << "= PID: " << tmp_triplet->pdg_id()
                  << ", Pt: " << tmp_triplet->momentum().perp()
                  << ", Eta: " << tmp_triplet->momentum().pseudoRapidity()  << endreq;
              if( tmp_triplet->momentum().perp() < m_TripletPtMin ) continue;
              if( fabs( tmp_triplet->momentum().pseudoRapidity() ) > m_TripletEtaMax ) continue;
              if( tmp_triplet->momentum().m() < m_TripletMassMin || m_TripletMassMax<tmp_triplet->momentum().m() ) continue;
              
              // additional source limitation
              if( m_ParentPdgId!=0 ) {
                HepMC::GenVertex* genvx = tmp_triplet->production_vertex();
                if( !genvx ) continue;
                HepMC::GenVertex::particle_iterator source = genvx->particles_begin(HepMC::parents);
                if( !(*source) ) continue;
                log << MSG::INFO << "=== loose triplet source===" << endreq
                    << "= ParPID: " << (*source)->pdg_id()
                    << ", ParPt: " << (*source)->momentum().perp() << endreq;
                if( (*source)->pdg_id()!=m_ParentPdgId ) continue;
                if( (*source)->momentum().perp() < m_ParentPtMin ) continue;
                if( fabs( (*source)->momentum().pseudoRapidity() ) > m_ParentEtaMax ) continue;
                if( (*source)->momentum().m() < m_ParentMassMin || m_ParentMassMax<(*source)->momentum().m() ) continue;
              }
            }
          }
          ntriplet++;

          if( m_NTriplet<0 && ntriplet>=-1*(m_NTriplet) ) { 
            log << MSG::INFO << "= TripletChainFilter got required number of triplets.(SUCCESS)" << endreq;;
            return StatusCode::SUCCESS;
          }
        }//pitr3
      }//pitr2
    }//pitr1
  }//McEventCollection
  
  log << MSG::INFO << "= nsignal: " << ntriplet << ", nloose: " << nloose  << endreq;;
  // success!
  if( m_NTriplet>=0 && ntriplet==m_NTriplet ) {
    log << MSG::INFO << "= TripletChainFilter got required number of triplets.(SUCCESS)" << endreq;;
    return StatusCode::SUCCESS;
  }

  //Haven't found anything...
  log << MSG::INFO << "= TripletChainFilter got no triplet(FAIL)" << endreq;;
  setFilterPassed(false);
  return StatusCode::SUCCESS;
}
