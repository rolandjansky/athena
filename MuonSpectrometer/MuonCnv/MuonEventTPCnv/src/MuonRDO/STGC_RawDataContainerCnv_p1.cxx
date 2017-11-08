/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonEventTPCnv/MuonRDO/STGC_RawDataContainerCnv_p1.h"
#include "MuonRDO/STGC_RawDataContainer.h"
#include "MuonEventTPCnv/MuonRDO/STGC_RawDataContainer_p1.h"
#include "MuonIdHelpers/sTgcIdHelper.h"

#define DEBUG 1

void Muon::STGC_RawDataContainerCnv_p1::transToPers(const Muon::STGC_RawDataContainer* transCont,  Muon::STGC_RawDataContainer_p1* persCont, MsgStream &log) 
{
  // Not using T_AthenaPoolTPPtrVectorCnv because someday we should do something a bit more sophisticated here.
  size_t numColl = transCont->size();
  if (DEBUG) std::cout<<"STGC_RawDataContainerCnv_p1::transToPers with STGC_RawDataContainer with this many collections: "<<numColl<<std::endl;
  
  persCont->resize(numColl);
  unsigned int i=0;
  for ( auto collection : *transCont){
    STGC_RawDataCollection_p1 persColl = (*persCont)[i++];
    persColl.resize(collection->size());
    persColl.m_idHash = collection->m_idHash;
    for ( unsigned int j=0 ; j < collection->size() ; ++j ){
      const STGC_RawData* transObj = (*collection)[j] ;
      STGC_RawData_p1* persObj =  &( persColl[j] );
      m_chanCnv.transToPers( transObj, persObj, log );
      if (DEBUG) std::cout<<*transObj<<std::endl;
    }
  }
}

void  Muon::STGC_RawDataContainerCnv_p1::persToTrans(const Muon::STGC_RawDataContainer_p1* persCont, Muon::STGC_RawDataContainer* transCont, MsgStream &log) 
{
  size_t numColl = persCont->size();
  if (DEBUG) std::cout<<"STGC_RawDataContainerCnv_p1::persToTrans with STGC_RawDataContainer_p1 with this many collections "<<numColl<<std::endl;
    
  Muon::STGC_RawDataCollection* coll = 0;
  for ( auto collection : *persCont){
    coll = new STGC_RawDataCollection(collection.m_idHash);
    transCont->addCollection(coll,collection.m_idHash);
    for ( unsigned int j=0 ; j < collection.size() ; ++j ){
      STGC_RawData_p1* persObj =  &( collection[j] );
      STGC_RawData* transObj =  m_chanCnv.createTransient( persObj, log );
      coll->push_back(transObj);
      if (DEBUG) std::cout<<*transObj<<std::endl;
    }
  }
}



//================================================================
Muon::STGC_RawDataContainer* Muon::STGC_RawDataContainerCnv_p1::createTransient(const Muon::STGC_RawDataContainer_p1* persObj, MsgStream& log) 
{
    std::unique_ptr<STGC_RawDataContainer> trans(new Muon::STGC_RawDataContainer(m_sTgcId->detectorElement_hash_max()));
    persToTrans(persObj, trans.get(), log);
    return(trans.release());
}


