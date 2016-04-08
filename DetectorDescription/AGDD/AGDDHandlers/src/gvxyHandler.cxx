/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDDHandlers/gvxyHandler.h"
#include "AGDDModel/AGDDGvxy.h"
#include "AGDDHandlers/gvxy_pointHandler.h"
#include "AGDDControl/XercesParser.h"
#include <iostream>

using namespace xercesc;

gvxyHandler::gvxyHandler(std::string s):XMLHandler(s)
{
}

void gvxyHandler::ElementHandle()
{
	bool res;
	std::string name=getAttributeAsString("name");
	std::string material=getAttributeAsString("material");
	double dZ=getAttributeAsDouble("dZ");
	
 	AGDDGvxy *vol=new AGDDGvxy(name);
 	vol->SetMaterial(material);
 	vol->SetDz(dZ);

	std::vector<TwoPoint> points;	
	StopLoop(true);	
	
	DOMNode *child;
	DOMNode *cElement=XercesParser::GetCurrentElement();
	for (child=cElement->getFirstChild();child!=0;child=child->getNextSibling())
	{
		if (child->getNodeType()==DOMNode::ELEMENT_NODE) {
			XercesParser::elementLoop(child);
			TwoPoint p=gvxy_pointHandler::CurrentTwoPoint();
			points.push_back(p);
		}
	}
	
	int nPoints=points.size();
	for (int i=0;i<nPoints;i++)
		vol->SetPoint(points[i]);
	
	std::string col=getAttributeAsString("color",res);
	if (res)
		vol->SetColor(col);
}
