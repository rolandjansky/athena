/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetAnalysisEDM/ConstituentIterator.h"

namespace JetAnalysisEDM {
  
  const size_t c_maxConstitIndex = (size_t) -1; 

  // ConstituentIterator::ConstituentIterator() : LorentzVector_t()
  //                                                             , m_constituentSource(NULL)
  //                                                             , m_indexVec(NULL)
  //                                                             , m_index( c_maxConstitIndex )
  //                                                             , m_useSigState(false)
  //                                                             , m_sigState(P4SignalState::CALIBRATED)
  // {}

  ConstituentIterator::ConstituentIterator(const Particle* p) : LorentzVector_t()
                                                              , m_constituentSource(NULL)
                                                              , m_indexVec(NULL)
                                                              , m_index( c_maxConstitIndex )
                                                              , m_useSigState(false)
                                                              , m_sigState(P4SignalState::CALIBRATED)
  {
    m_constituentSource = p->parentCollection()->constituentsContainer();
    bool valid;
    m_indexVec = & p->get< std::vector<int> & >(p->parentCollection()->constituentsIndexName() , valid );
    if(!valid) m_indexVec = NULL;
    
  }

  
  ConstituentIterator & ConstituentIterator::operator++ (){
    m_index++;
    update4Mom();
    return *this;
  }
  ConstituentIterator & ConstituentIterator::operator-- (){
    if( m_index == c_maxConstitIndex) m_index = m_indexVec->size();
    m_index--;
    update4Mom();
    return *this;
  }


  bool ConstituentIterator::operator== (const ConstituentIterator& other) const {
    return ( (other.m_indexVec == m_indexVec) && (other.m_index == m_index) );
  }
  bool ConstituentIterator::operator!= (const ConstituentIterator& other) const {
    return !( *this == other );
  }


  void ConstituentIterator::update4Mom(){
    const Particle *p = get_real_constit();
    if(m_useSigState){ LorentzVector_t::operator=( (LorentzVector_t) p->hlv(m_sigState) ) ;}
    else{ LorentzVector_t::operator=(*p);}
  }


  const Particle* ConstituentIterator::get_real_constit(){ 
    return m_constituentSource->at( m_indexVec->at(m_index) ) ;
  }
  
  ConstituentIterator ConstituentIterator::first(const Particle * p){
    ConstituentIterator it = ConstituentIterator(p);
    if( bool(it.m_indexVec) ) it.m_index = 0;
    return it;
  }

  ConstituentIterator ConstituentIterator::first(const Particle * p, P4SignalState::State s){
    ConstituentIterator it = ConstituentIterator(p);
    if( bool(it.m_indexVec) ) it.m_index = 0;
    it.m_useSigState = true; it.m_sigState = P4SignalState::UNCALIBRATED ; 
    return it;
  }
  ConstituentIterator ConstituentIterator::last(const Particle * p){
    ConstituentIterator it = ConstituentIterator(p);
    return it;
  }

}
