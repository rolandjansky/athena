/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef pconHandler_H
#define pconHandler_H

#include "AGDDControl/XMLHandler.h"
#include <string>

class pconHandler:public XMLHandler {
public:
	pconHandler(const std::string&,
                    AGDDController& c);
	virtual void ElementHandle(AGDDController& c,
                                   xercesc::DOMNode *t) override;
};

#endif
