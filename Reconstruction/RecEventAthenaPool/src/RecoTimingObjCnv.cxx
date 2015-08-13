/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


// Framework includes
#include "GaudiKernel/MsgStream.h"

// RecTPCnv includes
#include "RecEventTPCnv/RecoTimingObjCnv_p1.h"

// RecAthenaPool includes
#include "RecoTimingObjCnv.h"

RecoTimingObj_PERS* 
RecoTimingObjCnv::createPersistent( RecoTimingObj* transCont ) 
{
  // MsgStream msg( msgSvc(), "RecoTimingObjCnv" );
  if ( ! bool(m_msg) )
    m_msg=new MsgStream( msgSvc(), "RecoTimingObjCnv" );
  
  RecoTimingObjCnv_p1 cnv;
  RecoTimingObj_PERS *persObj = cnv.createPersistent( transCont, *m_msg );
  
  // msg << MSG::DEBUG << "::createPersistent [Success]" << endreq;
  return persObj; 
}

RecoTimingObj* RecoTimingObjCnv::createTransient() 
{
  // MsgStream msg( msgSvc(), "RecoTimingObjCnv" );
  if ( ! bool(m_msg) )
    m_msg=new MsgStream( msgSvc(), "RecoTimingObjCnv" );

  RecoTimingObj *transObj = 0;
  
  static pool::Guid p1_guid("4A28542C-DD7F-441A-9866-37A74ED49850");
  
  if ( compareClassGuid(p1_guid) )
    {
      // using auto_ptr ensures deletion of the persistent object
      std::auto_ptr<RecoTimingObj_p1> persObj( poolReadObject<RecoTimingObj_p1>() );
      RecoTimingObjCnv_p1 cnv;
      transObj = cnv.createTransient( persObj.get(), *m_msg );
    } else {
    throw std::runtime_error("Unsupported persistent version of RecoTimingObj");
  }
  
  return transObj;
}
