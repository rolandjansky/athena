/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigDBKeysFillerTool.cxx 504000 2012-06-05 16:28:50Z ssnyder $

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"
#include "CxxUtils/crc64.h"

// Local include(s):
#include "TrigDBKeysFillerTool.h"

namespace D3PD {

   TrigDBKeysFillerTool::TrigDBKeysFillerTool( const std::string& type,
                                               const std::string& name,
                                               const IInterface* parent )
      : BlockFillerTool< void >( type, name, parent ),
        m_configSvc( "TrigConf::TrigConfigSvc/TrigConfigSvc", name )   {
     book().ignore(); // Avoid coverity warnings.
   }

   StatusCode TrigDBKeysFillerTool::initialize() {

      // Retrieve the trigger configuration service:
      CHECK( m_configSvc.retrieve() );

      return BlockFillerTool< void >::initialize();
   }

   StatusCode TrigDBKeysFillerTool::book() {

      // Book the variables:
      CHECK( addVariable( "SMK",    m_smk,
                          "Super Master Key" ) );
      CHECK( addVariable( "L1PSK",  m_l1psk,
                          "LVL1 PreScale Key" ) );
      CHECK( addVariable( "HLTPSK", m_hltpsk,
                          "HLT PreScale Key" ) );

      return StatusCode::SUCCESS;
   }

   StatusCode TrigDBKeysFillerTool::fill() {

      // Fill the variables:
      *m_smk = m_configSvc->masterKey();
      *m_l1psk = m_configSvc->lvl1PrescaleKey();
      *m_hltpsk = m_configSvc->hltPrescaleKey();

      // If the keys returned by the configuration service don't seem to make sense,
      // use something else as the SMK. (Needed mostly for MC test jobs.)
      if( ( ( *m_smk == 0 ) && ( *m_l1psk == 0 ) && ( *m_hltpsk == 0 ) ) ||
          ( static_cast< int >( *m_smk )    < 0 ) ||
          ( static_cast< int >( *m_l1psk )  < 0 ) ||
          ( static_cast< int >( *m_hltpsk ) < 0 ) ) {
         // Turn the configuration source name (probably an XML file in this case) into an
         // imaginary Super Master Key:
         *m_smk = CxxUtils::crc64( m_configSvc->configurationSource() ) & 0xffff;
         *m_l1psk = 0;
         *m_hltpsk = 0;
      }

      return StatusCode::SUCCESS;
   }

} // namespace D3PD
