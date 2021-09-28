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
	bool res;
	std::string name=getAttributeAsString(c, t, "name");
	AGDDIntersection *is=new AGDDIntersection(name);
	
	AGDDPositionerStore* pS=AGDDPositionerStore::GetPositionerStore();
	
	StopLoop(true);

        DOMNode* child;

        int before=pS->NrOfPositioners();

        for (child=t->getFirstChild();child!=0;child=child->getNextSibling())
        {
               if (child->getNodeType()==DOMNode::ELEMENT_NODE) {
                 XercesParser::elementLoop(c, child);
               }
        }
	
	int after=pS->NrOfPositioners();
	for (int i=before;i<after;i++)
	{
		AGDDPositioner *posit=pS->GetPositioner(i);
		if (AGDDVolumeStore::GetVolumeStore()->Exist(posit->Volume()))
			is->AddDaughter(posit);
	}
	
	std::string col=getAttributeAsString(c, t, "color",res);
	if (res)
		is->SetColor(col);
}
