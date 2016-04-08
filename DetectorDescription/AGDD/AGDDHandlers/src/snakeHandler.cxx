/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDDHandlers/snakeHandler.h"
#include "AGDDControl/XercesParser.h"
#include "AGDDHandlers/snake_pointHandler.h"
#include "AGDDModel/AGDDSnake.h"
#include <iostream>

#include "CLHEP/Vector/ThreeVector.h"

#include <vector>

using namespace xercesc;

snakeHandler::snakeHandler(std::string s):XMLHandler(s)
{
//	std::cout<<"Creating handler for snake"<<std::endl;
}

void snakeHandler::ElementHandle()
{
	bool res;
	std::string name=getAttributeAsString("name",res);
	std::string material=getAttributeAsString("material",res);
	double radius=getAttributeAsDouble("radius",res);
	
	std::vector<CLHEP::Hep3Vector> points;
	
	AGDDSnake *vol=new AGDDSnake(name);
 	vol->SetMaterial(material);
	vol->Radius(radius);	
	
	StopLoop(true);
	DOMNode* child;

	const DOMNode* cElement=XercesParser::GetCurrentElement();
    for (child=cElement->getFirstChild();child!=0;child=child->getNextSibling())
    {
        if (child->getNodeType()==DOMNode::ELEMENT_NODE) {
        XercesParser::elementLoop(child);
		CLHEP::Hep3Vector p=snake_pointHandler::CurrentPoint();
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
