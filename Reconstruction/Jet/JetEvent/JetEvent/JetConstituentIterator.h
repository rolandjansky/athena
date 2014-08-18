/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////
/// \class JetConstituentIterator
/// \brief A safe iterator on jet constituents
///
/// Modifying the signal states of jet constituents may be
/// dangerous as it can accidentally leave the state undefined
/// for subsequent algorithm.
/// This implementation is safe. This iterator is a simple
/// copy of the iterator 4mom with the desired state.
/// It should be enough for most use case.
/// If access to the real constituent is necessary, this
/// is still possible throug JetConstituentIterator::get_real_constit()
///
////////////////////////////////////////////////////////////////////////

#ifndef JETEVENT_JETCONSTITUENTITERATOR_H
#define JETEVENT_JETCONSTITUENTITERATOR_H


#include "FourMom/P4PxPyPzE.h"

#include "NavFourMom/INavigable4MomentumCollection.h"

#include "JetEvent/Jet.h"


class JetConstituentIterator : public P4PxPyPzE {
 public:

  typedef Jet::const_iterator base_it;
  
  JetConstituentIterator( base_it b): m_actual(b), m_end(b), m_use_signal_state(false), m_defState(P4SignalState::UNKNOWN), m_curState(m_defState)
    {
      m_end.end();
      update_hlv();
    }
  
  JetConstituentIterator( base_it b, P4SignalState::State s): m_actual(b),m_end(b),  m_use_signal_state(true), m_defState(s), m_curState(m_defState)
    {    m_end.end(); update_hlv();}
  
  // operators ------------------------------------------------
  JetConstituentIterator& operator++ (){
    m_actual++;
    update_hlv();
    return *this;
  }
  
  JetConstituentIterator& operator-- (){
    m_actual--;
    update_hlv();
    return *this;
  }

  // for python, we alias operator++/--
  void inc(){ this->operator++();}
  void dec(){ this->operator--();}
  
  const JetConstituentIterator * operator*() const {    return this; }
  
  bool operator== (const JetConstituentIterator& other) const { return (m_actual == other.m_actual) ; }
  
  bool operator!= (const JetConstituentIterator& other) const { return (m_actual != other.m_actual) ; }
  // ------------------------------------------------------------
  
  /// access to the *real* constituent pointer
  const Jet::constituent_t* get_real_constit(){ return *m_actual ;}
  
  
  /// get current signal state of the iterator
  P4SignalState::State getSignalState();
  /// set current signal state of the iterator
  void setSignalState(P4SignalState::State s);
  
  
  // Static functions -------------------------------------------
  /// get a default iterator
  static JetConstituentIterator first(const Jet * jet){
    Jet::const_iterator begin = jet->begin();
    if( begin == jet->end() ) return JetConstituentIterator(begin);            
    P4SignalState::State constituentDef = getSignalState(*begin);
    if( (constituentDef == jet->constituentSignalState())  ||  // if default state is same as the one in jet
        (constituentDef == P4SignalState::UNKNOWN ) )          // or has no sigstate
      return JetConstituentIterator(begin);             // -> use the fast version
    else
      return JetConstituentIterator(begin, jet->constituentSignalState()); // else default to state in jet
  }
  /// get an iterator on constituent with Sig State @param s
  static JetConstituentIterator first(const Jet * jet, P4SignalState::State s){
    return JetConstituentIterator(jet->begin(), s);
  }
  /// get the end iterator
  static JetConstituentIterator last(const Jet * jet){
    return JetConstituentIterator(jet->end());
  }
  /// Helper to retrieve a signal state from any constituent type 
  static P4SignalState::State getSignalState(const Jet::constituent_t *constit); 
  // -------------------------------------------------------------
  
 protected:
  
  base_it m_actual;
  base_it m_end;
  Jet::hlv_t   m_hlv;
  bool m_use_signal_state;
  P4SignalState::State m_defState;
  P4SignalState::State m_curState;
  
  void update_hlv();

};
#endif
