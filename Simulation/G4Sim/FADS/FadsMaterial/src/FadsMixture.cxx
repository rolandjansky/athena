/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FadsMaterial/FadsMixture.h"

namespace FADS {

FadsMixture::FadsMixture(std::string n,double density,int nc,
	    State sta,double temp,double pressure): 
	    FadsSimpleMaterial(n,density,sta,temp,pressure),
	    ncomponents_(nc),nadded_(0)
{
}
void FadsMixture::CreateG4Material()
{
	if (ncomponents_>nadded_) 
		std::cout<<"the material is not complete!"<<std::endl;
	else
	{
		theMaterial=new G4Material(name_,density_,nadded_,
				(G4State)state,temperature_,pressure_);
		for (int i=0;i<nadded_;i++)
		{
			Type t=components[i].type;
			std::string nam=components[i].name;
			double f=components[i].fraction;
			if (t==Mat)
			{
				G4Material *el=matManager->GetMaterial(nam);
				theMaterial->AddMaterial(el,f);
			}
			else if (t==Elem)
			{
				G4Element *el=matManager->GetElement(nam);
				theMaterial->AddElement(el,f);
			}
		}
	}
}
void FadsMixture::AddMaterial(std::string nam, double fract)
{
	if (matManager->FindMaterial(nam))
	{
//		std::cout<<"FadsMixture::AddMaterial adding "<<nam<<std::endl;
		MaterialComponent el={Mat,nam,fract};
		components.push_back(el);
		nadded_++;
	}
}
void FadsMixture::AddElement(std::string nam, double fract)
{
	if (matManager->FindElement(nam))
	{
		MaterialComponent el={Elem,nam,fract};
		components.push_back(el);
		nadded_++;
	}
}

}	// end namespace
