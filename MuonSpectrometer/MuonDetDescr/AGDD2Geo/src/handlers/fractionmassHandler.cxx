/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDD2Geo/fractionmassHandler.h"
#include "AGDD2Geo/XercesParser.h"
#include "AGDD2Geo/AddMaterial.h"
#include "AGDD2Geo/globals.h"
#include <iostream>

fractionmassHandler::fractionmassHandler(std::string s):XMLHandler(s)
{
}

void fractionmassHandler::ElementHandle()
{
	bool res;
	double fractionmass=getAttributeAsDouble("fraction",res);
	globals::addMaterial.fractions.push_back(fractionmass);
	if (msgLog().level()<=MSG::DEBUG)
	msgLog()<<MSG::DEBUG<<"fractionmass "<<fractionmass<<endreq;
}
