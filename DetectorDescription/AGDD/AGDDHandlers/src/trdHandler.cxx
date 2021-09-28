/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDDHandlers/trdHandler.h"
#include "AGDDControl/AGDDController.h"
#include "AGDDModel/AGDDTrd.h"
#include <iostream>


trdHandler::trdHandler(const std::string& s,
                       AGDDController& c)
  : XMLHandler(s, c)
{
//	std::cout<<"Creating handler for trd"<<std::endl;
}

void trdHandler::ElementHandle(AGDDController& c,
                               xercesc::DOMNode *t)
{
	bool res;
	std::string name=getAttributeAsString(c, t, "name",res);
	std::string material=getAttributeAsString(c, t, "material",res);
	std::vector<double> vvv=getAttributeAsVector(c, t, "Xmp_Ymp_Z",res);
	AGDDTrd *b=new AGDDTrd(name, c.GetVolumeStore(), c.GetSectionStore());
	b->SetMaterial(material);
	b->SetXYZ(vvv);
	
	std::string col=getAttributeAsString(c, t, "color",res);
	if (res)
		b->SetColor(col);
}
