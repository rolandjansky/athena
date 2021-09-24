/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef colorHandler_H
#define colorHandler_H

#include "AGDDControl/XMLHandler.h"
#include <string>

class AGDDColor;

class colorHandler:public XMLHandler {
public:
	colorHandler(const std::string&,
                     AGDDController& c);
	virtual void ElementHandle(AGDDController& c,
                                   xercesc::DOMNode *t) override;
};

#endif
