/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDDHandlers/snakeHandler.h"
#include "AGDDControl/XercesParser.h"
#include "AGDDHandlers/snake_pointHandler.h"
#include "AGDDModel/AGDDSnake.h"

#include <iostream>
#include <vector>

using namespace xercesc;

snakeHandler::snakeHandler(const std::string& s,
                           AGDDController& c)
  : XMLHandler(s, c)
{
}

void snakeHandler::ElementHandle(AGDDController& c,
                                 xercesc::DOMNode *t)
{
	bool res=false;
	std::string name=getAttributeAsString(c, t, "name",res);
	std::string material=getAttributeAsString(c, t, "material",res);
	double radius=getAttributeAsDouble(c, t, "radius",res);
	
	std::vector<GeoTrf::Vector3D> points;
	
	AGDDSnake *vol=new AGDDSnake(name);
 	vol->SetMaterial(material);
	vol->Radius(radius);	
	
	StopLoop(true);
	DOMNode* child;

    for (child=t->getFirstChild();child!=0;child=child->getNextSibling())
    {
        if (child->getNodeType()==DOMNode::ELEMENT_NODE) {
                XercesParser::elementLoop(c, child);
		GeoTrf::Vector3D p=snake_pointHandler::CurrentPoint();
		points.push_back(p);
       }
    }
	
	int nPoints=points.size();
	for (int i=0;i<nPoints;i++)
		vol->SetPoint(points[i]);
	
	std::string col=getAttributeAsString(c, t,"color",res);
	if (res)
		vol->SetColor(col);
	
}
