/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: BunchConfigIDFillerTool.cxx 504000 2012-06-05 16:28:50Z ssnyder $

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"

// Local include(s):
#include "BunchConfigIDFillerTool.h"

namespace D3PD {

   BunchConfigIDFillerTool::BunchConfigIDFillerTool( const std::string& type,
                                                     const std::string& name,
                                                     const IInterface* parent )
      : BlockFillerTool< void >( type, name, parent ),
        m_bcConfProv( "Trig::TrigConfBunchCrossingTool/BunchCrossingTool" ) {

      declareProperty( "BCConfProvider", m_bcConfProv );

      book().ignore(); // Avoid coverity warnings.
   }

   StatusCode BunchConfigIDFillerTool::initialize() {

      // Retrieve the configuration provider:
      CHECK( m_bcConfProv.retrieve() );

      // Also initialize the base class:
      return BlockFillerTool< void >::initialize();
   }

   StatusCode BunchConfigIDFillerTool::book() {

      // Book the variable(s):
      CHECK( addVariable( "configID", m_configId,
                          "ID describing the bunch configuration of the event" ) );

      return StatusCode::SUCCESS;
   }

   StatusCode BunchConfigIDFillerTool::fill() {

      // Fill the variable(s):
      *m_configId = m_bcConfProv->configID();

      return StatusCode::SUCCESS;
   }

} // namespace D3PD
