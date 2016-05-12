/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FadsMaterial/MaterialManager.h"
#include "FadsMaterial/FadsElement.h"
#include "FadsMaterial/FadsSimpleMaterial.h"
#include "FadsMaterial/FadsMaterial.h"
#include "FadsMaterial/FadsMolecule.h"
#include "FadsMaterial/FadsMixture.h"

namespace FADS {

MaterialManager* MaterialManager::s_mpointer=0;

void MaterialManager::StoreElement(FadsElement *el)
{
	std::string symbol=el->GetSymbol();
// 	std::cout<<" This is MaterialManager::StoreElement "<<symbol<<std::endl;
	if (m_elist.find(symbol)!=m_elist.end())
	{
		std::cout<<"attempt of redefining an existing element "<<symbol<<std::endl;
              std::cout<<m_elist[symbol]<<std::endl;			
	}
	else
	{
		m_elist[symbol]=el;
	}
}
G4Element* MaterialManager::GetElement(std::string symbol)
{
	if (m_elist.find(symbol)==m_elist.end())
	{
		std::cout<<" element "<<symbol<<" not found!"<<std::endl;
		return NULL;
	}
	else
		return m_elist[symbol];
}
void MaterialManager::StoreMaterial(FadsSimpleMaterial *el)
{
	std::string name=el->name();
	m_mlist[name]=el;
}
G4Material* MaterialManager::GetMaterial(std::string name)
{
	if (m_mlist.find(name)==m_mlist.end())
	{
		std::cout<<" material "<<name<<" not found!"<<std::endl;
		return NULL;
	}
	else
	{
		// cout<<"returning material "<<name<<endl;
		return m_mlist[name]->GetG4Material();
	}
}
void MaterialManager::PrintMaterialTable()
{
	MaterialList::iterator it;
	for (it=m_mlist.begin();it!=m_mlist.end();it++)
	{
		std::cout<<(*it).second->GetG4Material()<<std::endl;
	}	
}
void MaterialManager::PrintElementTable()
{
	ElementList::iterator it;
	for (it=m_elist.begin();it!=m_elist.end();it++) std::cout<<(*it).second<<std::endl;
}
void MaterialManager::initialize()
{
//         std::string name,symbol;
//         double z,a,density;
//         int ncomponents,natoms;
// 	FadsElement *el;
// 	el=new FadsElement(name="Hydrogen",symbol="H" , z= 1., a=1.01*g/mole);
// 	el=new FadsElement(name="Carbon"  ,symbol="C" , z= 6., a=12.01*g/mole);
// 	el=new FadsElement(name="Nitrogen",symbol="N" , z= 7., a=14.01*g/mole);
// 	el=new FadsElement(name="Oxygen"  ,symbol="O" , z= 8., a=16.00*g/mole);
// 	el=new FadsElement(name="Silicon",symbol="Si" , z= 14., a=28.09*g/mole);
// 	el=new FadsElement(name="Argon",symbol="Ar",z=18.,a=39.95*g/mole);
// 	el=new FadsElement(name="Iron"    ,symbol="Fe", z=26., a=55.85*g/mole);
// 	el=new FadsElement(name="Alluminum",symbol="Al",z=13.,a=26.98*g/mole);
// 	el=new FadsElement(name="Lead",symbol="Pb",z=82.,a=207.19*g/mole);
// 	el=new FadsElement(name="Fluorine",symbol="F",z=9.,a=18.99*g/mole);
// 	el=new FadsElement(name="Chluorine",symbol="Cl",z=17.,a=35.45*g/mole);
// 	
// 	FadsMaterial *mat;
// 	FadsMolecule *mol;
// 	FadsMixture *mix;
// 	mat=new FadsMaterial(name="Aluminium", z=13., a = 26.98*g/mole, density = 2.700*g/cm3);
//         mat=new FadsMaterial(name="Iron",z=26., a=55.85*g/mole, density=7.87*g/cm3);
// 	mat=new FadsMaterial(name="Lead"     , z=82., a= 207.19*g/mole,density = 11.35*g/cm3);
// 	mat=new FadsMaterial(name="DilutedLead"     , z=82., a= 207.19*g/mole,density = 6.*g/cm3);
// 	mat=new FadsMaterial(name="Copper",z=29.,a=63.546*g/mole, density=8.96*g/cm3);
// 	mat=new FadsMaterial(name="Silicon",z=14.,a=28.0855*g/mole, density=2.33*g/cm3); 
// 	mol=new FadsMolecule(name="Scintillator", density=1.032*g/cm3, ncomponents=2);
// 		mol->AddElement("C",natoms=9);
// 		mol->AddElement("H",natoms=10);
// 	mix=new FadsMixture(name="Air"  , density=1.290*mg/cm3, ncomponents=2);
// 		mix->AddElement("N", .7);
// 		mix->AddElement("O", .3);
// 	mol=new FadsMolecule(name="CO2", density=1.977*g/cm3,ncomponents=2);
// 		mol->AddElement("C",natoms=1);
// 		mol->AddElement("O",natoms=2);
// 	mix=new FadsMixture(name="ArCO2",density=1.8*g/cm3,ncomponents=2);
// 		mix->AddElement("Ar",.93);
// 		mix->AddMaterial("CO2",.07);
// 	mol=new FadsMolecule(name="RPCgas", density=1.977*g/cm3,ncomponents=3);
// 		mol->AddElement("C",natoms=2);
// 		mol->AddElement("H",natoms=2);
// 		mol->AddElement("F",natoms=4);
// 	mol=new FadsMolecule(name="RPVC", density=1.4*g/cm3,ncomponents=3);
// 		mol->AddElement("C",natoms=2);
// 		mol->AddElement("H",natoms=3);
// 		mol->AddElement("Cl",natoms=1);
// 	mol=new FadsMolecule(name="Bakelite", density=1.4*g/cm3,ncomponents=3);
// 		mol->AddElement("C",natoms=1);
// 		mol->AddElement("H",natoms=4);
// 		mol->AddElement("O",natoms=2);
}
bool MaterialManager::FindElement(std::string name)
{
	return (m_elist.find(name)!=m_elist.end());
}
bool MaterialManager::FindMaterial(std::string name)
{
	return (m_mlist.find(name)!=m_mlist.end());
}

double MaterialManager::GetRadLen(std::string name)
{
	return GetMaterial(name)->GetRadlen();
}

double MaterialManager::getRadLen(std::string name)
{
	return GetRadLen(name);
}

}	// end namespace
