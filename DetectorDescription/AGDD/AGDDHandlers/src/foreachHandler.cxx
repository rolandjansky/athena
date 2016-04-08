/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDDHandlers/foreachHandler.h"
#include "AGDDControl/XercesParser.h"
#include <iostream>


using namespace xercesc;

foreachHandler::foreachHandler(std::string s):XMLHandler(s)
{
//	std::cout<<"Creating handler for foreach"<<std::endl;
}

void foreachHandler::ElementHandle()
{
	std::string index=getAttributeAsString("index");
	int ibegin=getAttributeAsInt("begin");
	int iloops=getAttributeAsInt("loops");
	StopLoop(true);
	Evaluator().RegisterVariable(index,ibegin);
	
	DOMNode *fChild=XercesParser::GetCurrentElement()->getFirstChild();
	for (int i=ibegin;i<ibegin+iloops;i++)
	{
			
//			std::cout<<"\t\tindex "<<i<<std::endl;
			Evaluator().RegisterVariable(index,i);
			
			DOMNode *child;
			for (child=fChild;child!=0;child=child->getNextSibling())
			{
				if (child->getNodeType()==DOMNode::ELEMENT_NODE) {
				XercesParser::elementLoop(child);
				}
			}
			
			
	}
	
}
