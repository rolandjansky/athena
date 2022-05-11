/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonByteStreamErrors/RpcByteStreamErrorContainer.h"
#include "RpcByteStreamErrorContainerCnv.h"
#include "MuonConditionsAthenaPool/MuonByteStreamErrors/RpcByteStreamErrorContainer_p1.h"
#include "MuonConditionsAthenaPool/MuonByteStreamErrors/RpcByteStreamErrorContainerCnv_p1.h"

static const RpcByteStreamErrorContainerCnv_p1   TPconverter_p1;

RpcByteStreamErrorContainer_PERS* RpcByteStreamErrorContainerCnv::createPersistent(Muon::RpcByteStreamErrorContainer* transObj) {
  using namespace Muon;
  MsgStream log(msgSvc(), "RpcByteStreamErrorContainer converter");
  RpcByteStreamErrorContainer_PERS *persObj = TPconverter_p1.createPersistentConst( transObj, log );
  return persObj;
}
   
Muon::RpcByteStreamErrorContainer* RpcByteStreamErrorContainerCnv::createTransient() {
  using namespace Muon;
  static const pool::Guid   p1_guid("4E46BDDC-E1F9-420A-A11F-47EF082A3E3A");
  if( compareClassGuid(p1_guid) ) {
    std::unique_ptr< RpcByteStreamErrorContainer_p1 > col_vect( poolReadObject< RpcByteStreamErrorContainer_p1 >() );
    MsgStream log(msgSvc(), "RpcByteStreamErrorContainer_p1" );
    return TPconverter_p1.createTransientConst( col_vect.get(), log );
  } 
  throw std::runtime_error("Unsupported persistent version of RpcByteStreamErrorContainer");
}
