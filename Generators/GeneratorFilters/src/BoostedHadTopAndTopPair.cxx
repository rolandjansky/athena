/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// GeneratorFilters/BoostedHadTopAndTopPair
//
// Allows the user to search for both top had pT and top pair pT in a given range, in tt events (thad is the hadronically decaying top with the highest momentum in each even).
//
// Author:
// Eloi Le Quilleuc    2014

#include "GeneratorFilters/BoostedHadTopAndTopPair.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include <iostream>
#include <cmath>

BoostedHadTopAndTopPair::BoostedHadTopAndTopPair(const std::string& name, ISvcLocator* pSvcLocator)
  : GenFilter(name,pSvcLocator) 
{
  // pT min et pT max :
  declareProperty("tHadPtMin",     m_tHadPtMin  =               0.0);
  declareProperty("tHadPtMax",     m_tHadPtMax  =         4000000.0);
  declareProperty("tPairPtMin",    m_tPairPtMin =               0.0);
  declareProperty("tPairPtMax",    m_tPairPtMax =         4000000.0);
  declareProperty("cutPtOf",       m_cutPtOf    =                 0);
}


StatusCode BoostedHadTopAndTopPair::filterEvent() {
  MsgStream log(messageService(), name());

  // if true, the event pass the filter :
  bool pass        = false;
  bool passTopHad  = false;
  bool passTopPair = false;

  HepMC::FourVector topListMomentum(0,0,0,0);
  HepMC::FourVector topbListMomentum(0,0,0,0);
  HepMC::FourVector topChildrenMomentum(0,0,0,0);
  HepMC::FourVector topbChildrenMomentum(0,0,0,0);

  double pTHadTopList = -1;
  double pTHadTopChildren = -1;
  const  HepMC::FourVector b(0,0,0,0);

  // Loop over all events in McEventCollection and extract the top pt
  McEventCollection::const_iterator itr;
  for (itr = events_const()->begin(); itr != events_const()->end(); ++itr) {
    const HepMC::GenEvent* genEvt = *itr;

    
    // Loop over all truth particles in the event
    HepMC::GenEvent::particle_const_iterator pitr;
    for (pitr = genEvt->particles_begin(); pitr != genEvt->particles_end(); ++pitr ) {

      const HepMC::GenParticle* part = (*pitr);
      int pdgId = part->pdg_id();
  
      // pdgId t quark = 6
      if ( pdgId == 6 && isFinalParticle(part) ){
        if ( part->momentum().perp() > topListMomentum.perp() )  topListMomentum  = part->momentum();
      }
  
      if ( pdgId == -6 && isFinalParticle(part) ){
        if ( part->momentum().perp() > topbListMomentum.perp() ) topbListMomentum = part->momentum();
      }
  
      // pdgId W boson = 24
      if ( abs(pdgId) != 24 || !isFinalParticle(part) ) continue; 
  
      if (isFromTop(part)){
        if (pdgId > 0) topChildrenMomentum.set(part->momentum().px() + momentumBofW(part).px(), part->momentum().py() + momentumBofW(part).py(), part->momentum().pz() + momentumBofW(part).pz(), part->momentum().e() + momentumBofW(part).e());
        else topbChildrenMomentum.set(part->momentum().px() + momentumBofW(part).px(), part->momentum().py() + momentumBofW(part).py(), part->momentum().pz() + momentumBofW(part).pz(), part->momentum().e() + momentumBofW(part).e());

        if (isHadronic(part)){
          double pT = sqrt( pow( part->momentum().px() + momentumBofW(part).px(),2) + pow( part->momentum().py() + momentumBofW(part).py(),2)); 
          if (pT > pTHadTopChildren){
            pTHadTopChildren = pT;
            if (pdgId > 0) pTHadTopList = topListMomentum.perp();
            else pTHadTopList = topbListMomentum.perp();
          }
        }
      }
    } // particle loop
  } // event loop

  double pTPairList = sqrt( pow( topListMomentum.px() + topbListMomentum.px() , 2 ) + pow( topListMomentum.py() + topbListMomentum.py() , 2 )); 
  double pTPairChildren = sqrt( pow( topChildrenMomentum.px() + topbChildrenMomentum.px() , 2 ) + pow( topChildrenMomentum.py() + topbChildrenMomentum.py() , 2 )); 

  if (m_cutPtOf == 0){ // cut on the pT of top on the truth list
    if (pTHadTopList   >= m_tHadPtMin    &&   pTHadTopList   < m_tHadPtMax  )  passTopHad  = true;
    if (pTPairList >= m_tPairPtMin   &&   pTPairList < m_tPairPtMax )  passTopPair = true;
  }else if( m_cutPtOf == 1){ // cut on the pT of top decay products (b, q, qbar') on the truth list
    if (pTHadTopChildren   >= m_tHadPtMin    &&   pTHadTopChildren   < m_tHadPtMax  )  passTopHad  = true;
    if (pTPairChildren >= m_tPairPtMin   &&   pTPairChildren < m_tPairPtMax )  passTopPair = true;
  }

  if ( passTopPair && passTopHad )  pass = true;
  setFilterPassed(pass);

  return StatusCode::SUCCESS;
}


