/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CTP_RDOFillerTool.cxx 621771 2014-10-14 16:50:32Z stelzer $

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"

// Local include(s):
#include "CTP_RDOFillerTool.h"

namespace D3PD {

   CTP_RDOFillerTool::CTP_RDOFillerTool( const std::string& type,
                                         const std::string& name,
                                         const IInterface* parent )
      : BlockFillerTool< CTP_RDO >( type, name, parent )
   {
     book().ignore(); // Avoid coverity warnings.
   }

   StatusCode CTP_RDOFillerTool::book() {

      CHECK( addVariable( "timeSec", m_timeSec,
                          "Time the event was recorded [seconds]" ) );
      CHECK( addVariable( "timeNanoSec", m_timeNanoSec,
                          "Time the event was recorded [nanoseconds]" ) );
      CHECK( addVariable( "numberOfBunches", m_numberOfBunches,
                          "Number of bunches in the raw data" ) );
      CHECK( addVariable( "L1AcceptBunchPosition", m_l1AcceptBunchPosition,
                          "Bunch position from which the LVL1 accept was calculated" ) );
      CHECK( addVariable( "PITWords", m_pitWords,
                          "160 Pattern In Time bits" ) );
      CHECK( addVariable( "TBPWords", m_tbpWords,
                          "256 Trigger Before Prescale bits" ) );
      CHECK( addVariable( "TAPWords", m_tapWords,
                          "256 Trigger After Prescale bits" ) );
      CHECK( addVariable( "TAVWords", m_tavWords,
                          "256 Trigger After Veto bits" ) );

      return StatusCode::SUCCESS;
   }

   StatusCode CTP_RDOFillerTool::fill( const CTP_RDO& rdo ) {

      *m_timeSec = rdo.getTimeSec();
      *m_timeNanoSec = rdo.getTimeNanoSec();
      *m_numberOfBunches = rdo.getNumberOfBunches();
      *m_l1AcceptBunchPosition = rdo.getL1AcceptBunchPosition();
      *m_pitWords = rdo.getTIPWords(); // this was changed from PIT (Run1 = 160 internal trigger lines) to TIP (Run2 = 320 internal trigger lines + 192 direct inputs from L1Topo + ALFA) // joerg
      *m_tbpWords = rdo.getTBPWords();
      *m_tapWords = rdo.getTAPWords();
      *m_tavWords = rdo.getTAVWords();

      return StatusCode::SUCCESS;
   }

} // namespace D3PD
