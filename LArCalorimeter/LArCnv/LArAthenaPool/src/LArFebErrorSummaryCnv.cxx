/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArFebErrorSummaryCnv.h"

#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "StoreGate/StoreGateSvc.h"
#include "LArTPCnv/LArFebErrorSummaryCnv_p1.h"


LArFebErrorSummaryCnv::LArFebErrorSummaryCnv(ISvcLocator* svcLoc) : 
  LArFebErrorSummaryCnvBase(svcLoc)
{}


LArFebErrorSummary_PERSISTENT*
LArFebErrorSummaryCnv::createPersistent(LArFebErrorSummary* transCont)
{
  MsgStream log(messageService(), "LArFebErrorSummaryConverter" );
  LArFebErrorSummary_p1      *persObj = m_converter.createPersistent( transCont, log );
  return persObj; 
}

LArFebErrorSummary*
LArFebErrorSummaryCnv::createTransient()
{
  MsgStream log(msgSvc(), "LArFebErrorSummaryConverter" );
  LArFebErrorSummary         *trans = NULL;
  
  // GUID for persistent classes
  static pool::Guid   guid_p1("9448FB64-AB7E-4995-A5FC-23E9A6C1AF80");

  if( compareClassGuid(guid_p1) ) {
      std::auto_ptr<LArFebErrorSummary_p1> col_vect( poolReadObject<LArFebErrorSummary_p1>() );
      trans = m_converter.createTransient( col_vect.get(), log );
  }
  else {
    //      log << MSG::ERROR << "failed trying to read : " << m_token << endreq;
      throw std::runtime_error("Unsupported persistent version of LArFebErrorSummary ") ;
    }
  return trans;
}
