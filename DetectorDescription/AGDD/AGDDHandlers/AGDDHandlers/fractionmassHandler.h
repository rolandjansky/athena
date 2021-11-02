/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef fractionmassHandler_H
#define fractionmassHandler_H

#include "AGDDControl/XMLHandler.h"
#include <string>

class fractionmassHandler:public XMLHandler {
public:
	fractionmassHandler(const std::string&,
                            AGDDController& c);
	virtual void ElementHandle(AGDDController& c,
                                   xercesc::DOMNode *t) override;
        std::vector<double> GetFractions();
private:
        std::vector<double> m_fractions;
};

#endif
