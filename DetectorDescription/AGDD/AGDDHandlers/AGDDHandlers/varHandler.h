/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef varHandler_H
#define varHandler_H

#include "AGDDControl/XMLHandler.h"
#include <string>

class varHandler:public XMLHandler {
public:
	varHandler(const std::string&,
                   AGDDController& c);
	virtual void ElementHandle(AGDDController& c,
                                   xercesc::DOMNode *t) override;
};

#endif
