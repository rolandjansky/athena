/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// #ifndef XAOD_ANALYSIS

#include "TrigRoiDescriptorCollectionCnv.h"
#include "TrigSteeringEventTPCnv/TrigRoiDescriptorCollection_p1.h"
#include "TrigSteeringEventTPCnv/TrigRoiDescriptorCollectionCnv_p1.h"
#include "TrigSteeringEventTPCnv/TrigRoiDescriptorCollection_p2.h"
#include "TrigSteeringEventTPCnv/TrigRoiDescriptorCollectionCnv_p2.h"
#include "TrigSteeringEventTPCnv/TrigRoiDescriptorCollection_p3.h"
#include "TrigSteeringEventTPCnv/TrigRoiDescriptorCollectionCnv_p3.h"
#include "TrigSteeringEventTPCnv/TrigRoiDescriptorCollection_tlp1.h"
#include "TrigSteeringEventTPCnv/TrigRoiDescriptorCollectionCnv_tlp1.h"

struct TrigRoiDescriptorCollectionCnv_impl
{
  TrigRoiDescriptorCollectionCnv_p1 m_TPConverter_p1;
  TrigRoiDescriptorCollectionCnv_tlp1 m_TPConverter_tlp1;

  TrigRoiDescriptorCollectionCnv_p2 m_TPConverter_p2;
  TrigRoiDescriptorCollectionCnv_p3 m_TPConverter_p3;



};

TrigRoiDescriptorCollectionCnv::TrigRoiDescriptorCollectionCnv(ISvcLocator* svcloc) :
  T_AthenaPoolCustomCnv<TrigRoiDescriptorCollection, TrigRoiDescriptorCollection_PERS >( svcloc ),
  m_impl (new TrigRoiDescriptorCollectionCnv_impl)
{
}
TrigRoiDescriptorCollectionCnv::~TrigRoiDescriptorCollectionCnv()
{
  delete m_impl;
}

//create persistent
TrigRoiDescriptorCollection_PERS* TrigRoiDescriptorCollectionCnv::createPersistent(TrigRoiDescriptorCollection* transObj) 
{
  MsgStream mlog(msgSvc(), "TrigRoiDescriptorCollectionConverter" );

  TrigRoiDescriptorCollection_PERS *persObj = m_impl->m_TPConverter_p3.createPersistent( transObj, mlog );
  
  return persObj;
}

//createTransient
TrigRoiDescriptorCollection* TrigRoiDescriptorCollectionCnv::createTransient() 
{
  MsgStream mlog(msgSvc(), "TrigRoiDescriptorCollectionConverter" );

  static const pool::Guid p3_guid("615418EF-EEFB-4E87-A396-7313E67C547E");
  static const pool::Guid p2_guid("D1A44F23-416C-4AB6-BFFA-2EA280565D4E");
  static const pool::Guid p1_guid("D0A0B6E7-9E0C-484E-AE8C-AC57B5111EA0");
  static const pool::Guid tlp1_guid("CE80FC4E-B16B-40B2-9D9E-EB4916B663B0");
  
  TrigRoiDescriptorCollection *trans_obj(0);

  if( compareClassGuid(p3_guid) ) {

    std::unique_ptr< TrigRoiDescriptorCollection_p3 >   col_vect( this->poolReadObject< TrigRoiDescriptorCollection_p3 >() );
    trans_obj = m_impl->m_TPConverter_p3.createTransient( col_vect.get(), mlog );    
  }
  else if( compareClassGuid(p2_guid) ) {
    
    std::unique_ptr< TrigRoiDescriptorCollection_p2 >   col_vect( this->poolReadObject< TrigRoiDescriptorCollection_p2 >() );
    trans_obj = m_impl->m_TPConverter_p2.createTransient( col_vect.get(), mlog );    
  }
  else if( compareClassGuid(tlp1_guid) ) {
    
    std::unique_ptr< TrigRoiDescriptorCollection_tlp1 >   col_vect( this->poolReadObject< TrigRoiDescriptorCollection_tlp1 >() );
    trans_obj = m_impl->m_TPConverter_tlp1.createTransient( col_vect.get(), mlog );    
  }
  else if( compareClassGuid(p1_guid) ) {
    
    std::unique_ptr< TrigRoiDescriptorCollection_p1 >   col_vect( this->poolReadObject< TrigRoiDescriptorCollection_p1 >() );
    trans_obj = m_impl->m_TPConverter_p1.createTransient( col_vect.get(), mlog );    
  }
  else {
    
    throw std::runtime_error("Unsupported persistent version of TrigRoiDescriptorCollection");
  }

  return trans_obj;

}

// #endif
