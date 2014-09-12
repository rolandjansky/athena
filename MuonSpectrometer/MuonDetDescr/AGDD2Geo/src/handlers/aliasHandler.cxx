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
	std::string name=getAttributeAsString("name");
	std::string value=getAttributeAsString("standsFor");
	AliasStore::GetAliasList()->AddAlias(name,value);
}
