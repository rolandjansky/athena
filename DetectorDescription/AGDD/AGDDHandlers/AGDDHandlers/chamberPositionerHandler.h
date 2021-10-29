/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef chamberPositionerHandler_H
#define chamberPositionerHandler_H

#include "AGDDControl/XMLHandler.h"

#include <string>

class chamberPositionerHandler:public XMLHandler {
public:
        chamberPositionerHandler(const std::string&,
                                 AGDDController& c);
        virtual void ElementHandle(AGDDController& c,
                                   xercesc::DOMNode *t) override;
};

#endif
