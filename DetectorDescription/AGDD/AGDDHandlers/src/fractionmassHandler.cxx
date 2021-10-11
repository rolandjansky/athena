/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDDHandlers/fractionmassHandler.h"
#include "AGDDHandlers/AddMaterial.h"
#include "AGDDHandlers/globals.h"
#include <iostream>

fractionmassHandler::fractionmassHandler(const std::string& s,
                                         AGDDController& c)
  : XMLHandler(s, c)
{
}

void fractionmassHandler::ElementHandle(AGDDController& c,
                                        xercesc::DOMNode *t)
{
	double fractionmass=getAttributeAsDouble(c, t, "fraction");
	globals::addMaterial.fractions.push_back(fractionmass);
}
