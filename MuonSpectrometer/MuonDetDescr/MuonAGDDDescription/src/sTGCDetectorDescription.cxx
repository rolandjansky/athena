/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "MuonAGDDDescription/sTGCDetectorDescription.h"
#include "MuonAGDDDescription/sTGC_Technology.h"
#include "AGDDModel/AGDDParameterStore.h"
#include "AGDDKernel/AGDDDetectorStore.h"

#include <sstream>

sTGCDetectorDescription* sTGCDetectorDescription::current=0;

sTGCDetectorDescription::sTGCDetectorDescription(std::string s):
    AGDDDetector(s,"sTGC"),_yCutout(0)
{
}

void sTGCDetectorDescription::Register()
{
	AGDDDetectorStore *s = AGDDDetectorStore::GetDetectorStore();
	s->RegisterDetector(this);
}


void sTGCDetectorDescription::SetDetectorAddress(AGDDDetectorPositioner* p)
{
		//std::cout<<"This is AGDDsTGC::SetDetectorAddress "<<GetName()<<" "<<
		// sType;
		p->ID.detectorType="sTGC";
		p->theDetector=this;
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

sTGC_Technology* sTGCDetectorDescription::GetTechnology()
{
   AGDDDetectorStore *ds=AGDDDetectorStore::GetDetectorStore();   
   sTGC_Technology* t = dynamic_cast<sTGC_Technology*>(ds->GetTechnology("sTGC_1")); //This needs to be the tech name not the chamber name

   return t;
}
