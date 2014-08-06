// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGCOSTD3PDMAKER_TRIGCOSTROBFILLERTOOL_H
#define TRIGCOSTD3PDMAKER_TRIGCOSTROBFILLERTOOL_H

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
    *         Fill Read Out Buffer details for trigger cost monitoring
    *
    * @author Tim Martin
    *
    * $Date: April 2013
    */
  class TrigCostROBFillerTool : public BlockFillerTool< TrigMonEventCollection > {

  public:
    /// Regular AlgTool constructor
    TrigCostROBFillerTool( const std::string& type, const std::string& name,
                           const IInterface* parent );

    /// Function booking the variables in the output
    virtual StatusCode book();
    /// Function filling the variables for the output 
    virtual StatusCode fill( const TrigMonEventCollection& eventCollection );

  private:
    uint32_t m_dataVectorEntryToSave;
    bool     m_failGracefully;
    
    //ROBs
    uint16_t*                               m_robN;
    std::vector< uint32_t >*                m_robRequestorId;
    std::vector< double >*                  m_robTimer;
    std::vector< uint32_t >*                m_robTimeStartSec;
    std::vector< uint32_t >*                m_robTimeStartMicroSec;
    std::vector< uint32_t >*                m_robTimeStopSec;
    std::vector< uint32_t >*                m_robTimeStopMicroSec;
    std::vector< uint16_t >*                m_robDataN;
    std::vector< std::vector< uint32_t > >* m_robDataRobId;
    std::vector< std::vector< uint32_t > >* m_robDataRobSize;
    std::vector< std::vector< uint8_t > >*  m_robDataIsUnclassified;
    std::vector< std::vector< uint8_t > >*  m_robDataIsCached;
    std::vector< std::vector< uint8_t > >*  m_robDataIsRetrieved;
    std::vector< std::vector< uint8_t > >*  m_robDataIsIgnored;
    std::vector< std::vector< uint8_t > >*  m_robDataIsDisabled;
    std::vector< std::vector< uint8_t > >*  m_robDataIsStatusOk;
    std::vector< std::vector< uint8_t > >*  m_robDataIsPrefetched;   
   
    std::vector< uint16_t >*                m_robSumDataN;
    std::vector< std::vector< uint32_t > >* m_robSumDataSize;
    std::vector< std::vector< uint32_t > >* m_robSumDataNRob;
    std::vector< std::vector< uint32_t > >* m_robSumDataSubDet;  
    std::vector< std::vector< uint8_t > >*  m_robSumDataIsUnclassified; 
    std::vector< std::vector< uint8_t > >*  m_robSumDataIsCached;
    std::vector< std::vector< uint8_t > >*  m_robSumDataIsRetrieved;
    std::vector< std::vector< uint8_t > >*  m_robSumDataIsIgnored;
    std::vector< std::vector< uint8_t > >*  m_robSumDataIsDisabled;
    
  }; // class TrigCostROBFillerTool

} // namespace D3PD

#endif // TRIGCOSTD3PDMAKER_TRIGCOSTROBFILLERTOOL_H
