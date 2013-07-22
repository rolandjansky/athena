/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef UserAction_H
#define UserAction_H

class G4Step;
class G4Event;
class G4Run;
#include "GaudiKernel/MsgStream.h" // So that all who inherit have full access

#include <string>
#include <map>
#include <iostream>

namespace FADS {

class UserAction;

typedef std::multimap<int,UserAction *, std::less<int> > actionMap; 

enum actionType {Step,BeginOfEvent,EndOfEvent,BeginOfRun,EndOfRun};

class UserAction {

typedef std::map<std::string, std::string> propMap; // map of properties

protected:
	std::string name;
	std::string registeredActions;
	void RegisterToStore(); 
	void UnregisterFromStore();
	propMap theProperties;
    MsgStream * m_log;
    MsgStream log();
public:
	UserAction(std::string s): name(s),m_log(0) {RegisterToStore();}
	virtual ~UserAction() 
	{
		log()<<MSG::INFO<<"Action "<<name<<" being killed"<<std::endl;
		UnregisterFromStore();
		if (m_log) delete m_log;
	}
	virtual void BeginOfEventAction(const G4Event*) {}
	virtual void EndOfEventAction(const G4Event*) {}
	virtual void BeginOfRunAction(const G4Run*) {}
	virtual void EndOfRunAction(const G4Run*) {}
	virtual void SteppingAction(const G4Step*) {}
	void SetProperty(std::string propName, std::string propValue);
	void ResetPriority(int);
	void RegisterFor(actionType t);
	void RegisterFor(std::string s);
	std::string GetName() const;
};

inline std::string UserAction::GetName() const 
{
	return name;
}

}	// end namespace

#endif
