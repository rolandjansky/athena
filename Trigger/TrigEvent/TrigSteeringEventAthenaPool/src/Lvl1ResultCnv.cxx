/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAOD_ANALYSIS

#include "Lvl1ResultCnv.h"
#include "TrigSteeringEventTPCnv/Lvl1ResultCnv_p1.h"
#include "TrigSteeringEventTPCnv/Lvl1ResultCnv_p2.h"
#include "TrigSteeringEventTPCnv/Lvl1Result_p1.h"
#include "TrigSteeringEventTPCnv/Lvl1Result_p2.h"

struct Lvl1ResultCnv_impl
{
  LVL1CTP::Lvl1ResultCnv_p1 m_TPConverter_p1;
  LVL1CTP::Lvl1ResultCnv_p2 m_TPConverter_p2;
};

Lvl1ResultCnv::Lvl1ResultCnv (ISvcLocator* svcloc) :
  Lvl1ResultCnvBase(svcloc),
  m_impl (new Lvl1ResultCnv_impl)
{
}

Lvl1ResultCnv::~Lvl1ResultCnv()
{
  delete m_impl;
}

//create persistent
Lvl1Result_PERS* Lvl1ResultCnv::createPersistent(LVL1CTP::Lvl1Result* transCont)
{
  MsgStream mlog(messageService(), "Lvl1ResultConverter" );
  mlog << MSG::DEBUG << "Lvl1ResultCnv::createPersistent " << endreq;

  Lvl1Result_PERS* persObj = m_impl->m_TPConverter_p2.createPersistent( transCont, mlog );

  return persObj;
}

//create transient
LVL1CTP::Lvl1Result* Lvl1ResultCnv::createTransient()
{
  MsgStream mlog(messageService(), "Lvl1ResultConverter" );
  mlog << MSG::DEBUG << "Lvl1ResultCnv::createTransient " << endreq;

  static pool::Guid p2_guid("F4208CCF-9FAA-4F01-9C0B-26E9E59CE49F");
  static pool::Guid p1_guid("226FF990-4D6A-4957-AAAD-42172D174773");
  static pool::Guid p0_guid("18DE165E-3DB5-4EAC-867D-807DA3E217C0");

  if ( compareClassGuid(p1_guid) ) {
    mlog << MSG::DEBUG << "Lvl1Result::createTransient reading p1 persistent object" << endreq;
    // using auto_ptr ensures deletion of the persistent object
    std::auto_ptr< LVL1CTP::Lvl1Result_p1 > col_vect( poolReadObject< LVL1CTP::Lvl1Result_p1 >() );
    return m_impl->m_TPConverter_p1.createTransient( col_vect.get(), mlog );
  } else if( compareClassGuid(p0_guid) ){
    mlog << MSG::DEBUG << "Lvl1Result::createTransient reading p0 persistent object" << endreq;
    // old version from before TP separation, just return it
    return this->poolReadObject<LVL1CTP::Lvl1Result>();
  } else if ( compareClassGuid(p2_guid) ) {
    mlog << MSG::DEBUG << "Lvl1Result::createTransient reading p2 persistent object" << endreq;
    // using auto_ptr ensures deletion of the persistent object
    std::auto_ptr< LVL1CTP::Lvl1Result_p2 > col_vect( poolReadObject< LVL1CTP::Lvl1Result_p2 >() );
    return m_impl->m_TPConverter_p2.createTransient( col_vect.get(), mlog );
  }

  throw std::runtime_error("Unsupported persistent version of Lvl1Result");
}

#endif
