/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDDHandlers/importHandler.h"
#include "AGDDControl/XMLHandler.h"
#include "AGDDControl/IAGDDParser.h"
#include "AGDDControl/AGDDController.h"
#include <iostream>


importHandler::importHandler(std::string s):XMLHandler(s)
{
}

void importHandler::ElementHandle()
{
	std::string filename=getAttributeAsString("filename");
	std::cout<<" trying to parse file "<<filename<<std::endl;
	(AGDDController::GetController()->GetParser())->ParseFileAndNavigate(filename);
}
