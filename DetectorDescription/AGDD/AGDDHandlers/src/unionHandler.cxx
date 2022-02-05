/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDDHandlers/unionHandler.h"
#include "AGDDControl/XercesParser.h"
#include "AGDDControl/AGDDController.h"
#include "AGDDModel/AGDDUnion.h"
#include <iostream>
#include "AGDDKernel/AGDDPositioner.h"
#include "AGDDKernel/AGDDVolumeStore.h"
#include "AGDDKernel/AGDDPositionerStore.h"

using namespace xercesc;

unionHandler::unionHandler(const std::string& s,
                           AGDDController& c)
  : XMLHandler(s, c)
{
//	std::cout<<"Creating handler for composition"<<std::endl;
}

void unionHandler::ElementHandle(AGDDController& c,
                                 xercesc::DOMNode *t)
{
    AGDDVolumeStore& vs = c.GetVolumeStore();
    AGDDSectionStore& ss = c.GetSectionStore();
    AGDDPositionerStore& ps = c.GetPositionerStore();

    bool res;
    std::string name=getAttributeAsString(c, t, "name",res);
	
    AGDDUnion *u=new AGDDUnion(name, vs, ss);
	
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
        u->AddDaughter(posit);
    }
	
    std::string col=getAttributeAsString(c, t, "color",res);
    if (res)
      u->SetColor(col);
}
