///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// CaloRingsContainerCnv.cxx 
// Implementation file for class CaloRingsContainerCnv
// Author: D.E.Ferreira de Lima<dferreir@mail.cern.ch>
/////////////////////////////////////////////////////////////////// 

// STL includes

// Framework includes
#include "GaudiKernel/MsgStream.h"

// RecTPCnv includes
#include "egammaEventTPCnv/CaloRingsContainerCnv_p1.h"

// RecAthenaPool includes
#include "CaloRingsContainerCnv.h"
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


CaloRingsContainer_PERS* 
CaloRingsContainerCnv::createPersistent( CaloRingsContainer* transCont ) 
{
  MsgStream msg( msgSvc(), "CaloRingsContainerCnv" );

  CaloRingsContainerCnv_p1 cnv;
  CaloRingsContainer_PERS *persObj = cnv.createPersistent( transCont, msg );

  MSG_DEBUG(msg, "::createPersistent [Success]");
  return persObj; 
}

CaloRingsContainer* CaloRingsContainerCnv::createTransient() 
{
  MsgStream msg( msgSvc(), "CaloRingsContainerCnv" );

  CaloRingsContainer *transObj = 0;

  static pool::Guid tr_guid("DA20948A-A38A-11DE-BFE4-001CC0754D20");
  static pool::Guid p1_guid("E6E0435E-BFEC-11DE-B657-001CC0754D20");

  if ( compareClassGuid(p1_guid) ) {
    // using unique_ptr ensures deletion of the persistent object
    std::unique_ptr<CaloRingsContainer_p1> persObj( poolReadObject<CaloRingsContainer_p1>() );
    CaloRingsContainerCnv_p1 cnv;
    transObj = cnv.createTransient( persObj.get(), msg );
  } else if ( compareClassGuid(tr_guid) ) {
    // regular object from before the T/P separation
    return poolReadObject<CaloRingsContainer>();
  } else {
    throw std::runtime_error("Unsupported persistent version of CaloRingsContainer");
  }

  return transObj;
}
