/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ElementFacade_H
#define ElementFacade_H

#include <string>

class Element {
public:
	Element(const std::string name, const std::string symbol, double z, double a,double density=-1.);
	static void List();
};


#endif
