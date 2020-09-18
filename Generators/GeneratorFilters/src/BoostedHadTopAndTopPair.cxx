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
  for (itr = events()->begin(); itr != events()->end(); ++itr) {
    const HepMC::GenEvent* genEvt = *itr;

    
  for (auto part: *genEvt){
      int pdgId = part->pdg_id();
  
      // pdgId t quark = 6
      if ( pdgId == 6 && isFinalParticle(part) ){
        if ( part->momentum().perp() > topListMomentum.perp() )  topListMomentum  = part->momentum();
      }
  
      if ( pdgId == -6 && isFinalParticle(part) ){
        if ( part->momentum().perp() > topbListMomentum.perp() ) topbListMomentum = part->momentum();
      }
  
      // pdgId W boson = 24
      if ( std::abs(pdgId) != 24 || !isFinalParticle(part) ) continue; 
  
      if (isFromTop(part)){
        if (pdgId > 0) topChildrenMomentum.set(part->momentum().px() + momentumBofW(part).px(), part->momentum().py() + momentumBofW(part).py(), part->momentum().pz() + momentumBofW(part).pz(), part->momentum().e() + momentumBofW(part).e());
        else topbChildrenMomentum.set(part->momentum().px() + momentumBofW(part).px(), part->momentum().py() + momentumBofW(part).py(), part->momentum().pz() + momentumBofW(part).pz(), part->momentum().e() + momentumBofW(part).e());

        if (isHadronic(part)){
          double pT = std::sqrt( std::pow( part->momentum().px() + momentumBofW(part).px(),2) + std::pow( part->momentum().py() + momentumBofW(part).py(),2)); 
          if (pT > pTHadTopChildren){
            pTHadTopChildren = pT;
            if (pdgId > 0) pTHadTopList = topListMomentum.perp();
            else pTHadTopList = topbListMomentum.perp();
          }
        }
      }
    } // particle loop
  } // event loop

  double pTPairList = std::sqrt( std::pow( topListMomentum.px() + topbListMomentum.px() , 2 ) + std::pow( topListMomentum.py() + topbListMomentum.py() , 2 )); 
  double pTPairChildren = std::sqrt( std::pow( topChildrenMomentum.px() + topbChildrenMomentum.px() , 2 ) + std::pow( topChildrenMomentum.py() + topbChildrenMomentum.py() , 2 )); 

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


bool BoostedHadTopAndTopPair::isFromTop(HepMC::ConstGenParticlePtr part) const{

  auto initpart = findInitial(part);
  auto prod = initpart->production_vertex();

  if(!prod) return false;

#ifdef HEPMC3
   for (auto p: prod->particles_in()) if (std::abs(p->pdg_id()) == 6) return true;
#else
  HepMC::GenVertex::particle_iterator firstParent = prod->particles_begin(HepMC::parents);
  HepMC::GenVertex::particle_iterator endParent = prod->particles_end(HepMC::parents);
  for(;firstParent!=endParent; ++firstParent){
    //if( part->barcode() < (*firstParent)->barcode() ) continue; /// protection for sherpa
    if( std::abs( (*firstParent)->pdg_id() ) == 6 ) return true;
  }
#endif
  return false;
}


HepMC::ConstGenParticlePtr  BoostedHadTopAndTopPair::findInitial(HepMC::ConstGenParticlePtr part) const{

  auto prod = part->production_vertex();

  if(!prod) return part;

#ifdef HEPMC3
   for (auto p: prod->particles_in()) if (part->pdg_id() == p->pdg_id()) return findInitial(part);
#else
  HepMC::GenVertex::particle_iterator firstParent = prod->particles_begin(HepMC::parents);
  HepMC::GenVertex::particle_iterator endParent = prod->particles_end(HepMC::parents);
  for(;firstParent!=endParent; ++firstParent){
    //if( part->barcode() < (*firstParent)->barcode() ) continue; /// protection for sherpa
    if( part->pdg_id() == (*firstParent)->pdg_id() )  return findInitial(*firstParent);
  }
#endif  
  return part;
}


bool BoostedHadTopAndTopPair::isHadronic(HepMC::ConstGenParticlePtr part) const{

  auto end = part->end_vertex();
  if (end) {
  for(auto firstChild: *end){
    if( std::abs(firstChild->pdg_id()) <= 5 ) return true;
  }
  }
  return false;
}


bool BoostedHadTopAndTopPair::isFinalParticle(HepMC::ConstGenParticlePtr part) const{

  auto end = part->end_vertex();
  if(end){
    int type = part->pdg_id();
    for(auto  firstChild: *end){
      if( firstChild->pdg_id() == type ) return false;
    }
  }
  return true;
}


HepMC::FourVector BoostedHadTopAndTopPair::momentumBofW(HepMC::ConstGenParticlePtr part){

  auto initpart = findInitial(part);
  auto prod = initpart->production_vertex();

  HepMC::FourVector b(0,0,0,0);
if (prod) { 
 for( auto firstChild: *prod){
    if( std::abs( firstChild->pdg_id() ) == 5 ){
    b.set(firstChild->momentum().x(), firstChild->momentum().y(), firstChild->momentum().z(), firstChild->momentum().t());
    }
  }
}
  return b;
}
