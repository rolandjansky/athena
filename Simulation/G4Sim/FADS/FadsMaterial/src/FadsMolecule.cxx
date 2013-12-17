/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FadsMaterial/FadsMolecule.h"

namespace FADS {

FadsMolecule::FadsMolecule(std::string n,double density,int nc,
	    State sta,double temp,double pressure): 
	    FadsSimpleMaterial(n,density,sta,temp,pressure),
	    ncomponents_(nc),nadded_(0)
{
}
void FadsMolecule::CreateG4Material()
{
// 	std::cout<<"FadsMolecule::CreateG4Material for "<<name_<<std::endl;
	if (ncomponents_>nadded_) 
		std::cout<<"the material is not complete!"<<std::endl;
	else
	{
// 		std::cout<<" nadded "<<nadded_<<std::endl;
// 		std::cout<<" name,dens "<<name_<<" "<<density_<<std::endl;
		theMaterial=new G4Material(name_,density_,nadded_,
				(G4State)state,temperature_,pressure_);
		for (int i=0;i<nadded_;i++)
		{
			std::string nam=elements[i].name;
			int num=elements[i].natoms;
// 			std::cout<<" getting -"<<nam<<"-"<<std::endl;
			G4Element *el=matManager->GetElement(nam);
// 			std::cout<<i<<" Add "<<num<<" atoms of "<<el<<std::endl;
			theMaterial->AddElement(el,num);
		}
	}
}
void FadsMolecule::AddElement(std::string nam, int natoms)
{
// 	std::cout<<" This is FadsMolecule::AddElement!!! "<<nam<<std::endl;
	if (matManager->FindElement(nam))
	{
// 		std::cout<<" Found! "<<std::endl;
		Element el={nam,natoms};
		elements.push_back(el);
		nadded_++;
	}
}

}	// end namespace
