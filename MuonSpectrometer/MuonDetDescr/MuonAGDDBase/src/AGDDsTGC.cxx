/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/



#include "MuonAGDDBase/AGDDsTGC.h"
#include "AGDDModel/AGDDParameterStore.h"
#include "AGDDKernel/AGDDDetectorStore.h"
#include "AGDDKernel/AGDDVolume.h"

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
#include "MuonGeoModel/MYSQL.h"

#include <sstream>

using MuonGM::MYSQL;


AGDDsTGC::AGDDsTGC(std::string s):
    sTGCDetectorDescription(s),AGDDVolume(s,true)
{
    current=this;
    Register();
}

void AGDDsTGC::CreateSolid() 
{

}

void AGDDsTGC::CreateVolume() 
{
	
	MuonGM::sTGCComponent *stgc_comp=new MuonGM::sTGCComponent;
	stgc_comp->name=tech;
	stgc_comp->dx1=small_x();
	stgc_comp->dx2=large_x();
	stgc_comp->dy=y();
	stgc_comp->subType=subType();
	stgc_comp->yCutout=_yCutout;
	
	MuonGM::sTGC *cham=new MuonGM::sTGC(stgc_comp);
	GeoPhysVol *vvv=(GeoPhysVol*)cham->build(1);

	CreateSolid();

	if (!GetVolume())
	{
		SetVolume(vvv);
	}
	
	delete cham;
}

