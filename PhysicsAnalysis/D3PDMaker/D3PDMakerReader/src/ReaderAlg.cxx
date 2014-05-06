/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ReaderAlg.cxx 462802 2011-10-12 16:06:24Z krasznaa $

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"

// Local include(s):
#include "ReaderAlg.h"
#include "IReaderD3PD.h"

namespace D3PD {

   ReaderAlg::ReaderAlg( const std::string& name, ISvcLocator* svcloc )
      : AthAlgorithm( name, svcloc ),
        m_d3pdSvc( "D3PD::RootReaderD3PDSvc", name ),
        m_tools( this ),
        m_d3pd( 0 ),
        m_booked( false ) {

        declareProperty( "D3PDSvc", m_d3pdSvc,
                         "The D3PD creation service." );
        declareProperty( "Tools", m_tools,
                         "List of IObjFillerTool instances to run." );
        declareProperty( "TuplePath", m_tuplePath = "dummy",
                         "The name of the tuple. The interpretation of this "
                         "depends on the D3PDSvc." );

        declareProperty( "Prefix", m_prefix = "",
                         "Common prefix to the D3PD variable names" );

        declareProperty( "Directory", m_dir = "./",
                         "Output directory for the generated sources" );
        declareProperty( "ClassName", m_classname = "Dummy",
                         "Name of the C++ class to be generated" );
   }

   StatusCode ReaderAlg::initialize() {

      ATH_MSG_INFO( "Initializing - Package version: " << PACKAGE_VERSION );

      CHECK( m_d3pdSvc.retrieve() );
      CHECK( m_tools.retrieve() );

      // Create a new D3PD:
      ID3PD* d3pd = 0;
      CHECK( m_d3pdSvc->make( m_tuplePath, d3pd ) );

      // Check that the service created the correct type of D3PD object:
      m_d3pd = dynamic_cast< IReaderD3PD* >( d3pd );
      if( ! m_d3pd ) {
         REPORT_MESSAGE( MSG::ERROR ) << "The configured service ("
                                      << m_d3pdSvc << ") did not create a "
                                      << "D3PD::IReaderD3PD object!";
         return StatusCode::FAILURE;
      }

      // Configure the object:
      m_d3pd->setPrefix( m_prefix );

      // Configure each tool.
      for( size_t i = 0; i < m_tools.size(); ++i ) {
         CHECK( m_tools[ i ]->configureD3PD( m_d3pd ) );
      }

      m_booked = false;
      return StatusCode::SUCCESS;
   }

   StatusCode ReaderAlg::finalize() {

      // Let the D3PD generate the source code that can read it back:
      CHECK( m_d3pd->createReader( m_classname, m_dir ) );

      return StatusCode::SUCCESS;
   }

   StatusCode ReaderAlg::execute() {

      // Instruct all the tools to declare their variables to the D3PD object:
      if( ! m_booked ) {
         m_booked = true;
         for( size_t i = 0; i < m_tools.size(); ++i ) {
            CHECK( m_tools[ i ]->book() );
         }
      }

      return StatusCode::SUCCESS;
   }

} // namespace D3PD
