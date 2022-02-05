/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef aliasHandler_H
#define aliasHandler_H

#include "AGDDControl/XMLHandler.h"
#include <string>

class aliasHandler:public XMLHandler {
public:
	aliasHandler(const std::string&,
                     AGDDController& c);
	virtual void ElementHandle(AGDDController& c,
                                   xercesc::DOMNode *t) override;
};

#endif
