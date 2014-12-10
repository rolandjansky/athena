/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDDHandlers/msgHandler.h"

#include <iostream>

msgHandler::msgHandler(std::string s):XMLHandler(s)
{
//	std::cout<<"Creating handler for msg"<<std::endl;
}

void msgHandler::ElementHandle()
{
//	std::cout<<"handling for msg";
	bool bres;
	std::string text=getAttributeAsString("text",bres);
	std::string expression=getAttributeAsString("expression",bres);
	std::cout<<" msg: "<<text<<" expression="<<expression;
	double res=getAttributeAsDouble("expression",bres);
	std::cout<<" evaluates to "<<res<<std::endl;
}
