/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDD2Geo/foreachHandler.h"
#include "AGDD2Geo/XercesParser.h"
#include <iostream>


foreachHandler::foreachHandler(std::string s):XMLHandler(s)
{
//	std::cout<<"Creating handler for foreach"<<std::endl;
}

void foreachHandler::ElementHandle()
{
	bool res;
	std::string index=getAttributeAsString("index",res);
	int ibegin=getAttributeAsInt("begin",res);
	int iloops=getAttributeAsInt("loops",res);
	if (msgLog().level()<=MSG::DEBUG)
	msgLog()<<MSG::DEBUG<<"foreach index "<<index<<" values = "<<ibegin<<" "<<iloops<<endreq;
	StopLoop(true);
	XercesParser::Evaluator().RegisterVariable(index,ibegin);
	
	DOMNode *fChild=XercesParser::GetCurrentElement()->getFirstChild();
	for (int i=ibegin;i<ibegin+iloops;i++)
	{
			
//			std::cout<<"\t\tindex "<<i<<std::endl;
			XercesParser::Evaluator().RegisterVariable(index,i);
			
			DOMNode *child;
			for (child=fChild;child!=0;child=child->getNextSibling())
			{
				if (child->getNodeType()==DOMNode::ELEMENT_NODE) {
				XercesParser::elementLoop(child);
				}
			}
			
			
	}
	
}
