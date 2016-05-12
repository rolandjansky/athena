/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FadsMaterial/FadsMolecule.h"

namespace FADS {

FadsMolecule::FadsMolecule(std::string n,double density,int nc,
	    State sta,double temp,double pressure): 
	    FadsSimpleMaterial(n,density,sta,temp,pressure),
	    m_ncomponents(nc),m_nadded(0)
{
}
void FadsMolecule::CreateG4Material()
{
// 	std::cout<<"FadsMolecule::CreateG4Material for "<<name_<<std::endl;
	if (m_ncomponents>m_nadded) 
		std::cout<<"the material is not complete!"<<std::endl;
	else
	{
// 		std::cout<<" nadded "<<m_nadded<<std::endl;
// 		std::cout<<" name,dens "<<name_<<" "<<density_<<std::endl;
		m_theMaterial=new G4Material(m_name,m_density,m_nadded,
				(G4State)m_state,m_temperature,m_pressure);
		for (int i=0;i<m_nadded;i++)
		{
			std::string nam=m_elements[i].m_name;
			int num=m_elements[i].m_natoms;
// 			std::cout<<" getting -"<<nam<<"-"<<std::endl;
			G4Element *el=m_matManager->GetElement(nam);
// 			std::cout<<i<<" Add "<<num<<" atoms of "<<el<<std::endl;
			m_theMaterial->AddElement(el,num);
		}
	}
}
void FadsMolecule::AddElement(std::string nam, int natoms)
{
// 	std::cout<<" This is FadsMolecule::AddElement!!! "<<nam<<std::endl;
	if (m_matManager->FindElement(nam))
	{
// 		std::cout<<" Found! "<<std::endl;
		Element el={nam,natoms};
		m_elements.push_back(el);
		m_nadded++;
	}
}

}	// end namespace
