/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef InteractionLengthIntegrator_H
#define InteractionLengthIntegrator_H

#include "FadsActions/ActionsBase.h"
#include "FadsActions/UserAction.h"
#include <string>
#include <map>

class TProfile;

// User action to evaluate the thickness (in %interaction length) of all detectors
// traversed by outgoing particles
// this action should be activated at the beginning and the end of an event,
// and at step level

class InteractionLengthIntegrator: public FADS::ActionsBase , public FADS::UserAction {

  public:
    InteractionLengthIntegrator(std::string s): FADS::ActionsBase(s),FADS::UserAction(s),etaPrimary(0),phiPrimary(0) {}
    void BeginOfEventAction(const G4Event*);
    void EndOfEventAction(const G4Event*);
    void BeginOfRunAction(const G4Run*);
    void EndOfRunAction(const G4Run*);
    void SteppingAction(const G4Step*);

  private:
    double etaPrimary ;
    double phiPrimary ;
    std::map<std::string,double,std::less<std::string> > detThick;
    std::map<std::string,TProfile*,std::less<std::string> > histoMap;

};

#endif
