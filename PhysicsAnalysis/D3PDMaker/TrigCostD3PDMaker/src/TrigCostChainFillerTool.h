// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGCOSTD3PDMAKER_TRIGCOSTCHAINFILLERTOOL_H
#define TRIGCOSTD3PDMAKER_TRIGCOSTCHAINFILLERTOOL_H

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
    *         This filler outputs the TrigMonChain items from TrigMonEvent
    *
    * @author Tim Martin
    *
    * $Date: April 2013
    */
  class TrigCostChainFillerTool : public BlockFillerTool< TrigMonEventCollection > {

  public:
    /// Regular AlgTool constructor
    TrigCostChainFillerTool( const std::string& type, const std::string& name,
                             const IInterface* parent );

    /// Function booking the variables in the output
    virtual StatusCode book();
    /// Function filling the variables for the output
    virtual StatusCode fill( const TrigMonEventCollection& eventCollection );

  private:
    uint32_t m_dataVectorEntryToSave;
    bool     m_failGracefully;
    
    //Chain
    uint16_t*                m_chainN; 
    std::vector< uint16_t >* m_chainCounter;
    std::vector< uint16_t >* m_chainLevel;    
    std::vector< float >*    m_chainTimer;         
    std::vector< uint8_t >*  m_chainIsPassed;   
    std::vector< uint8_t >*  m_chainIsPassedRaw;
    std::vector< uint8_t >*  m_chainIsPassthrough;
    std::vector< uint8_t >*  m_chainIsResurrected;
    std::vector< uint8_t >*  m_chainIsPrescaled;
    std::vector< uint8_t >*  m_chainWasL1AfterVeto;
    std::vector< uint8_t >*  m_chainWasL1BeforePrescale;
    std::vector< uint8_t >*  m_chainWasL1AfterPrescale;
    std::vector< uint8_t >*  m_chainIsExpressStream;
      
  }; // class TrigCostChainFillerTool

} // namespace D3PD

#endif // TRIGCOSTD3PDMAKER_TRIGCOSTCHAINFILLERTOOL_H
