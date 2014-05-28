/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDD2Geo/natomsHandler.h"
#include "AGDD2Geo/XercesParser.h"
#include "AGDD2Geo/AddMaterial.h"
#include "AGDD2Geo/globals.h"
#include <iostream>

natomsHandler::natomsHandler(std::string s):XMLHandler(s)
{
}

void natomsHandler::ElementHandle()
{
	bool res;
	int natoms=getAttributeAsInt("n",res);
	globals::addMaterial.natoms.push_back(natoms);
	if (msgLog().level()<=MSG::DEBUG)
	msgLog()<<MSG::DEBUG<<"\tnatoms "<<natoms<<endreq;
}
