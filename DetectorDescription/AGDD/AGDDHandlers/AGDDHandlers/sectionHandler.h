/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef sectionHandler_H
#define sectionHandler_H

#include "AGDDControl/XMLHandler.h"
#include <string>

class sectionHandler:public XMLHandler {
public:
	sectionHandler(const std::string&,
                       AGDDController& c);
	virtual void ElementHandle(AGDDController& c,
                                   xercesc::DOMNode *t) override;
};

#endif
