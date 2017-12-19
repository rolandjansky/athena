/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "MuonAGDDBase/AGDDMMSpacer.h"

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

#include "MuonGeoModel/MMSpacerComponent.h"
#include "MuonGeoModel/MMSpacer.h"

void AGDDMMSpacer::CreateSolid() 
{
//	std::cout<<"this is AGDDMMSpacer::CreateSolid()"<<std::endl;
}

void AGDDMMSpacer::CreateVolume() 
{
//    std::cout<<"this is AGDDMMSpacer::CreateVolume()"<<std::endl;
	
	MuonGM::MMSpacerComponent *mm_comp=new MuonGM::MMSpacerComponent;
	mm_comp->name=tech;
	mm_comp->dx1=m_small_x;
	mm_comp->dx2=m_large_x;
	mm_comp->dy=m_y;
	
	MuonGM::MMSpacer cham(mm_comp);
	GeoPhysVol *vvv=cham.build(1);

	CreateSolid();

	if (!GetVolume())
	{
//	    std::cout<<"setting volume "<<vvv<<std::endl;
		SetVolume(vvv);
//		std::cout<<"done creating volume "<<std::endl;
	}
}

GeoMaterial* AGDDMMSpacer::GetMMMaterial(std::string name)
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
