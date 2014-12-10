/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDDHandlers/natomsHandler.h"
#include "AGDDHandlers/AddMaterial.h"
#include "AGDDHandlers/globals.h"
#include <iostream>

natomsHandler::natomsHandler(std::string s):XMLHandler(s)
{
}

void natomsHandler::ElementHandle()
{
	bool res;
	int natoms=getAttributeAsInt("n",res);
	globals::addMaterial.natoms.push_back(natoms);
}
