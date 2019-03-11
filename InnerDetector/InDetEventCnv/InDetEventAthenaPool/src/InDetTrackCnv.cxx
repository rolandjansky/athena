/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/MsgStream.h"

#include "InDetTrackCnv.h"
#include "TrkTrack/TrackCollection.h"

#include <iostream>
#include <stdexcept>

InDetTrackCnv::InDetTrackCnv(ISvcLocator* svcloc):
    InDetTrackCnvBase(svcloc)
{
}


StatusCode InDetTrackCnv::initialize()
{
   if( !InDetTrackCnvBase::initialize().isSuccess() )
      return StatusCode::FAILURE;

   
   // std::cout << " ------->>---- In InDetTrackCnv::initialize()" << std::endl;

   return StatusCode::SUCCESS;
}


// this method just reads the persistent object - no TP conversion here
void 
InDetTrackCnv::readObjectFromPool( const std::string& token )
{
    const pool::Guid p2_guid( "4E3778E2-1497-4F10-8746-AA02319FAC83" );
    const pool::Guid p1_guid( "8380F7AC-4A8F-4382-95A5-1234E43D3B08" );

   // set the POOL token which will be used for reading from POOL
   setToken( token );

   // select the object type based on its GUID 
   if( compareClassGuid( p2_guid ) )     {
      // read the object using the main TP converter
      poolReadObject< InDetTrack_PERS >( m_TPConverter2 );
   }else if( compareClassGuid( p1_guid ) )     {
      poolReadObject< InDet::Track_tlp1 >( m_TPConverter1 );
   }
   else
      throw std::runtime_error( "Unsupported version of InDetTrack_PERS (unknown GUID)" );
}


