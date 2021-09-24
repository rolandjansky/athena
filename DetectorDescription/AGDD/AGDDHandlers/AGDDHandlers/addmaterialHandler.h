/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef addmaterialHandler_H
#define addmaterialHandler_H

#include "AGDDControl/XMLHandler.h"
#include <string>

class addmaterialHandler:public XMLHandler {
public:
	addmaterialHandler(const std::string&,
                           AGDDController& c);
	virtual void ElementHandle(AGDDController& c,
                                   xercesc::DOMNode *t) override;
        std::vector<std::string> GetNames();
private:
        std::vector<std::string> m_names;
};

#endif
