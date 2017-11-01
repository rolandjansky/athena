/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonEventTPCnv/MuonRDO/STGC_RawDataContainerCnv_p1.h"
#include "MuonRDO/STGC_RawDataContainer.h"
#include "MuonEventTPCnv/MuonRDO/STGC_RawDataContainer_p1.h"
#include "MuonIdHelpers/sTgcIdHelper.h"

void Muon::STGC_RawDataContainerCnv_p1::transToPers(const Muon::STGC_RawDataContainer* transCont,  Muon::STGC_RawDataContainer_p1* persCont, MsgStream &log) 
{
  std::cout<<"STGC_RawDataContainerCnv_p1::transToPers"<<std::endl;
  // Not using T_AthenaPoolTPPtrVectorCnv here because someday we should do something a bit more sophisticated here.
  STGC_RawDataCnv_p1  chanCnv;
  size_t numColl = transCont->size();
  persCont->resize(numColl);
  // ATH_MSG_DEBUG("About to loop over STGC_RawDataContainer with "+numColl);
  unsigned int i=0;
  for ( auto collection : *transCont){
    STGC_RawDataCollection_p1 persColl = (*persCont)[i++];
    persColl.m_idHash = collection->m_idHash;
    for ( unsigned int j=0 ; j < collection->size() ; ++j ){
      const STGC_RawData* transObj = (*collection)[j] ;
      STGC_RawData_p1* persObj =  &( persColl[j] );
      chanCnv.transToPers( transObj, persObj, log );
    }
  }
}

void  Muon::STGC_RawDataContainerCnv_p1::persToTrans(const Muon::STGC_RawDataContainer_p1* persCont, Muon::STGC_RawDataContainer* transCont, MsgStream &log) 
{
  std::cout<<"STGC_RawDataContainerCnv_p1::persToTrans"<<std::endl;
  
//
    STGC_RawDataCnv_p1  chanCnv;

}



//================================================================
Muon::STGC_RawDataContainer* Muon::STGC_RawDataContainerCnv_p1::createTransient(const Muon::STGC_RawDataContainer_p1* persObj, MsgStream& log) 
{
  // std::cout<<"STGC_RawDataContainerCnv_p1::createTransient"<<std::endl;
    std::unique_ptr<STGC_RawDataContainer> trans(new Muon::STGC_RawDataContainer(m_sTgcId->detectorElement_hash_max()));
    persToTrans(persObj, trans.get(), log);
    return(trans.release());
}


