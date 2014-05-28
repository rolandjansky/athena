/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDD2Geo/sectorHandler.h"
#include "AGDD2Geo/XercesParser.h"
#include <iostream>

sectorHandler::sectorHandler(std::string s):XMLHandler(s)
{
//	std::cout<<"Creating handler for sector"<<std::endl;
}

void sectorHandler::ElementHandle()
{
//	std::cout<<"handling for sector";
//	bool res;
//	int i_sector=getAttributeAsInt("value",res);
}
