///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TileTTL1ContainerCnv.cxx 
// Implementation file for class TileTTL1ContainerCnv
// Author: Alexander Solodkov <Sanya.Solodkov@cern.ch>
// Date:   June 2009
/////////////////////////////////////////////////////////////////// 

// Framework includes
#include "GaudiKernel/MsgStream.h"

// TileTPCnv includes
#include "TileTPCnv/TileTTL1ContainerCnv_p1.h"

// TileEventAthenaPool includes
#include "TileTTL1ContainerCnv.h"

TileTTL1Container_PERS* 
TileTTL1ContainerCnv::createPersistent( TileTTL1Container* transCont ) 
{
  MsgStream msg( msgSvc(), "TileTTL1ContainerCnv" );

  TileTTL1ContainerCnv_p1 cnv;
  TileTTL1Container_PERS *persObj = cnv.createPersistent( transCont, msg );

  if (msg.level()<=MSG::DEBUG)
    msg << MSG::DEBUG << "::createPersistent [Success]" << endmsg;
  return persObj; 
}

TileTTL1Container* TileTTL1ContainerCnv::createTransient() {

  MsgStream msg( msgSvc(), "TileTTL1ContainerCnv" );

  TileTTL1Container *transObj = 0;

  static pool::Guid tr_guid("F06C00C1-896A-4773-9F9C-21893C1E0CE7");
  static pool::Guid p1_guid("213FA88B-52AE-4229-8886-62D7D54FA7A3");

  if ( compareClassGuid(tr_guid) ) {

    // regular object from before the T/P separation
    return poolReadObject<TileTTL1Container>();

  } else if ( compareClassGuid(p1_guid) ) {

    // using auto_ptr ensures deletion of the persistent object
    std::auto_ptr<TileTTL1Container_p1> persObj( poolReadObject<TileTTL1Container_p1>() );
    TileTTL1ContainerCnv_p1 cnv;
    transObj = cnv.createTransient( persObj.get(), msg );
  } else {
    throw std::runtime_error("Unsupported persistent version of TileTTL1Container");
  }

  return transObj;
}
