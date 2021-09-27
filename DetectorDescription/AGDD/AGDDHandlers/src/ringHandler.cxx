/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDDHandlers/ringHandler.h"

#include <iostream>

ringHandler::ringHandler(const std::string& s,
                         AGDDController& c)
  : XMLHandler(s, c)
{
//	std::cout<<"Creating handler for ring"<<std::endl;
}

void ringHandler::ElementHandle(AGDDController& /*c*/,
                                xercesc::DOMNode */*t*/)
{
//	std::cout<<"handling for ring";
//	bool res;
//	int i_ring=getAttributeAsInt(c, t, "value",res);
}
