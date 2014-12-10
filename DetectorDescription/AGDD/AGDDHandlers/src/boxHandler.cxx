/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDDHandlers/boxHandler.h"
#include "AGDDModel/AGDDBox.h"
#include <iostream>


boxHandler::boxHandler(std::string s):XMLHandler(s)
{
}

void boxHandler::ElementHandle()
{
//	std::cout<<"this is boxHandler"<<std::endl;
	bool res;
	std::string name=getAttributeAsString("name");
//	std::cout<<"   name "<<name<<std::endl;
	std::string material=getAttributeAsString("material");
//	std::cout<<"   material "<<material<<std::endl;
	std::vector<double> vvv=getAttributeAsVector("X_Y_Z");

	AGDDBox *b=new AGDDBox(name);
	b->SetMaterial(material);
	b->SetXYZ(vvv);
//	std::cout<<"   dims "<<vvv[0]<<" "<<vvv[1]<<" "<<vvv[2]<<std::endl;
	
	std::string col=getAttributeAsString("color",res);
	if (res)
		b->SetColor(col);
//	std::cout<<" exiting boxHandler"<<std::endl;
}
