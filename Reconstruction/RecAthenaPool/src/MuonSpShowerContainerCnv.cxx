/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Framework includes
#include "GaudiKernel/MsgStream.h"

// RecTPCnv includes
#include "RecTPCnv/MuonSpShowerContainerCnv_p1.h"

// RecAthenaPool includes
#include "MuonSpShowerContainerCnv.h"

MuonSpShowerContainer_PERS* 
MuonSpShowerContainerCnv::createPersistent( Rec::MuonSpShowerContainer* transCont ) 
{
    MsgStream msg( msgSvc(), "MuonSpShowerContainerCnv" );
    // std::cout<<"Test - MuonSpShowerContainerCnv::createPersistent()"<<std::endl;

    MuonSpShowerContainerCnv_p1 cnv;
    MuonSpShowerContainer_PERS *persObj = cnv.createPersistent( transCont, msg );

    msg << MSG::DEBUG << "::createPersistent [Success]" << endreq;
    return persObj; 
}

Rec::MuonSpShowerContainer* 
MuonSpShowerContainerCnv::createTransient() 
{
    MsgStream msg( msgSvc(), "MuonSpShowerContainerConverter" );
    // std::cout<<"Test - MuonSpShowerContainerCnv::createTransient()"<<std::endl;
    Rec::MuonSpShowerContainer *transObj = 0;

    static pool::Guid tr_guid("64AE1E07-67EC-549C-DE00-A953259EBC9E");
    static pool::Guid p1_guid("522A7143-856B-41E6-A3FA-AB97693010D8");

    if ( compareClassGuid(tr_guid) ) {

    // regular object from before the T/P separation
        return poolReadObject<Rec::MuonSpShowerContainer>();

    } else if ( compareClassGuid(p1_guid) ) {

    // using unique_ptr ensures deletion of the persistent object
        std::unique_ptr<MuonSpShowerContainer_p1> persObj( poolReadObject<MuonSpShowerContainer_p1>() );
        MuonSpShowerContainerCnv_p1 cnv;
        transObj = cnv.createTransient( persObj.get(), msg );
    } else {
        throw std::runtime_error("Unsupported persistent version of MuonSpShowerContainer");
    }

    return transObj;
}
