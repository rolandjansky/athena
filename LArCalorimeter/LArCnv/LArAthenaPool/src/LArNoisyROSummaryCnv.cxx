/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArNoisyROSummaryCnv.h"

#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "StoreGate/StoreGateSvc.h"
#include "LArTPCnv/LArNoisyROSummaryCnv_p1.h"
#include "LArTPCnv/LArNoisyROSummaryCnv_p2.h"
#include "LArTPCnv/LArNoisyROSummaryCnv_p3.h"
#include "LArTPCnv/LArNoisyROSummaryCnv_p4.h"


LArNoisyROSummaryCnv::LArNoisyROSummaryCnv(ISvcLocator* svcLoc) : 
  LArNoisyROSummaryCnvBase(svcLoc)
{}


LArNoisyROSummary_PERSISTENT*
LArNoisyROSummaryCnv::createPersistent(LArNoisyROSummary* transCont)
{
  MsgStream log(messageService(), "LArNoisyROSummaryConverter" );
  LArNoisyROSummary_p5      *persObj = m_converter.createPersistent( transCont, log );
  return persObj; 
}

LArNoisyROSummary*
LArNoisyROSummaryCnv::createTransient()
{
  MsgStream log(msgSvc(), "LArNoisyROSummaryConverter" );
  LArNoisyROSummary         *trans = NULL;
  
  // GUID for persistent classes
  static pool::Guid   guid_p1("4681BC21-3C00-4540-BED6-58E37700D9B9");
  static pool::Guid   guid_p2("C33CED2C-2101-4B0C-9BCB-739B004639F4");
  static pool::Guid   guid_p3("7801CF21-F2F2-4E87-9B87-744F31A37D1B");
  static pool::Guid   guid_p4("8F9E9A44-699E-4056-96CC-555ADA1179D4");
  static pool::Guid   guid_p5("4AE11DAE-F40C-4B90-B105-0A7BA5D29C1D");

  if( compareClassGuid(guid_p5) ) {
     std::auto_ptr<LArNoisyROSummary_p5> col_vect( poolReadObject<LArNoisyROSummary_p5>() );
     trans = m_converter.createTransient( col_vect.get(), log );
  }
  else if( compareClassGuid(guid_p4) ) {
     LArNoisyROSummaryCnv_p4   converter;
     std::auto_ptr<LArNoisyROSummary_p4> col_vect( poolReadObject<LArNoisyROSummary_p4>() );
     trans = converter.createTransient( col_vect.get(), log );
  }
  else if( compareClassGuid(guid_p3) ) {
      LArNoisyROSummaryCnv_p3   converter;
      std::auto_ptr<LArNoisyROSummary_p3> col_vect( poolReadObject<LArNoisyROSummary_p3>() );
      trans = converter.createTransient( col_vect.get(), log );
  }
  else if( compareClassGuid(guid_p2) ) {
      LArNoisyROSummaryCnv_p2   converter;
      std::auto_ptr<LArNoisyROSummary_p2> col_vect( poolReadObject<LArNoisyROSummary_p2>() );
      trans = converter.createTransient( col_vect.get(), log );
  }
  else if( compareClassGuid(guid_p1) ) {
      LArNoisyROSummaryCnv_p1   converter;
      std::auto_ptr<LArNoisyROSummary_p1> col_vect( poolReadObject<LArNoisyROSummary_p1>() );
      trans = converter.createTransient( col_vect.get(), log );
  }
  else {
    //      log << MSG::ERROR << "failed trying to read : " << m_token << endreq;
      throw std::runtime_error("Unsupported persistent version of LArNoisyROSummary ") ;
    }
  return trans;
}
