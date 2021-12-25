/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonAGDDDescription/sTGCDetectorHelper.h"
#include "MuonAGDDDescription/sTGCDetectorDescription.h"
#include "AGDDKernel/AGDDDetectorStore.h"
#include "AGDDKernel/AGDDPositionerStore.h"
#include "AGDDKernel/AGDDDetectorPositioner.h"
#include "AGDDControl/AGDDController.h"

#include <vector>

sTGCDetectorHelper::sTGCDetectorHelper()
{
        if (m_svc.retrieve().isFailure()) {
          std::abort();
        }
        IAGDDtoGeoSvc::LockedController c = m_svc->getController();
	detectorList vl= c->GetDetectorStore().GetDetectorList();
	
	for ( const auto& vl_iter: vl)
	{
		sTGCDetectorDescription* st=dynamic_cast<sTGCDetectorDescription*>(vl_iter.second);
		if (st) {
			m_sTGCList[vl_iter.first]=st;
			m_sTGCListSubType[vl_iter.second->subType()]=st;
		}
	}
	
}

sTGCDetectorDescription* sTGCDetectorHelper::Get_sTGCDetector(char type,int ieta,int iphi,int layer,char side) 
{
	sTGCDetectorDescription* tgc=nullptr;
	
        IAGDDtoGeoSvc::LockedController c = m_svc->getController();
        AGDDPositionerStore& ps = c->GetPositionerStore();
	for (unsigned int i=0;i<ps.size();i++)
	{
                AGDDDetectorPositioner* dp=dynamic_cast<AGDDDetectorPositioner*>(ps[i]);
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
	sTGCDetectorDescription* tgc=nullptr;
	AGDDDetectorPositioner* dp=nullptr;
	
        IAGDDtoGeoSvc::LockedController c = m_svc->getController();
        AGDDPositionerStore& ps = c->GetPositionerStore();
	for (unsigned int i=0;i<ps.size();i++)
	{
                dp=dynamic_cast<AGDDDetectorPositioner*>(ps[i]);
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

sTGCDetectorDescription* sTGCDetectorHelper::Get_sTGCDetectorType(const std::string& type)
{
	if (auto itr = m_sTGCList.find(type); itr != m_sTGCList.end()) return itr->second;
	return nullptr;
}

sTGCDetectorDescription* sTGCDetectorHelper::Get_sTGCDetectorSubType(const std::string& type)
{
	if (auto itr = m_sTGCListSubType.find(type); itr != m_sTGCListSubType.end()) return itr->second;
	return nullptr;
}

sTGCDetectorDescription* sTGCDetectorHelper::Get_sTGCDetectorType(std::string_view type)
{
	for(const auto &pair : m_sTGCList){
     if(pair.first == type) return pair.second;
	}
	return nullptr;
}

sTGCDetectorDescription* sTGCDetectorHelper::Get_sTGCDetectorSubType(std::string_view type)
{
	for(const auto &pair : m_sTGCListSubType){
     if(pair.first == type) return pair.second;
	}
	return nullptr;
}

IAGDDtoGeoSvc::LockedController sTGCDetectorHelper::Get_Controller()
{
  return m_svc->getController();
}
