/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FadsMaterial/FadsMaterial.h"

namespace FADS {

FadsMaterial::FadsMaterial(std::string n,double z,double a,double density,
		    State sta,double temp,double pressure): 
		    FadsSimpleMaterial(n,density,sta,temp,pressure),
		    z_(z),a_(a)
{
}
void FadsMaterial::CreateG4Material()
{
	theMaterial=new G4Material(name_,z_,a_,density_,(G4State)state,
				   temperature_,pressure_);
}

}	// end namespace
