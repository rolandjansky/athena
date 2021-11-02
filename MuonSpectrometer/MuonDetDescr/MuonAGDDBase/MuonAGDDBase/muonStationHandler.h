/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef muonStationHandler_H
#define muonStationHandler_H

#include "AGDDControl/XMLHandler.h"
#include <string>

class muonStationHandler:public XMLHandler {
public:
	muonStationHandler(const std::string&,
                           AGDDController& c);
	virtual void ElementHandle(AGDDController& c,
                                   xercesc::DOMNode *t) override;
};

#endif
