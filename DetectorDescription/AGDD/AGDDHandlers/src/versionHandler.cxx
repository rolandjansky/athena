/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDDHandlers/versionHandler.h"

#include <iostream>

versionHandler::versionHandler(std::string s):XMLHandler(s)
{
}

void versionHandler::ElementHandle()
{
	bool bres;
	std::string fileName=getAttributeAsString("filename");
	std::string version=getAttributeAsString("version");
	std::string comment=getAttributeAsString("comment",bres);
	std::cout<<" --->  File being parsed: "<<fileName<<" version: "<<version;
	if (bres) std::cout<<" comment: "<<comment;
	std::cout<<std::endl;
}
