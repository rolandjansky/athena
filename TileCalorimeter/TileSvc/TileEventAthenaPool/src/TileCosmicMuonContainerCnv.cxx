///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TileCosmicMuonContainerCnv.cxx 
// Implementation file for class TileCosmicMuonContainerCnv
// Author: Aranzazu Ruiz <aranzazu.ruiz.martinez@cern.ch>
// Date:   July 2008
// Revised:   July 2009 maneira@lip.pt
/////////////////////////////////////////////////////////////////// 

// Framework includes
#include "GaudiKernel/MsgStream.h"

// TileTPCnv includes
#include "TileTPCnv/TileCosmicMuonContainerCnv_p1.h"
#include "TileTPCnv/TileCosmicMuonContainerCnv_p2.h"

// TileEventAthenaPool includes
#include "TileCosmicMuonContainerCnv.h"

TileCosmicMuonContainer_PERS* 
TileCosmicMuonContainerCnv::createPersistent( TileCosmicMuonContainer* transCont ) 
{
  MsgStream msg( msgSvc(), "TileCosmicMuonContainerCnv" );

  TileCosmicMuonContainerCnv_p2 cnv;
  TileCosmicMuonContainer_PERS *persObj = cnv.createPersistent( transCont, msg );

  if (msg.level()<=MSG::DEBUG)
    msg << MSG::DEBUG << "::createPersistent [Success]" << endmsg;
  return persObj; 
}

TileCosmicMuonContainer* TileCosmicMuonContainerCnv::createTransient() {

  MsgStream msg( msgSvc(), "TileCosmicMuonContainerCnv" );

  TileCosmicMuonContainer *transObj = 0;

  static pool::Guid tr_guid("81711CFC-C75C-49F4-BA12-31D6FAA2D93");
  static pool::Guid p0_guid("51A1E48E-5D5D-4491-85D0-3EFCCEBA5253"); //old before T/P
  static pool::Guid p1_guid("716F406D-8F59-4879-AA07-C28BA374E6EF");
  static pool::Guid p2_guid("8957FF04-12E8-43BA-A0C8-D7D9638E242E");

  if ( compareClassGuid(tr_guid) ) {

    // regular object from before the T/P separation
    return poolReadObject<TileCosmicMuonContainer>();

  } else if ( compareClassGuid(p1_guid) ) {

    // using auto_ptr ensures deletion of the persistent object
    std::auto_ptr<TileCosmicMuonContainer_p1> persObj( poolReadObject<TileCosmicMuonContainer_p1>() );
    TileCosmicMuonContainerCnv_p1 cnv;
    transObj = cnv.createTransient( persObj.get(), msg );
  } else if ( compareClassGuid(p2_guid) ) {

    // using auto_ptr ensures deletion of the persistent object
    std::auto_ptr<TileCosmicMuonContainer_p2> persObj( poolReadObject<TileCosmicMuonContainer_p2>() );
    TileCosmicMuonContainerCnv_p2 cnv;
    transObj = cnv.createTransient( persObj.get(), msg );
  } else {
    throw std::runtime_error("Unsupported persistent version of TileCosmicMuonContainer");
  }

  return transObj;
}
