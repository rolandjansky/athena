/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDD2Geo/intersectionHandler.h"
#include "AGDD2Geo/XercesParser.h"
#include "AGDD2Geo/AGDDIntersection.h"
#include <iostream>
#include "AGDD2Geo/AGDDPositioner.h"
#include "AGDD2Geo/AGDDVolumeStore.h"
#include "AGDD2Geo/AGDDPositionerStore.h"

intersectionHandler::intersectionHandler(std::string s):XMLHandler(s)
{
}

void intersectionHandler::ElementHandle()
{
	bool res;
	std::string name=getAttributeAsString("name",res);
	if (msgLog().level()<=MSG::DEBUG)
	msgLog()<<MSG::DEBUG<<"intersection name "<<name<<endreq;
	AGDDIntersection *c=new AGDDIntersection(name);
	
	AGDDPositionerStore* pS=AGDDPositionerStore::GetPositionerStore();
	
	StopLoop(true);

        DOMNode* child;

        int before=pS->NrOfPositioners();

        const DOMNode* cElement=XercesParser::GetCurrentElement();
        for (child=cElement->getFirstChild();child!=0;child=child->getNextSibling())
        {
               if (child->getNodeType()==DOMNode::ELEMENT_NODE) {
               XercesParser::elementLoop(child);
               }
        }
	
	int after=pS->NrOfPositioners();
	for (int i=before;i<after;i++)
	{
		AGDDPositioner *posit=pS->GetPositioner(i);
		if (AGDDVolumeStore::GetVolumeStore()->Exist(posit->Volume()))
			c->AddDaughter(posit);
	}
	
	std::string col=getAttributeAsString("color",res);
	if (res)
		c->SetColor(col);
}
