/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef boxHandler_H
#define boxHandler_H

#include "AGDDControl/XMLHandler.h"
#include <string>

class boxHandler:public XMLHandler {
public:
	boxHandler(const std::string&,
                   AGDDController& c);
	virtual void ElementHandle(AGDDController& c,
                                   xercesc::DOMNode *t) override;
};

#endif
