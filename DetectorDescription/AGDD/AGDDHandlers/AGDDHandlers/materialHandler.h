/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef materialHandler_H
#define materialHandler_H

#include "AGDDControl/XMLHandler.h"
#include <string>

class AGDDMaterial;
class materialHandler:public XMLHandler {
public:
	materialHandler(const std::string&,
                        AGDDController& c);
	virtual void ElementHandle(AGDDController& c,
                                   xercesc::DOMNode *t) override;
};

#endif
