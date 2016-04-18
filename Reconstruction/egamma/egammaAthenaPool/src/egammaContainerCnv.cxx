///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// egammaContainerCnv.cxx 
// Implementation file for class egammaContainerCnv
// Author: K.Cranmer<cranmer@cern.ch>
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 

// STL includes

// Framework includes
#include "GaudiKernel/MsgStream.h"

// egammaEventTPCnv includes
#include "egammaEventTPCnv/egammaContainerCnv_p1.h"
#include "egammaEventTPCnv/egammaContainerCnv_p2.h"
#include "egammaEventTPCnv/egammaContainerCnv_p3.h"
#include "egammaEventTPCnv/egammaContainerCnv_p4.h"
#include "egammaEventTPCnv/egammaContainerCnv_p5.h"

// RecAthenaPool includes
#include "egammaContainerCnv.h"
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


egammaContainer_PERS* 
egammaContainerCnv::createPersistent( egammaContainer* transCont ) 
{
  MsgStream msg( msgSvc(), "egammaContainerCnv" );

  egammaContainerCnv_p5 cnv;
  egammaContainer_PERS *persObj = cnv.createPersistent( transCont, msg );

  MSG_DEBUG(msg, "::createPersistent [Success]");
  return persObj; 
}

egammaContainer* egammaContainerCnv::createTransient() 
{
  MsgStream msg( msgSvc(), "egammaContainerCnv" );

  egammaContainer *transObj = 0;

  static pool::Guid tr_guid("7CAB4F67-1765-4CDC-9614-60EB93E5551B");
  static pool::Guid p1_guid("DBA382E5-52F7-4B40-8623-E807E134412E");
  static pool::Guid p2_guid("5576188C-ECA9-4BD1-B1F1-563C728FE51F");
  static pool::Guid p3_guid("95BD7D50-F070-4245-91DE-ECAB4A8D722D");
  static pool::Guid p4_guid("451AACD2-5B23-4557-8A3F-6CD1AB36BF9F"); 
  static pool::Guid p5_guid("1AA8DE2E-D1CC-4076-B74A-82BBCC551798");

  if ( compareClassGuid(p5_guid) ) {

    // using unique_ptr ensures deletion of the persistent object
    std::unique_ptr<egammaContainer_p5> persObj( poolReadObject<egammaContainer_p5>() );
    egammaContainerCnv_p5 cnv;
    transObj = cnv.createTransient( persObj.get(), msg );

  } else if ( compareClassGuid(p4_guid) ) {

    // using unique_ptr ensures deletion of the persistent object
    std::unique_ptr<egammaContainer_p4> persObj( poolReadObject<egammaContainer_p4>() );
    egammaContainerCnv_p4 cnv;
    transObj = cnv.createTransient( persObj.get(), msg );

  } else if ( compareClassGuid(p3_guid) ) {

    // using unique_ptr ensures deletion of the persistent object
    std::unique_ptr<egammaContainer_p3> persObj( poolReadObject<egammaContainer_p3>() );
    egammaContainerCnv_p3 cnv;
    transObj = cnv.createTransient( persObj.get(), msg );

  } else if ( compareClassGuid(p2_guid) ) {

    // using unique_ptr ensures deletion of the persistent object
    std::unique_ptr<egammaContainer_p2> persObj( poolReadObject<egammaContainer_p2>() );
    egammaContainerCnv_p2 cnv;
    transObj = cnv.createTransient( persObj.get(), msg );

  } else if ( compareClassGuid(p1_guid) ) {

    // using unique_ptr ensures deletion of the persistent object
    std::unique_ptr<egammaContainer_p1> persObj( poolReadObject<egammaContainer_p1>() );
    egammaContainerCnv_p1 cnv;
    transObj = cnv.createTransient( persObj.get(), msg );

 } else if ( compareClassGuid(tr_guid) ) {

    // regular object from before the T/P separation
    return poolReadObject<egammaContainer>();

  } else {
    throw std::runtime_error("Unsupported persistent version of egammaContainer");
  }

  return transObj;
}
