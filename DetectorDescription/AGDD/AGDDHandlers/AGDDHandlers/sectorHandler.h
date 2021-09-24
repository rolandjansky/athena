/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef sectorHandler_H
#define sectorHandler_H

#include "AGDDControl/XMLHandler.h"
#include <string>

class sectorHandler:public XMLHandler {
public:
	sectorHandler(const std::string&,
                      AGDDController& c);
	virtual void ElementHandle(AGDDController& c,
                                   xercesc::DOMNode *t) override;
};

#endif
