/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4AtlasControl/PhysicsRegionProxy.h"

#include "G4LogicalVolume.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4ProductionCuts.hh"

PhysicsRegionProxy::PhysicsRegionProxy(): regionName("NULL"), theRegion(0)
{
}
PhysicsRegionProxy::PhysicsRegionProxy(G4Region *r): regionName("NULL"), theRegion(r)
{
}
PhysicsRegionProxy::PhysicsRegionProxy(std::string n, G4Region *r): regionName(n),theRegion(r)
{
}
void PhysicsRegionProxy::addVolume(const std::string volName)
{
	G4LogicalVolumeStore& lvs=*(G4LogicalVolumeStore::GetInstance());

	int ndone=0;
	for (unsigned int i=0;i<lvs.size();i++)
	{
		G4LogicalVolume *lv=lvs[i];
		std::string n=lv->GetName();
		if (n==volName)
		{
			ndone++;
			lv->SetRegion(theRegion);
			theRegion->AddRootLogicalVolume(lv);
		}
	}
	if (ndone==0)
		std::cout<<"No volume called "<<volName<<" found in "
			 <<"the LV store "<<std::endl; 
}
void PhysicsRegionProxy::setCut(double vCut, const std::string pName)
{
	theRegion->GetProductionCuts()->SetProductionCut(vCut,pName);
}
void PhysicsRegionProxy::setRegion(G4Region *r)
{
	theRegion=r;
}
void PhysicsRegionProxy::setName(const std::string n)
{
	regionName=n;
}
std::string PhysicsRegionProxy::getName()
{
	return regionName;
}
