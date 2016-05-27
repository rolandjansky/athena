///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ElectronContainerCnv.cxx 
// Implementation file for class ElectronContainerCnv
// Author: K.Cranmer<cranmer@cern.ch>
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 

// STL includes

// Framework includes
#include "GaudiKernel/MsgStream.h"

// egammaEventTPCnv includes
#include "egammaEventTPCnv/ElectronContainerCnv_p1.h"
#include "egammaEventTPCnv/ElectronContainerCnv_p2.h"
#include "egammaEventTPCnv/ElectronContainerCnv_p3.h"
#include "egammaEventTPCnv/ElectronContainerCnv_p4.h"
#include "egammaEventTPCnv/ElectronContainerCnv_p5.h"

// RecAthenaPool includes
#include "ElectronContainerCnv.h"
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


ElectronContainer_PERS* 
ElectronContainerCnv::createPersistent( ElectronContainer* transCont ) 
{
  MsgStream msg( msgSvc(), "ElectronContainerCnv" );

  ElectronContainerCnv_p5 cnv;
  ElectronContainer_PERS *persObj = cnv.createPersistent( transCont, msg );

  MSG_DEBUG(msg, "::createPersistent [Success]");
  return persObj; 
}

ElectronContainer* ElectronContainerCnv::createTransient() 
{
  MsgStream msg( msgSvc(), "ElectronContainerCnv" );

  ElectronContainer *transObj = 0;

  static pool::Guid tr_guid("417E86E5-CA20-4F50-A7A2-C91BF44C9125");
  static pool::Guid p1_guid("48355DA8-3BCE-43E0-B29B-51525BCEEDDD");
  static pool::Guid p2_guid("A4377CE2-C0DC-4F80-8CFD-3BBE9A8EFD52");
  static pool::Guid p3_guid("51C149D6-2FBA-4ADD-B6AA-8AE39E07D202");
  static pool::Guid p4_guid("1A7C1F66-D9D2-49E9-80A8-592A0CEE3FDF");
  static pool::Guid p5_guid("A7C69641-E985-4D31-92D4-D33206C197F1");

  if ( compareClassGuid(p5_guid) ) {

    // using unique_ptr ensures deletion of the persistent object
    std::unique_ptr<ElectronContainer_p5> persObj( poolReadObject<ElectronContainer_p5>() );
    ElectronContainerCnv_p5 cnv;
    transObj = cnv.createTransient( persObj.get(), msg );

  } else if ( compareClassGuid(p4_guid) ) {

    // using unique_ptr ensures deletion of the persistent object
    std::unique_ptr<ElectronContainer_p4> persObj( poolReadObject<ElectronContainer_p4>() );
    ElectronContainerCnv_p4 cnv;
    transObj = cnv.createTransient( persObj.get(), msg );

  } else if ( compareClassGuid(p3_guid) ) {

    // using unique_ptr ensures deletion of the persistent object
    std::unique_ptr<ElectronContainer_p3> persObj( poolReadObject<ElectronContainer_p3>() );
    ElectronContainerCnv_p3 cnv;
    transObj = cnv.createTransient( persObj.get(), msg );

  } else if ( compareClassGuid(p2_guid) ) {

    // using unique_ptr ensures deletion of the persistent object
    std::unique_ptr<ElectronContainer_p2> persObj( poolReadObject<ElectronContainer_p2>() );
    ElectronContainerCnv_p2 cnv;
    transObj = cnv.createTransient( persObj.get(), msg );

  } else if ( compareClassGuid(p1_guid) ) {

    // using unique_ptr ensures deletion of the persistent object
    std::unique_ptr<ElectronContainer_p1> persObj( poolReadObject<ElectronContainer_p1>() );
    ElectronContainerCnv_p1 cnv;
    transObj = cnv.createTransient( persObj.get(), msg );

 } else if ( compareClassGuid(tr_guid) ) {

    // regular object from before the T/P separation
    return poolReadObject<ElectronContainer>();

  } else {
    throw std::runtime_error("Unsupported persistent version of ElectronContainer");
  }

  return transObj;
}
