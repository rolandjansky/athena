/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "RingerRingsCnv.h"
#include "TrigCaloEventTPCnv/RingerRingsCnv_tlp1.h"

RingerRingsCnv::RingerRingsCnv(ISvcLocator* svcloc) :
  T_AthenaPoolCustomCnv<RingerRings, RingerRings_PERS >( svcloc ),
  m_TPConverter (new RingerRingsCnv_tlp1)
{ }
RingerRingsCnv::~RingerRingsCnv()
{
  delete m_TPConverter;
}



RingerRings_PERS* RingerRingsCnv::createPersistent(RingerRings* transCont) {
  MsgStream mlog(msgSvc(), "RingerRingsConverter" );
  mlog << MSG::DEBUG << "RingerRingsCnv::createPersistent" << endmsg;

  RingerRings_PERS *persObj = m_TPConverter->createPersistent(transCont, mlog);
  
  return persObj;
}


RingerRings* RingerRingsCnv::createTransient() {
  MsgStream mlog(msgSvc(), "RingerRingsConverter" );
  mlog << MSG::DEBUG << "RingerRingsCnv::createTransient " << endmsg;
  
  static const pool::Guid tlp1_guid("7A3BC015-8ECE-44A9-A2BD-9D52B3D2B354");
  
  RingerRings *trans_cont(0);
  
  if (compareClassGuid(tlp1_guid) ) {
    mlog << MSG::DEBUG << "RingerRingsCnv::reading tlp1 persistent object" << endmsg;
    std::unique_ptr<RingerRings_tlp1> col_vect(this->poolReadObject<RingerRings_tlp1>());
    trans_cont = m_TPConverter->createTransient(col_vect.get(), mlog);
  } else {
    throw std::runtime_error("Unsupported persistent version of Data container");
  }
  return trans_cont;
}

