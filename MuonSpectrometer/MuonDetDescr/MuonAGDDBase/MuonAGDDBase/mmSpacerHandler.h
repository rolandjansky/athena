/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef mmSpacerHandler_H
#define mmSpacerHandler_H

#include "AGDDControl/XMLHandler.h"
#include <string>

class mmSpacerHandler:public XMLHandler {
public:
	mmSpacerHandler(const std::string&,
                        AGDDController& c);
	virtual void ElementHandle(AGDDController& c,
                                   xercesc::DOMNode *t) override;
};

#endif
