/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "AGDD2Geo/AGDDsTGC.h"
#include "AGDD2Geo/AGDDController.h"
#include "AGDD2Geo/AGDDBuilder.h"
#include "AGDD2Geo/AGDDParameterStore.h"

#include "GeoModelKernel/GeoTrd.h"
#include "GeoModelKernel/GeoShape.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelKernel/GeoMaterial.h"

#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "GeoModelInterfaces/StoredMaterialManager.h"

#include "MuonGeoModel/sTGCComponent.h"
#include "MuonGeoModel/sTGC.h"

void AGDDsTGC::CreateSolid() 
{

}

void AGDDsTGC::CreateVolume() 
{
	
	MuonGM::sTGCComponent *stgc_comp=new MuonGM::sTGCComponent;
	stgc_comp->name=tech;
	stgc_comp->dx1=_small_x;
	stgc_comp->dx2=_large_x;
	stgc_comp->dy=_y;
	stgc_comp->subType=sType;
	stgc_comp->yCutout=_yCutout;
	
	MuonGM::sTGC *cham=new MuonGM::sTGC(stgc_comp);
	GeoPhysVol *vvv=(GeoPhysVol*)cham->build(1);

	CreateSolid();

	if (!GetVolume())
	{
		SetVolume(vvv);
	}

	if(AGDDParameterStore::GetParameterStore()->Exist((*this).GetName())) {
		std::cout << " parameters for volume " << (*this).GetName() << " already registered" << std::endl;
	}
	else {
		AGDDParameterBagsTGC* paraBag = new AGDDParameterBagsTGC();
		paraBag->largeX = _large_x;
		paraBag->smallX = _small_x;
		paraBag->lengthY = _y;
		AGDDParameterBagsTGCTech* techparaBag = dynamic_cast<AGDDParameterBagsTGCTech*> (AGDDParameterStore::GetParameterStore()->GetParameterBag(tech));
		if(!paraBag) std::cout << " not possible to retrieve technology parameters for <" << tech << ">" << std::endl;
		paraBag->TechParameters = techparaBag; 
		AGDDParameterStore::GetParameterStore()->RegisterParameterBag((*this).GetName(), paraBag);
	}
}

GeoMaterial* AGDDsTGC::GetMMMaterial(std::string name)
{
	StoreGateSvc* pDetStore=0;
	ISvcLocator* svcLocator = Gaudi::svcLocator();
	StatusCode sc=svcLocator->service("DetectorStore",pDetStore);
	if(sc.isSuccess())
	{
		DataHandle<StoredMaterialManager> theMaterialManager;
		sc = pDetStore->retrieve(theMaterialManager, "MATERIALS");
		if(sc.isSuccess())
        {
			return theMaterialManager->getMaterial(name);
        }
	}
	return 0;
}
