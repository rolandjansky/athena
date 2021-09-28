/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef gvxysxyHandler_H
#define gvxysxyHandler_H

#include "AGDDControl/XMLHandler.h"
#include <string>

class gvxysxyHandler:public XMLHandler {
public:
	gvxysxyHandler(const std::string&,
                       AGDDController& c);
	virtual void ElementHandle(AGDDController& c,
                                   xercesc::DOMNode *t) override;
};

#endif
