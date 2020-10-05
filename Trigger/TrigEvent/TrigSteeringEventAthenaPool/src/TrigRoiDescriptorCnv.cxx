/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// #ifndef XAOD_ANALYSIS

#include "TrigRoiDescriptorCnv.h"
#include "TrigSteeringEventTPCnv/TrigRoiDescriptor_p1.h"
#include "TrigSteeringEventTPCnv/TrigRoiDescriptor_p2.h"
#include "TrigSteeringEventTPCnv/TrigRoiDescriptor_p3.h"
#include "TrigSteeringEventTPCnv/TrigRoiDescriptorCnv_p1.h"
#include "TrigSteeringEventTPCnv/TrigRoiDescriptorCnv_p2.h"
#include "TrigSteeringEventTPCnv/TrigRoiDescriptorCnv_p3.h"


struct TrigRoiDescriptorCnv_tp {
  TrigRoiDescriptorCnv_p1 p1;
  TrigRoiDescriptorCnv_p2 p2;
  TrigRoiDescriptorCnv_p3 p3;
};

TrigRoiDescriptorCnv::TrigRoiDescriptorCnv(ISvcLocator* svcloc) :
  T_AthenaPoolCustomCnv<TrigRoiDescriptor, TrigRoiDescriptor_PERS >( svcloc ),
  m_TPConverters (new TrigRoiDescriptorCnv_tp )
{
}
TrigRoiDescriptorCnv::~TrigRoiDescriptorCnv()
{
  delete m_TPConverters;
}

//create persistent
TrigRoiDescriptor_PERS* TrigRoiDescriptorCnv::createPersistent(TrigRoiDescriptor* transObj) 
{
  MsgStream mlog(msgSvc(), "TrigRoiDescriptorConverter" );

  TrigRoiDescriptor_PERS *persObj = m_TPConverters->p3.createPersistent( transObj, mlog );
  
  return persObj;
}

//createTransient
TrigRoiDescriptor* TrigRoiDescriptorCnv::createTransient() 
{
  MsgStream mlog(msgSvc(), "TrigRoiDescriptorConverter" );

  static const pool::Guid p3_guid("28F5BCC8-1F3D-47B1-8286-087F1B298F0A");
  static const pool::Guid p2_guid("D53CE59B-99A8-4B25-87D5-C08D1AF4BA8A");
  static const pool::Guid p1_guid("391FFE21-5D82-471E-9FFC-B77150142B8F");
  static const pool::Guid p0_guid("B2C86E23-8421-4F34-8014-AE4A7E4BA0A7");

  TrigRoiDescriptor *trans_obj(0);
  
  if( compareClassGuid(p3_guid) ) {
    
    std::unique_ptr< TrigRoiDescriptor_p3 >   col_vect( this->poolReadObject< TrigRoiDescriptor_p3 >() );
    trans_obj = m_TPConverters->p3.createTransient( col_vect.get(), mlog );
    
  }
  else if( compareClassGuid(p2_guid) ) {
    
    std::unique_ptr< TrigRoiDescriptor_p2 >   col_vect( this->poolReadObject< TrigRoiDescriptor_p2 >() );
    trans_obj = m_TPConverters->p2.createTransient( col_vect.get(), mlog );
    
  }
  else if( compareClassGuid(p1_guid) ) {
    
    std::unique_ptr< TrigRoiDescriptor_p1 >   col_vect( this->poolReadObject< TrigRoiDescriptor_p1 >() );
    trans_obj = m_TPConverters->p1.createTransient( col_vect.get(), mlog );
    
  }
  else if( compareClassGuid(p0_guid) ) {

    // old version from before TP separation, just return it
    trans_obj = this->poolReadObject<TrigRoiDescriptor>();

  }  else {
    
    throw std::runtime_error("Unsupported persistent version of Data container");
  }

  return trans_obj;

}

// #endif
