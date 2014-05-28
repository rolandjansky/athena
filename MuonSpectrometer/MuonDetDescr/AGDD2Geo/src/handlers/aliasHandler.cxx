/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDD2Geo/aliasHandler.h"
#include "AGDD2Geo/XMLHandler.h"
#include "AGDD2Geo/XercesParser.h"
#include "AGDD2Geo/AliasStore.h"
#include "GaudiKernel/MsgStream.h"
#include <iostream>


aliasHandler::aliasHandler(std::string s):XMLHandler(s)
{
}

void aliasHandler::ElementHandle()
{
	bool res;
	std::string name=getAttributeAsString("name",res);
	std::string value=getAttributeAsString("standsFor",res);
	AliasStore::GetAliasList()->AddAlias(name,value);
}
