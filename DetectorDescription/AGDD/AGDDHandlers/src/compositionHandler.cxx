/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDDHandlers/compositionHandler.h"
#include "AGDDModel/AGDDComposition.h"
#include "AGDDControl/XercesParser.h"
#include <iostream>
#include "AGDDKernel/AGDDPositioner.h"
#include "AGDDKernel/AGDDPositionerStore.h"
#include "AGDDKernel/AGDDVolumeStore.h"


using namespace xercesc;

compositionHandler::compositionHandler(std::string s):XMLHandler(s)
{
}

void compositionHandler::ElementHandle()
{
	std::string name=getAttributeAsString("name");
//	if (msgLog().level()<=MSG::DEBUG)
//	msgLog()<<MSG::DEBUG<<" Composition "<<name<<endreq;
	
	AGDDComposition *c=new AGDDComposition(name);
	
	AGDDPositionerStore* pS=AGDDPositionerStore::GetPositionerStore();
	
	StopLoop(true);
	DOMNode *child;
	
	int before=pS->NrOfPositioners();
	
	for (child=XercesParser::GetCurrentElement()->getFirstChild();child!=0;child=child->getNextSibling())
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
	
}
