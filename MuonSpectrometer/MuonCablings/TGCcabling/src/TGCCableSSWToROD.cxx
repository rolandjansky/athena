/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "TGCcabling/TGCCableSSWToROD.h"

namespace LVL1TGCCabling8 {

// Constructor & Destructor
TGCCableSSWToROD::TGCCableSSWToROD (std::string filename)
  : TGCCable(TGCCable::SSWToROD)
{
  m_database = new TGCDatabaseSLBToROD(filename,"SSW ALL");
}
  
TGCCableSSWToROD::~TGCCableSSWToROD (void)
{
  delete m_database;
}
  

TGCModuleMap* TGCCableSSWToROD::getModule (const TGCModuleId* moduleId) const {
  if(moduleId){
    if(moduleId->getModuleIdType()==TGCModuleId::SSW)
      return getModuleOut(moduleId);
    if(moduleId->getModuleIdType()==TGCModuleId::ROD)
      return getModuleIn(moduleId);
  }
  return 0;
}

TGCModuleMap* TGCCableSSWToROD::getModuleIn (const TGCModuleId* rod) const {
  if(rod->isValid()==false) return 0;

  TGCIdBase::SideType rodSideType = rod->getSideType();
  int rodOctant = rod->getOctant();

  TGCModuleMap* mapId = 0;
  int MaxEntry = m_database->getMaxEntry();
  for(int i=0; i<MaxEntry; i++){
    int id = m_database->getEntry(i,0);
    int block = m_database->getEntry(i,1);
    TGCModuleSSW* ssw = new TGCModuleSSW(rodSideType,
					 rodOctant,
					 id);
    
    if(mapId==0) mapId = new TGCModuleMap();
    mapId->insert(block,ssw); 
  }
  return mapId;
}
  
TGCModuleMap* TGCCableSSWToROD::getModuleOut (const TGCModuleId* ssw) const {
  if(ssw->isValid()==false) return 0;

  TGCModuleMap* mapId = 0;
  int MaxEntry = m_database->getMaxEntry();
  for(int i=0; i<MaxEntry; i++){
    if(m_database->getEntry(i,0)==ssw->getId()){
      int block = m_database->getEntry(i,1);
      TGCModuleROD* rod = new TGCModuleROD(ssw->getSideType(),
					   ssw->getOctant());
      
      mapId = new TGCModuleMap();
      mapId->insert(block,rod);
      break;
    }
  }
  return mapId;
}
  
} //end of namespace
