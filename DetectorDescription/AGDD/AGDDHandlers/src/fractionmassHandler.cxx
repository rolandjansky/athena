/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDDHandlers/fractionmassHandler.h"
#include "AGDDHandlers/AddMaterial.h"
#include "AGDDHandlers/globals.h"
#include <iostream>

fractionmassHandler::fractionmassHandler(std::string s):XMLHandler(s)
{
}

void fractionmassHandler::ElementHandle()
{
	double fractionmass=getAttributeAsDouble("fraction");
	globals::addMaterial.fractions.push_back(fractionmass);
}
