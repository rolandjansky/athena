/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: RootReaderD3PDSvc.cxx 452707 2011-08-08 11:45:42Z krasznaa $

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"

// Local include(s):
#include "RootReaderD3PDSvc.h"
#include "RootReaderD3PD_v1.h"
#include "RootReaderD3PD_v2.h"

namespace D3PD {

   RootReaderD3PDSvc::RootReaderD3PDSvc( const std::string& name, ISvcLocator* svcloc )
      : AthService( name, svcloc ) {

      declareProperty( "Version", m_version = 2,
                       "Version of the D3PDReader code to be generated" );
   }

   StatusCode RootReaderD3PDSvc::initialize() {

      // Just print a hello...
      ATH_MSG_INFO( "Initializing - Package version: " << PACKAGE_VERSION );

      if( ( m_version < 1 ) || ( m_version > 2 ) ) {
         REPORT_MESSAGE( MSG::ERROR ) << "D3PDReader version " << m_version
                                      << " not known";
         return StatusCode::FAILURE;
      }

      ATH_MSG_INFO( "Generating sources of version: " << m_version );

      return StatusCode::SUCCESS;
   }

   StatusCode RootReaderD3PDSvc::finalize() {

      // Delete all the D3PD objects from memory:
      std::vector< ID3PD* >::iterator itr = m_d3pds.begin();
      std::vector< ID3PD* >::iterator end = m_d3pds.end();
      for( ; itr != end; ++itr ) {
         delete ( *itr );
      }
      m_d3pds.clear();

      return StatusCode::SUCCESS;
   }

   StatusCode RootReaderD3PDSvc::make( const std::string& name, ID3PD*& d3pd ) {

      // Create a new D3PD:
      ID3PD* d = 0;
      switch( m_version ) {
      case 1:
         REPORT_MESSAGE( MSG::VERBOSE ) << "Creating RootReaderD3PD_v1 with name: "
                                        << name;
         d = new RootReaderD3PD_v1();
         break;
      case 2:
         REPORT_MESSAGE( MSG::VERBOSE ) << "Creating RootReaderD3PD_v2 with name: "
                                        << name;
         d = new RootReaderD3PD_v2();
         break;
      default:
         REPORT_MESSAGE( MSG::ERROR ) << "Version number not recognised!";
         return StatusCode::FAILURE;
         break;
      }

      m_d3pds.push_back( d );
      d3pd = d;

      // This function can actually never fail...
      return StatusCode::SUCCESS;
   }

   StatusCode RootReaderD3PDSvc::queryInterface( const InterfaceID& riid,
                                                 void** ppvIf ) {

      if( riid == ID3PDSvc::interfaceID() )  {
         *ppvIf = static_cast< ID3PDSvc* >( this );
         addRef();
         return StatusCode::SUCCESS;
      }

      return AthService::queryInterface( riid, ppvIf );
   }

} // namespace D3PD
