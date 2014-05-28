/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDD2Geo/materialsHandler.h"
#include "AGDD2Geo/XercesParser.h"
#include "AGDD2Geo/AGDDMaterialStore.h"
#include <iostream>

materialsHandler::materialsHandler(std::string s):XMLHandler(s)
{
}

void materialsHandler::ElementHandle()
{
	bool res;
	double version=getAttributeAsDouble("version",res);
	std::string date=getAttributeAsString("date",res);
	std::string author=getAttributeAsString("author",res);
	std::string dtd_version=getAttributeAsString("DTD_version",res);
	
	AGDDMaterialStore *ms=AGDDMaterialStore::GetMaterialStore();
	ms->SetVersion(version);
	ms->SetAuthor(author);
	ms->SetDate(date);
	ms->SetDTDVersion(dtd_version);
	
	if (msgLog().level()<=MSG::DEBUG)
	msgLog()<<MSG::DEBUG<<"Materials version "<<version<<" date "<<date<<" author "<<author<<endreq;
}
