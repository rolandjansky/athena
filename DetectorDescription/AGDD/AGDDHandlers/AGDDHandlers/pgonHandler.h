/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef pgonHandler_H
#define pgonHandler_H

#include "AGDDControl/XMLHandler.h"
#include <string>

class pgonHandler:public XMLHandler {
public:
	pgonHandler(const std::string&,
                    AGDDController& c);
	virtual void ElementHandle(AGDDController& c,
                                   xercesc::DOMNode *t) override;
};

#endif
