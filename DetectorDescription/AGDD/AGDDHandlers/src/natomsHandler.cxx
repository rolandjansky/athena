/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDDHandlers/natomsHandler.h"
#include <iostream>

natomsHandler::natomsHandler(const std::string& s,
                             AGDDController& c)
  : XMLHandler(s, c)
{
}

void natomsHandler::ElementHandle(AGDDController& c,
                                  xercesc::DOMNode *t)
{
	bool res;
	int natoms=getAttributeAsInt(c, t, "n",res);
        m_natoms.push_back (natoms);
}


std::vector<int> natomsHandler::GetNatoms()
{
  std::vector<int> v;
  v.swap (m_natoms);
  return v;
}
