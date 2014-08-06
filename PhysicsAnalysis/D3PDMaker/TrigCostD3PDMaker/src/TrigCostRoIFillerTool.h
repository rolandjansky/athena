// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGCOSTD3PDMAKER_TRIGCOSTROIFILLERTOOL_H
#define TRIGCOSTD3PDMAKER_TRIGCOSTROIFILLERTOOL_H

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
    *         Fills HLT RoI descriptors for cost monitoring
    *
    * @author Tim Martin
    *
    * $Date: April 2013
    */
  class TrigCostRoIFillerTool : public BlockFillerTool< TrigMonEventCollection > {

  public:
    /// Regular AlgTool constructor
    TrigCostRoIFillerTool( const std::string& type, const std::string& name,
                           const IInterface* parent );

    /// Function booking the variables in the output
    virtual StatusCode book();
    /// Function filling the variables for the output
    virtual StatusCode fill( const TrigMonEventCollection& eventCollection );

  private:
    uint32_t m_dataVectorEntryToSave;
    bool     m_failGracefully;
    
    uint16_t*                 m_roiN;
    std::vector< uint8_t >*   m_roiIsNoneType;
    std::vector< uint8_t >*   m_roiIsMuonType;
    std::vector< uint8_t >*   m_roiIsEmTauType;
    std::vector< uint8_t >*   m_roiIsJetType;
    std::vector< uint8_t >*   m_roiIsJetEtType;
    std::vector< uint8_t >*   m_roiIsEnergyType;
    std::vector< uint32_t >*  m_roiId;
    std::vector< uint8_t >*   m_roiNL1thresholds;
    std::vector< float >*     m_roiEta;
    std::vector< float >*     m_roiPhi;
    std::vector< float >*     m_roiArea;

  }; // class TrigCostRoIFillerTool

} // namespace D3PD

#endif // TRIGCOSTD3PDMAKER_TRIGCOSTROIFILLERTOOL_H
