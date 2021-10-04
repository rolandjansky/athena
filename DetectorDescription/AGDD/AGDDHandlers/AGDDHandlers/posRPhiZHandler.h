/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef posRPhiZHandler_H
#define posRPhiZHandler_H

#include "AGDDControl/XMLHandler.h"

#include <string>

class posRPhiZHandler:public XMLHandler {
public:
        posRPhiZHandler(const std::string&,
                        AGDDController& c);
        virtual void ElementHandle(AGDDController& c,
                                   xercesc::DOMNode *t) override;
};

#endif
