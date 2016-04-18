/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
// file:   TauDetailsContainerCnv.cxx
// author: Lukasz Janyst
// date:   July 1007
//-----------------------------------------------------------------------------

#include "GaudiKernel/MsgStream.h"
#include "tauEventTPCnv/TauDetailsContainerCnv_tlp1.h"
#include "tauEventTPCnv/TauDetailsContainerCnv_tlp2.h"
#include "tauEventTPCnv/TauDetailsContainerCnv_tlp3.h"
#include "tauEventTPCnv/TauDetailsContainerCnv_tlp4.h"
#include "TauDetailsContainerCnv.h"

TauDetailsContainer_PERS*
TauDetailsContainerCnv :: createPersistent( Analysis :: TauDetailsContainer *transCont )
{
    MsgStream msg( msgSvc(), "TauDetailsContainerCnv" );
    TauDetailsContainerCnv_tlp4 cnv;
    TauDetailsContainer_PERS *persObj = cnv.createPersistent( transCont, msg );
    return persObj;
}

Analysis :: TauDetailsContainer *TauDetailsContainerCnv :: createTransient()
{
    MsgStream msg( msgSvc(), "TauDetailsContainerCnv" );

    Analysis :: TauDetailsContainer *transObj = 0;

    static pool :: Guid p1_guid( "727655C6-9CF7-476F-8AB4-747834E1FB02" );
    static pool :: Guid p2_guid( "AF3C1607-E62F-46C0-8BF5-C26874647C07" );
    static pool :: Guid p3_guid( "5973152f-5913-4a0c-84de-5cd392945858" );
    static pool :: Guid p4_guid( "FA5D3190-EC7A-4608-A7C6-A154D7FEFE18" );

    if( compareClassGuid( p4_guid ) ){
        // using unique_ptr ensures deletion of the persistent object
        std :: unique_ptr<TauDetailsContainer_tlp4> persObj( poolReadObject<TauDetailsContainer_tlp4>() );
        TauDetailsContainerCnv_tlp4 cnv;
        transObj = cnv.createTransient( persObj.get(), msg );
    }
        else if( compareClassGuid( p3_guid ) ){
        // using unique_ptr ensures deletion of the persistent object
        std :: unique_ptr<TauDetailsContainer_tlp3> persObj( poolReadObject<TauDetailsContainer_tlp3>() );
        TauDetailsContainerCnv_tlp3 cnv;
        transObj = cnv.createTransient( persObj.get(), msg );
    }
        else if( compareClassGuid( p2_guid ) ){
        // using unique_ptr ensures deletion of the persistent object
        std :: unique_ptr<TauDetailsContainer_tlp2> persObj( poolReadObject<TauDetailsContainer_tlp2>() );
        TauDetailsContainerCnv_tlp2 cnv;
        transObj = cnv.createTransient( persObj.get(), msg );
    }
	else if( compareClassGuid( p1_guid ) ){
        // using unique_ptr ensures deletion of the persistent object
        std :: unique_ptr<TauDetailsContainer_tlp1> persObj( poolReadObject<TauDetailsContainer_tlp1>() );
        TauDetailsContainerCnv_tlp1 cnv;
        transObj = cnv.createTransient( persObj.get(), msg );
    }
    else
        throw std::runtime_error("Unsupported persistent version of TauJetContainer");

  return transObj;
}
