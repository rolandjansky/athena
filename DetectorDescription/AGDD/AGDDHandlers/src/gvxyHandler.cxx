/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDDHandlers/gvxyHandler.h"
#include "AGDDModel/AGDDGvxy.h"
#include "AGDDHandlers/gvxy_pointHandler.h"
#include "AGDDControl/XercesParser.h"
#include "AGDDControl/XMLHandlerStore.h"
#include "AGDDControl/AGDDController.h"
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
	
 	AGDDGvxy *vol=new AGDDGvxy(name, c.GetVolumeStore(), c.GetSectionStore());
 	vol->SetMaterial(material);
 	vol->SetDz(dZ);

	std::vector<TwoPoint> points;	
	StopLoop(true);	
	
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
	for (int i=0;i<nPoints;i++)
		vol->SetPoint(points[i]);
	
	std::string col=getAttributeAsString(c, t, "color",res);
	if (res)
		vol->SetColor(col);
}
