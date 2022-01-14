/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonTGC_Cabling/TGCCableSSWToROD.h"

#include "TGCcablingInterface/TGCIdBase.h"
#include "MuonTGC_Cabling/TGCDatabaseSLBToROD.h" 
#include "MuonTGC_Cabling/TGCModuleSSW.h"
#include "MuonTGC_Cabling/TGCModuleROD.h"

namespace MuonTGC_Cabling {

// Constructor & Destructor
TGCCableSSWToROD::TGCCableSSWToROD(const std::string& filename)
  : TGCCable(TGCCable::SSWToROD)
{
  m_database = new TGCDatabaseSLBToROD(filename,"SSW ALL");
}

TGCCableSSWToROD::TGCCableSSWToROD (void)
  : TGCCable(TGCCable::SSWToROD),
    m_database(nullptr)
{
}

TGCCableSSWToROD::TGCCableSSWToROD (const TGCCableSSWToROD& right)
  : TGCCable(TGCCable::SSWToROD),
    m_database(nullptr)
{
  TGCDatabaseSLBToROD* mypointer = dynamic_cast<TGCDatabaseSLBToROD*>(right.m_database);
  if(mypointer) m_database = new TGCDatabaseSLBToROD(*mypointer);
  else m_database = nullptr;
}

TGCCableSSWToROD& TGCCableSSWToROD::operator=(const TGCCableSSWToROD& right)
{
  if (this != &right) {
    delete m_database;
    TGCDatabaseSLBToROD* mypointer = dynamic_cast<TGCDatabaseSLBToROD*>(right.m_database);
    if(mypointer) m_database = new TGCDatabaseSLBToROD(*mypointer);
    else m_database = nullptr;
  }
  return *this;
}
  
TGCCableSSWToROD::~TGCCableSSWToROD(void)
{
  delete m_database;
}
 

TGCModuleMap* TGCCableSSWToROD::getModule(const TGCModuleId* moduleId) const {
  if(moduleId){
    if(moduleId->getModuleIdType()==TGCModuleId::SSW)
      return getModuleOut(moduleId);
    if(moduleId->getModuleIdType()==TGCModuleId::ROD)
      return getModuleIn(moduleId);
  }
  return nullptr;
}

TGCModuleMap* TGCCableSSWToROD::getModuleIn(const TGCModuleId* rod) const {
  if(!rod->isValid()) return nullptr;

  const TGCIdBase::SideType rodSideType = rod->getSideType(); 
  const int rodReadoutSector = rod->getReadoutSector();

  TGCModuleMap* mapId = nullptr;
  const int MaxEntry = m_database->getMaxEntry();
  for(int i=0; i<MaxEntry; i++){
    int id = m_database->getEntry(i,0);
    int block = m_database->getEntry(i,1);
    TGCModuleSSW* ssw = new TGCModuleSSW(rodSideType,
					 rodReadoutSector,
					 id);
    if(mapId==nullptr) mapId = new TGCModuleMap();
    mapId->insert(block,ssw); 
  }
  return mapId;
}
  
TGCModuleMap* TGCCableSSWToROD::getModuleOut(const TGCModuleId* ssw) const {
  if(!ssw->isValid()) return nullptr;

  const int sswId = ssw->getId();

  TGCModuleMap* mapId = nullptr;
  const int MaxEntry = m_database->getMaxEntry();
  for(int i=0; i<MaxEntry; i++){
    if(m_database->getEntry(i,0)==sswId){
      int block = m_database->getEntry(i,1);
      TGCModuleROD* rod = new TGCModuleROD(ssw->getSideType(),
					   ssw->getReadoutSector());      
      mapId = new TGCModuleMap();
      mapId->insert(block,rod);
      break;
    }
  }
  return mapId;
}
  
} //end of namespace
