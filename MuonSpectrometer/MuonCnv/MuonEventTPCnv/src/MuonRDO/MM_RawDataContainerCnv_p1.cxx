/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonEventTPCnv/MuonRDO/MM_RawDataContainerCnv_p1.h"
#include "MuonRDO/MM_RawDataContainer.h"
#include "MuonEventTPCnv/MuonRDO/MM_RawDataContainer_p1.h"
#include "MuonIdHelpers/MmIdHelper.h"

#define DEBUG 0

void Muon::MM_RawDataContainerCnv_p1::transToPers(const Muon::MM_RawDataContainer* transCont,  Muon::MM_RawDataContainer_p1* persCont, MsgStream &log) 
{
  // Not using T_AthenaPoolTPPtrVectorCnv because someday we should do something a bit more sophisticated here.
  size_t numColl = transCont->numberOfCollections();
  if (DEBUG) log<<MSG::VERBOSE<<"MM_RawDataContainerCnv_p1::transToPers with MM_RawDataContainer with this many collections: "<<numColl<<std::endl;
  
  persCont->resize(numColl);
  unsigned int i=0;
  for ( auto collection : *transCont){
    MM_RawDataCollection_p1& persColl = (*persCont)[i++];
    persColl.resize(collection->size());
    persColl.m_idHash = collection->m_idHash;
    for ( unsigned int j=0 ; j < collection->size() ; ++j ){
      const MM_RawData* transObj = (*collection)[j] ;
      MM_RawData_p1* persObj =  &( persColl[j] );
      m_chanCnv.transToPers( transObj, persObj, log );
      if (DEBUG) {
        log<<MSG::VERBOSE<<"Trans:"<<*transObj<<" Pers: "<<std::hex<<persObj->m_id<<std::dec<<std::endl;
      }
    }
  }
}

void  Muon::MM_RawDataContainerCnv_p1::persToTrans(const Muon::MM_RawDataContainer_p1* persCont, Muon::MM_RawDataContainer* transCont, MsgStream &log) 
{

  size_t numColl = persCont->size();

  if (DEBUG) log<<MSG::VERBOSE<<"MM_RawDataContainerCnv_p1::persToTrans with MM_RawDataContainer_p1 with this many collections "<<numColl<<std::endl;
    
  MM_RawDataCollection* coll = nullptr;
  for ( const MM_RawDataCollection_p1& collection : *persCont){

    coll = new MM_RawDataCollection(collection.m_idHash);
    if ( transCont->addCollection(coll,collection.m_idHash).isFailure() ) {
      log << MSG::WARNING << "Could not add the MM RDO collection during persistent -> transient conversion" << std::endl;
      return;
    }

    if (DEBUG) log<<MSG::VERBOSE<<"Have created empty transient collection, now looping over persistent coll with this many entries: "<<collection.size() <<std::endl;
    
    for ( unsigned int j=0 ; j < collection.size() ; ++j ){
      const MM_RawData_p1* persObj =  &( collection[j] );
      MM_RawData* transObj =  m_chanCnv.createTransient( persObj, log );
      coll->push_back(transObj);
      if (DEBUG) log<<MSG::VERBOSE<<*transObj<<std::endl;
    }
  }
  if (DEBUG) log<<MSG::VERBOSE<<"MM_RawDataContainerCnv_p1::persToTrans with MM_RawDataCollection with this many collections "<<transCont->numberOfCollections()<<std::endl;
}



//================================================================
Muon::MM_RawDataContainer* Muon::MM_RawDataContainerCnv_p1::createTransient(const Muon::MM_RawDataContainer_p1* persObj, MsgStream& log) 
{
  std::unique_ptr<MM_RawDataContainer> trans(new Muon::MM_RawDataContainer(m_idHelper->module_hash_max())); 
  if (DEBUG) log<<MSG::VERBOSE<<"MM_RawDataContainerCnv_p1::createTransient() Created new MM_RawDataContainer with this many collections "<<trans->numberOfCollections()<<std::endl;
  
  persToTrans(persObj, trans.get(), log);
  if (DEBUG) log<<MSG::VERBOSE<<"MM_RawDataContainerCnv_p1::createTransient() After persToTrans this many collections "<<trans->numberOfCollections()<<std::endl;
  
  return(trans.release());
}

void Muon::MM_RawDataContainerCnv_p1::initialize(const MmIdHelper* idHelper) 
{  
  m_idHelper = idHelper;
}
