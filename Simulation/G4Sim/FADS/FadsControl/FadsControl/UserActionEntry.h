/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef UserActionEntry_H
#define UserActionEntry_H

#include "FadsControl/UserActionEntryBase.h"
#include "FadsControl/FadsActionBase.h"

template<class T> 
class UserActionEntry:public UserActionEntryBase {
public:
	 UserActionEntry(std::string n):UserActionEntryBase(n),
				        theUserAction(0) {}
//	void CreateEntry() {theUserAction=new T;}
	void CreateTrackingAction() {theUserAction=(FADS::FadsActionBase*)(G4UserTrackingAction*)(new T);}
	void CreateStackingAction() {theUserAction=(FADS::FadsActionBase*)(G4UserStackingAction*)(new T);}
	void CreateEventAction() {theUserAction=(FADS::FadsActionBase*)(G4UserEventAction*)(new T);}
	void CreateRunAction() {theUserAction=(FADS::FadsActionBase*)(G4UserRunAction*)(new T);}
	void CreateSteppingAction() {theUserAction=(FADS::FadsActionBase*)(G4UserSteppingAction*)(new T);}
	void DeleteEntry() {
		if (theUserAction) 
		{delete theUserAction;theUserAction=0;}
	}
	FADS::FadsActionBase* UserAction() {return theUserAction;} 
private:
	FADS::FadsActionBase* theUserAction;
};
#endif
