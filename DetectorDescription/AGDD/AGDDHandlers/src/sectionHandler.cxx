/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDDHandlers/sectionHandler.h"
#include "AGDDControl/AGDDController.h"
#include "AGDDKernel/AGDDSection.h"
#include <iostream>


sectionHandler::sectionHandler(const std::string& s,
                               AGDDController& c)
  : XMLHandler(s, c)
{
//	std::cout<<"Creating handler for section"<<std::endl;
}

void sectionHandler::ElementHandle(AGDDController& c,
                                   xercesc::DOMNode *t)
{
	bool res;
	std::string name = getAttributeAsString(c, t, "name",res);
	std::string version = getAttributeAsString(c, t, "version",res);
	std::string date = getAttributeAsString(c, t, "date",res);
	std::string author = getAttributeAsString(c, t, "author",res);
        std::string top="";
	top=getAttributeAsString(c, t, "top_volume",res);

	new AGDDSection(name,version,author,date,top,c.GetSectionStore(),res);
}
