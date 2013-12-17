/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FadsMaterial/FadsSimpleMaterial.h"
#include "FadsMaterial/MaterialManager.h"

namespace FADS {

FadsSimpleMaterial* FadsSimpleMaterial::currentMaterial=0;

FadsSimpleMaterial* FadsSimpleMaterial::GetCurrentMaterial()
{
	return currentMaterial;
}

FadsSimpleMaterial::FadsSimpleMaterial(std::string n,double d,State sta,
				double temp,double pressure): 
				name_(n),density_(d),state(sta),
		    	temperature_(temp),pressure_(pressure)
{
	theMaterial=0;
	matManager=MaterialManager::GetMaterialManager();
	matManager->StoreMaterial(this);
	currentMaterial=this;
}

G4Material* FadsSimpleMaterial::GetG4Material()
{
	if (!theMaterial) CreateG4Material();
	return theMaterial;
}

FadsSimpleMaterial::~FadsSimpleMaterial()
{
}

std::string FadsSimpleMaterial::name()
{
	return name_;
}

double FadsSimpleMaterial::density()
{
	return density_;
}

}	// end namespace
