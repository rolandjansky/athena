/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef unionHandler_H
#define unionHandler_H

#include "AGDDControl/XMLHandler.h"
#include <string>

class unionHandler:public XMLHandler {
public:
	unionHandler(const std::string&,
                     AGDDController& c);
        virtual void ElementHandle(AGDDController& c,
                                   xercesc::DOMNode *t) override;
};

#endif
