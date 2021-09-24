/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef elementHandler_H
#define elementHandler_H

#include "AGDDControl/XMLHandler.h"
#include <string>

class AGDDMaterial;

class elementHandler:public XMLHandler {
public:
        elementHandler(const std::string&,
                       AGDDController& c);
	virtual void ElementHandle(AGDDController& c,
                                   xercesc::DOMNode *t) override;
};

#endif
