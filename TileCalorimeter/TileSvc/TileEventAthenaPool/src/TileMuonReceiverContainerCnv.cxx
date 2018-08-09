///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TileMuonReceveirContainerCnv.cxx 
// Implementation file for class TileMuonReceiverContainerCnv
// Author: Joao Gentil Saraiva <joao.gentil.saraiva@cern.ch>
// Date:   March 2014
/////////////////////////////////////////////////////////////////// 

// Framework includes
#include "GaudiKernel/MsgStream.h"

// TileTPCnv includes
#include "TileTPCnv/TileMuonReceiverContainerCnv_p1.h"

// TileEventAthenaPool includes
#include "TileMuonReceiverContainerCnv.h"

TileMuonReceiverContainer_PERS* 
TileMuonReceiverContainerCnv::createPersistent( TileMuonReceiverContainer* transCont ) 
{
  MsgStream msg( msgSvc(), "TileMuonReceiverContainerCnv" );

  TileMuonReceiverContainerCnv_p1 cnv;
  TileMuonReceiverContainer_PERS *persObj = cnv.createPersistent( transCont, msg );

  if (msg.level()<=MSG::DEBUG)
    msg << MSG::DEBUG << "::createPersistent [Success]" << endmsg;
  return persObj; 
}

TileMuonReceiverContainer* TileMuonReceiverContainerCnv::createTransient() {

  MsgStream msg( msgSvc(), "TileMuonReceiverContainerCnv" );

  TileMuonReceiverContainer *transObj = 0;

  static pool::Guid tr_guid("A01F819F-4F0B-4713-A275-7DA5F47F77E5");
  static pool::Guid p1_guid("7F0DA5A3-7773-4852-AE55-A9920FEB31AB");

  if ( compareClassGuid(tr_guid) ) {

    // regular object from before the T/P separation
    return poolReadObject<TileMuonReceiverContainer>();

  } else if ( compareClassGuid(p1_guid) ) {
    // using auto_ptr ensures deletion of the persistent object
    std::auto_ptr<TileMuonReceiverContainer_p1> persObj( poolReadObject<TileMuonReceiverContainer_p1>() );
    TileMuonReceiverContainerCnv_p1 cnv;
    transObj = cnv.createTransient( persObj.get(), msg );
  } else {
    throw std::runtime_error("Unsupported persistent version of TileMuonReceiverContainer");
  }

  return transObj;
}
