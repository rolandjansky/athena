/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonAGDD/AGDDMicromegas.h"
#include "AGDDModel/AGDDParameterStore.h"
#include "AGDDKernel/AGDDDetectorStore.h"

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

#include "MuonGeoModel/MicromegasComponent.h"
#include "MuonGeoModel/Micromegas.h"

AGDDMicromegas* AGDDMicromegas::current=0;

AGDDMicromegas::AGDDMicromegas(std::string s): AGDDDetector(s,"Micromegas")
{
	current=this;
	Register();
}

void AGDDMicromegas::Register()
{
	AGDDDetectorStore *s = AGDDDetectorStore::GetDetectorStore();
	s->RegisterDetector(this);
}

void AGDDMicromegas::CreateSolid() 
{
//	std::cout<<"this is AGDDMicromegas::CreateSolid()"<<std::endl;
//	void *p=GetSolid();
//	if (!p)
//	{
//		std::cout<<" creating solid with dimensions "<<
//		_small_x<<" "<<_large_x<<" "<<_y<<" "<<_z<<std::endl;
//		GeoShape* solid=new GeoTrd(_small_x/2.,_large_x/2.,_y/2.,_y/2.,_z/2.);
//		SetSolid(solid);
//	}

}

void AGDDMicromegas::CreateVolume() 
{
//    std::cout<<"this is AGDDMicromegas::CreateVolume()"<<std::endl;
	
	MuonGM::MicromegasComponent *mm_comp=new MuonGM::MicromegasComponent;
	mm_comp->name=tech;
	mm_comp->dx1=_small_x;
	mm_comp->dx2=_large_x;
	mm_comp->dy=_y;
	mm_comp->subType=sType;
	
	MuonGM::Micromegas *cham=new MuonGM::Micromegas(mm_comp);
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
		AGDDParameterBagMM* paraBag = new AGDDParameterBagMM();
		paraBag->largeX = _large_x;
		paraBag->smallX = _small_x;
		paraBag->lengthY = _y;
		AGDDParameterBagMMTech* techparaBag = dynamic_cast<AGDDParameterBagMMTech*> (AGDDParameterStore::GetParameterStore()->GetParameterBag(tech));
		if(!techparaBag) std::cout << " not possible to retrieve technology parameters for <" << tech << ">" << std::endl;
		paraBag->TechParameters = techparaBag; 
		AGDDParameterStore::GetParameterStore()->RegisterParameterBag((*this).GetName(), paraBag);
	}
	
	delete cham;
}
void AGDDMicromegas::SetDetectorAddress(AGDDDetectorPositioner* p)
{
		//std::cout<<"This is AGDDMicromegas::SetDetectorAddress "<<GetName()<<" "<<
		//sType;
		std::stringstream stringone;
		std::string side="A";
		if (p->ID.sideIndex<0) side="C";
		int ctype=0;
		int ml=atoi(sType.substr(3,1).c_str());
		if (sType.substr(2,1)=="L") ctype=1;
		else if (sType.substr(2,1)=="S") ctype=3;
		int etaIndex=atoi(sType.substr(1,1).c_str());
		stringone<<"sMD"<<ctype<<"-"<<etaIndex<<"-"<<ml<<"-phi"<<p->ID.phiIndex+1<<side<<std::endl;
		//std::cout<<" stringone "<<stringone.str()<<std::endl;
		p->ID.detectorAddress=stringone.str();
}
