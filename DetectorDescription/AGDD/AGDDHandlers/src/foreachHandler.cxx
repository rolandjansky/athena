/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDDHandlers/foreachHandler.h"
#include "AGDDControl/XercesParser.h"
#include "AGDDControl/AGDDController.h"
#include <iostream>


using namespace xercesc;

foreachHandler::foreachHandler(const std::string& s,
                               AGDDController& c)
  : XMLHandler(s, c)
{
//	std::cout<<"Creating handler for foreach"<<std::endl;
}

void foreachHandler::ElementHandle(AGDDController& c,
                                   xercesc::DOMNode *t)
{
	std::string index=getAttributeAsString(c, t, "index");
	int ibegin=getAttributeAsInt(c, t, "begin");
	int iloops=getAttributeAsInt(c, t, "loops");
	StopLoop(true);
	Evaluator().RegisterVariable(index,ibegin);
	
	DOMNode *fChild=t->getFirstChild();
	for (int i=ibegin;i<ibegin+iloops;i++)
	{
			
//			std::cout<<"\t\tindex "<<i<<std::endl;
			Evaluator().RegisterVariable(index,i);
			
			DOMNode *child;
			for (child=fChild;child!=0;child=child->getNextSibling())
			{
				if (child->getNodeType()==DOMNode::ELEMENT_NODE) {
                                  XercesParser::elementLoop(c, child);
				}
			}
			
			
	}
	
}
