/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDDHandlers/materialsHandler.h"
#include "AGDDModel/AGDDMaterialStore.h"
#include <iostream>

materialsHandler::materialsHandler(const std::string& s,
                                   AGDDController& c)
  : XMLHandler(s, c)
{
}

void materialsHandler::ElementHandle(AGDDController& /*c*/,
                                     xercesc::DOMNode */*t*/)
{
	
}
