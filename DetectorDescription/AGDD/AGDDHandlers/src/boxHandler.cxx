/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDDHandlers/boxHandler.h"
#include "AGDDModel/AGDDBox.h"
#include "AGDDControl/AGDDController.h"
#include <iostream>


boxHandler::boxHandler(const std::string& s,
                       AGDDController& c)
  : XMLHandler(s, c)
{
}

void boxHandler::ElementHandle(AGDDController& c,
                               xercesc::DOMNode *t)
{
//	std::cout<<"this is boxHandler"<<std::endl;
	bool res;
	std::string name=getAttributeAsString(c, t, "name");
//	std::cout<<"   name "<<name<<std::endl;
	std::string material=getAttributeAsString(c, t, "material");
//	std::cout<<"   material "<<material<<std::endl;
	std::vector<double> vvv=getAttributeAsVector(c, t, "X_Y_Z");

	AGDDBox *b=new AGDDBox(name, c.GetVolumeStore(), c.GetSectionStore());
	b->SetMaterial(material);
	b->SetXYZ(vvv);
//	std::cout<<"   dims "<<vvv[0]<<" "<<vvv[1]<<" "<<vvv[2]<<std::endl;
	
	std::string col=getAttributeAsString(c, t, "color",res);
	if (res)
		b->SetColor(col);
//	std::cout<<" exiting boxHandler"<<std::endl;
}
