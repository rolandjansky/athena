// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGCOSTD3PDMAKER_TRIGCOSTL1FILLERTOOL_H
#define TRIGCOSTD3PDMAKER_TRIGCOSTL1FILLERTOOL_H

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
    *         Fills TrigMonL1Item objects for cost monitoring
    *
    * @author Tim Martin
    *
    * $Date: April 2013
    */
  class TrigCostL1FillerTool : public BlockFillerTool< TrigMonEventCollection > {

  public:
    /// Regular AlgTool constructor
    TrigCostL1FillerTool(  const std::string& type, const std::string& name,
                           const IInterface* parent );

    /// Function booking the variables in the output
    virtual StatusCode book();
    /// Function filling the variables for the output
    virtual StatusCode fill( const TrigMonEventCollection& eventCollection );

  private:
    uint32_t m_dataVectorEntryToSave;
    bool     m_failGracefully;
    
    //L1Item
    uint16_t*                m_l1N; 
    std::vector< uint16_t >* m_l1CtpId;
    std::vector< uint8_t >*  m_l1PassedBeforePrescale;    
    std::vector< uint8_t >*  m_l1PassedAfterPrescale;    
    std::vector< uint8_t >*  m_l1PassedAfterVeto;
    std::vector< uint8_t >*  m_l1Passed;
    std::vector< uint8_t >*  m_l1Prescaled;
    std::vector< uint8_t >*  m_l1Vetoed;  
    
  }; // class TrigCostL1FillerTool

} // namespace D3PD

#endif // TRIGCOSTD3PDMAKER_TRIGCOSTL1FILLERTOOL_H
