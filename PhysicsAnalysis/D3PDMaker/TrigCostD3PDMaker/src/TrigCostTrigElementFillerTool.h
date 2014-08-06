// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGCOSTD3PDMAKER_TRIGCOSTTRIGELEMENTFILLERTOOL_H
#define TRIGCOSTD3PDMAKER_TRIGCOSTTRIGELEMENTFILLERTOOL_H

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
    *         Save details from TrigElement summary objects
    *
    * @author Tim Martin
    *
    * $Date: April 2013
    */
  class TrigCostTrigElementFillerTool : public BlockFillerTool< TrigMonEventCollection > {

  public:
    /// Regular AlgTool constructor
    TrigCostTrigElementFillerTool( const std::string& type, const std::string& name,
                                   const IInterface* parent );

    /// Function booking the variables in the output
    virtual StatusCode book();
    /// Function filling the variables for the output
    virtual StatusCode fill( const TrigMonEventCollection& eventCollection );

  private:
    uint32_t m_dataVectorEntryToSave;
    bool     m_failGracefully;
    
    //TEs
    uint16_t*                                 m_teN; 
    std::vector< uint16_t >*                  m_teId;
    std::vector< uint16_t >*                  m_teIndex;
    std::vector< uint8_t >*                   m_teIsRegularTE;
    std::vector< uint8_t >*                   m_teIsInitialTE;
    std::vector< uint8_t >*                   m_teIsRoITE;
    std::vector< uint8_t >*                   m_teIsL1ThreshTE;
    std::vector< uint8_t >*                   m_teActiveState;
    std::vector< uint8_t >*                   m_teErrorState;
    std::vector< uint8_t >*                   m_teIsTerminalNode;
    std::vector< uint8_t >*                   m_teIsOutputL2Node;
    std::vector< uint8_t >*                   m_teIsOutputEFNode;
    std::vector< uint8_t >*                   m_teIsTopologicalTE;
    std::vector< std::vector< uint16_t > >*   m_teChildIndex;
    std::vector< std::vector< uint16_t > >*   m_teParentIndex;
    std::vector< std::vector< uint8_t > >*    m_teRoiId;
    std::vector< std::vector< uint32_t > >*   m_teClid;
      
  }; // class TrigCostTrigElementFillerTool

} // namespace D3PD

#endif // TRIGCOSTD3PDMAKER_TRIGCOSTTRIGELEMENTFILLERTOOL_H
