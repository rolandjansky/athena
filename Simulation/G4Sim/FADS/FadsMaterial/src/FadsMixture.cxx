/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FadsMaterial/FadsMixture.h"

namespace FADS {

FadsMixture::FadsMixture(std::string n,double density,int nc,
	    State sta,double temp,double pressure): 
	    FadsSimpleMaterial(n,density,sta,temp,pressure),
	    m_ncomponents(nc),m_nadded(0)
{
}
void FadsMixture::CreateG4Material()
{
	if (m_ncomponents>m_nadded) 
		std::cout<<"the material is not complete!"<<std::endl;
	else
	{
		m_theMaterial=new G4Material(m_name,m_density,m_nadded,
				(G4State)m_state,m_temperature,m_pressure);
		for (int i=0;i<m_nadded;i++)
		{
			Type t=m_components[i].type;
			std::string nam=m_components[i].name;
			double f=m_components[i].fraction;
			if (t==Mat)
			{
				G4Material *el=m_matManager->GetMaterial(nam);
				m_theMaterial->AddMaterial(el,f);
			}
			else if (t==Elem)
			{
				G4Element *el=m_matManager->GetElement(nam);
				m_theMaterial->AddElement(el,f);
			}
		}
	}
}
void FadsMixture::AddMaterial(std::string nam, double fract)
{
	if (m_matManager->FindMaterial(nam))
	{
//		std::cout<<"FadsMixture::AddMaterial adding "<<nam<<std::endl;
		MaterialComponent el={Mat,nam,fract};
		m_components.push_back(el);
		m_nadded++;
	}
}
void FadsMixture::AddElement(std::string nam, double fract)
{
	if (m_matManager->FindElement(nam))
	{
		MaterialComponent el={Elem,nam,fract};
		m_components.push_back(el);
		m_nadded++;
	}
}

}	// end namespace
