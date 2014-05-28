/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDD2Geo/subtractionHandler.h"
#include "AGDD2Geo/XercesParser.h"
#include "AGDD2Geo/AGDDSubtraction.h"
#include <iostream>
#include "AGDD2Geo/AGDDPositioner.h"
#include "AGDD2Geo/AGDDVolumeStore.h"
#include "AGDD2Geo/AGDDPositionerStore.h"

subtractionHandler::subtractionHandler(std::string s):XMLHandler(s)
{
//	std::cout<<"Creating handler for composition"<<std::endl;
}

void subtractionHandler::ElementHandle()
{
	bool res;
	std::string name=getAttributeAsString("name",res);
	AGDDSubtraction *c=new AGDDSubtraction(name);
	if (msgLog().level()<=MSG::DEBUG)
	msgLog()<<MSG::DEBUG<<"subtraction name "<<name<<endreq;
	
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
