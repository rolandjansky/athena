/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ActionsStore_H
#define ActionsStore_H

#include <string>
#include <map>

namespace FADS {

class UserAction;

typedef std::map<std::string,UserAction*,std::less<std::string> > userActionMap;

class ActionsStore {
private:
	userActionMap theMap;
	static ActionsStore* thePointer;
	ActionsStore();
	ActionsStore(const ActionsStore&);
public:
	static ActionsStore* GetActionsStore();
	void Register(UserAction *);
	void Unregister(const UserAction *);
	void ListActions();
	void SetActionProperty(std::string actionName, std::string propName, std::string propValue);
	UserAction* Retrieve(std::string n);
};

}	// end namespace

#endif
