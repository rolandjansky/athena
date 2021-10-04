/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef UbeamHandler_H
#define UbeamHandler_H

#include "AGDDControl/XMLHandler.h"
#include <string>

class UbeamHandler:public XMLHandler {
public:
	UbeamHandler(const std::string&,
                     AGDDController& c);
        virtual void ElementHandle(AGDDController& c,
                                   xercesc::DOMNode *t) override;
};

#endif
