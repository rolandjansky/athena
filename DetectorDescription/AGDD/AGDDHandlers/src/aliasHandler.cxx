/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDDHandlers/aliasHandler.h"
#include "AGDDControl/XMLHandler.h"
#include "AGDDKernel/AliasStore.h"
#include <iostream>


aliasHandler::aliasHandler(std::string s):XMLHandler(s)
{
}

void aliasHandler::ElementHandle()
{
	std::string name=getAttributeAsString("name");
	std::string value=getAttributeAsString("standsFor");
	AliasStore::GetAliasList()->AddAlias(name,value);
}
