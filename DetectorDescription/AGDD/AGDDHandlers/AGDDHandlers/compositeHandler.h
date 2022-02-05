/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef compositeHandler_H
#define compositeHandler_H

#include "AGDDControl/XMLHandler.h"
#include <string>

class compositeHandler:public XMLHandler {
public:
	compositeHandler(const std::string&,
                         AGDDController& c);
	virtual void ElementHandle(AGDDController& c,
                                   xercesc::DOMNode *t) override;
};

#endif
