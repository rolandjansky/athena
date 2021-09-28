/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef gvxy_pointHandler_H
#define gvxy_pointHandler_H

#include "AGDDControl/XMLHandler.h"
#include "AGDDKernel/TwoPoint.h"
#include <string>

class gvxy_pointHandler:public XMLHandler {
public:
	gvxy_pointHandler(const std::string&,
                          AGDDController& c);
	virtual void ElementHandle(AGDDController& c,
                                   xercesc::DOMNode *t) override;
	static TwoPoint CurrentTwoPoint() {return s_point;}
private:
	static TwoPoint s_point;
};

#endif
