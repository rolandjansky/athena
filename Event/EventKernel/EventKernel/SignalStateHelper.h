///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTKERNEL_SIGNALSTATEHELPER_H
#define EVENTKERNEL_SIGNALSTATEHELPER_H

#include "EventKernel/ISignalState.h"

/** 
   @class SignalStateHelper // SignalStateCollHelper

   Simple helper class which allows "safe" signal state switching.
   As soon as the helper goes out of scope, the wrapped object
   is returned to its original SignalState.
    
   @author Pierre-Antoine Delsart <delsart AT in2p3.fr>
   @author Pier-Olivier DeViveiros <viveiros AT physics.utoronto.ca>
 */

class SignalStateHelper
{
public:

  /** Empty Constructor */
  SignalStateHelper();

  /** Constructor with initialization */
  SignalStateHelper(const ISignalState* theObject);
  SignalStateHelper(P4SignalState::State s);
  SignalStateHelper(const ISignalState* theObject, P4SignalState::State s);
  
  /** Destructor */
  virtual ~SignalStateHelper();
  
  /** Change the signal state for future operations */
  virtual bool setSignalState(P4SignalState::State s) ;

  /** Reset the Signal State of the controlled object and use it for future operations */
  virtual bool resetSignalState() ;

  /** Change the object controlled, this releases previous object and resets its SigState */
  virtual bool controlObject(const ISignalState* theObject);
  
  /** Release controled object (this resets its signal state) */
  virtual bool releaseObject();
  
protected:

  /** Storage of the states */
  P4SignalState::State m_originalState;
  P4SignalState::State m_currentState;
  
  /** Pointer to the owned object */
  const ISignalState* m_object;  

};


// COLL must be a collection of ISignalState objects with an iterator
template <class COLL>
class SignalStateCollHelper
{
public:
  
  /** typedefs to make declarations simpler */
  typedef COLL                       coll_t;
  typedef typename coll_t::const_iterator  iterator_t;

  /** Empty Constructor */
  SignalStateCollHelper();

  /** Constructor with initialization */
  SignalStateCollHelper(const coll_t* theCollection);
  SignalStateCollHelper(P4SignalState::State s);
  SignalStateCollHelper(const coll_t* theCollection, P4SignalState::State s);

  /** Destructor */
  virtual ~SignalStateCollHelper();

  /** Change the signal state for future operations */
  virtual bool setSignalState(P4SignalState::State s);

  /** Reset the Signal State of the controlled object and use it for future operations */
  virtual bool resetSignalState();

  /** Change the collection controlled, this releases previous object and resets its SigState */
  virtual bool controlCollection(const coll_t* theCollection);

  /** Release controled object (this resets its signal state) */
  virtual bool releaseCollection();

protected:

  void acquireOriginalState();
  void switchCollection(P4SignalState::State s);

  /** Storage of the states */
  P4SignalState::State m_originalState;
  P4SignalState::State m_currentState;

  /** Pointer to the owned object */
  const coll_t* m_collection;
};

//*****************************************************************
//
// SignalStateCollHelper
//
//*****************************************************************

/** Empty Constructor */
template <class COLL>
SignalStateCollHelper<COLL>::SignalStateCollHelper():
  m_currentState(P4SignalState::UNKNOWN),
  m_collection(0)
{}

/** Constructor with initialization */
template <class COLL>
SignalStateCollHelper<COLL>::SignalStateCollHelper(const COLL* theCollection):
  m_currentState(P4SignalState::UNKNOWN),
  m_collection(theCollection)
{ 
  this->acquireOriginalState();
}

template <class COLL>
SignalStateCollHelper<COLL>::SignalStateCollHelper(P4SignalState::State s):
  m_currentState(s),
  m_collection(0)
{
  setSignalState(s);
}

template <class COLL>
SignalStateCollHelper<COLL>::SignalStateCollHelper(const COLL* theCollection, P4SignalState::State s):
  m_currentState(s),
  m_collection(theCollection)
{
  if( m_collection != NULL ){
    this->acquireOriginalState();
    if(m_currentState!=P4SignalState::UNKNOWN)
      {
        this->switchCollection( m_currentState );
      }
  }
}

template <class COLL>
SignalStateCollHelper<COLL>::~SignalStateCollHelper()
{
  releaseCollection();
}
template <class COLL>
void SignalStateCollHelper<COLL>::acquireOriginalState(){
  iterator_t firstObject = m_collection->begin();
  iterator_t lastObject  = m_collection->end();
  if( firstObject != lastObject) 
    m_originalState = (*firstObject)->signalState();
  else
    m_originalState = P4SignalState::CALIBRATED;
}

template <class COLL>
void SignalStateCollHelper<COLL>::switchCollection(P4SignalState::State s){

  iterator_t firstObject = m_collection->begin();
  iterator_t lastObject  = m_collection->end();
  for( ; firstObject != lastObject; firstObject++ )
    (*firstObject)->setSignalState(s); 

}


/** Change the object controlled */
template <class COLL>
bool SignalStateCollHelper<COLL>::controlCollection(const COLL* theCollection)
{

  m_collection = theCollection;
  this->acquireOriginalState();

  if(m_currentState!=P4SignalState::UNKNOWN)
    {
      this->switchCollection( m_currentState );
    }
  return true;
}

template <class COLL>
bool SignalStateCollHelper<COLL>::setSignalState(P4SignalState::State s)
{ 
  m_currentState = s;
  // If we do have an object attached, switch its signal state
  if(m_collection != 0 && m_currentState!=P4SignalState::UNKNOWN)
    { 
      this->switchCollection( m_currentState );
      return true;
    }
  return false;
}

template <class COLL>
bool SignalStateCollHelper<COLL>::releaseCollection()
{
  // If we have an object attached, revert and remove
  if(m_collection!=0)
    {
      this->switchCollection( m_originalState );
      m_collection=0;
      return true;
    }
  return false;
}

template <class COLL>
bool SignalStateCollHelper<COLL>::resetSignalState()
{ 
  if(m_collection!=0)
    { 
      m_currentState = m_originalState;
      this->switchCollection( m_originalState );
      return true;
    }
  return false;
}

#endif
