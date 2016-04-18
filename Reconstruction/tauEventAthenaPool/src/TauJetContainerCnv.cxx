/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
// file:   TauJetContainerCnv.cxx
// author: Kyle Cranmer, Sebastien Binet, Lukasz Janyst
// date:   April 1007
//-----------------------------------------------------------------------------

#include "GaudiKernel/MsgStream.h"
#include "tauEventTPCnv/TauJetContainerCnv_p1.h"
#include "tauEventTPCnv/TauJetContainerCnv_p2.h"
#include "tauEventTPCnv/TauJetContainerCnv_p3.h"
#include "tauEventTPCnv/TauJetContainerCnv_p4.h"
#include "tauEventTPCnv/TauJetContainerCnv_p5.h"
#include "TauJetContainerCnv.h"

TauJetContainer_PERS*
TauJetContainerCnv :: createPersistent( Analysis :: TauJetContainer *transCont )
{
    MsgStream msg( msgSvc(), "TauJetContainerCnv" );
    TauJetContainerCnv_p5 cnv;
    TauJetContainer_PERS *persObj = cnv.createPersistent( transCont, msg );
    return persObj;
}

Analysis :: TauJetContainer *TauJetContainerCnv :: createTransient()
{
    MsgStream msg( msgSvc(), "TauJetContainerCnv" );

    Analysis :: TauJetContainer *transObj = 0;

    static pool::Guid p1_guid( "AD52E539-5A69-493A-B33C-7BE558348EBA" );
    static pool::Guid p2_guid( "3F9C4AF7-1B48-4DBC-BA24-F7CF658E7820" );
    static pool::Guid p3_guid( "3B6CC0D5-D033-45A6-9440-0276EE55B4C5" );
    static pool::Guid p4_guid( "87896E97-2688-4373-BD6F-6FC0ED406C2B" );
    static pool::Guid p5_guid( "7D98A685-ED84-43AD-9B35-69295E19D402" );

    if( compareClassGuid( p5_guid ) ){
        // using unique_ptr ensures deletion of the persistent object
        std :: unique_ptr<TauJetContainer_p5> persObj(poolReadObject<TauJetContainer_p5>());
        TauJetContainerCnv_p5 cnv;
        transObj = cnv.createTransient( persObj.get(), msg );
    }
    else if( compareClassGuid( p4_guid ) ){
        // using unique_ptr ensures deletion of the persistent object
        std :: unique_ptr<TauJetContainer_p4> persObj(poolReadObject<TauJetContainer_p4>());
        TauJetContainerCnv_p4 cnv;
        transObj = cnv.createTransient( persObj.get(), msg );
    }
    else if( compareClassGuid( p3_guid ) ){
        // using unique_ptr ensures deletion of the persistent object
        std :: unique_ptr<TauJetContainer_p3> persObj( poolReadObject<TauJetContainer_p3>() );
        TauJetContainerCnv_p3 cnv;
        transObj = cnv.createTransient( persObj.get(), msg );
    }
    else if( compareClassGuid( p2_guid ) ){
        // using unique_ptr ensures deletion of the persistent object
        std :: unique_ptr<TauJetContainer_p2> persObj( poolReadObject<TauJetContainer_p2>() );
        TauJetContainerCnv_p2 cnv;
        transObj = cnv.createTransient( persObj.get(), msg );
    }
    else if( compareClassGuid( p1_guid ) ){
        // using unique_ptr ensures deletion of the persistent object
        std :: unique_ptr<TauJetContainer_p1> persObj( poolReadObject<TauJetContainer_p1>() );
        TauJetContainerCnv_p1 cnv;
        transObj = cnv.createTransient( persObj.get(), msg );
    }
    else
        throw std::runtime_error("Unsupported persistent version of TauJetContainer");

  return transObj;
}
