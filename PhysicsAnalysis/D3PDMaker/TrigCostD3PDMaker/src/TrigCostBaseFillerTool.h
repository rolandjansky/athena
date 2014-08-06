// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGCOSTD3PDMAKER_TRIGCOSTBASEFILLERTOOL_H
#define TRIGCOSTD3PDMAKER_TRIGCOSTBASEFILLERTOOL_H

// STL include(s):
#include <vector>
#include <string>

// EDM include(s):
#include "TrigMonitoringEvent/TrigMonEventCollection.h"

// D3PD include(s):
#include "D3PDMakerUtils/BlockFillerTool.h"

namespace D3PD {

   /**
    *  @short Filler tool for TrigMonEvent objects
    *
    *         This base filler tool outputs global properties of the event mon
    *         object, some of these are availbe elsewhere if running with full framework.
    *
    * @author Tim Martin
    *
    * $Date: April 2013
    */
  class TrigCostBaseFillerTool : public BlockFillerTool< TrigMonEventCollection > {

  public:
    /// Regular AlgTool constructor
    TrigCostBaseFillerTool( const std::string& type, const std::string& name,
                            const IInterface* parent );

    /// Function booking the variables in the output
    virtual StatusCode book();
    /// Function filling the variables for the output
    virtual StatusCode fill( const TrigMonEventCollection& eventCollection );

  private:
    uint32_t m_dataVectorEntryToSave;
    bool     m_failGracefully;
    
    //TrigMonEvent Variables
    uint32_t* m_event;              // Event number
    uint32_t* m_lumi;               // Lumi block (low 16 bits of TrigMonEvent.m_lumi) 
    float*    m_lumiLength;         // Lumi block length in seconds
    uint32_t* m_bunch_id;           // bunch crossing id (high 16 bits of TrigMonEvent.m_lumi)
    uint32_t* m_run;                // Run number
    uint32_t* m_sec;                // Event second
    uint32_t* m_nsec;               // Event ns
    float*    m_timer;              // Event timer

    float*    m_timerTrigCost;      // Detailed time - Time to process TrigCost OPI tools
    float*    m_timerEndSteer;      // Detailed time - End of OPI steering
    float*    m_timerChainProcess;  // Detailed time - chain processing
    float*    m_timerResultBuilder; // Detailed time - result building
    float*    m_timerMon;           // Detailed time - monitoring 
    float*    m_costEvent;          // Sequential cost event number

    uint32_t* m_costRunSec;         // Cost processing time second
    uint32_t* m_costRunNsec;        // Cost processing time ns
    uint32_t* m_appId;              // Application ID - hash of trigger XPU node

  }; // class TrigCostBaseFillerTool

} // namespace D3PD

#endif // TRIGCOSTD3PDMAKER_TRIGCOSTBASEFILLERTOOL_H
