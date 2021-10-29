/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef importHandler_H
#define importHandler_H

#include "AGDDControl/XMLHandler.h"
#include <string>
class AGDDController;


class importHandler:public XMLHandler {
public:
        importHandler(const std::string&,
                      AGDDController& c);
        virtual void ElementHandle(AGDDController& c,
                                   xercesc::DOMNode *t) override;
};

#endif
