/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonAGDDDescription/MMDetectorDescription.h"
#include "AGDDModel/AGDDParameterStore.h"
#include "AGDDKernel/AGDDDetectorStore.h"

#include <sstream>

MMDetectorDescription* MMDetectorDescription::current=0;


MMDetectorDescription::MMDetectorDescription(std::string s): 
	AGDDDetector(s,"Micromegas")
{
}

void MMDetectorDescription::Register()
{
	AGDDDetectorStore *s = AGDDDetectorStore::GetDetectorStore();
	s->RegisterDetector(this);
}


void MMDetectorDescription::SetDetectorAddress(AGDDDetectorPositioner* p)
{
		//std::cout<<"This is AGDDMicromegas::SetDetectorAddress "<<GetName()<<" "<<
		//sType;
		p->ID.detectorType="Micromegas";
		p->theDetector=this;
		std::stringstream stringone;
		std::string side="A";
		if (p->ID.sideIndex<0) side="C";
		int ctype=0;
		int ml=atoi(subType().substr(3,1).c_str());
		if (subType().substr(2,1)=="L") ctype=1;
		else if (subType().substr(2,1)=="S") ctype=3;
		int etaIndex=atoi(subType().substr(1,1).c_str());
		stringone<<"sMD"<<ctype<<"-"<<etaIndex<<"-"<<ml<<"-phi"<<p->ID.phiIndex+1<<side<<std::endl;
		//std::cout<<" stringone "<<stringone.str()<<std::endl;
		p->ID.detectorAddress=stringone.str();
}

MM_Technology* MMDetectorDescription::GetTechnology()
{
   AGDDDetectorStore *ds=AGDDDetectorStore::GetDetectorStore();    
   MM_Technology* t = dynamic_cast<MM_Technology*>(ds->GetTechnology(GetName()));
   return t;
}
