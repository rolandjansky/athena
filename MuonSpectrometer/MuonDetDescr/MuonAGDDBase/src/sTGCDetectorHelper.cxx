/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonAGDDBase/sTGCDetectorHelper.h"
#include "MuonAGDDBase/AGDDsTGC.h"
#include "MuonAGDDBase/AGDDMicromegas.h"
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
		std::cout<<"detector "<<vl_iter.second->GetName()<<std::endl;
		AGDDsTGC* st=dynamic_cast<AGDDsTGC*>(vl_iter.second);
		//AGDDMicromegas* st1=dynamic_cast<AGDDMicromegas*>(st);
		std::cout<<" st pointer "<<st<<std::endl;
		if (st) 
			sTGCList[vl_iter.first]=st;
	}
	
}

AGDDsTGC* sTGCDetectorHelper::Get_sTGCDetector(char type,int ieta,int iphi,int layer,char side)
{
	AGDDPositionerStore *ps=AGDDPositionerStore::GetPositionerStore();
	
	AGDDsTGC* tgc=0;
	
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
			
			std::cout<<" Detector Positioner "<<dp->ID.detectorType<<" "<<dp->ID.detectorAddress<<std::endl;
			tgc=dynamic_cast<AGDDsTGC*>(dp->theDetector);
		}
	}
	if (!tgc) std::cout<<" could not find a positioned sTGC!!!! "<<std::endl;
	return tgc;
}
