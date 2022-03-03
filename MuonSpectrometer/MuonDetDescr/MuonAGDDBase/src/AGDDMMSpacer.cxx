/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


#include "MuonAGDDBase/AGDDMMSpacer.h"

#include "GeoModelKernel/GeoTrd.h"
#include "GeoModelKernel/GeoShape.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "AGDDKernel/AGDDBuilder.h"

#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "GeoModelInterfaces/StoredMaterialManager.h"

#include "MuonGeoModel/MMSpacerComponent.h"
#include "MuonGeoModel/MMSpacer.h"
#include "MuonGeoModel/MYSQL.h"

void AGDDMMSpacer::CreateSolid (const AGDDBuilder& /*builder*/)
{
//	std::cout<<"this is AGDDMMSpacer::CreateSolid()"<<std::endl;
}

void AGDDMMSpacer::CreateVolume (AGDDBuilder& builder)
{
//    std::cout<<"this is AGDDMMSpacer::CreateVolume()"<<std::endl;
	
	MuonGM::MMSpacerComponent *mm_comp=new MuonGM::MMSpacerComponent;
	mm_comp->name=tech;
	mm_comp->dx1=m_small_x;
	mm_comp->dx2=m_large_x;
	mm_comp->dy=m_y;

        MuonGM::MYSQL::LockedMYSQL mysql = MuonGM::MYSQL::GetPointer();
	MuonGM::MMSpacer cham(*mysql, mm_comp);
	GeoPhysVol *vvv=cham.build(builder.GetMaterialManager(), *mysql, 1);

	CreateSolid (builder);

	if (!GetVolume())
	{
//	    std::cout<<"setting volume "<<vvv<<std::endl;
		SetVolume(vvv);
//		std::cout<<"done creating volume "<<std::endl;
	}
}
