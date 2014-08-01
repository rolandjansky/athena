/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: MDTSimHitFillerTool.cxx 503989 2012-06-05 16:07:54Z ssnyder $

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"

// Local include(s):
#include "MDTSimHitFillerTool.h"

namespace D3PD {

   MDTSimHitFillerTool::MDTSimHitFillerTool( const std::string& type,
                                             const std::string& name,
                                             const IInterface* parent )
      : BlockFillerTool< MDTSimHit >( type, name, parent )
   {
     book().ignore(); // Avoid coverity warnings.
   }

   StatusCode MDTSimHitFillerTool::book() {

      CHECK( addVariable( "globalTime", m_globalTime,
                          "Add description here..." ) );
      CHECK( addVariable( "driftRadius", m_driftRadius,
                          "Add description here..." ) );
      CHECK( addVariable( "stepLength", m_stepLength,
                          "Add description here..." ) );
      CHECK( addVariable( "energyDeposit", m_energyDeposit,
                          "Add description here..." ) );
      CHECK( addVariable( "particleEncoding", m_particleEncoding,
                          "Add description here..." ) );
      CHECK( addVariable( "kineticEnergy", m_kineticEnergy,
                          "Add description here..." ) );
      CHECK( addVariable( "MDTid", m_MDTid,
                          "Add description here..." ) );
      CHECK( addVariable( "trackNumber", m_trackNumber,
                          "Number of track which released this energy" ) );

      return StatusCode::SUCCESS;
   }

   StatusCode MDTSimHitFillerTool::fill( const MDTSimHit& obj ) {

      *m_globalTime       = obj.globalTime();
      *m_driftRadius      = obj.driftRadius();
      *m_stepLength       = obj.stepLength();
      *m_energyDeposit    = obj.energyDeposit();
      *m_particleEncoding = obj.particleEncoding();
      *m_kineticEnergy    = obj.kineticEnergy();
      *m_MDTid            = obj.MDTid();
      *m_trackNumber      = obj.trackNumber();

      return StatusCode::SUCCESS;
   }

} // namespace D3PD
