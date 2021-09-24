/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef natomsHandler_H
#define natomsHandler_H

#include "AGDDControl/XMLHandler.h"
#include <string>

class natomsHandler:public XMLHandler {
public:
	natomsHandler(const std::string&,
                      AGDDController& c);
	virtual void ElementHandle(AGDDController& c,
                                   xercesc::DOMNode *t) override;
        std::vector<int> GetNatoms();
private:
        std::vector<int> m_natoms;
};

#endif
