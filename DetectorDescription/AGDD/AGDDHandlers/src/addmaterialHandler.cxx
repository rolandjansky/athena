/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDDHandlers/addmaterialHandler.h"
#include "AGDDHandlers/AddMaterial.h"
#include "AGDDHandlers/globals.h"
#include <iostream>

addmaterialHandler::addmaterialHandler(std::string s):XMLHandler(s)
{
//	std::cout<<"Creating handler for addmaterial"<<std::endl;
}

void addmaterialHandler::ElementHandle()
{
//	std::cout<<"handling for addmaterial";

	std::string material=getAttributeAsString("material");
//	std::cout<<" material= "<<material<<std::endl;
	
	globals::addMaterial.names.push_back(material);
}
