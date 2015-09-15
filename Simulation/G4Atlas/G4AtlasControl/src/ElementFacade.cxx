/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4AtlasControl/ElementFacade.h"

#include "FadsMaterial/FadsElement.h"
#include "FadsMaterial/FadsMaterial.h"

#include "G4ElementTable.hh"
#include "G4Element.hh"

Element::Element(const std::string name, const std::string symbol, double z, double a,double density)
{
	FADS::FadsElement *el __attribute ((unused)) =new FADS::FadsElement(name,symbol,z,a);
	if (density>0) 
	{
		FADS::FadsMaterial *mat __attribute ((unused)) =new FADS::FadsMaterial(name,z,a,density);
	}
}

void Element::List()
{
	const G4ElementTable *et=G4Element::GetElementTable();
	std::cout<<" Number of Elements currently defined: "<<
		G4Element::GetNumberOfElements()<<std::endl<<std::endl;
	for (unsigned int i=0;i<et->size();i++)
		std::cout << "Element   "<<*((*et)[i])<<std::endl;
}

