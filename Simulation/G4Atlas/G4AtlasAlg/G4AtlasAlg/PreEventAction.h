/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PreEventAction_H
#define PreEventAction_H

#include <string>

class PreEventAction {
private:
	std::string name;
public:
	PreEventAction(std::string);
	virtual ~PreEventAction() {}
	std::string GetName();
	virtual void Execute() = 0;
};

#endif
