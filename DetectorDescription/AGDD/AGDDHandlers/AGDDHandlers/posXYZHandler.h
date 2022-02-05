/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef posXYZHandler_H
#define posXYZHandler_H

#include "AGDDControl/XMLHandler.h"

#include <string>

class posXYZHandler:public XMLHandler {
public:
        posXYZHandler(const std::string&,
                      AGDDController& c);
        virtual void ElementHandle(AGDDController& c,
                                   xercesc::DOMNode *t) override;
};

#endif
