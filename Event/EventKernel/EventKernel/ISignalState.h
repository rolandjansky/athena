///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTKERNEL_ISIGNALSTATE_H
#define EVENTKERNEL_ISIGNALSTATE_H

#include "CLHEP/Vector/LorentzVector.h"

/** 
   @class ISignalState

   ISignalState is an abstract base class providing support for 
   various Signal States to be associated with an object. In the
   case of Jets and CaloClusters, Signal States correspond to
   different calibrations, represented by their respective
   four-vectors.
    
   @author Pierre-Antoine Delsart <delsart AT in2p3.fr>
   @author Pier-Olivier DeViveiros <viveiros AT physics.utoronto.ca>
   @author Peter Loch <loch AT physics.arizona.edu>
 */

struct P4SignalState {

  enum State { 
    UNKNOWN             = -1,
    UNCALIBRATED        =  0, // calorimeter signals
    CALIBRATED          =  1,
    ALTCALIBRATED       =  2,
    JETEMSCALE          =  0, // jet signals
    JETFINAL            =  1,
    JETCONSTITUENTSCALE =  2 }; 
};

class SignalStateHelper;
template <class COLL>
class SignalStateCollHelper;

class ISignalState
{
public:

  /** virtual destructor needed */
  virtual ~ISignalState();

  /** obtain the current signal state */
  virtual P4SignalState::State signalState() const = 0;

  /** obtain the CLHEP HepLorentzVector */
  virtual CLHEP::HepLorentzVector hlv(P4SignalState::State s) const = 0;
  
  /** check if signal state exists for current implementation*/
  virtual bool hasSignalState(P4SignalState::State s) const = 0;
  
  /** check if we are at the passed state */
  virtual bool isAtSignalState(P4SignalState::State s) const = 0;
  
protected:
  
  /** set the current signal state */
  virtual bool setSignalState(P4SignalState::State s) const = 0;
  
  /** reset the signal state */
  virtual void resetSignalState() const = 0;

  /** make friends! */
  friend class SignalStateHelper;
  template <class COLL>
  friend class SignalStateCollHelper;
  
};
#endif
