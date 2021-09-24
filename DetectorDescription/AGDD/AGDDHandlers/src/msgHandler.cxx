/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDDHandlers/msgHandler.h"
#include "AGDDControl/AGDDController.h"

#include <iostream>

msgHandler::msgHandler(const std::string& s,
                       AGDDController& c)
  : XMLHandler(s, c)
{
//	std::cout<<"Creating handler for msg"<<std::endl;
}

void msgHandler::ElementHandle(AGDDController& c,
                               xercesc::DOMNode *t)
{
//	std::cout<<"handling for msg";
	bool bres;
	std::string text=getAttributeAsString(c, t, "text",bres);
	std::string expression=getAttributeAsString(c, t, "expression",bres);
	std::cout<<" msg: "<<text<<" expression="<<expression;
	double res=getAttributeAsDouble(c, t, "expression",bres);
	std::cout<<" evaluates to "<<res<<std::endl;
}
