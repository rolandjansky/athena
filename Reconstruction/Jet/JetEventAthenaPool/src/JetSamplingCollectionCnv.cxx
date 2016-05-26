///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// JetSamplingCollectionCnv.cxx 
// Implementation file for class JetSamplingCollectionCnv
// Author: S.Binet<binet@cern.ch>
// Modified: Belen.Salvachua@cern.ch  Dc 2008  Add JetSampling_p2
/////////////////////////////////////////////////////////////////// 

// STL includes

// Framework includes
#include "GaudiKernel/MsgStream.h"

// JetEventTPCnv includes
#include "JetEventTPCnv/JetSamplingCollectionCnv_p1.h"
#include "JetEventTPCnv/JetSamplingCollectionCnv_p2.h"

// RecAthenaPool includes
#include "JetSamplingCollectionCnv.h"

/////////////////////////////////////////////////////////////////// 
/// Public methods: 
/////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////// 
/// Const methods: 
/////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////// 
// Protected methods: 
/////////////////////////////////////////////////////////////////// 


JetSamplingCollection_PERS* 
JetSamplingCollectionCnv::createPersistent( JetSamplingCollection* transCont ) 
{
  MsgStream msg( msgSvc(), "JetSamplingCollectionCnv" );

  JetSamplingCollectionCnv_p2 cnv;
  JetSamplingCollection_PERS *persObj = cnv.createPersistent( transCont, msg );

  msg << MSG::DEBUG << "::createPersistent [Success]" << endreq;
  return persObj; 
}

JetSamplingCollection* JetSamplingCollectionCnv::createTransient() 
{
  MsgStream msg( msgSvc(), "JetSamplingCollectionCnv" );

  JetSamplingCollection *transObj = 0;

  static pool::Guid tr_guid("EA7497BA-7AAA-43A2-8735-EB071FCA5276");
  static pool::Guid p1_guid("E6C1B2E1-A7E7-4997-8A32-8F641BC4D741");
  static pool::Guid p2_guid("FB2AAFA3-90D4-4838-8889-900ACC4C2578");

  if ( compareClassGuid(tr_guid) ) {
    // regular object from before the T/P separation
    return poolReadObject<JetSamplingCollection>();

  } else if ( compareClassGuid(p2_guid) ) {
    // using unique_ptr ensures deletion of the persistent object
    std::unique_ptr<JetSamplingCollection_p2> persObj( poolReadObject<JetSamplingCollection_p2>() );
    JetSamplingCollectionCnv_p2 cnv;
    transObj = cnv.createTransient( persObj.get(), msg );

  } else if ( compareClassGuid(p1_guid) ) {
    // using unique_ptr ensures deletion of the persistent object
    std::unique_ptr<JetSamplingCollection_p1> persObj( poolReadObject<JetSamplingCollection_p1>() );
    JetSamplingCollectionCnv_p1 cnv;
    transObj = cnv.createTransient( persObj.get(), msg );

  } else {
    throw std::runtime_error("Unsupported persistent version of JetSamplingCollection");
  }

  return transObj;
}
