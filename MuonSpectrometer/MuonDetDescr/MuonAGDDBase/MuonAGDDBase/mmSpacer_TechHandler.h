/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef mmSpacer_TechHandler_H
#define mmSpacer_TechHandler_H

#include "AGDDControl/XMLHandler.h"
#include <string>

class mmSpacer_TechHandler:public XMLHandler {
public:
	mmSpacer_TechHandler(const std::string&,
                             AGDDController& c);
	virtual void ElementHandle(AGDDController& c,
                                   xercesc::DOMNode *t) override;
};

#endif
