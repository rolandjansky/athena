/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PreEventActionManager_H
#define PreEventActionManager_H

#include <string>
#include <map>

class PreEventAction;
typedef std::map<std::string, PreEventAction*, std::less<std::string> > preEvActionMap;


class PreEventActionManager {
public:
	static PreEventActionManager* GetPreEventActionManager();
	void Execute();
	void RegisterAction(PreEventAction *);
private:
	static PreEventActionManager* thePointer;
	PreEventActionManager();
	PreEventActionManager(const PreEventActionManager&) {}
	preEvActionMap theMap;
};

#endif
