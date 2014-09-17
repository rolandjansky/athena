/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ApplicationAction_H
#define ApplicationAction_H

class G4Step;
class G4Event;
class G4Run;
class ApplicationAction;

// base class for an action class to be implemented at the application level
// same signature as the standard user action. must be assigned to the 
// various Fads actions explicitely, by means of the SetApplicationAction()
// method

namespace FADS {

class ApplicationAction {
public:
	ApplicationAction() {}
	virtual ~ApplicationAction() {}
	virtual void BeginOfEventAction(const G4Event*) {}
	virtual void EndOfEventAction(const G4Event*) {}
	virtual void BeginOfRunAction(const G4Run*) {}
	virtual void EndOfRunAction(const G4Run*) {}
	virtual void SteppingAction(const G4Step*) {}
};

}	// end namespace

#endif
