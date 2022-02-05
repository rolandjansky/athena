/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef mm_readoutHandler_H
#define mm_readoutHandler_H

#include "AGDDControl/XMLHandler.h"
#include <string>

class mm_readoutHandler:public XMLHandler {
public:
	mm_readoutHandler(const std::string&,
                          AGDDController& c);
	virtual void ElementHandle(AGDDController& c,
                                   xercesc::DOMNode *t) override;
};

#endif
