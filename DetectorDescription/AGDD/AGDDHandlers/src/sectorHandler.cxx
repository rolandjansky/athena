/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDDHandlers/sectorHandler.h"
#include <iostream>

sectorHandler::sectorHandler(const std::string& s,
                             AGDDController& c)
  : XMLHandler(s, c)
{
//	std::cout<<"Creating handler for sector"<<std::endl;
}

void sectorHandler::ElementHandle(AGDDController& /*c*/,
                                  xercesc::DOMNode */*t*/)
{
//	std::cout<<"handling for sector";
//	bool res;
//	int i_sector=getAttributeAsInt(c, t, "value",res);
}
