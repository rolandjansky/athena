/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonAGDDDescription/sTGCDetectorHelper.h"
#include "MuonAGDDDescription/sTGCDetectorDescription.h"
#include "AGDDKernel/AGDDDetectorStore.h"
#include "AGDDKernel/AGDDPositionerStore.h"
#include "AGDDKernel/AGDDDetectorPositioner.h"

#include <vector>

sTGCDetectorHelper::sTGCDetectorHelper()
{
	AGDDDetectorStore* ds=AGDDDetectorStore::GetDetectorStore();
	detectorList vl= ds->GetDetectorList();
	
	for ( auto vl_iter: vl)
	{
		sTGCDetectorDescription* st=dynamic_cast<sTGCDetectorDescription*>(vl_iter.second);
		//AGDDMicromegas* st1=dynamic_cast<AGDDMicromegas*>(st);
		if (st) 
			m_sTGCList[vl_iter.first]=st;
	}
	
}

sTGCDetectorDescription* sTGCDetectorHelper::Get_sTGCDetector(char type,int ieta,int iphi,int layer,char side) 
{
	AGDDPositionerStore *ps=AGDDPositionerStore::GetPositionerStore();
	
	sTGCDetectorDescription* tgc=0;
	
	for (unsigned int i=0;i<ps->size();i++)
	{
		AGDDDetectorPositioner* dp=dynamic_cast<AGDDDetectorPositioner*>((*ps)[i]);
		if (dp)
		{
			if (dp->ID.detectorType != "sTGC") continue;
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
			
			tgc=dynamic_cast<sTGCDetectorDescription*>(dp->theDetector);
		}
		if(tgc) break;
	}
	if (!tgc) std::cout<<" could not find a positioned sTGC!!!! "<<std::endl;
	return tgc;
}

AGDDPositionedDetector sTGCDetectorHelper::Get_sTGCPositionedDetector(char type,int ieta,int iphi,int layer,char side) 
{
	AGDDPositionerStore *ps=AGDDPositionerStore::GetPositionerStore();
	
	sTGCDetectorDescription* tgc=0;
	AGDDDetectorPositioner* dp=0;
	
	for (unsigned int i=0;i<ps->size();i++)
	{
		dp=dynamic_cast<AGDDDetectorPositioner*>((*ps)[i]);
		if (dp)
		{
			if (dp->ID.detectorType != "sTGC") continue;
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
			
			tgc=dynamic_cast<sTGCDetectorDescription*>(dp->theDetector);
		}
		if(tgc) break;
	}
	if (!tgc) std::cout<<" could not find a positioned sTGC!!!! "<<std::endl;
	AGDDPositionedDetector p_sTGC(tgc,dp);
	return p_sTGC;
}

sTGCDetectorDescription* sTGCDetectorHelper::Get_sTGCDetectorType(std::string type)
{
	if (m_sTGCList.find(type) != m_sTGCList.end()) return m_sTGCList[type];
	return nullptr;
}
