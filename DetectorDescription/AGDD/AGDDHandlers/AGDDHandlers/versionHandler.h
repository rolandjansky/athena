/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef versionHandler_H
#define versionHandler_H

#include "AGDDControl/XMLHandler.h"
#include <string>

class versionHandler:public XMLHandler {
public:
	versionHandler(const std::string&,
                       AGDDController& c);
	virtual void ElementHandle(AGDDController& c,
                                   xercesc::DOMNode *t) override;
};

#endif
