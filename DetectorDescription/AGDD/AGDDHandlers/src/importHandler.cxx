/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDDHandlers/importHandler.h"
#include "AGDDControl/XMLHandler.h"
#include "AGDDControl/IAGDDParser.h"
#include "AGDDControl/AGDDController.h"
#include <iostream>


importHandler::importHandler(const std::string& s,
                             AGDDController& controller)
  : XMLHandler(s),
    m_controller (controller)
{
}

void importHandler::ElementHandle()
{
	std::string filename=getAttributeAsString("filename");
	std::cout<<" trying to parse file "<<filename<<std::endl;
	m_controller.GetParser()->ParseFileAndNavigate(filename);
}
