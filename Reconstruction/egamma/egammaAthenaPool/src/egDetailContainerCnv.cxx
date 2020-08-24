///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// egDetailContainerCnv.cxx 
// Implementation file for class egDetailContainerCnv
// Author: K.Cranmer<cranmer@cern.ch>
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 

// STL includes

// Framework includes
#include "GaudiKernel/MsgStream.h"

// egammaEventTPCnv includes
#include "egammaEventTPCnv/egDetailContainerCnv_p1.h"
#include "egammaEventTPCnv/egDetailContainerCnv_p2.h"

// RecAthenaPool includes
#include "egDetailContainerCnv.h"
#include "egammaMsgUtil.h"

/////////////////////////////////////////////////////////////////// 
/// Public methods: 
/////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////// 
/// Const methods: 
/////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////// 
// Protected methods: 
/////////////////////////////////////////////////////////////////// 


egDetailContainer_PERS* 
egDetailContainerCnv::createPersistent( egDetailContainer* transCont ) 
{
  MsgStream msg( msgSvc(), "egDetailContainerCnv" );

  egDetailContainerCnv_p2 cnv;
  egDetailContainer_PERS *persObj = cnv.createPersistent( transCont, msg );

  MSG_DEBUG(msg, "::createPersistent [Success]");
  return persObj; 
}

egDetailContainer* egDetailContainerCnv::createTransient() 
{
  MsgStream msg( msgSvc(), "egDetailContainerCnv" );

  egDetailContainer *transObj = nullptr;

  static pool::Guid tr_guid("F8C02E11-E6C5-4843-A57D-3DB2EFACCC5B");
  static pool::Guid p1_guid("C3E26ADE-A984-41AC-83DF-F430F6DF1F1F");
  static pool::Guid p2_guid("6AB9ADFD-836F-416D-B343-3DDF7D4F6E18");

  if ( compareClassGuid(tr_guid) ) {

    // regular object from before the T/P separation
    return poolReadObject<egDetailContainer>();

  } if ( compareClassGuid(p1_guid) ) {

    // using unique_ptr ensures deletion of the persistent object
    std::unique_ptr<egDetailContainer_p1> persObj( poolReadObject<egDetailContainer_p1>() );
    egDetailContainerCnv_p1 cnv;
    transObj = cnv.createTransient( persObj.get(), msg );
  } else if ( compareClassGuid(p2_guid) ) {

    // using unique_ptr ensures deletion of the persistent object
    std::unique_ptr<egDetailContainer_p2> persObj( poolReadObject<egDetailContainer_p2>() );
    egDetailContainerCnv_p2 cnv;
    transObj = cnv.createTransient( persObj.get(), msg );
  } else {
    throw std::runtime_error("Unsupported persistent version of egDetailContainer");
  }

  return transObj;
}

