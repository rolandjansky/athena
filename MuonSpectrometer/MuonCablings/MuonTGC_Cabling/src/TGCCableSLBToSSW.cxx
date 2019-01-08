/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonTGC_Cabling/TGCCableSLBToSSW.h"

#include "MuonTGC_Cabling/TGCDatabaseSLBToROD.h" 
#include "MuonTGC_Cabling/TGCModuleSLB.h"
#include "MuonTGC_Cabling/TGCModuleSSW.h"

namespace MuonTGC_Cabling {

// Constructor & Destructor
TGCCableSLBToSSW::TGCCableSLBToSSW(std::string filename)
  : TGCCable(TGCCable::SLBToSSW)
{
  m_database[TGCIdBase::Endcap][TGCIdBase::WT] =
    new TGCDatabaseSLBToROD(filename,"SLB EWT");
  m_database[TGCIdBase::Endcap][TGCIdBase::WD] =
    new TGCDatabaseSLBToROD(filename,"SLB EWD");
  m_database[TGCIdBase::Endcap][TGCIdBase::ST] =
    new TGCDatabaseSLBToROD(filename,"SLB EST");
  m_database[TGCIdBase::Endcap][TGCIdBase::SD] =
    new TGCDatabaseSLBToROD(filename,"SLB ESD");
  m_database[TGCIdBase::Endcap][TGCIdBase::WI] =
    new TGCDatabaseSLBToROD(filename,"SLB EWI");
  m_database[TGCIdBase::Endcap][TGCIdBase::SI] =
    new TGCDatabaseSLBToROD(filename,"SLB ESI");
  m_database[TGCIdBase::Endcap][SL] =
    new TGCDatabaseSLBToROD(filename,"SLB ESL");

  m_database[TGCIdBase::Forward][TGCIdBase::WT] =
    new TGCDatabaseSLBToROD(filename,"SLB FWT");
  m_database[TGCIdBase::Forward][TGCIdBase::WD] =
    new TGCDatabaseSLBToROD(filename,"SLB FWD");
  m_database[TGCIdBase::Forward][TGCIdBase::ST] =
    new TGCDatabaseSLBToROD(filename,"SLB FST");
  m_database[TGCIdBase::Forward][TGCIdBase::SD] =
    new TGCDatabaseSLBToROD(filename,"SLB FSD");
  m_database[TGCIdBase::Forward][TGCIdBase::WI] =
    new TGCDatabaseSLBToROD(filename,"SLB FWI");
  m_database[TGCIdBase::Forward][TGCIdBase::SI] =
    new TGCDatabaseSLBToROD(filename,"SLB FSI");
  m_database[TGCIdBase::Forward][SL] =
    new TGCDatabaseSLBToROD(filename,"SLB FSL");
}

TGCCableSLBToSSW::~TGCCableSLBToSSW(void)
{
  delete m_database[TGCIdBase::Endcap][TGCIdBase::WT];
  delete m_database[TGCIdBase::Endcap][TGCIdBase::WD];
  delete m_database[TGCIdBase::Endcap][TGCIdBase::ST];
  delete m_database[TGCIdBase::Endcap][TGCIdBase::SD];
  delete m_database[TGCIdBase::Endcap][TGCIdBase::WI];
  delete m_database[TGCIdBase::Endcap][TGCIdBase::SI];
  delete m_database[TGCIdBase::Endcap][SL];

  delete m_database[TGCIdBase::Forward][TGCIdBase::WT];
  delete m_database[TGCIdBase::Forward][TGCIdBase::WD];
  delete m_database[TGCIdBase::Forward][TGCIdBase::ST];
  delete m_database[TGCIdBase::Forward][TGCIdBase::SD];
  delete m_database[TGCIdBase::Forward][TGCIdBase::WI];
  delete m_database[TGCIdBase::Forward][TGCIdBase::SI];
  delete m_database[TGCIdBase::Forward][SL];
}
  

TGCModuleMap* TGCCableSLBToSSW::getModule(const TGCModuleId* moduleId) const {
  if(moduleId){
    if(moduleId->getModuleIdType()==TGCModuleId::SLB)
      return getModuleOut(moduleId);
    if(moduleId->getModuleIdType()==TGCModuleId::SSW)
      return getModuleIn(moduleId);
  }
  return 0;
}

TGCModuleMap* TGCCableSLBToSSW::getModuleIn(const TGCModuleId* ssw) const {
  if(ssw->isValid()==false) return 0;

  const int sswId = ssw->getId();

  TGCDatabase* databaseP[TGCIdBase::MaxRegionType * MaxModuleType];
  TGCIdBase::ModuleType module[TGCIdBase::MaxRegionType * MaxModuleType];
  TGCIdBase::RegionType region[TGCIdBase::MaxRegionType * MaxModuleType];
  for(int i=0; i<TGCIdBase::MaxRegionType; i++){
    for(int j=0; j< MaxModuleType; j++){
      databaseP[i*MaxModuleType + j] = m_database[i][j];
      region[i*MaxModuleType + j] = static_cast<TGCIdBase::RegionType>(i);
      module[i*MaxModuleType + j] = static_cast<TGCIdBase::ModuleType>(j);
    }
  }

  TGCModuleMap* mapId = 0;
  for(int type=0; type<TGCIdBase::MaxRegionType* MaxModuleType; type++){
    const int MaxEntry = databaseP[type]->getMaxEntry();
    for(int i=0; i<MaxEntry; i++){

      if(databaseP[type]->getEntry(i,4) == sswId) {
	int sector = databaseP[type]->getEntry(i,0);

	// Inner
	if(module[type]==TGCIdBase::WI) {
	  // EI/FI 
          //  wire(TGCIdBase::WI) and strip(TGCIdBase::SI) of a chamber
          //  use the same SLB chip             

	  int sswSector = ssw->getReadoutSector();
          // one ROD (rodId = 2,5,8,11) covers 6 Inner sectors
	  if(sswSector %3 != 1) continue;
          sswSector -= 1;

	  sector += sswSector*2;
	  int id = databaseP[type]->getEntry(i,1);
	  int block = databaseP[type]->getEntry(i,5);
	  int sbLoc = databaseP[type]->getEntry(i,2);
          int slbAddr = databaseP[type]->getEntry(i,3);
	  for(int ip= 0; ip<3 ; ip++){ 
	    int psector  = sector  + ip*2;
	    int pblock   = block   + ip*4;
	    int psbLoc   = sbLoc   + ip*4;	    
            int pslbAddr = slbAddr + ip*2;
	    TGCModuleSLB* slb = new TGCModuleSLB(ssw->getSideType(),
						 module[type],
						 region[type],
						 psector,
						 id,
						 psbLoc,
						 pslbAddr);
	  
	    if(mapId==0) mapId = new TGCModuleMap();
	    mapId->insert(pblock,slb);
	  }

	} else if(module[type]==TGCIdBase::SI) {
	  // EI/FI 
          //  wire(TGCIdBase::WI) and strip(TGCIdBase::SI) of a chamber
          //  use the same SLB chip             

	  // do nothing
          
	} else {
	  if(region[type]==TGCIdBase::Endcap) {
	    sector += ssw->getReadoutSector() * (TGCId::NumberOfEndcapSector/TGCId::NumberOfReadoutSector);
	  } else {
	    sector += ssw->getReadoutSector() * (TGCId::NumberOfForwardSector/TGCId::NumberOfReadoutSector);
	  }
	  int id = databaseP[type]->getEntry(i,1);
	  int sbLoc = databaseP[type]->getEntry(i,2);
          int slbAddr = databaseP[type]->getEntry(i,3);
	  int block = databaseP[type]->getEntry(i,5);
	  TGCModuleSLB* slb = new TGCModuleSLB(ssw->getSideType(),
					       module[type],
					       region[type],
					       sector,
					       id,
					       sbLoc,
					       slbAddr);
	  
	  if(mapId==0) mapId = new TGCModuleMap();
	  mapId->insert(block,slb);
	} 

      }
    }
  }

  return mapId;
}


TGCModuleMap* TGCCableSLBToSSW::getModuleOut(const TGCModuleId* slb) const {
  if(slb->isValid()==false) return 0;

  const int slbId = slb->getId();
  const int sector = slb->getSectorInReadout();
  int readoutSector = slb->getReadoutSector();
  
  TGCModuleMap* mapId = 0;

  TGCDatabase* databaseP =m_database[slb->getRegionType()][slb->getModuleType()];
  const int MaxEntry = databaseP->getMaxEntry();
  if(slb->getModuleType() == TGCIdBase::WI ||
     slb->getModuleType() == TGCIdBase::SI) {
    // inner 
    for(int i=0; i<MaxEntry; i++){
      if(databaseP->getEntry(i,0)==sector &&
	 databaseP->getEntry(i,1)==slbId) {
	
        int sswSectorRO =   readoutSector - (readoutSector%3) + 1;
	int id = databaseP->getEntry(i,4);
	int block = databaseP->getEntry(i,5) + 4*(readoutSector%3);
        
	TGCModuleSSW* ssw = new TGCModuleSSW(slb->getSideType(),
					     sswSectorRO,
					     id);
        mapId = new TGCModuleMap();
        mapId->insert(block,ssw);
	break;
      } 
    }
    
  } else { 
    for(int i=0; i<MaxEntry; i++){
      if(databaseP->getEntry(i,0)==sector&&
	 databaseP->getEntry(i,1)==slbId) {
	
	int id = databaseP->getEntry(i,4);
	int block = databaseP->getEntry(i,5);
	TGCModuleSSW* ssw = new TGCModuleSSW(slb->getSideType(),
					     readoutSector,
					     id);
        mapId = new TGCModuleMap();
        mapId->insert(block,ssw);
	break;
      } 
    }

  }
  return mapId;
}
  
} //end of namespace
