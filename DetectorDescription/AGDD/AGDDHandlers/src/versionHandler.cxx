/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDDHandlers/versionHandler.h"

#include <iostream>

versionHandler::versionHandler(const std::string& s,
                               AGDDController& c)
  : XMLHandler(s, c)
{
}

void versionHandler::ElementHandle(AGDDController& c,
                                   xercesc::DOMNode *t)
{
	bool bres;
	std::string fileName=getAttributeAsString(c, t, "filename");
	std::string version=getAttributeAsString(c, t, "version");
	std::string comment=getAttributeAsString(c, t, "comment",bres);
	std::cout<<" --->  File being parsed: "<<fileName<<" version: "<<version;
	if (bres) std::cout<<" comment: "<<comment;
	std::cout<<std::endl;
}
