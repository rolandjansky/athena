/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonTGC_Cabling/TGCCableSSWToROD.h"

#include "TGCcablingInterface/TGCIdBase.h"
#include "MuonTGC_Cabling/TGCDatabaseSLBToROD.h" 
#include "MuonTGC_Cabling/TGCModuleSSW.h"
#include "MuonTGC_Cabling/TGCModuleROD.h"

namespace MuonTGC_Cabling {

// Constructor & Destructor
TGCCableSSWToROD::TGCCableSSWToROD(std::string filename)
  : TGCCable(TGCCable::SSWToROD)
{
  database = new TGCDatabaseSLBToROD(filename,"SSW ALL");
}

TGCCableSSWToROD::TGCCableSSWToROD (void)
  : TGCCable(TGCCable::SSWToROD),
    database(0)
{
}

TGCCableSSWToROD::TGCCableSSWToROD (const TGCCableSSWToROD& right)
  : TGCCable(TGCCable::SSWToROD),
    database(0)
{
  TGCDatabaseSLBToROD* mypointer = dynamic_cast<TGCDatabaseSLBToROD*>(right.database);
  if(mypointer) database = new TGCDatabaseSLBToROD(*mypointer);
  else database = 0;
}

TGCCableSSWToROD& TGCCableSSWToROD::operator=(const TGCCableSSWToROD& right)
{
  if (this != &right) {
    delete database;
    TGCDatabaseSLBToROD* mypointer = dynamic_cast<TGCDatabaseSLBToROD*>(right.database);
    if(mypointer) database = new TGCDatabaseSLBToROD(*mypointer);
    else database = 0;
  }
  return *this;
}
  
TGCCableSSWToROD::~TGCCableSSWToROD(void)
{
  delete database;
}
 

TGCModuleMap* TGCCableSSWToROD::getModule(const TGCModuleId* moduleId) const {
  if(moduleId){
    if(moduleId->getModuleIdType()==TGCModuleId::SSW)
      return getModuleOut(moduleId);
    if(moduleId->getModuleIdType()==TGCModuleId::ROD)
      return getModuleIn(moduleId);
  }
  return 0;
}

TGCModuleMap* TGCCableSSWToROD::getModuleIn(const TGCModuleId* rod) const {
  if(!rod->isValid()) return 0;

  const TGCIdBase::SideType rodSideType = rod->getSideType(); 
  const int rodReadoutSector = rod->getReadoutSector();

  TGCModuleMap* mapId = 0;
  const int MaxEntry = database->getMaxEntry();
  for(int i=0; i<MaxEntry; i++){
    int id = database->getEntry(i,0);
    int block = database->getEntry(i,1);
    TGCModuleSSW* ssw = new TGCModuleSSW(rodSideType,
					 rodReadoutSector,
					 id);
    if(mapId==0) mapId = new TGCModuleMap();
    mapId->insert(block,ssw); 
  }
  return mapId;
}
  
TGCModuleMap* TGCCableSSWToROD::getModuleOut(const TGCModuleId* ssw) const {
  if(!ssw->isValid()) return 0;

  const int sswId = ssw->getId();

  TGCModuleMap* mapId = 0;
  const int MaxEntry = database->getMaxEntry();
  for(int i=0; i<MaxEntry; i++){
    if(database->getEntry(i,0)==sswId){
      int block = database->getEntry(i,1);
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
