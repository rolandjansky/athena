/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArRawChannelContainerCnv.h"

#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "StoreGate/StoreGateSvc.h"
#include "LArTPCnv/LArRawChannelContainerCnv_p1.h"

// LArRawChannelContainerCnv, used for T/P separation
// real work is done in LArRawChannelContainerCnv_p2 for reading new data or
// below for reading old data
// author R.Seuster <seuster@cern.ch>

template<>
LArRawChannelContainer_PERSISTENT*
LArRawChannelContainerCnv::createPersistent(LArRawChannelContainer* transCont)
{
  MsgStream log(messageService(), "LArRawChannelContainerConverter" );
  LArRawChannelContainerCnv_p4   converter;
  LArRawChannelContainer_p4      *persObj = converter.createPersistent( transCont, log );
  return persObj; 
}

template<>
LArRawChannelContainer*
LArRawChannelContainerCnv::createTransient()
{
  MsgStream log(msgSvc(), "LArRawChannelContainerConverter" );
  LArRawChannelContainer         *trans_cont = NULL;
  
  // GUID for new and old persistent classes
  static pool::Guid   guid_p4("E3B70B7A-0D69-429A-AD43-B68941E879FB");
  static pool::Guid   guid_p3("95401B94-3D69-49BD-B901-C7B71940D746");
  static pool::Guid   guid_p2("AD521521-4043-472B-BF8B-FDFDC1870FD9");
  static pool::Guid   guid_p1("1569AF5C-5F27-4C02-A87B-47FF192F1605");
  static pool::Guid   guid_p0("9D624DCE-B943-4515-B33A-6F6928F8EB90");
 // even newer representation of LArRawChannelContainer
  if( compareClassGuid(guid_p4) ) {
      LArRawChannelContainerCnv_p4   converter;
      std::auto_ptr<LArRawChannelContainer_p4> col_vect( poolReadObject<LArRawChannelContainer_p4>() );
      trans_cont = converter.createTransient( col_vect.get(), log );
  }
  else if( compareClassGuid(guid_p3) ) {
      LArRawChannelContainerCnv_p3   converter;
      std::auto_ptr<LArRawChannelContainer_p3> col_vect( poolReadObject<LArRawChannelContainer_p3>() );
      trans_cont = converter.createTransient( col_vect.get(), log );
  }
  // new representation of LArRawChannelContainer
  else if( compareClassGuid(guid_p2) ) {
      LArRawChannelContainerCnv_p2   converter;
      std::auto_ptr<LArRawChannelContainer_p2> col_vect( poolReadObject<LArRawChannelContainer_p2>() );
      trans_cont = converter.createTransient( col_vect.get(), log );

  }
  // previous representation of LArRawChannelContainer
  else if( compareClassGuid(guid_p1) )
    {
      LArRawChannelContainerCnv_p1   converter;
      std::auto_ptr<LArRawChannelContainer_p1> col_vect( poolReadObject<LArRawChannelContainer_p1>() );
      trans_cont = converter.createTransient( col_vect.get(), log );
    }
  // old representation of LArRawChannelContainer
  else if( compareClassGuid(guid_p0) )
    {
      log << MSG::ERROR << "Reading of version 0 of LArRawChannelContainer not supported any more" << endreq;
    
    } 
  else {
//      log << MSG::ERROR << "failed trying to read : " << m_token << endreq;
      throw std::runtime_error("Unsupported persistent version of LArRawChannel container");
    }
  return trans_cont;
}
