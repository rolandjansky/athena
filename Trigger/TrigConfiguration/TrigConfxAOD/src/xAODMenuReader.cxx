/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODMenuReader.cxx 612011 2014-08-15 13:43:57Z krasznaa $

// Local include(s):
#include "xAODMenuReader.h"

namespace TrigConf {

   xAODMenuReader::xAODMenuReader( const std::string& name,
                                   ISvcLocator* svcLoc )
      : AthAlgorithm( name, svcLoc ),
        m_configSvc( "TrigConf::xAODConfigSvc", name ),
        m_configTool( "TrigConf::xAODConfigTool" ) {

      declareProperty( "TrigConfigSvc", m_configSvc );
      declareProperty( "TrigConfigTool", m_configTool );
   }

   StatusCode xAODMenuReader::initialize() {

      // Greet the user:
      ATH_MSG_INFO( "Initialising - Package version: " << PACKAGE_VERSION );

      // Retrieve the needed components:
      ATH_CHECK( m_configSvc.retrieve() );
      ATH_CHECK( m_configTool.retrieve() );

      // Reset our internal counter:
      m_eventCounter = 0;

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

   StatusCode xAODMenuReader::execute() {

      // Check if we want to do anything:
      ++m_eventCounter;
      if( m_eventCounter > 10 ) {
         return StatusCode::SUCCESS;
      }

      // Print some info:
      ATH_MSG_INFO( "Configuration loaded by the service:" );
      ATH_MSG_INFO( "  SMK: " << m_configSvc->masterKey()
                    << ", L1PSK: " << m_configSvc->lvl1PrescaleKey()
                    << ", HLTPSK: " << m_configSvc->hltPrescaleKey() );

      ATH_MSG_INFO( "Configuration loaded by the tool:" );
      ATH_MSG_INFO( "  SMK: " << m_configTool->masterKey()
                    << ", L1PSK: " << m_configTool->lvl1PrescaleKey()
                    << ", HLTPSK: " << m_configTool->hltPrescaleKey() );

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

} // namespace TrigConf
