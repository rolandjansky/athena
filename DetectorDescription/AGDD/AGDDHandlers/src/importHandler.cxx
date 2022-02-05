/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDDHandlers/importHandler.h"
#include "AGDDControl/XMLHandler.h"
#include "AGDDControl/IAGDDParser.h"
#include "AGDDControl/AGDDController.h"
#include <iostream>


importHandler::importHandler(const std::string& s,
                             AGDDController& c)
  : XMLHandler(s, c)
{
}

void importHandler::ElementHandle(AGDDController& c,
                                  xercesc::DOMNode *t)
{
	std::string filename=getAttributeAsString(c, t, "filename");
	std::cout<<" trying to parse file "<<filename<<std::endl;
	c.GetParser()->ParseFileAndNavigate(c, filename);
}
