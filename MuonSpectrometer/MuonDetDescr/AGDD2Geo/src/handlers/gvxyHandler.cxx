/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDD2Geo/gvxyHandler.h"
#include "AGDD2Geo/XercesParser.h"
#include "AGDD2Geo/AGDDGvxy.h"
#include "AGDD2Geo/gvxy_pointHandler.h"
#include <iostream>

gvxyHandler::gvxyHandler(std::string s):XMLHandler(s)
{
}

void gvxyHandler::ElementHandle()
{
	bool res;
	std::string name=getAttributeAsString("name",res);
	std::string material=getAttributeAsString("material",res);
	double dZ=getAttributeAsDouble("dZ",res);
	if (msgLog().level()<=MSG::DEBUG)
    msgLog()<<MSG::DEBUG<<"gvxy name "<<name<<" material="<<material<<" dZ "<<dZ<<endreq;
	
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
