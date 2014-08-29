/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4UserActions_MomentumConservation_H
#define G4UserActions_MomentumConservation_H

#include "FadsActions/UserAction.h"
#include <string>


/// User action to check for event-level momentum/energy violation
class MomentumConservation: public FADS::UserAction {
public:

  /// Constructor
  MomentumConservation(const std::string& s)
    : UserAction(s),
      _sum_edep(0), _sum_eesc(0)
  {  }

  /// Check that primary and deposited/escaped energy agree
  void EndOfEventAction(const G4Event* anEvent);

  /// Sum the deposited/escaped energy at each step
  void SteppingAction(const G4Step* aStep);


private:

  /// Sum of energy deposited in the detector
  double _sum_edep;

  /// Sum of energy escaping the detector (volume = 0)
  double _sum_eesc;

};

#endif
