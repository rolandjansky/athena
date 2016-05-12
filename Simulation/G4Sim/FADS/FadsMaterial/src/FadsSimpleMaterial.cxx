/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FadsMaterial/FadsSimpleMaterial.h"
#include "FadsMaterial/MaterialManager.h"

namespace FADS {

FadsSimpleMaterial* FadsSimpleMaterial::s_currentMaterial=0;

FadsSimpleMaterial* FadsSimpleMaterial::GetCurrentMaterial()
{
	return s_currentMaterial;
}

FadsSimpleMaterial::FadsSimpleMaterial(std::string n,double d,State sta,
				double temp,double pressure): 
				m_name(n),m_density(d),m_state(sta),
		    	m_temperature(temp),m_pressure(pressure)
{
	m_theMaterial=0;
	m_matManager=MaterialManager::GetMaterialManager();
	m_matManager->StoreMaterial(this);
	s_currentMaterial=this;
}

G4Material* FadsSimpleMaterial::GetG4Material()
{
	if (!m_theMaterial) CreateG4Material();
	return m_theMaterial;
}

FadsSimpleMaterial::~FadsSimpleMaterial()
{
}

std::string FadsSimpleMaterial::name()
{
	return m_name;
}

double FadsSimpleMaterial::density()
{
	return m_density;
}

}	// end namespace
