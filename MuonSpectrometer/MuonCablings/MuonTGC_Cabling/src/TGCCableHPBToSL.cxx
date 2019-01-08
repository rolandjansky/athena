/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonTGC_Cabling/TGCCableHPBToSL.h"

#include "MuonTGC_Cabling/TGCDatabasePPToSL.h" 
#include "MuonTGC_Cabling/TGCModuleHPB.h"
#include "MuonTGC_Cabling/TGCModuleSL.h"

namespace MuonTGC_Cabling {

// Constructor & Destructor
TGCCableHPBToSL::TGCCableHPBToSL(std::string filename)
  : TGCCable(TGCCable::HPBToSL)
{
  m_database[TGCIdBase::Endcap][TGCIdBase::Wire] =
    new TGCDatabasePPToSL(filename,"HPB EW");
  m_database[TGCIdBase::Endcap][TGCIdBase::Strip] =
    new TGCDatabasePPToSL(filename,"HPB ES");
  m_database[TGCIdBase::Forward][TGCIdBase::Wire] =
    new TGCDatabasePPToSL(filename,"HPB FW");
  m_database[TGCIdBase::Forward][TGCIdBase::Strip] = 
    new TGCDatabasePPToSL(filename,"HPB FS");
  }
  
TGCCableHPBToSL::~TGCCableHPBToSL(void)
{
  delete m_database[TGCIdBase::Endcap][TGCIdBase::Wire];
  delete m_database[TGCIdBase::Endcap][TGCIdBase::Strip];
  delete m_database[TGCIdBase::Forward][TGCIdBase::Wire];
  delete m_database[TGCIdBase::Forward][TGCIdBase::Strip];
}
  

TGCModuleMap* TGCCableHPBToSL::getModule(const TGCModuleId* moduleId) const {
  if(moduleId){
    if(moduleId->getModuleIdType()==TGCModuleId::HPB)
      return getModuleOut(moduleId);
    if(moduleId->getModuleIdType()==TGCModuleId::SL)
      return getModuleIn(moduleId);
  }
  return 0;
}

TGCModuleMap* TGCCableHPBToSL::getModuleIn(const TGCModuleId* sl) const {
  if(sl->isValid()==false) return 0;  

  TGCDatabase* wireP = m_database[sl->getRegionType()][TGCIdBase::Wire];
  TGCDatabase* stripP = m_database[sl->getRegionType()][TGCIdBase::Strip];

  TGCModuleMap* mapId = 0;
  const int wireMaxEntry = wireP->getMaxEntry();
  for(int i=0; i<wireMaxEntry; i++){
    int id = wireP->getEntry(i,0);
    int block = wireP->getEntry(i,1);
    TGCModuleHPB* hpb = new TGCModuleHPB(sl->getSideType(),
					 TGCIdBase::Wire,
					 sl->getRegionType(),
					 sl->getSector(),
					 id);
    if(mapId==0) mapId = new TGCModuleMap();
    mapId->insert(block,hpb);
  } 

  const int stripMaxEntry = stripP->getMaxEntry();
  for(int i=0; i<stripMaxEntry; i++){
    int id = stripP->getEntry(i,0);
    int block = stripP->getEntry(i,1);
    TGCModuleHPB* hpb = new TGCModuleHPB(sl->getSideType(),
					 TGCIdBase::Strip,
					 sl->getRegionType(),
					 sl->getSector(),
					 id);
    if(mapId==0) mapId = new TGCModuleMap();
    mapId->insert(block,hpb);	
  }

  return mapId;
}
  

TGCModuleMap* TGCCableHPBToSL::getModuleOut(const TGCModuleId* hpb) const {
  if(hpb->isValid()==false) return 0;  

  const int hpbId = hpb->getId();

  TGCDatabase* databaseP =m_database[hpb->getRegionType()][hpb->getSignalType()];

  TGCModuleMap* mapId = 0;
  const int MaxEntry = databaseP->getMaxEntry();
  for(int i=0; i<MaxEntry; i++){
    if(databaseP->getEntry(i,0)==hpbId)
      {
	int block = databaseP->getEntry(i,1);
	TGCModuleSL* sl = new TGCModuleSL(hpb->getSideType(),
					  hpb->getRegionType(),
					  hpb->getSector());

	mapId = new TGCModuleMap();
	mapId->insert(block,sl);
	break;
      } 
  }
 
  return mapId;
}
  
} //end of namespace
