/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef UserActionManager_H
#define UserActionManager_H

#include <iostream>
#include <string>
#include <map>

class UserActionEntryBase;
class G4UserRunAction;
class G4UserEventAction;
class G4UserSteppingAction;
class G4UserTrackingAction;
class G4UserStackingAction;

#include "G4RunManager.hh"

typedef std::map<std::string, UserActionEntryBase*, std::less<std::string> > userActionMap;

class  UserActionManager {
public:
	UserActionManager();
	
	static void AddUserActionEntry(UserActionEntryBase * const);
	static void PrintUserActions();
	G4UserRunAction* CreateRunAction(std::string);
	G4UserEventAction* CreateEventAction(std::string);
	G4UserSteppingAction* CreateSteppingAction(std::string);
	G4UserTrackingAction* CreateTrackingAction(std::string);
	G4UserStackingAction* CreateStackingAction(std::string);
	
private:	
	static userActionMap theUserActions;
};

struct UserActionManagerFacade
{ 				
	void PrintUserActions()
		{UserActionManager::PrintUserActions();}
	G4UserRunAction* CreateRunAction(std::string s)
		{UserActionManager a; return a.CreateRunAction(s);}
	G4UserEventAction* CreateEventAction(std::string s)
		{UserActionManager a; return a.CreateEventAction(s);}
	G4UserSteppingAction* CreateSteppingAction(std::string s)
		{UserActionManager a; return a.CreateSteppingAction(s);}
	G4UserTrackingAction* CreateTrackingAction(std::string s)
		{UserActionManager a; return a.CreateTrackingAction(s);}
	G4UserStackingAction* CreateStackingAction(std::string s)
		{UserActionManager a; return a.CreateStackingAction(s);}
	void SetSteppingAction(std::string s)
		{G4RunManager *rm=G4RunManager::GetRunManager();
		 rm->SetUserAction(CreateSteppingAction(s));}
	void SetRunAction(std::string s)
		{G4RunManager *rm=G4RunManager::GetRunManager();
		 rm->SetUserAction(CreateRunAction(s));}
	void SetEventAction(std::string s)
		{G4RunManager *rm=G4RunManager::GetRunManager();
		 rm->SetUserAction(CreateEventAction(s));}
	void SetTrackingAction(std::string s)
		{G4RunManager *rm=G4RunManager::GetRunManager();
		 rm->SetUserAction(CreateTrackingAction(s));}
	void SetStackingAction(std::string s)
		{G4RunManager *rm=G4RunManager::GetRunManager();
		 rm->SetUserAction(CreateStackingAction(s));}
};

#endif
