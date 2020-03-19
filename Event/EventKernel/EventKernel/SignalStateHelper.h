///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTKERNEL_SIGNALSTATEHELPER_H
#define EVENTKERNEL_SIGNALSTATEHELPER_H

#include "EventKernel/ISignalState.h"
#include "CxxUtils/checker_macros.h"

/** 
   @class SignalStateHelper

   Simple helper class which allows "safe" signal state switching.
   As soon as the helper goes out of scope, the wrapped object
   is returned to its original SignalState.
    
   @author Pierre-Antoine Delsart <delsart AT in2p3.fr>
   @author Pier-Olivier DeViveiros <viveiros AT physics.utoronto.ca>
 */

class SignalStateHelper final
{
public:

  /** Empty Constructor */
  SignalStateHelper();

  /** Constructor with initialization */
  SignalStateHelper(ISignalState* theObject);
  SignalStateHelper(P4SignalState::State s);
  SignalStateHelper(ISignalState* theObject, P4SignalState::State s);
  
  /** Destructor */
  ~SignalStateHelper();
  
  /** Change the signal state for future operations */
  bool setSignalState(P4SignalState::State s) ;

  /** Reset the Signal State of the controlled object and use it for future operations */
  bool resetSignalState() ;

  /** Change the object controlled, this releases previous object and resets its SigState */
  bool controlObject(ISignalState* theObject);
  
  /** Release controled object (this resets its signal state) */
  bool releaseObject();
  
protected:

  /** Storage of the states */
  P4SignalState::State m_originalState;
  P4SignalState::State m_currentState;
  
  /** Pointer to the owned object */
  ISignalState* m_object;  

};


/** 
   @class SignalStateConstHelper

   Simple helper class which allows "safe" signal state switching.
   As soon as the helper goes out of scope, the wrapped object
   is returned to its original SignalState.
    
   @author Pierre-Antoine Delsart <delsart AT in2p3.fr>
   @author Pier-Olivier DeViveiros <viveiros AT physics.utoronto.ca>
 */

class ATLAS_NOT_THREAD_SAFE SignalStateConstHelper final
{
public:

  /** Empty Constructor */
  SignalStateConstHelper();

  /** Constructor with initialization */
  SignalStateConstHelper(const ISignalState* theObject);
  SignalStateConstHelper(P4SignalState::State s);
  SignalStateConstHelper(const ISignalState* theObject, P4SignalState::State s);
  
  /** Destructor */
  ~SignalStateConstHelper();
  
  /** Change the signal state for future operations */
  bool setSignalState(P4SignalState::State s) ;

  /** Reset the Signal State of the controlled object and use it for future operations */
  bool resetSignalState() ;

  /** Change the object controlled, this releases previous object and resets its SigState */
  bool controlObject(const ISignalState* theObject);
  
  /** Release controled object (this resets its signal state) */
  bool releaseObject();
  
protected:

  /** Storage of the states */
  P4SignalState::State m_originalState;
  P4SignalState::State m_currentState;
  
  /** Pointer to the owned object */
  ISignalState* m_object;  

};


#endif
