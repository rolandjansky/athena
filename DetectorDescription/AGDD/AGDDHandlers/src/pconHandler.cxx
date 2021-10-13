/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDDHandlers/pconHandler.h"
#include "AGDDControl/XercesParser.h"
#include "AGDDControl/XMLHandlerStore.h"
#include "AGDDControl/AGDDController.h"
#include "AGDDModel/AGDDPcon.h"
#include "AGDDHandlers/polyplaneHandler.h"
#include <iostream>


using namespace xercesc;

pconHandler::pconHandler(const std::string& s,
                         AGDDController& c)
  : XMLHandler(s, c)
{
}

void pconHandler::ElementHandle(AGDDController& c,
                                xercesc::DOMNode *t)
{
	bool res;
	std::string name=getAttributeAsString(c, t, "name",res);
	std::string material=getAttributeAsString(c, t, "material",res);
	
 	AGDDPcon *vol=new AGDDPcon(name, c.GetVolumeStore(), c.GetSectionStore());
 	vol->SetMaterial(material);

	std::string col=getAttributeAsString(c, t, "color",res);
	if (res)
		vol->SetColor(col);
		
	std::vector<double> vvv=getAttributeAsVector(c, t, "profile",res);
	if (res)
		vol->SetProfile(vvv[0],vvv[1]);
	else
		vol->SetProfile(0.,360.);
	
	StopLoop(true);
	DOMNode* child;

        polyplaneHandler* pplaneHand = dynamic_cast<polyplaneHandler*>
          (c.GetHandlerStore().GetHandler("polyplane"));
        if (!pplaneHand) std::abort();

        IAGDDParser& parser = *c.GetParser();
        for (child=t->getFirstChild();child!=0;child=child->getNextSibling())
        {
                if (child->getNodeType()==DOMNode::ELEMENT_NODE) {
                  parser.elementLoop(c, child);
                  Polyplane p = pplaneHand->CurrentPolyplane();
                  vol->SetPlane(p.rin(),p.rou(),p.z());
                }
        }

	
}
