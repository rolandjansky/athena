/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonFeatureDetailsContainerCnv.h"
#include "TrigMuonEventTPCnv/MuonFeatureDetailsContainer_tlp1.h"
#include "TrigMuonEventTPCnv/MuonFeatureDetailsContainerCnv_tlp1.h"

#include "TrigMuonEventTPCnv/MuonFeatureDetailsContainer_p2.h"
#include "TrigMuonEventTPCnv/MuonFeatureDetailsContainerCnv_p2.h"

static MuonFeatureDetailsContainerCnv_tlp1   TLPconverter1;
static MuonFeatureDetailsContainerCnv_p2     TPconverter;
  
MuonFeatureDetailsContainer_PERS * MuonFeatureDetailsContainerCnv::createPersistent( MuonFeatureDetailsContainer *transObj) {
    MsgStream mlog(messageService(), "MuonFeatureDetailsContainerConverter" );
    mlog << MSG::DEBUG << "MuonFeatureDetailsContainerCnv::createPersistent called" << endreq;

    MuonFeatureDetailsContainer_PERS * p_cont = TPconverter.createPersistent( transObj, mlog );

    return p_cont;
}


MuonFeatureDetailsContainer * MuonFeatureDetailsContainerCnv::createTransient() {
    
    MsgStream mlog(messageService(), "MuonFeatureDetailsContainerConverter" );
    // mlog << MSG::DEBUG << "MuonFeatureDetailsContainerCnv::createTransient called" << endreq;

    static pool::Guid tlp1_guid( "CF2FFCB2-3936-4800-9146-52B203A47478" );
    static pool::Guid p2_guid( "95327E52-C8B2-45E4-9EAF-C65A17AB27F5" );
    
    if( compareClassGuid( p2_guid ) ){
         std::auto_ptr< MuonFeatureDetailsContainer_p2 > col_vect( poolReadObject< MuonFeatureDetailsContainer_p2 >() );
         // std::cout << "Reading MFDC p2" << std::endl; 
         return TPconverter.createTransient( col_vect.get(), mlog ) ;
    }
    else if( compareClassGuid( tlp1_guid ) ){
         std::auto_ptr< MuonFeatureDetailsContainer_tlp1 > col_vect( poolReadObject< MuonFeatureDetailsContainer_tlp1 >() );
         // std::cout << "Reading MFDC tlp1" << std::endl; 
         return TLPconverter1.createTransient( col_vect.get(), mlog );
        
    }
    else  throw std::runtime_error( "Unsupported persistent version of MuonFeatureDetailsContainer" );

}


