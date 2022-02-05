/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef intersectionHandler_H
#define intersectionHandler_H

#include "AGDDControl/XMLHandler.h"
#include <string>

class intersectionHandler:public XMLHandler {
public:
        intersectionHandler(const std::string&,
                            AGDDController& c);
        virtual void ElementHandle(AGDDController& c,
                                   xercesc::DOMNode *t) override;
};

#endif
