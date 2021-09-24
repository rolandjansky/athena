/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef sTGC_TechHandler_H
#define sTGC_TechHandler_H

#include "AGDDControl/XMLHandler.h"
#include <string>

class sTGC_TechHandler:public XMLHandler {
public:
	sTGC_TechHandler(const std::string&,
                         AGDDController& c);
	virtual void ElementHandle(AGDDController& c,
                                   xercesc::DOMNode *t) override;
};

#endif
