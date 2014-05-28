/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDD2Geo/compositeHandler.h"
#include "AGDD2Geo/XercesParser.h"
#include "AGDD2Geo/AddMaterial.h"
#include "AGDD2Geo/AGDDMolecule.h"
#include "AGDD2Geo/AGDDMixture.h"
#include "AGDD2Geo/globals.h"
#include <iostream>

compositeHandler::compositeHandler(std::string s):XMLHandler(s)
{
}

void compositeHandler::ElementHandle()
{
	bool res;
	globals::addMaterial.Clear();

	std::string name=getAttributeAsString("name",res);
	double density=getAttributeAsDouble("density",res);
	if (msgLog().level()<=MSG::DEBUG)
	msgLog()<<MSG::DEBUG<<"composite name "<<name<<" density="<<density<<endreq;
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
		    if (msgLog().level()<=MSG::DEBUG)
			msgLog()<<MSG::DEBUG<<" \tMolecule: Adding "<<globals::addMaterial.names[i]
				<<" natoms "<<globals::addMaterial.natoms[i]<<endreq;
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
		    if (msgLog().level()<=MSG::DEBUG)
			msgLog()<<MSG::DEBUG<<" \tMixture: Adding "<<globals::addMaterial.names[i]
				<<" fraction "<<globals::addMaterial.fractions[i]<<endreq;
			m->AddMaterial(globals::addMaterial.names[i]);
			m->Fraction(globals::addMaterial.fractions[i]);
		}
		
	}
	else
	    if (msgLog().level()<=MSG::DEBUG)
		msgLog()<<MSG::ERROR<<"Something wrong: "<<name<<" neither molecule nor mixture!!!"<<endreq;
	
	
}
