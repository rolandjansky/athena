/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonAGDDDescription/MMDetectorHelper.h"
#include "MuonAGDDDescription/MMDetectorDescription.h"
#include "AGDDKernel/AGDDDetectorStore.h"
#include "AGDDKernel/AGDDPositionerStore.h"
#include "AGDDKernel/AGDDDetectorPositioner.h"

#include <vector>

MMDetectorHelper::MMDetectorHelper()
{
	AGDDDetectorStore* ds=AGDDDetectorStore::GetDetectorStore();
	detectorList vl= ds->GetDetectorList();
	
	for ( auto vl_iter: vl)
	{
		MMDetectorDescription* st=dynamic_cast<MMDetectorDescription*>(vl_iter.second);
		if (st) 
			m_MicromegasList[vl_iter.first]=st;
	}
	
}

MMDetectorDescription* MMDetectorHelper::Get_MMDetector(char type,int ieta,int iphi,int layer,char side)
{
	AGDDPositionerStore *ps=AGDDPositionerStore::GetPositionerStore();
	
	MMDetectorDescription* mm=0;
	
	for (unsigned int i=0;i<ps->size();i++)
	{
		AGDDDetectorPositioner* dp=dynamic_cast<AGDDDetectorPositioner*>((*ps)[i]);
		if (dp)
		{
			if (dp->ID.detectorType != "Micromegas") continue;
			std::string dad=dp->ID.detectorAddress;

			char dtype=dad[3];
			char dctype='L';
			if (dtype=='3') dctype='S';
			if (dctype!=type) continue;
			int deta=atoi(dad.substr(5,1).c_str());
			if (deta!=ieta) continue;
			int dphi=atoi(dad.substr(12,1).c_str());
			if (dphi!=iphi) continue;
			int dlayer=atoi(dad.substr(7,1).c_str());
			if (dlayer!=layer) continue;
			char dside=dad[13];
			if (dside!=side) continue;
			
			mm=dynamic_cast<MMDetectorDescription*>(dp->theDetector);
		}
		if(mm) break;
	}
	if (!mm) std::cout<<" could not find a positioned Micromegas!!!! "<<std::endl;
	return mm;
}

AGDDPositionedDetector MMDetectorHelper::Get_MMPositionedDetector(char type,int ieta,int iphi,int layer,char side)
{
	AGDDPositionerStore *ps=AGDDPositionerStore::GetPositionerStore();
	
	MMDetectorDescription* mm=0;
	AGDDDetectorPositioner* dp=0;
	
	for (unsigned int i=0;i<ps->size();i++)
	{
		dp=dynamic_cast<AGDDDetectorPositioner*>((*ps)[i]);
		if (dp)
		{
			if (dp->ID.detectorType != "Micromegas") continue;
			std::string dad=dp->ID.detectorAddress;

			char dtype=dad[3];
			char dctype='L';
			if (dtype=='3') dctype='S';
			if (dctype!=type) continue;
			int deta=atoi(dad.substr(5,1).c_str());
			if (deta!=ieta) continue;
			int dphi=atoi(dad.substr(12,1).c_str());
			if (dphi!=iphi) continue;
			int dlayer=atoi(dad.substr(7,1).c_str());
			if (dlayer!=layer) continue;
			char dside=dad[13];
			if (dside!=side) continue;
			
			mm=dynamic_cast<MMDetectorDescription*>(dp->theDetector);
		}
		if(mm) break;
	}
	if (!mm) std::cout<<" could not find a positioned Micromegas!!!! "<<std::endl;
	AGDDPositionedDetector p_mm(mm,dp);
	return p_mm;
}

MMDetectorDescription* MMDetectorHelper::Get_MMDetectorType(std::string type)
{
	if (m_MicromegasList.find(type) != m_MicromegasList.end()) return m_MicromegasList[type];
	return nullptr;
}
