/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/



#include "MuonAGDD/AGDDsTGC.h"
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

#include "MuonGeoModel/sTGCComponent.h"
#include "MuonGeoModel/sTGC.h"

#include <sstream>

AGDDsTGC* AGDDsTGC::current=0;

AGDDsTGC::AGDDsTGC(std::string s):
    AGDDDetector(s,"sTGC"),_yCutout(0)
{
    current=this;
    Register();
}

void AGDDsTGC::Register()
{
	AGDDDetectorStore *s = AGDDDetectorStore::GetDetectorStore();
	s->RegisterDetector(this);
}

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
		if(!techparaBag) std::cout << " not possible to retrieve technology parameters for <" << tech << ">" << std::endl;
		paraBag->TechParameters = techparaBag; 
		AGDDParameterStore::GetParameterStore()->RegisterParameterBag((*this).GetName(), paraBag);
	}
	
	delete cham;
}
void AGDDsTGC::SetDetectorAddress(AGDDDetectorPositioner* p)
{
		//std::cout<<"This is AGDDsTGC::SetDetectorAddress "<<GetName()<<" "<<
		// sType;
		std::stringstream stringone;
		std::string side="A";
		if (p->ID.sideIndex<0) side="C";
		int ctype=0;
		int ml=1;
		if (sType.substr(1,1)=="S" && sType.substr(3,1)=="P") ml=2;
		else if (sType.substr(1,1)=="L" && sType.substr(3,1)=="C") ml=2;
		if (sType.substr(1,1)=="S") ctype=3;
		else if (sType.substr(1,1)=="L") ctype=1;
		stringone<<"sTG"<<ctype<<"-"<<sType.substr(2,1)<<"-"<<ml<<"-phi"<<p->ID.phiIndex+1<<side<<std::endl;
		//std::cout<<" stringone "<<stringone.str()<<std::endl;
		p->ID.detectorAddress=stringone.str();
}
