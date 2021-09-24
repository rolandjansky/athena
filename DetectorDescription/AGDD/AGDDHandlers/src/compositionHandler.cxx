/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDDHandlers/compositionHandler.h"
#include "AGDDModel/AGDDComposition.h"
#include "AGDDControl/XercesParser.h"
#include "AGDDControl/AGDDController.h"
#include <iostream>
#include "AGDDKernel/AGDDPositioner.h"
#include "AGDDKernel/AGDDPositionerStore.h"
#include "AGDDKernel/AGDDVolumeStore.h"


using namespace xercesc;

compositionHandler::compositionHandler(const std::string& s,
                                       AGDDController& c)
  : XMLHandler(s, c)
{
}

void compositionHandler::ElementHandle(AGDDController& c,
                                       xercesc::DOMNode *t)
{
	std::string name=getAttributeAsString(c, t, "name");
//	if (msgLog().level()<=MSG::DEBUG)
//	msgLog()<<MSG::DEBUG<<" Composition "<<name<<endmsg;
	
	AGDDComposition *cm=new AGDDComposition(name);
	
	AGDDPositionerStore* pS=AGDDPositionerStore::GetPositionerStore();
	
	StopLoop(true);
	DOMNode *child;
	
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
			cm->AddDaughter(posit);
	}
	
}
