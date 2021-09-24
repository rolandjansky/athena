/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDDHandlers/AGDDHandler.h"

AGDDHandler::AGDDHandler(const std::string& s, AGDDController& c)
  : XMLHandler(s, c)
{
}

void AGDDHandler::ElementHandle(AGDDController& /*c*/, xercesc::DOMNode */*t*/)
{

}
