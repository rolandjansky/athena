/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDDHandlers/pconHandler.h"
#include "AGDDControl/XercesParser.h"
#include "AGDDModel/AGDDPcon.h"
#include "AGDDHandlers/polyplaneHandler.h"
#include <iostream>


using namespace xercesc;

pconHandler::pconHandler(std::string s):XMLHandler(s)
{
}

void pconHandler::ElementHandle()
{
	bool res;
	std::string name=getAttributeAsString("name",res);
	std::string material=getAttributeAsString("material",res);
	
 	AGDDPcon *vol=new AGDDPcon(name);
 	vol->SetMaterial(material);

	std::string col=getAttributeAsString("color",res);
	if (res)
		vol->SetColor(col);
		
	std::vector<double> vvv=getAttributeAsVector("profile",res);
	if (res)
		vol->SetProfile(vvv[0],vvv[1]);
	else
		vol->SetProfile(0.,360.);
	
	StopLoop(true);
	DOMNode* child;

	const DOMNode* cElement=XercesParser::GetCurrentElement();
        for (child=cElement->getFirstChild();child!=0;child=child->getNextSibling())
        {
                if (child->getNodeType()==DOMNode::ELEMENT_NODE) {
                XercesParser::elementLoop(child);
		Polyplane p=polyplaneHandler::CurrentPolyplane();
                vol->SetPlane(p.rin(),p.rou(),p.z());
                }
        }

	
}
