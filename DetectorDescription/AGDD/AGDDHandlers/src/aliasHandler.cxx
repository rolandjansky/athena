/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDDHandlers/aliasHandler.h"
#include "AGDDControl/XMLHandler.h"
#include "AGDDControl/AGDDController.h"
#include "AGDDKernel/AliasStore.h"
#include <iostream>


aliasHandler::aliasHandler(const std::string& s,
                           AGDDController& c)
  : XMLHandler(s, c)
{
}

void aliasHandler::ElementHandle(AGDDController& c,
                                 xercesc::DOMNode *t)
{
	std::string name=getAttributeAsString(c, t, "name");
	std::string value=getAttributeAsString(c, t, "standsFor");
	c.GetAliasStore().AddAlias(name,value);
}
