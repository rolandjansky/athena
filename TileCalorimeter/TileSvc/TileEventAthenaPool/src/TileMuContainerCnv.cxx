///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TileMuContainerCnv.cxx 
// Implementation file for class TileMuContainerCnv
// Author: Aranzazu Ruiz <aranzazu.ruiz.martinez@cern.ch>
// Date:   July 2008
/////////////////////////////////////////////////////////////////// 

// Framework includes
#include "GaudiKernel/MsgStream.h"

// TileTPCnv includes
#include "TileTPCnv/TileMuContainerCnv_p1.h"

// TileEventAthenaPool includes
#include "TileMuContainerCnv.h"

TileMuContainer_PERS* 
TileMuContainerCnv::createPersistent( TileMuContainer* transCont ) 
{
  MsgStream msg( msgSvc(), "TileMuContainerCnv" );

  TileMuContainerCnv_p1 cnv;
  TileMuContainer_PERS *persObj = cnv.createPersistent( transCont, msg );

  if (msg.level()<=MSG::DEBUG)
    msg << MSG::DEBUG << "::createPersistent [Success]" << endmsg;
  return persObj; 
}

TileMuContainer* TileMuContainerCnv::createTransient() {

  MsgStream msg( msgSvc(), "TileMuContainerCnv" );

  TileMuContainer *transObj = 0;

  static pool::Guid tr_guid("FC0456E4-912B-425B-9AA2-4DDD0C6B2275");
  static pool::Guid p1_guid("DE8904EB-25FD-495A-8DD5-E31B05E397C6");

  if ( compareClassGuid(tr_guid) ) {

    // regular object from before the T/P separation
    return poolReadObject<TileMuContainer>();

  } else if ( compareClassGuid(p1_guid) ) {

    // using auto_ptr ensures deletion of the persistent object
    std::auto_ptr<TileMuContainer_p1> persObj( poolReadObject<TileMuContainer_p1>() );
    TileMuContainerCnv_p1 cnv;
    transObj = cnv.createTransient( persObj.get(), msg );
  } else {
    throw std::runtime_error("Unsupported persistent version of TileMuContainer");
  }

  return transObj;
}
