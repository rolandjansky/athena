/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IbeamHandler_H
#define IbeamHandler_H

#include "AGDDControl/XMLHandler.h"
#include <string>

class IbeamHandler:public XMLHandler {
public:
	IbeamHandler(const std::string&,
                    AGDDController& c);
	virtual void ElementHandle(AGDDController& c,
                                   xercesc::DOMNode *t) override;
};

#endif
