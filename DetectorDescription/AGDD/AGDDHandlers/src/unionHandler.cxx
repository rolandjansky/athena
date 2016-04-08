/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDDHandlers/unionHandler.h"
#include "AGDDControl/XercesParser.h"
#include "AGDDModel/AGDDUnion.h"
#include <iostream>
#include "AGDDKernel/AGDDPositioner.h"
#include "AGDDKernel/AGDDVolumeStore.h"
#include "AGDDKernel/AGDDPositionerStore.h"

using namespace xercesc;

unionHandler::unionHandler(std::string s):XMLHandler(s)
{
//	std::cout<<"Creating handler for composition"<<std::endl;
}

void unionHandler::ElementHandle()
{
	bool res;
	std::string name=getAttributeAsString("name",res);
	
	AGDDUnion *c=new AGDDUnion(name);
	
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
