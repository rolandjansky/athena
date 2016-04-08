/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDDHandlers/gvxysxHandler.h"
#include "AGDDControl/XercesParser.h"
#include "AGDDModel/AGDDGvxy.h"
#include "AGDDHandlers/gvxy_pointHandler.h"
#include <iostream>

using namespace xercesc;

gvxysxHandler::gvxysxHandler(std::string s):XMLHandler(s)
{
//	std::cout<<"Creating handler for gvxysx"<<std::endl;
}

void gvxysxHandler::ElementHandle()
{
	bool res;
	std::string name=getAttributeAsString("name");
	std::string material=getAttributeAsString("material");
	double dZ=getAttributeAsDouble("dZ");
	
	AGDDGvxy *vol=new AGDDGvxy(name);
 	vol->SetMaterial(material);
 	vol->SetDz(dZ);
	
	std::vector<double> xvalues=getAttributeAsVector("X",res);
	
	if (res)
	{
		std::vector<double> yvalues=getAttributeAsVector("Y");
		
		// check we have a consistent set of points
		if(xvalues.size() != yvalues.size()) throw;
		int nPoints=xvalues.size();
	
		TwoPoint* v=new TwoPoint[2*nPoints];
		for (int i=0;i<nPoints;i++)
		{
			v[i].x(xvalues[i]);
			v[i].y(yvalues[i]);
			v[2*nPoints-i-1].x(-xvalues[i]);
			v[2*nPoints-i-1].y(yvalues[i]);
		}
	
		for (int i=0;i<2*nPoints;i++)
		{
			vol->SetPoint(v[i]);
		}
		delete[] v;
	}
	else
	{
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
		
		TwoPoint* v=new TwoPoint[2*nPoints];
		for (int i=0;i<nPoints;i++)
		{
			v[i].x(points[i].x());
			v[i].y(points[i].y());
			v[2*nPoints-i-1].x(-points[i].x());
			v[2*nPoints-i-1].y(points[i].y());
		}
	
		for (int i=0;i<2*nPoints;i++)
		{
			vol->SetPoint(v[i]);
		}
		delete [] v;
	}
	
	std::string col=getAttributeAsString("color",res);
	if (res)
		vol->SetColor(col);
}
