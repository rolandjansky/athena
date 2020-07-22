/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAOD_ANALYSIS

#include "HLTResultCnv.h"
#include "TrigSteeringEventTPCnv/HLTResultCnv_p1.h"
#include "TrigSteeringEventTPCnv/HLTResult_p1.h"

HLTResultCnv::HLTResultCnv (ISvcLocator* svcloc) :
  HLTResultCnvBase(svcloc),
  m_TPConverter (new HLT::HLTResultCnv_p1)
{}

HLTResultCnv::~HLTResultCnv()
{
  delete m_TPConverter;
}


//create persistent
HLTResult_PERS* HLTResultCnv::createPersistent(HLT::HLTResult* transCont) 
{
  MsgStream mlog(msgSvc(), "HLTResultConverter" );

  HLTResult_PERS* persObj = m_TPConverter->createPersistent( transCont, mlog );

  return persObj; 
}
    
//create transient
HLT::HLTResult* HLTResultCnv::createTransient() 
{
  MsgStream mlog(msgSvc(), "HLTResultConverter" );

  static const pool::Guid p1_guid("9567573D-F35E-4D5E-9A1A-A43B07D3CF3B");
  static const pool::Guid p0_guid("559D6CB9-9A54-4284-A03D-9C745352281D");
  
  if ( compareClassGuid(p1_guid) ) {
    // using unique_ptr ensures deletion of the persistent object
    std::unique_ptr< HLT::HLTResult_p1 > col_vect( poolReadObject< HLT::HLTResult_p1 >() );
    return m_TPConverter->createTransient( col_vect.get(), mlog );
  }
  
  else if( compareClassGuid(p0_guid) ){
    // old version from before TP separation, just return it
    return this->poolReadObject<HLT::HLTResult>(); 
  }

  throw std::runtime_error("Unsupported persistent version of HLTResult");
}

#endif
