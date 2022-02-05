/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

// $Id: EventTypeFillerTool.cxx 603418 2014-06-24 15:37:50Z ssnyder $

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"

// Local include(s):
#include "EventTypeFillerTool.h"

using xAOD::EventInfo;

namespace D3PD {

   EventTypeFillerTool::EventTypeFillerTool( const std::string& type,
                                             const std::string& name,
                                             const IInterface* parent )
      : BlockFillerTool< EventInfo >( type, name, parent ) {

      EventTypeFillerTool::book().ignore(); // Avoid coverity warnings.
   }

   StatusCode EventTypeFillerTool::book() {

     CHECK( addVariable( "isSimulation", m_is_simulation,
                         "True if the event comes from MC simulation" ) );
     CHECK( addVariable( "isCalibration", m_is_calibration,
                         "True if this is a calibration data event" ) );
     CHECK( addVariable( "isTestBeam", m_is_testbeam,
                         "True if the event was recorded at a test-beam" ) );

     return StatusCode::SUCCESS;
   }

   StatusCode EventTypeFillerTool::fill( const EventInfo& ei ) {

     *m_is_simulation  = ei.eventType( EventInfo::IS_SIMULATION );
     *m_is_calibration = ei.eventType( EventInfo::IS_CALIBRATION );
     *m_is_testbeam    = ei.eventType( EventInfo::IS_TESTBEAM );

     return StatusCode::SUCCESS;
   }

} // namespace D3PD
