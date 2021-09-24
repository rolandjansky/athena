/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef snakeHandler_H
#define snakeHandler_H

#include "AGDDControl/XMLHandler.h"

#include <string>

class snakeHandler:public XMLHandler {
public:
	snakeHandler(const std::string&,
                     AGDDController& c);
	virtual void ElementHandle(AGDDController& c,
                                   xercesc::DOMNode *t) override;
};

#endif
