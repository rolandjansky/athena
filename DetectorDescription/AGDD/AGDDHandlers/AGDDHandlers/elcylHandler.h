/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef elcylHandler_H
#define elcylHandler_H

#include "AGDDControl/XMLHandler.h"
#include <string>

class elcylHandler:public XMLHandler {
public:
	elcylHandler(const std::string&,
                     AGDDController& c);
	virtual void ElementHandle(AGDDController& c,
                                   xercesc::DOMNode *t) override;
};

#endif
