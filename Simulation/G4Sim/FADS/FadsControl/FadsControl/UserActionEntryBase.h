/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef UserActionEntryBase_H
#define  UserActionEntryBase_H

#include <string>

#include "FadsControl/UserActionManager.h"

#include "FadsControl/FadsActionBase.h"

class UserActionEntryBase {
public:
	UserActionEntryBase(std::string n):name_(n) 
	{
		UserActionManager::AddUserActionEntry(this);
	}
	virtual ~UserActionEntryBase() {}
	std::string name() {return name_;}
//	virtual void CreateEntry() =0;
	virtual void CreateTrackingAction() = 0;
	virtual void CreateStackingAction() = 0;
	virtual void CreateEventAction() = 0;
	virtual void CreateRunAction() = 0;
	virtual void CreateSteppingAction() = 0;
	virtual void DeleteEntry() =0;
	virtual FADS::FadsActionBase* UserAction() = 0;
protected:
	std::string name_;
};

#endif
