/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FadsControl/UserActionManager.h"
#include "FadsControl/UserActionEntryBase.h"

#include "G4RunManager.hh"
#include "G4UserRunAction.hh"
#include "G4UserEventAction.hh"
#include "G4UserTrackingAction.hh"
#include "G4UserSteppingAction.hh"
#include "G4UserStackingAction.hh"

#include "FadsControl/FadsActionBase.h"


userActionMap UserActionManager::theUserActions=userActionMap();

UserActionManager::UserActionManager()
{
}
void UserActionManager::AddUserActionEntry(UserActionEntryBase *pl)
{
	std::string tempName=pl->name();
	std::cout<<"trying to add user action "<<tempName<<" to catalog"<<std::endl;
	if (theUserActions.find(tempName)!=theUserActions.end())
		std::cout<<" User action entry "<<tempName<<" already exists!"
			 <<std::endl;
	else
		theUserActions[tempName]=pl;
	std::cout<<"UserActionManager done"<<std::endl;
}


void UserActionManager::PrintUserActions()
{
	std::cout<<" -- Catalog of all available user actions --"<<std::endl;
 	userActionMap::const_iterator it;
 	for (it=theUserActions.begin();it!=theUserActions.end();it++)
 	{
		std::cout<<"\t\t "<<(*it).first<<std::endl;
 	}
	std::cout<<" --"<<std::endl;
}

G4UserRunAction* UserActionManager::CreateRunAction(std::string s)
{
	if (theUserActions.find(s)!=theUserActions.end())
	{
		theUserActions[s]->CreateRunAction();
		FADS::FadsActionBase* t=theUserActions[s]->UserAction();
		G4UserRunAction* temp=(G4UserRunAction*)(t);
		return temp;
	}
	else
		std::cout<<"Action "<<s<<" not found in UserActionManager: returning 0"<<std::endl;
	return 0;
}
G4UserEventAction* UserActionManager::CreateEventAction(std::string s)
{
	if (theUserActions.find(s)!=theUserActions.end())
	{
		theUserActions[s]->CreateEventAction();
		FADS::FadsActionBase* t=theUserActions[s]->UserAction();
		G4UserEventAction* temp=(G4UserEventAction*)(t);
		return temp;
	}
	else
		std::cout<<"Action "<<s<<" not found in UserActionManager: returning 0"<<std::endl;
	return 0;
}
G4UserSteppingAction* UserActionManager::CreateSteppingAction(std::string s)
{
	if (theUserActions.find(s)!=theUserActions.end())
	{
		theUserActions[s]->CreateSteppingAction();
		FADS::FadsActionBase* t=theUserActions[s]->UserAction();
		G4UserSteppingAction* temp=(G4UserSteppingAction*)(t);
		return temp;
	}
	else
		std::cout<<"Action "<<s<<" not found in UserActionManager: returning 0"<<std::endl;
	return 0;
}
G4UserTrackingAction* UserActionManager::CreateTrackingAction(std::string s)
{
	if (theUserActions.find(s)!=theUserActions.end())
	{
		theUserActions[s]->CreateTrackingAction();
		FADS::FadsActionBase* t=theUserActions[s]->UserAction();
		G4UserTrackingAction* temp=(G4UserTrackingAction*)(t);
		return temp;
	}
	else
		std::cout<<"Action "<<s<<" not found in UserActionManager: returning 0"<<std::endl;
	return 0;
}
G4UserStackingAction* UserActionManager::CreateStackingAction(std::string s)
{
	if (theUserActions.find(s)!=theUserActions.end())
	{
		theUserActions[s]->CreateStackingAction();
		FADS::FadsActionBase* t=theUserActions[s]->UserAction();
		G4UserStackingAction* temp=(G4UserStackingAction*)(t);
		return temp;
	}
	else
		std::cout<<"Action "<<s<<" not found in UserActionManager: returning 0"<<std::endl;
	return 0;
}	
