/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef tubsHandler_H
#define tubsHandler_H

#include "AGDDControl/XMLHandler.h"

#include <string>

class tubsHandler:public XMLHandler {
public:
	tubsHandler(const std::string&,
                    AGDDController& c);
	virtual void ElementHandle(AGDDController& c,
                                   xercesc::DOMNode *t) override;
};

#endif
