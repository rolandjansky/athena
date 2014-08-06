/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigCostBaseFillerTool.cxx tamartin $

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"

// Local include(s):
#include "TrigCostBaseFillerTool.h"
#include "TrigCostD3PDUtils.h"

// EDM include(s):
#include "TrigMonitoringEvent/TrigMonEvent.h"

namespace D3PD {

  TrigCostBaseFillerTool::TrigCostBaseFillerTool( const std::string& type,
                                                  const std::string& name,
                                                  const IInterface* parent )
      : BlockFillerTool< TrigMonEventCollection >( type, name, parent )
  {
      // Avoid coverity warnings
    book().ignore();
      
    declareProperty( "DataVectorEntryToSave", m_dataVectorEntryToSave = 0  );
    declareProperty( "FailGracefully", m_failGracefully = true );
  }

  StatusCode TrigCostBaseFillerTool::book() {
    //Base info
    CHECK( addVariable( "eventNumber", m_event, "Event number" ) );
    CHECK( addVariable( "lumi", m_lumi, "Lumi block number for this event" ) );
    CHECK( addVariable( "lumiLength", m_lumiLength, "Length in s of this lumi block" ) );
    CHECK( addVariable( "bunchCrossingId", m_bunch_id, "BCID for this event" ) );
    CHECK( addVariable( "runNumber", m_run, "Run number for this event" ) );
    CHECK( addVariable( "seconds", m_sec, "Start of the event in seconds" ) );
    CHECK( addVariable( "nanoSeconds", m_nsec, "Start of the event nanoseconds" ) );
    CHECK( addVariable( "timer", m_timer, "Nominal time for this event" ) );

    CHECK( addVariable( "timerTrigCost", m_timerTrigCost, "Detailed: Time to run the OPI trig cost tools" ) );
    CHECK( addVariable( "timerEndSteer", m_timerEndSteer, "Detailed: Time to end of steering" ) );
    CHECK( addVariable( "timerChainProcess", m_timerChainProcess, "Detailed: Time of chain execution" ) );
    CHECK( addVariable( "timerResultBuilder", m_timerResultBuilder, "Detailed: Time of ResultBuilder tool execution" ) );
    CHECK( addVariable( "timerMon", m_timerMon, "Detailed: Total time of monitoring tool(s) execution" ) );

    CHECK( addVariable( "costRunSec", m_costRunSec, "Second the data were saved by CostMon" ) );
    CHECK( addVariable( "costRunNsec", m_costRunNsec, "Nanosecond the data were saved by CostMon" ) );
    CHECK( addVariable( "costEvent", m_costEvent, "Sequential number of cost events processed" ) );
    CHECK( addVariable( "appId", m_appId, "Hash of AppId of the XPU node processing this event." ) );

    return StatusCode::SUCCESS;
  }

  StatusCode TrigCostBaseFillerTool::fill( const TrigMonEventCollection& eventCollection ) {

    //Get requested event from vector
    const TrigMonEvent* event = getObjectFromVector(eventCollection, m_dataVectorEntryToSave);
    if ( event == 0 && m_failGracefully == true ) {
      REPORT_MESSAGE( MSG::DEBUG ) << "Requesting entry not contained in DataVector! Continuing run...";
      return StatusCode::SUCCESS;
    } else if ( event == 0 && m_failGracefully == false ) {
      REPORT_MESSAGE( MSG::FATAL ) << "Requesting entry not contained in DataVector! Could not run TrigCostD3PDMaker.";
      return StatusCode::FAILURE;
    }
    
    //Base info (as this package may be run in future without other standard D3PD framework, keep these options alive)
    *m_event    = event->getEvent();
    *m_lumi     = event->getLumi();
    *m_bunch_id = event->getBunchId();
    *m_run      = event->getRun();
    *m_sec      = event->getSec();
    *m_nsec     = event->getNanoSec();
    *m_timer    = event->getTimer();

    for (unsigned i=0; i < event->getWord().size(); ++i) {
      switch (i) {
        case 0:
          *m_costRunSec = event->getWord().at(i);
          break;
        case 1:
          *m_costRunNsec = event->getWord().at(i);
          break;
        case 2:
          *m_appId = event->getWord().at(i);
          break;
        default:
          break;
      }
    }  

    // Check and save additonal payload relating to the event
    if ( event->getVarKey().size() != event->getVarVal().size() ) {
      REPORT_MESSAGE( MSG::WARNING ) << "Trigger event-mon-base additional integer payload size different to key size, skipping.";
    } else {
      for (unsigned i=0; i < event->getVarKey().size(); ++i) {
        switch (event->getVarKey().at(i)) {
          case 100:
            *m_timerTrigCost = event->getVarVal().at(i);
            break;
          case 101:
            *m_timerEndSteer = event->getVarVal().at(i);
            break;
          case 102:
            *m_timerChainProcess = event->getVarVal().at(i);
            break;
          case 103:
            *m_timerResultBuilder = event->getVarVal().at(i);
            break;
          case 104:
            *m_timerMon = event->getVarVal().at(i);
            break;
          case 9999:
            *m_costEvent = event->getVarVal().at(i);
            break;
          case 43:
            *m_lumiLength = event->getVarVal().at(i);
            break;
          default:
            REPORT_MESSAGE( MSG::DEBUG ) << "Ignored unknown int=>float ("
              <<event->getVarKey().at(i)<<" => "<<event->getVarVal().at(i)
              <<") mapped data in TrigMonEvent D3PD Making.";
            break;
        }
      }
    }  

    return StatusCode::SUCCESS;
  }
  


} // namespace D3PD
