/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetEvent/JetConstituentIterator.h"
#include "EventKernel/ISignalState.h"

P4SignalState::State JetConstituentIterator::getSignalState(const Jet::constituent_t *constit){ 
  const ISignalState* sig = dynamic_cast<const ISignalState*>(constit); 
  if( sig ) return sig->signalState(); 
  return P4SignalState::UNKNOWN; 
} 

void JetConstituentIterator::update_hlv(){
  
  if( m_actual != m_end) { // check current iterator is valid
    if( ! (*m_actual) ) {// check the pointer is valid (may be not if ElementLink broken)
      m_actual++;
      while ( m_actual != m_end){
        // loop till next valid pointer
        if( *m_actual ) break;
        m_actual++;
      }
      if (m_actual == m_end) return ;
    }
    
    CLHEP::HepLorentzVector myVector = (*m_actual)->hlv();
    
    // if user requested a signal state try to get it.
    if(m_use_signal_state) {
      const ISignalState* sigStateObject = dynamic_cast<const ISignalState*>(*m_actual);
      if( sigStateObject ){
	
	// Obtain the hlv at the desired state
	myVector = sigStateObject->hlv(m_defState);
      }
    }
    // set the 4 Momentum
    set4Mom( myVector ) ;
  }
}

P4SignalState::State JetConstituentIterator::getSignalState(){
  if(m_use_signal_state) return m_curState;
  else return m_defState;
}

void JetConstituentIterator::setSignalState(P4SignalState::State s){
  // remember internal members
  P4SignalState::State tmp_sstate = m_defState;
  bool tmp_use_sstate = m_use_signal_state;
  // change temporally the internal
  m_defState = s; m_use_signal_state = true;
  // now update 4 momentum
  update_hlv();

  // reset internals :
  m_use_signal_state = tmp_use_sstate;
  m_defState = tmp_sstate;
  m_curState = s;
}

