/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ringHandler_H
#define ringHandler_H

#include "AGDDControl/XMLHandler.h"
#include <string>

class ringHandler:public XMLHandler {
public:
	ringHandler(const std::string&,
                    AGDDController& c);
	virtual void ElementHandle(AGDDController& c,
                                   xercesc::DOMNode *t) override;
};

#endif
