/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonFeatureDetailsContainerCnv.h"
#include "TrigMuonEventTPCnv/MuonFeatureDetailsContainer_tlp1.h"
#include "TrigMuonEventTPCnv/MuonFeatureDetailsContainer_p2.h"


MuonFeatureDetailsContainer_PERS * MuonFeatureDetailsContainerCnv::createPersistent( MuonFeatureDetailsContainer *transObj) {
    MsgStream mlog(msgSvc(), "MuonFeatureDetailsContainerConverter" );
    mlog << MSG::DEBUG << "MuonFeatureDetailsContainerCnv::createPersistent called" << endmsg;

    MuonFeatureDetailsContainer_PERS * p_cont = m_converter.createPersistent( transObj, mlog );

    return p_cont;
}


MuonFeatureDetailsContainer * MuonFeatureDetailsContainerCnv::createTransient() {
    
    MsgStream mlog(msgSvc(), "MuonFeatureDetailsContainerConverter" );
    // mlog << MSG::DEBUG << "MuonFeatureDetailsContainerCnv::createTransient called" << endmsg;

    static const pool::Guid tlp1_guid( "CF2FFCB2-3936-4800-9146-52B203A47478" );
    static const pool::Guid p2_guid( "95327E52-C8B2-45E4-9EAF-C65A17AB27F5" );
    
    if( compareClassGuid( p2_guid ) ){
         std::unique_ptr< MuonFeatureDetailsContainer_p2 > col_vect( poolReadObject< MuonFeatureDetailsContainer_p2 >() );
         // std::cout << "Reading MFDC p2" << std::endl; 
         return m_converter.createTransient( col_vect.get(), mlog ) ;
    }
    else if( compareClassGuid( tlp1_guid ) ){
         std::unique_ptr< MuonFeatureDetailsContainer_tlp1 > col_vect( poolReadObject< MuonFeatureDetailsContainer_tlp1 >() );
         // std::cout << "Reading MFDC tlp1" << std::endl; 
         return m_converter1.createTransient( col_vect.get(), mlog );
        
    }
    else  throw std::runtime_error( "Unsupported persistent version of MuonFeatureDetailsContainer" );

}


