/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDDHandlers/gvxysxyHandler.h"
#include "AGDDControl/XercesParser.h"
#include "AGDDModel/AGDDGvxy.h"
#include "AGDDHandlers/gvxy_pointHandler.h"
#include <iostream>

using namespace xercesc;

gvxysxyHandler::gvxysxyHandler(std::string s):XMLHandler(s)
{
//	std::cout<<"Creating handler for gvxy"<<std::endl;
}

void gvxysxyHandler::ElementHandle()
{
	bool res;
	std::string name=getAttributeAsString("name");
	std::string material=getAttributeAsString("material");
	double dZ=getAttributeAsDouble("dZ");
	
 	AGDDGvxy *vol=new AGDDGvxy(name);
 	vol->SetMaterial(material);
 	vol->SetDz(dZ);
	
	StopLoop(true);

	std::vector<TwoPoint> points;	
	
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
//	std::cout<<"npoints "<<nPoints<<std::endl;
	TwoPoint *v=new TwoPoint[4*nPoints];
	for (int i=0;i<nPoints;i++)
	{
		v[i]=points[i];
		v[nPoints+i].x(-points[nPoints-1-i].x());
		v[nPoints+i].y(points[nPoints-1-i].y());
		v[2*nPoints+i].x(-points[i].x());
		v[2*nPoints+i].y(-points[i].y());
		v[3*nPoints+i].x(points[nPoints-1-i].x());
		v[3*nPoints+i].y(-points[nPoints-1-i].y());
	}
	nPoints=4*nPoints;
	
	for (int i=0;i<nPoints;i++)
		vol->SetPoint(v[i]);
		
	delete[] v;
	
	std::string col=getAttributeAsString("color",res);
	if (res)
		vol->SetColor(col);
}
