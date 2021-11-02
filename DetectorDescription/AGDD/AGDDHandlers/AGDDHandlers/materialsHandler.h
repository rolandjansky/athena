/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef materialsHandler_H
#define materialsHandler_H

#include "AGDDControl/XMLHandler.h"
#include <string>

class materialsHandler:public XMLHandler {
public:
	materialsHandler(const std::string&,
                         AGDDController& c);
	virtual void ElementHandle(AGDDController& c,
                                   xercesc::DOMNode *t) override;
};

#endif
