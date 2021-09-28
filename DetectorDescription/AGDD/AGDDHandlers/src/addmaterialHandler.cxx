/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDDHandlers/addmaterialHandler.h"
#include "AGDDHandlers/AddMaterial.h"
#include "AGDDHandlers/globals.h"
#include <iostream>

addmaterialHandler::addmaterialHandler(const std::string& s,
                                       AGDDController& c)
  : XMLHandler(s, c)
{
//	std::cout<<"Creating handler for addmaterial"<<std::endl;
}

void addmaterialHandler::ElementHandle(AGDDController& c,
                                       xercesc::DOMNode *t)
{
//	std::cout<<"handling for addmaterial";

	std::string material=getAttributeAsString(c, t, "material");
//	std::cout<<" material= "<<material<<std::endl;
	
	globals::addMaterial.names.push_back(material);
}
