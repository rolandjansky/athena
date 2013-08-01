/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// removal candidate - together will all MuonCaloEnergies* things.


#include "GaudiKernel/MsgStream.h"
#include "MuonCaloEnergiesCnv.h"
#include <iostream>
#include <stdexcept>

MuonCaloEnergiesCnv::MuonCaloEnergiesCnv(ISvcLocator* svcloc):
    MuonCaloEnergiesCnvBase(svcloc)
{
}


StatusCode MuonCaloEnergiesCnv::initialize()
{
   if( !MuonCaloEnergiesCnvBase::initialize().isSuccess() )
      return StatusCode::FAILURE;
   // std::cout << " ------->>---- In MuonCaloEnergiesCnv::initialize()" << std::endl;

   return StatusCode::SUCCESS;
}


// this method just reads the persistent object - no TP conversion here
void 
MuonCaloEnergiesCnv::readObjectFromPool( const std::string& token )
{
    
   static pool::Guid p1_guid( "E60630E0-FAA1-4658-9BDD-8BB977C4200B" );

   // set the POOL token which will be used for reading from POOL
   setToken( token );

   // select the object type based on its GUID 
   if( compareClassGuid( p1_guid ) )     {
      // read MuonCaloEnergies_PERS object from POOL using given TLP converter
      poolReadObject< MuonCaloEnergies_PERS >( m_TPConverter );
   }
   else
      throw std::runtime_error( "Unsupported version of MuonCaloEnergies_PERS (unknown GUID)" );
}


