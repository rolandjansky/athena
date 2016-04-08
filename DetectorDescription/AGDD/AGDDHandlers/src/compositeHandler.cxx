/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDDHandlers/compositeHandler.h"
#include "AGDDControl/XercesParser.h"
#include "AGDDHandlers/AddMaterial.h"
#include "AGDDModel/AGDDMolecule.h"
#include "AGDDModel/AGDDMixture.h"
#include "AGDDHandlers/globals.h"
#include <iostream>

using namespace xercesc;

compositeHandler::compositeHandler(std::string s):XMLHandler(s)
{
}

void compositeHandler::ElementHandle()
{
	globals::addMaterial.Clear();

	std::string name=getAttributeAsString("name");
	double density=getAttributeAsDouble("density");
	StopLoop(true);
	
	DOMNode* child;

	const DOMNode* cElement=XercesParser::GetCurrentElement();
    for (child=cElement->getFirstChild();child!=0;child=child->getNextSibling())
    {
        if (child->getNodeType()==DOMNode::ELEMENT_NODE) {
        XercesParser::elementLoop(child);

        }
    }
	

	if (globals::addMaterial.natoms.size())
	{
		// it's a molecule
		AGDDMolecule *m=new AGDDMolecule(name,density);
		for (unsigned int i=0;i<globals::addMaterial.natoms.size();i++)
		{
			m->AddElement(globals::addMaterial.names[i]);
			m->NAtoms(globals::addMaterial.natoms[i]);
		}
	}
	else if (globals::addMaterial.fractions.size())
	{
		// it's a mixture
		AGDDMixture *m=new AGDDMixture(name,density);
		for (unsigned int i=0;i<globals::addMaterial.fractions.size();i++)
		{
			m->AddMaterial(globals::addMaterial.names[i]);
			m->Fraction(globals::addMaterial.fractions[i]);
		}
		
	}
	else
	    std::cout<<"Something wrong: "<<name<<" neither molecule nor mixture!!!"<<std::endl;
	
	
}
