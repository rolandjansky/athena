/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BoltHandler_H
#define BoltHandler_H

#include "AGDDControl/XMLHandler.h"
#include <string>

class BoltHandler:public XMLHandler {
public:
	BoltHandler(const std::string&,
                    AGDDController& c);
	virtual void ElementHandle(AGDDController& c,
                                   xercesc::DOMNode *t) override;
};

#endif
