/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "JetMomentMapCnv.h"
#include "JetEventTPCnv/JetMomentMapCnv_p6.h"
#include "JetEventTPCnv/JetMomentMapCnv_p1.h"


JetMomentMap_PERS* JetMomentMapCnv::createPersistent(JetMomentMap* transObj)
{
  MsgStream report( messageService(), "JetMomentMapConverter" );
  report << MSG::DEBUG << "Writing JetMomentMap_p6" << endreq; 
  JetMomentMapCnv_p6   TPconverter;
  JetMomentMap_PERS *persObj = TPconverter.createPersistent( transObj, report );

  // will be deleted shortly
  // RS RS RS transObj->m_instance = 0;
  return persObj;
}

JetMomentMap* JetMomentMapCnv::createTransient()
{
  static pool::Guid   p1_guid("12E5A61A-6C0B-42EF-9E1D-DF33129500F4");
  static pool::Guid   p6_guid("7E53BA8F-B022-4DA7-9D8E-334262C4CE6F");
  if( compareClassGuid(p1_guid) )
    {
      JetMomentMapCnv_p1   TPconverter;
      // using unique_ptr ensures deletion of the persistent object
      std::unique_ptr<JetMomentMap_p1> jet_momentdescr( poolReadObject< JetMomentMap_p1 >() );
      MsgStream report( messageService(), "JetMomentMapCnv" );
      report << MSG::DEBUG << "Reading JetMomentMap_p6" << endreq; 
      return TPconverter.createTransient( jet_momentdescr.get(), report );
    }else if( compareClassGuid(p6_guid) )
    {
      JetMomentMapCnv_p6   TPconverter;
      // using unique_ptr ensures deletion of the persistent object
      std::unique_ptr<JetMomentMap_p6> jet_momentdescr( poolReadObject< JetMomentMap_p6 >() );
      MsgStream report( messageService(), "JetMomentMapCnv" );
      report << MSG::DEBUG << "Reading JetMomentMap_p6" << endreq; 
      return TPconverter.createTransient( jet_momentdescr.get(), report );
    }

  throw std::runtime_error("Unsupported persistent version of JetMomentMap");
}
