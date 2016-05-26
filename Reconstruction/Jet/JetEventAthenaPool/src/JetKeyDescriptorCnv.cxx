/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "JetKeyDescriptorCnv.h"
#include "JetEventTPCnv/JetKeyDescriptor_p1.h"
#include "JetEventTPCnv/JetKeyDescriptorCnv_p1.h"

JetKeyDescriptor_PERS* JetKeyDescriptorCnv::createPersistent(JetKeyDescriptor* transObj)
{
  MsgStream report( messageService(), "JetKeyDescriptorConverter" );
  report << MSG::DEBUG << "Writing JetKeyDescriptor_p1" << endreq; 
  JetKeyDescriptorCnv_p1   TPconverter;
  JetKeyDescriptor_PERS *persObj = TPconverter.createPersistent( transObj, report );

  // will be deleted shortly
  // RS RS RS transObj->m_instance = 0;
  return persObj;
}

JetKeyDescriptor* JetKeyDescriptorCnv::createTransient()
{
  static pool::Guid   p1_guid("9F364413-5313-485F-9DBC-8DD8E8CA4AC4");
  if( compareClassGuid(p1_guid) )
    {
      JetKeyDescriptorCnv_p1   TPconverter;
      // using unique_ptr ensures deletion of the persistent object
      std::unique_ptr<JetKeyDescriptor_p1> jet_keydescr( poolReadObject< JetKeyDescriptor_p1 >() );
      MsgStream report( messageService(), "JetKeyDescriptorCnv" );
      report << MSG::DEBUG << "Reading JetKeyDescriptor_p1" << endreq; 
      return TPconverter.createTransient( jet_keydescr.get(), report );
    }
  throw std::runtime_error("Unsupported persistent version of JetKeyDescriptor");
}
