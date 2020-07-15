/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigPassBitsCollectionCnv.h"
#include "TrigSteeringEventTPCnv/TrigPassBitsCollection_p1.h"
#include "TrigSteeringEventTPCnv/TrigPassBitsCollectionCnv_p1.h"

struct TrigPassBitsCollection_impl
{
  TrigPassBitsCollectionCnv_p1 m_TPconverter_p1;
};

TrigPassBitsCollectionCnv::TrigPassBitsCollectionCnv( ISvcLocator *svcloc ) :
  TrigPassBitsCollectionCnvBase(svcloc),
  m_impl(std::make_unique<TrigPassBitsCollection_impl>())
{}

TrigPassBitsCollectionCnv::~TrigPassBitsCollectionCnv() = default;

//createPersistent 
TrigPassBitsCollection_PERS * TrigPassBitsCollectionCnv::createPersistent( TrigPassBitsCollection *transObj)
{
  MsgStream mlog(msgSvc(), "TrigPassBitsCollectionConverter" );
  
  TrigPassBitsCollection_PERS * p_cont = m_impl->m_TPconverter_p1.createPersistent( transObj, mlog );
  
  return p_cont;
  
}//end of create persistent method


//createTransient
TrigPassBitsCollection * TrigPassBitsCollectionCnv::createTransient()
{
  MsgStream mlog(msgSvc(), "TrigPassBitsCollectionConverter" );
  
  static const pool::Guid p1_guid( "E6E4F396-D696-4C1C-A0C4-7766E0AF9BB5" );

  //  TrigPassBitsCollection *p_collection = 0;
  if( compareClassGuid( p1_guid ) )     
  {
         std::unique_ptr< TrigPassBitsCollection_p1 > col_vect( poolReadObject< TrigPassBitsCollection_p1 >() );
         return m_impl->m_TPconverter_p1.createTransient( col_vect.get(), mlog ) ;
  }
  else  throw std::runtime_error( "Unsupported persistent version of TrigPassBitsCollection" );
  
}//end of create transient method

