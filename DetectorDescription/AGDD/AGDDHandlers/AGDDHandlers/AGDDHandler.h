/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AGDDHandler_H
#define AGDDHandler_H

#include "AGDDControl/XMLHandler.h"
#include <string>

class AGDDHandler:public XMLHandler {
public:
	AGDDHandler(const std::string&, AGDDController& c);
        virtual void ElementHandle(AGDDController& c,
                                   xercesc::DOMNode *t) override;

};

#endif
