/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4AtlasControl/MaterialFacade.h"

#include "FadsMaterial/FadsMaterial.h"
#include "FadsMaterial/FadsSimpleMaterial.h"
#include "FadsMaterial/FadsMolecule.h"
#include "FadsMaterial/FadsMixture.h"
#include "G4MaterialTable.hh"
#include "G4ElementTable.hh"
#include "G4Element.hh"

Material::Material(const std::string,double, double, double)
{
// Empty constructor added by DRQ in order to satisfy --as-needed --no-undefined linkopts
}

void Material::List()
{
	const G4MaterialTable *et=G4Material::GetMaterialTable();
    std::cout<<" Number of Materials currently defined: "<<
             G4Material::GetNumberOfMaterials()<<std::endl<<std::endl;
    for (unsigned int i=0;i<et->size();i++)
             std::cout << "Material   "<<*((*et)[i])<<std::endl;

}

CompositeMaterial::CompositeMaterial(const std::string n,double d):sm(0),
							name(n),density(d)
{
}

void CompositeMaterial::AddMaterial(const std::string n,double fractionMass)
{
	if (!sm) 
		sm=new FADS::FadsMixture(name,density);
	else
	{
		if (!dynamic_cast<FADS::FadsMixture *>(sm))
		{
			std::cout<<" CompositeMaterial::AddMaterial "<<
				   ": trying to add a material to not-mixture"<<
				   std::endl;
			return;
		}
	}
	if (sm){
		FADS::FadsMixture *mixt=dynamic_cast<FADS::FadsMixture *>(sm);
		if (mixt) mixt->AddMaterial(n,fractionMass);		  
	}
}

void CompositeMaterial::AddElement(const std::string n,int nAtoms)
{
	if (!sm) 
		sm=new FADS::FadsMolecule(name,density);
	else
	{
		if (!dynamic_cast<FADS::FadsMolecule *>(sm))
		{
			std::cout<<" CompositeMaterial::AddElement "<<
				   ": trying to add an element to not-molecule"<<
				   std::endl;
			return;
		}
	}
	if (sm){
		FADS::FadsMolecule *mol=dynamic_cast<FADS::FadsMolecule *>(sm);
		if (mol) mol->AddElement(n,nAtoms);
	}
}
