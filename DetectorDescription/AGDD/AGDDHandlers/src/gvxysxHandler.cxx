/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDDHandlers/gvxysxHandler.h"
#include "AGDDControl/XercesParser.h"
#include "AGDDModel/AGDDGvxy.h"
#include "AGDDHandlers/gvxy_pointHandler.h"
#include <iostream>

using namespace xercesc;

gvxysxHandler::gvxysxHandler(const std::string& s,
                             AGDDController& c)
  : XMLHandler(s, c)
{
//	std::cout<<"Creating handler for gvxysx"<<std::endl;
}

void gvxysxHandler::ElementHandle(AGDDController& c,
                                  xercesc::DOMNode *t)
{
	bool res;
	std::string name=getAttributeAsString(c, t, "name");
	std::string material=getAttributeAsString(c, t, "material");
	double dZ=getAttributeAsDouble(c, t, "dZ");
	
	AGDDGvxy *vol=new AGDDGvxy(name);
 	vol->SetMaterial(material);
 	vol->SetDz(dZ);
	
	std::vector<double> xvalues=getAttributeAsVector(c, t, "X",res);
	
	if (res)
	{
		std::vector<double> yvalues=getAttributeAsVector(c, t, "Y");
		
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
		for (child=t->getFirstChild();child!=0;child=child->getNextSibling())
		{
			if (child->getNodeType()==DOMNode::ELEMENT_NODE) {
				XercesParser::elementLoop(c, child);
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
	
	std::string col=getAttributeAsString(c, t, "color",res);
	if (res)
		vol->SetColor(col);
}
