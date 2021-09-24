/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef defaultsHandler_H
#define defaultsHandler_H

#include "AGDDControl/XMLHandler.h"
#include <string>

class defaultsHandler:public XMLHandler {
public:
	defaultsHandler(const std::string&,
                        AGDDController& c);
	virtual void ElementHandle(AGDDController& c,
                                   xercesc::DOMNode *t) override;
};

#endif
