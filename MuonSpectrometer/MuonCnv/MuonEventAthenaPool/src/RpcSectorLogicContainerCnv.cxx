/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonRDO/RpcSectorLogicContainer.h"
#include "RpcSectorLogicContainerCnv.h"
#include "MuonEventAthenaPool/RpcSectorLogicContainer_p1.h"
#include "RpcSectorLogicContainerCnv_p1.h"

static RpcSectorLogicContainerCnv_p1   TPconverter_p1;

RpcSectorLogicContainer_p1* RpcSectorLogicContainerCnv::createPersistent(RpcSectorLogicContainer* transObj) {
    MsgStream log(msgSvc(), "MuonRpcSectorLogicContainerConverter" );
    RpcSectorLogicContainer_p1 *persObj = TPconverter_p1.createPersistent( transObj, log );
    if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "RpcSectorLogicContainer write Success" << endmsg;
    return persObj;
}
   
RpcSectorLogicContainer* RpcSectorLogicContainerCnv::createTransient() {
    static pool::Guid   p1_guid("93035F54-0FA9-4A56-98E0-A808DD23C089");
    if( compareClassGuid(p1_guid) ) {
        // using unique_ptr ensures deletion of the persistent object
        std::unique_ptr< RpcSectorLogicContainer_p1 > col_vect( poolReadObject< RpcSectorLogicContainer_p1 >() );
        MsgStream log(msgSvc(), "RpcSectorLogicContainerCnv_p1" );
        //log << MSG::DEBUG << "Reading RpcSectorLogicContainer_p1" << endmsg;
        return TPconverter_p1.createTransient( col_vect.get(), log );
    } 
    throw std::runtime_error("Unsupported persistent version of RpcSectorLogicContainer");
}
