/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef mm_TechHandler_H
#define mm_TechHandler_H

#include "AGDDControl/XMLHandler.h"
#include <string>

class mm_TechHandler:public XMLHandler {
public:
	mm_TechHandler(const std::string&,
                       AGDDController& c);
	virtual void ElementHandle(AGDDController& c,
                                   xercesc::DOMNode *t) override;
};

#endif
