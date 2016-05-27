///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// PhotonContainerCnv.cxx 
// Implementation file for class PhotonContainerCnv
// Author: K.Cranmer<cranmer@cern.ch>
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 

// STL includes

// Framework includes
#include "GaudiKernel/MsgStream.h"

// egammaEventTPCnv includes
#include "egammaEventTPCnv/PhotonContainerCnv_p1.h"
#include "egammaEventTPCnv/PhotonContainerCnv_p2.h"
#include "egammaEventTPCnv/PhotonContainerCnv_p3.h"
#include "egammaEventTPCnv/PhotonContainerCnv_p4.h"
#include "egammaEventTPCnv/PhotonContainerCnv_p5.h"

// RecAthenaPool includes
#include "PhotonContainerCnv.h"
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


PhotonContainer_PERS* 
PhotonContainerCnv::createPersistent( PhotonContainer* transCont ) 
{
  MsgStream msg( msgSvc(), "PhotonContainerCnv" );

  PhotonContainerCnv_p5 cnv;
  PhotonContainer_PERS *persObj = cnv.createPersistent( transCont, msg );

  MSG_DEBUG(msg,  "::createPersistent [Success]");
  return persObj; 
}

PhotonContainer* PhotonContainerCnv::createTransient() 
{
  MsgStream msg( msgSvc(), "PhotonContainerCnv" );

  PhotonContainer *transObj = 0;

  static pool::Guid tr_guid("B0DC1628-F17C-4951-BC85-55BBECF4D4D1");
  static pool::Guid p1_guid("7D40A2D2-C03C-470C-8F0C-937592D34B5B");
  static pool::Guid p2_guid("E807B07C-8B27-4FF1-A638-05FBCA034C0B");
  static pool::Guid p3_guid("BC1DF3E5-98F5-444D-826A-DFBB85E1C870");
  static pool::Guid p4_guid("554B12C0-AB10-451D-963C-F9714FB29695"); 
  static pool::Guid p5_guid("64942DEF-C0FE-4CA7-A735-70187092DC27");

  if ( compareClassGuid(p5_guid) ) {

    // using unique_ptr ensures deletion of the persistent object
    std::unique_ptr<PhotonContainer_p5> persObj( poolReadObject<PhotonContainer_p5>() );
    PhotonContainerCnv_p5 cnv;
    transObj = cnv.createTransient( persObj.get(), msg );

  } else if ( compareClassGuid(p4_guid) ) {

    // using unique_ptr ensures deletion of the persistent object
    std::unique_ptr<PhotonContainer_p4> persObj( poolReadObject<PhotonContainer_p4>() );
    PhotonContainerCnv_p4 cnv;
    transObj = cnv.createTransient( persObj.get(), msg );

  } else if ( compareClassGuid(p3_guid) ) {

    // using unique_ptr ensures deletion of the persistent object
    std::unique_ptr<PhotonContainer_p3> persObj( poolReadObject<PhotonContainer_p3>() );
    PhotonContainerCnv_p3 cnv;
    transObj = cnv.createTransient( persObj.get(), msg );

  } else if ( compareClassGuid(p2_guid) ) {

    // using unique_ptr ensures deletion of the persistent object
    std::unique_ptr<PhotonContainer_p2> persObj( poolReadObject<PhotonContainer_p2>() );
    PhotonContainerCnv_p2 cnv;
    transObj = cnv.createTransient( persObj.get(), msg );

  } else if ( compareClassGuid(p1_guid) ) {

    // using unique_ptr ensures deletion of the persistent object
    std::unique_ptr<PhotonContainer_p1> persObj( poolReadObject<PhotonContainer_p1>() );
    PhotonContainerCnv_p1 cnv;
    transObj = cnv.createTransient( persObj.get(), msg );

  } else if ( compareClassGuid(tr_guid) ) {

    // regular object from before the T/P separation
    return poolReadObject<PhotonContainer>();

  } else {
    throw std::runtime_error("Unsupported persistent version of PhotonContainer");
  }

  return transObj;
}
