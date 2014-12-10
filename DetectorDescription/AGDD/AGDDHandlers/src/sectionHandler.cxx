/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDDHandlers/sectionHandler.h"
#include "AGDDKernel/AGDDSection.h"
#include <iostream>


sectionHandler::sectionHandler(std::string s):XMLHandler(s),sect(0)
{
//	std::cout<<"Creating handler for section"<<std::endl;
}

void sectionHandler::ElementHandle()
{
	bool res;
	std::string name = getAttributeAsString("name",res);
	std::string version = getAttributeAsString("version",res);
	std::string date = getAttributeAsString("date",res);
	std::string author = getAttributeAsString("author",res);
    std::string top="";
	top=getAttributeAsString("top_volume",res);

	sect=new AGDDSection(name,version,author,date,top,res);
}
