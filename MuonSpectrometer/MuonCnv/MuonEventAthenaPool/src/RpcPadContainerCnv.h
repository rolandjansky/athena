/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
  custom POOL converter for RPC PAD

  @author Tadashi Maeno 
       based on SCT_RDO_ContainerCnv by RD Schaffer
*/

#ifndef MUONEVENTATHENAPOOL_RPCPADCONTAINERCNV_H
#define MUONEVENTATHENAPOOL_RPCPADCONTAINERCNV_H

#include "MuonRdoContainerTPCnv.h"
#include "MuonRDO/RpcPadContainer.h"
#include "RpcPadContainerCnv_p1.h"
#include "RpcPadContainerCnv_p2.h"
#include "MuonEventAthenaPool/RpcPadContainer_p1.h"
#include "MuonEventAthenaPool/RpcPadContainer_p2.h"


typedef MuonRdoContainerTPCnv<RpcPadContainer, RpcPadContainer_p2, RpcPadContainerCnv_p2 >
RpcPadContainerCnv;
#include "MuonEventAthenaPool/CreateTransientTemplates.h"

template < >
inline
RpcPadContainer*
RpcPadContainerCnv::createTransient()
{
    MsgStream log(msgSvc(), "RpcPadContainerCnv" );
    if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << " **** Entered createTransient() "<< endmsg;

    // the use of auto pointers ensures that the persistent object is deleted
    // using the correct persistent type pointer

    RpcPadContainer *trans_cont = 0;
    static pool::Guid	p2_guid("3B29433B-015F-4664-AB3F-A6B2FAA8C97F");
    static pool::Guid	p1_guid("60A912DD-7920-4DCE-986E-6CD5644835BD");
    static pool::Guid	p0_guid("85B897F6-E15D-4215-9DAC-EA2828BCEEC9");
    if( compareClassGuid(p2_guid) ) {
        if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << " **** createTransient - p2 " << endmsg;

        std::unique_ptr< RpcPadContainer_p2 > col_vect( poolReadObject< RpcPadContainer_p2 >() );
        trans_cont = m_TPconverter.createTransient( col_vect.get(), log );
    } else if( compareClassGuid(p1_guid) ) {
        if (log.level() <= MSG::DEBUG)log << MSG::DEBUG << " **** createTransient - p1 "
            << endmsg;

        std::unique_ptr< RpcPadContainer_p1 > col_vect( poolReadObject< RpcPadContainer_p1 >() );
        RpcPadContainerCnv_p1 cnv;
        trans_cont = cnv.createTransient( col_vect.get(), log );
    } else if( compareClassGuid(p0_guid) ) {
        if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << " **** createTransient - p0 "
            << endmsg;

        // old version from before TP separation
        std::unique_ptr< COLL_vector >	col_vect( this->poolReadObject< COLL_vector >() );
        trans_cont = createTransientFrom_p0( col_vect.get(), log );
    }
    else {
        throw std::runtime_error("Unsupported persistent version of RPC RDO container");
    }
    return trans_cont;
}

#endif




