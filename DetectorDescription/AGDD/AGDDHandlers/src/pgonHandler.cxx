/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDDHandlers/pgonHandler.h"
#include "AGDDControl/XercesParser.h"
#include "AGDDModel/AGDDPgon.h"
#include "AGDDHandlers/polyplaneHandler.h"
#include <iostream>

using namespace xercesc;

pgonHandler::pgonHandler(std::string s):XMLHandler(s)
{
//	std::cout<<"Creating handler for pgon"<<std::endl;
}

void pgonHandler::ElementHandle()
{
//	std::cout<<"handling for pgon";
	bool res;
	std::string name=getAttributeAsString("name",res);
	std::string material=getAttributeAsString("material",res);
	int nbPhi=getAttributeAsInt("nbPhi",res);
	
 	AGDDPgon *vol=new AGDDPgon(name);
 	vol->SetMaterial(material);
	vol->SetNbPhi(nbPhi);

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
