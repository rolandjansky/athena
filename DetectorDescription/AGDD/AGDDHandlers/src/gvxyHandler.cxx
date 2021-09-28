/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDDHandlers/gvxyHandler.h"
#include "AGDDModel/AGDDGvxy.h"
#include "AGDDHandlers/gvxy_pointHandler.h"
#include "AGDDControl/XercesParser.h"
#include <iostream>

using namespace xercesc;

gvxyHandler::gvxyHandler(const std::string& s,
                         AGDDController& c)
  : XMLHandler(s, c)
{
}

void gvxyHandler::ElementHandle(AGDDController& c,
                                xercesc::DOMNode *t)
{
	bool res;
	std::string name=getAttributeAsString(c, t, "name");
	std::string material=getAttributeAsString(c, t, "material");
	double dZ=getAttributeAsDouble(c, t, "dZ");
	
 	AGDDGvxy *vol=new AGDDGvxy(name);
 	vol->SetMaterial(material);
 	vol->SetDz(dZ);

	std::vector<TwoPoint> points;	
	StopLoop(true);	
	
	DOMNode *child;
	for (child=t->getFirstChild();child!=0;child=child->getNextSibling())
	{
		if (child->getNodeType()==DOMNode::ELEMENT_NODE) {
			XercesParser::elementLoop(c, child);
			TwoPoint p=gvxy_pointHandler::CurrentTwoPoint();
			points.push_back(p);
		}
	}
	
	int nPoints=points.size();
	for (int i=0;i<nPoints;i++)
		vol->SetPoint(points[i]);
	
	std::string col=getAttributeAsString(c, t, "color",res);
	if (res)
		vol->SetColor(col);
}
