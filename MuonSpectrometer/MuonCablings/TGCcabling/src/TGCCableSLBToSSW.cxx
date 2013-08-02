/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TGCcabling/TGCCableSLBToSSW.h"

namespace LVL1TGCCabling8 {

// Constructor & Destructor
TGCCableSLBToSSW::TGCCableSLBToSSW (std::string filename)
  : TGCCable(TGCCable::SLBToSSW)
{
  database[TGCIdBase::Endcap][TGCIdBase::WT] =
    new TGCDatabaseSLBToROD(filename,"SLB EWT");
  database[TGCIdBase::Endcap][TGCIdBase::WD] =
    new TGCDatabaseSLBToROD(filename,"SLB EWD");
  database[TGCIdBase::Endcap][TGCIdBase::ST] =
    new TGCDatabaseSLBToROD(filename,"SLB EST");
  database[TGCIdBase::Endcap][TGCIdBase::SD] =
    new TGCDatabaseSLBToROD(filename,"SLB ESD");
  database[TGCIdBase::Endcap][TGCIdBase::WI] =
    new TGCDatabaseSLBToROD(filename,"SLB EWI");
  database[TGCIdBase::Endcap][TGCIdBase::SI] =
    new TGCDatabaseSLBToROD(filename,"SLB ESI");
  database[TGCIdBase::Forward][TGCIdBase::WT] =
    new TGCDatabaseSLBToROD(filename,"SLB FWT");
  database[TGCIdBase::Forward][TGCIdBase::WD] =
    new TGCDatabaseSLBToROD(filename,"SLB FWD");
  database[TGCIdBase::Forward][TGCIdBase::ST] =
    new TGCDatabaseSLBToROD(filename,"SLB FST");
  database[TGCIdBase::Forward][TGCIdBase::SD] =
    new TGCDatabaseSLBToROD(filename,"SLB FSD");
  database[TGCIdBase::Forward][TGCIdBase::WI] =
    new TGCDatabaseSLBToROD(filename,"SLB FWI");
  database[TGCIdBase::Forward][TGCIdBase::SI] =
    new TGCDatabaseSLBToROD(filename,"SLB FSI");
}

TGCCableSLBToSSW::~TGCCableSLBToSSW (void)
{
  delete database[TGCIdBase::Endcap][TGCIdBase::WT];
  delete database[TGCIdBase::Endcap][TGCIdBase::WD];
  delete database[TGCIdBase::Endcap][TGCIdBase::ST];
  delete database[TGCIdBase::Endcap][TGCIdBase::SD];
  delete database[TGCIdBase::Endcap][TGCIdBase::WI];
  delete database[TGCIdBase::Endcap][TGCIdBase::SI];

  delete database[TGCIdBase::Forward][TGCIdBase::WT];
  delete database[TGCIdBase::Forward][TGCIdBase::WD];
  delete database[TGCIdBase::Forward][TGCIdBase::ST];
  delete database[TGCIdBase::Forward][TGCIdBase::SD];
  delete database[TGCIdBase::Forward][TGCIdBase::WI];
  delete database[TGCIdBase::Forward][TGCIdBase::SI];
}
  

TGCModuleMap* TGCCableSLBToSSW::getModule (const TGCModuleId* moduleId) const {
  if(moduleId){
    if(moduleId->getModuleIdType()==TGCModuleId::SLB)
      return getModuleOut(moduleId);
    if(moduleId->getModuleIdType()==TGCModuleId::SSW)
      return getModuleIn(moduleId);
  }
  return 0;
}

TGCModuleMap* TGCCableSLBToSSW::getModuleIn (const TGCModuleId* ssw) const {
  if(ssw->isValid()==false) return 0;

  TGCDatabase* databaseP[TGCIdBase::MaxRegionType*TGCIdBase::MaxModuleType];
  TGCIdBase::ModuleType module[TGCIdBase::MaxRegionType*TGCIdBase::MaxModuleType];
  TGCIdBase::RegionType region[TGCIdBase::MaxRegionType*TGCIdBase::MaxModuleType];
  for(int i=0; i<TGCIdBase::MaxRegionType; i++){
    for(int j=0; j<TGCIdBase::MaxModuleType; j++){
      databaseP[i*TGCIdBase::MaxModuleType+j] = database[i][j];
      region[i*TGCIdBase::MaxModuleType+j] = static_cast<TGCIdBase::RegionType>(i);
      module[i*TGCIdBase::MaxModuleType+j] = static_cast<TGCIdBase::ModuleType>(j);
    }
  }

  int sswId = ssw->getId();
  TGCIdBase::SideType sswSideType = ssw->getSideType();
  int sswOctant = ssw->getOctant();

  TGCModuleMap* mapId = 0;
  for(int type=0; type<TGCIdBase::MaxRegionType*TGCIdBase::MaxModuleType; type++){
    int MaxEntry = databaseP[type]->getMaxEntry();
    for(int i=0; i<MaxEntry; i++){
      if(databaseP[type]->getEntry(i,2)==sswId)
	{
	  int sector = databaseP[type]->getEntry(i,0);
	  if(region[type]==TGCIdBase::Endcap&&
             !(module[type]==TGCIdBase::WI||module[type]==TGCIdBase::SI))
	    sector += 6*sswOctant;
	  else
	    sector += 3*sswOctant;
	  int id = databaseP[type]->getEntry(i,1);
	  int block = databaseP[type]->getEntry(i,3);
	  TGCModuleSLB* slb = new TGCModuleSLB(sswSideType,
					       module[type],
					       region[type],
					       sector,
					       id);

	  if(mapId==0) mapId = new TGCModuleMap();
	  mapId->insert(block,slb);
	} 
    }
  }

  return mapId;
}


TGCModuleMap* TGCCableSLBToSSW::getModuleOut (const TGCModuleId* slb) const {
  if(slb->isValid()==false) return 0;

  TGCDatabase* databaseP =database[slb->getRegionType()][slb->getModuleType()];

  int octant = slb->getOctant();
  int sector = slb->getSectorInOctant();
  
  TGCModuleMap* mapId = 0;
  int MaxEntry = databaseP->getMaxEntry();
  for(int i=0; i<MaxEntry; i++){
    if(databaseP->getEntry(i,0)==sector&&
       databaseP->getEntry(i,1)==slb->getId())
      {
	int id = databaseP->getEntry(i,2);
	int block = databaseP->getEntry(i,3);
	TGCModuleSSW* ssw = new TGCModuleSSW(slb->getSideType(),
					     octant,
					     id);
        mapId = new TGCModuleMap();
        mapId->insert(block,ssw);
	break;
      } 
  }
  
  return mapId;
}
  
} //end of namespace