bool BoostedHadTopAndTopPair::isFromTop(const HepMC::GenParticle* part) const{

  const HepMC::GenParticle* initpart = findInitial(part);
  HepMC::GenVertex* prod = initpart->production_vertex();

  if(!prod) return false;

  HepMC::GenVertex::particle_iterator firstParent = prod->particles_begin(HepMC::parents);
  HepMC::GenVertex::particle_iterator endParent = prod->particles_end(HepMC::parents);
  for(;firstParent!=endParent; ++firstParent){
    //if( part->barcode() < (*firstParent)->barcode() ) continue; /// protection for sherpa
    if( abs( (*firstParent)->pdg_id() ) == 6 ) return true;
  }
  return false;
}


const HepMC::GenParticle*  BoostedHadTopAndTopPair::findInitial(const HepMC::GenParticle* part) const{

  HepMC::GenVertex* prod = part->production_vertex();

  if(!prod) return part;

  HepMC::GenVertex::particle_iterator firstParent = prod->particles_begin(HepMC::parents);
  HepMC::GenVertex::particle_iterator endParent = prod->particles_end(HepMC::parents);
  for(;firstParent!=endParent; ++firstParent){
    //if( part->barcode() < (*firstParent)->barcode() ) continue; /// protection for sherpa
    if( part->pdg_id() == (*firstParent)->pdg_id() )  return findInitial(*firstParent);
  }
  return part;
}


bool BoostedHadTopAndTopPair::isHadronic(const HepMC::GenParticle* part) const{

  HepMC::GenVertex* end = part->end_vertex();

  HepMC::GenVertex::particle_iterator firstChild = end->particles_begin(HepMC::children);
  HepMC::GenVertex::particle_iterator endChild   = end->particles_end  (HepMC::children);
  for(;firstChild!=endChild; ++firstChild){
    //if( part->barcode() > (*firstChild)->barcode() ) continue; /// protection for sherpa
    if( abs((*firstChild)->pdg_id()) <= 5 ) return true;
  }
  return false;
}


bool BoostedHadTopAndTopPair::isFinalParticle(const HepMC::GenParticle* part) const{

  HepMC::GenVertex* end = part->end_vertex();
  if(end){
    int type = part->pdg_id();
    HepMC::GenVertex::particle_iterator firstChild = end->particles_begin(HepMC::children);
    HepMC::GenVertex::particle_iterator endChild = end->particles_end(HepMC::children);
    for(;firstChild!=endChild; ++firstChild){
      //if( part->barcode() > (*firstChild)->barcode() ) continue; /// protection for sherpa
      int childtype = (*firstChild)->pdg_id();
      if( childtype == type ) return false;
    }
  }
  return true;
}


HepMC::FourVector BoostedHadTopAndTopPair::momentumBofW(const HepMC::GenParticle* part){

  const HepMC::GenParticle* initpart = findInitial(part);
  HepMC::GenVertex* prod = initpart->production_vertex();

  HepMC::FourVector b(0,0,0,0);

  HepMC::GenVertex::particle_iterator firstChild = prod->particles_begin(HepMC::children);
  HepMC::GenVertex::particle_iterator endChild = prod->particles_end(HepMC::children);
  for(;firstChild!=endChild; ++firstChild){
    //if( part->barcode() > (*firstChild)->barcode() ) continue; /// protection for sherpa
    if( abs( (*firstChild)->pdg_id() ) == 5 ){
    b.set((*firstChild)->momentum().x(), (*firstChild)->momentum().y(), (*firstChild)->momentum().z(), (*firstChild)->momentum().t());
    }
  }
  return b;
}
