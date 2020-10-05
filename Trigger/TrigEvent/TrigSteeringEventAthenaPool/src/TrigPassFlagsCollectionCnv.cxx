/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigPassFlagsCollectionCnv.h"
#include "TrigSteeringEventTPCnv/TrigPassFlagsCollection_p1.h"
#include "TrigSteeringEventTPCnv/TrigPassFlagsCollectionCnv_p1.h"

struct TrigPassFlagsCollectionCnv_impl
{
  TrigPassFlagsCollectionCnv_p1   m_TPconverter_p1;
};

TrigPassFlagsCollectionCnv::TrigPassFlagsCollectionCnv( ISvcLocator *svcloc ) :
  TrigPassFlagsCollectionCnvBase(svcloc),
  m_impl(std::make_unique<TrigPassFlagsCollectionCnv_impl>())
{}

TrigPassFlagsCollectionCnv::~TrigPassFlagsCollectionCnv() = default;

//createPersistent 
TrigPassFlagsCollection_PERS * TrigPassFlagsCollectionCnv::createPersistent( TrigPassFlagsCollection *transObj)
{
  MsgStream mlog(msgSvc(), "TrigPassFlagsCollectionConverter" );
  
  TrigPassFlagsCollection_PERS * p_cont = m_impl->m_TPconverter_p1.createPersistent( transObj, mlog );
  
  return p_cont;
  
}//end of create persistent method


//createTransient
TrigPassFlagsCollection * TrigPassFlagsCollectionCnv::createTransient()
{
  MsgStream mlog(msgSvc(), "TrigPassFlagsCollectionConverter" );
  
  static const pool::Guid p1_guid( "7DE670AF-A256-4E8E-BC44-1CA30810F294" );

  //  TrigPassFlagsCollection *p_collection = 0;
  if( compareClassGuid( p1_guid ) )     
  {
         std::unique_ptr< TrigPassFlagsCollection_p1 > col_vect( poolReadObject< TrigPassFlagsCollection_p1 >() );
         return m_impl->m_TPconverter_p1.createTransient( col_vect.get(), mlog ) ;
  }
  else  throw std::runtime_error( "Unsupported persistent version of TrigPassFlagsCollection" );
  
}//end of create transient method

