/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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
  MsgStream mlog(msgSvc(), "Lvl1ResultConverter" );

  Lvl1Result_PERS* persObj = m_impl->m_TPConverter_p2.createPersistent( transCont, mlog );

  return persObj;
}

//create transient
LVL1CTP::Lvl1Result* Lvl1ResultCnv::createTransient()
{
  MsgStream mlog(msgSvc(), "Lvl1ResultConverter" );

  static const pool::Guid p2_guid("F4208CCF-9FAA-4F01-9C0B-26E9E59CE49F");
  static const pool::Guid p1_guid("226FF990-4D6A-4957-AAAD-42172D174773");
  static const pool::Guid p0_guid("18DE165E-3DB5-4EAC-867D-807DA3E217C0");

  if ( compareClassGuid(p1_guid) ) {
    // using unique_ptr ensures deletion of the persistent object
    std::unique_ptr< LVL1CTP::Lvl1Result_p1 > col_vect( poolReadObject< LVL1CTP::Lvl1Result_p1 >() );
    return m_impl->m_TPConverter_p1.createTransient( col_vect.get(), mlog );
  } else if( compareClassGuid(p0_guid) ){
    // old version from before TP separation, just return it
    return this->poolReadObject<LVL1CTP::Lvl1Result>();
  } else if ( compareClassGuid(p2_guid) ) {
    // using unique_ptr ensures deletion of the persistent object
    std::unique_ptr< LVL1CTP::Lvl1Result_p2 > col_vect( poolReadObject< LVL1CTP::Lvl1Result_p2 >() );
    return m_impl->m_TPConverter_p2.createTransient( col_vect.get(), mlog );
  }

  throw std::runtime_error("Unsupported persistent version of Lvl1Result");
}

#endif
