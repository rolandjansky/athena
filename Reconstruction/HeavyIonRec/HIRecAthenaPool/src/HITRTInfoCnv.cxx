/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


// Framework includes
#include "GaudiKernel/MsgStream.h"

// RecTPCnv includes
#include "HIRecTPCnv/HITRTInfoCnv_p1.h"

// RecAthenaPool includes
#include "HITRTInfoCnv.h"

HITRTInfo_PERS* 
HITRTInfoCnv::createPersistent( HITRTInfo* transCont ) 
{
  // MsgStream msg( msgSvc(), "HITRTInfoCnv" );
  if ( ! bool(m_msg) )
    m_msg=new MsgStream( msgSvc(), "HITRTInfoCnv" );
  
  HITRTInfoCnv_p1 cnv;
  HITRTInfo_PERS *persObj = cnv.createPersistent( transCont, *m_msg );
  
  // msg << MSG::DEBUG << "::createPersistent [Success]" << endreq;
  return persObj; 
}

HITRTInfo* HITRTInfoCnv::createTransient() 
{
  // MsgStream msg( msgSvc(), "HITRTInfoCnv" );
  if ( ! bool(m_msg) )
    m_msg=new MsgStream( msgSvc(), "HITRTInfoCnv" );

  HITRTInfo *transObj = 0;
  
  static pool::Guid p1_guid("8F3FEA42-D7E6-4FD7-84C1-91D5B551B537");
  
  if ( compareClassGuid(p1_guid) )
    {
      // using auto_ptr ensures deletion of the persistent object
      std::auto_ptr<HITRTInfo_p1> persObj( poolReadObject<HITRTInfo_p1>() );
      HITRTInfoCnv_p1 cnv;
      transObj = cnv.createTransient( persObj.get(), *m_msg );
    } else {
    throw std::runtime_error("Unsupported persistent version of HITRTInfo");
  }
  
  return transObj;
}
