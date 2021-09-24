/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef msgHandler_H
#define msgHandler_H

#include "AGDDControl/XMLHandler.h"
#include <string>

class msgHandler:public XMLHandler {
public:
	msgHandler(const std::string&,
                   AGDDController& c);
	virtual void ElementHandle(AGDDController& c,
                                   xercesc::DOMNode *t) override;

};

#endif
