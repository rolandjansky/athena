/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


#include "MuonAGDDBase/AGDDMuonStation.h"

#include "GeoModelKernel/GeoTrd.h"
#include "GeoModelKernel/GeoShape.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoMaterial.h"

#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "GeoModelInterfaces/StoredMaterialManager.h"

void AGDDMuonStation::CreateSolid (const AGDDBuilder& /*builder*/)
{
	std::cout<<"this is AGDDMuonStation::CreateSolid()"<<std::endl;
	void *p=GetSolid();
	if (!p)
	{
		std::cout<<" creating solid with dimensions "<<
		m_small_x<<" "<<m_large_x<<" "<<m_y<<" "<<m_z<<std::endl;
		GeoShape* solid=new GeoTrd(m_small_x/2.,m_large_x/2.,m_y/2.,m_y/2.,m_z/2.);
		SetSolid(solid);
	}
}

void AGDDMuonStation::CreateVolume (AGDDBuilder& builder)
{
    std::cout<<"this is AGDDMuonStation::CreateVolume()"<<std::endl;
	static const GeoMaterial* const air = GetMMMaterial("std::Air");
        if (!air) std::cout<<" Air not found!"<<std::endl;

	CreateSolid (builder);

	if (!GetVolume())
	{
	    std::cout<<"trying to create logical volume"<<std::endl;
		const GeoShape* shape=(GeoShape *)GetSolid();
		if (!shape) std::cout<<" something wrong, shape is 0"<<std::endl;
		GeoLogVol *a=new GeoLogVol(GetName(),shape,air);
		GeoPhysVol *a_phys=new GeoPhysVol(a);
		SetVolume(a_phys);
		std::cout<<"done creating volume "<<std::endl;
	}
}

const GeoMaterial* AGDDMuonStation::GetMMMaterial(const std::string& name) const
{
	StoreGateSvc* pDetStore=nullptr;
	ISvcLocator* svcLocator = Gaudi::svcLocator();
	StatusCode sc=svcLocator->service("DetectorStore",pDetStore);
	if(sc.isSuccess())
	{
                const StoredMaterialManager* theMaterialManager = nullptr;
		sc = pDetStore->retrieve(theMaterialManager, "MATERIALS");
		if(sc.isSuccess())
        {
			return theMaterialManager->getMaterial(name);
        }
	}
	return nullptr;
}
