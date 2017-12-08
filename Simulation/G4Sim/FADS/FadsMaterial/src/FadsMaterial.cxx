/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FadsMaterial/FadsMaterial.h"

namespace FADS {

FadsMaterial::FadsMaterial(std::string n,double z,double a,double density,
		    State sta,double temp,double pressure): 
		    FadsSimpleMaterial(n,density,sta,temp,pressure),
		    m_z(z),m_a(a)
{
}
void FadsMaterial::CreateG4Material()
{
	m_theMaterial=new G4Material(m_name,m_z,m_a,m_density,(G4State)m_state,
                                     m_temperature,m_pressure);
}

}	// end namespace
