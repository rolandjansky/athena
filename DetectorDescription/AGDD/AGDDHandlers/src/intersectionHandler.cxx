/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDDHandlers/intersectionHandler.h"
#include "AGDDModel/AGDDIntersection.h"
#include "AGDDControl/XercesParser.h"
#include "AGDDControl/AGDDController.h"
#include <iostream>
#include "AGDDKernel/AGDDPositioner.h"
#include "AGDDKernel/AGDDVolumeStore.h"
#include "AGDDKernel/AGDDPositionerStore.h"

using namespace xercesc;

intersectionHandler::intersectionHandler(const std::string& s,
                                         AGDDController& c)
  : XMLHandler(s, c)
{
}

void intersectionHandler::ElementHandle(AGDDController& c,
                                        xercesc::DOMNode *t)
{
        AGDDVolumeStore& vs = c.GetVolumeStore();
        AGDDSectionStore& ss = c.GetSectionStore();
        AGDDPositionerStore& ps = c.GetPositionerStore();

	bool res;
	std::string name=getAttributeAsString(c, t, "name");
	AGDDIntersection *is = new AGDDIntersection(name, vs, ss);
	
	StopLoop(true);

        DOMNode* child;

        int before=ps.NrOfPositioners();

        IAGDDParser& parser = *c.GetParser();
        for (child=t->getFirstChild();child!=0;child=child->getNextSibling())
        {
               if (child->getNodeType()==DOMNode::ELEMENT_NODE) {
                 parser.elementLoop(c, child);
               }
        }
	
	int after=ps.NrOfPositioners();
	for (int i=before;i<after;i++)
	{
		AGDDPositioner *posit=ps.GetPositioner(i);
		if (vs.Exist(posit->Volume()))
			is->AddDaughter(posit);
	}
	
	std::string col=getAttributeAsString(c, t, "color",res);
	if (res)
		is->SetColor(col);
}
