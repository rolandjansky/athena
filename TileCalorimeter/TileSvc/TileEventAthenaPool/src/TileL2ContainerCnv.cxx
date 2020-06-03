///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TileL2ContainerCnv.cxx 
// Implementation file for class TileL2ContainerCnv
// Author: Aranzazu Ruiz <aranzazu.ruiz.martinez@cern.ch>
// Date:   July 2008
/////////////////////////////////////////////////////////////////// 

// Framework includes
#include "GaudiKernel/MsgStream.h"

// TileTPCnv includes
#include "TileTPCnv/TileL2ContainerCnv_p1.h"
#include "TileTPCnv/TileL2ContainerCnv_p2.h"

// TileEventAthenaPool includes
#include "TileL2ContainerCnv.h"

TileL2Container_PERS* 
TileL2ContainerCnv::createPersistent( TileL2Container* transCont ) 
{
  MsgStream msg( msgSvc(), "TileL2ContainerCnv" );

  TileL2ContainerCnv_p2 cnv;
  TileL2Container_PERS *persObj = cnv.createPersistent( transCont, msg );

  if (msg.level()<=MSG::DEBUG)
    msg << MSG::DEBUG << "::createPersistent [Success]" << endmsg;
  return persObj; 
}

TileL2Container* TileL2ContainerCnv::createTransient() {

  MsgStream msg( msgSvc(), "TileL2ContainerCnv" );

  TileL2Container *transObj = 0;

  static pool::Guid tr_guid("E9BB4497-FF43-4D70-8A64-B509081DAD88");
  static pool::Guid p1_guid("FE877FF9-475D-42D9-8DC3-CABD220BFD27");
  static pool::Guid p2_guid("3283BF32-0B18-408F-9B46-6A1B26B37D5C");

  if ( compareClassGuid(tr_guid) ) {

    // regular object from before the T/P separation
    return poolReadObject<TileL2Container>();

  } else if ( compareClassGuid(p1_guid) ) {

    // using auto_ptr ensures deletion of the persistent object
    std::auto_ptr<TileL2Container_p1> persObj( poolReadObject<TileL2Container_p1>() );
    TileL2ContainerCnv_p1 cnv;
    transObj = cnv.createTransient( persObj.get(), msg );
  } else if ( compareClassGuid(p2_guid) ) {

    // using auto_ptr ensures deletion of the persistent object
    std::auto_ptr<TileL2Container_p2> persObj( poolReadObject<TileL2Container_p2>() );
    TileL2ContainerCnv_p2 cnv;
    transObj = cnv.createTransient( persObj.get(), msg );
  } else {
    throw std::runtime_error("Unsupported persistent version of TileL2Container");
  }

  return transObj;
}
