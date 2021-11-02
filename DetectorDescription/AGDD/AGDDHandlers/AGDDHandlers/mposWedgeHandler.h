/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef mposWedgeHandler_H
#define mposWedgeHandler_H

#include "AGDDControl/XMLHandler.h"
#include <string>

class mposWedgeHandler:public XMLHandler {
public:
        mposWedgeHandler(const std::string&,
                         AGDDController& c);
        virtual void ElementHandle(AGDDController& c,
                                   xercesc::DOMNode *t) override;
};

#endif
