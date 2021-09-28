/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef mposPhiHandler_H
#define mposPhiHandler_H

#include "AGDDControl/XMLHandler.h"
#include <string>

class mposPhiHandler:public XMLHandler {
public:
        mposPhiHandler(const std::string&,
                       AGDDController& c);
        virtual void ElementHandle(AGDDController& c,
                                   xercesc::DOMNode *t) override;
};

#endif
