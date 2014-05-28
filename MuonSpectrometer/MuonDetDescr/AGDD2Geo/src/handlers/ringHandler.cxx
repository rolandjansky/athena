/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDD2Geo/ringHandler.h"
#include "AGDD2Geo/XercesParser.h"
#include <iostream>

ringHandler::ringHandler(std::string s):XMLHandler(s)
{
//	std::cout<<"Creating handler for ring"<<std::endl;
}

void ringHandler::ElementHandle()
{
//	std::cout<<"handling for ring";
//	bool res;
//	int i_ring=getAttributeAsInt("value",res);
}
