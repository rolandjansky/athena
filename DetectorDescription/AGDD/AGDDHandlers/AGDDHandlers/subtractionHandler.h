/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef subtractionHandler_H
#define subtractionHandler_H

#include "AGDDControl/XMLHandler.h"

#include <string>

class subtractionHandler:public XMLHandler {
public:
	subtractionHandler(const std::string&,
                           AGDDController& c);
        virtual void ElementHandle(AGDDController& c,
                                   xercesc::DOMNode *t) override;
};

#endif
