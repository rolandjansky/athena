/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDDHandlers/gvxysxyHandler.h"
#include "AGDDControl/XercesParser.h"
#include "AGDDControl/XMLHandlerStore.h"
#include "AGDDControl/AGDDController.h"
#include "AGDDModel/AGDDGvxy.h"
#include "AGDDHandlers/gvxy_pointHandler.h"
#include <iostream>

using namespace xercesc;

gvxysxyHandler::gvxysxyHandler(const std::string& s,
                               AGDDController& c)
  : XMLHandler(s, c)
{
//	std::cout<<"Creating handler for gvxy"<<std::endl;
}

void gvxysxyHandler::ElementHandle(AGDDController& c,
                                   xercesc::DOMNode *t)
{
	bool res;
	std::string name=getAttributeAsString(c, t, "name");
	std::string material=getAttributeAsString(c, t, "material");
	double dZ=getAttributeAsDouble(c, t, "dZ");
	
	AGDDGvxy *vol=new AGDDGvxy(name, c.GetVolumeStore(), c.GetSectionStore());
 	vol->SetMaterial(material);
 	vol->SetDz(dZ);
	
	StopLoop(true);

	std::vector<TwoPoint> points;	
	
        gvxy_pointHandler* pointHand = dynamic_cast<gvxy_pointHandler*>
          (c.GetHandlerStore().GetHandler("gvxy_point"));
        if (!pointHand) std::abort();

        IAGDDParser& parser = *c.GetParser();
	DOMNode *child;
	for (child=t->getFirstChild();child!=0;child=child->getNextSibling())
	{
		if (child->getNodeType()==DOMNode::ELEMENT_NODE) {
			parser.elementLoop(c, child);
			TwoPoint p = pointHand->CurrentTwoPoint();
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
	
	std::string col=getAttributeAsString(c, t, "color",res);
	if (res)
		vol->SetColor(col);
}
