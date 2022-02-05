/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef arrayHandler_H
#define arrayHandler_H

#include "AGDDControl/XMLHandler.h"
#include <string>

class arrayHandler:public XMLHandler {
public:
	arrayHandler(const std::string&,
                     AGDDController& c);
	virtual void ElementHandle(AGDDController& c,
                                   xercesc::DOMNode *t) override;
};

#endif
