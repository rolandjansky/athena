/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef consHandler_H
#define consHandler_H

#include "AGDDControl/XMLHandler.h"
#include <string>

class consHandler:public XMLHandler {
public:
	consHandler(const std::string&,
                    AGDDController& c);
	virtual void ElementHandle(AGDDController& c,
                                   xercesc::DOMNode *t) override;
};

#endif
