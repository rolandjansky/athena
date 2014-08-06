// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGCOSTD3PDMAKER_SEQUENCEFILLERTOOL_H
#define TRIGCOSTD3PDMAKER_SEQUENCEFILLERTOOL_H

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
    *         Saves HLT Trigger Sequence details for costing purposes,
    *         includes algorithm details from within sequences
    *
    * @author Tim Martin
    *
    * $Date: April 2013
    */
  class TrigCostSequenceFillerTool : public BlockFillerTool< TrigMonEventCollection > { 

  public:
    /// Regular AlgTool constructor
    TrigCostSequenceFillerTool( const std::string& type, const std::string& name,
                                const IInterface* parent );

    /// Function booking the variables in the output
    virtual StatusCode book();
    /// Function filling the variables for the output
    virtual StatusCode fill( const TrigMonEventCollection& eventCollection );

  private:
    uint32_t m_dataVectorEntryToSave;
    bool     m_failGracefully;
    
    //SEQs
    uint16_t*                               m_seqN;
    std::vector< uint8_t >*                 m_seqIsInitial;
    std::vector< uint8_t >*                 m_seqIsExecuted;
    std::vector< uint8_t >*                 m_seqIsAlreadyExecuted;
    std::vector< uint8_t >*                 m_seqIsPrevious;
    std::vector< uint16_t >*                m_seqLevel;
    std::vector< uint16_t >*                m_seqChannelCounter;
    std::vector< uint16_t >*                m_seqIndex; 
    std::vector< float >*                   m_seqSequenceTimer; 
    std::vector< double >*                  m_seqAlgTimerTot; 
    std::vector< uint16_t >*                m_seqAlgN; 
    std::vector< std::vector< uint8_t > >*  m_seqAlgPosition; 
    std::vector< std::vector< uint8_t > >*  m_seqAlgNRoIs; // RoI's are held in another varaible
    std::vector< std::vector< uint32_t > >* m_seqAlgRoIIndex;
    std::vector< std::vector< uint8_t > >*  m_seqRoI; // Hold the RoI data
    std::vector< std::vector< uint8_t > >*  m_seqAlgIsCached;
    std::vector< std::vector< uint8_t > >*  m_seqAlgIsCalled;
    std::vector< std::vector< double > >*   m_seqAlgTimer; 
    std::vector< std::vector< uint32_t > >* m_seqAlgTimeStartSec; 
    std::vector< std::vector< uint32_t > >* m_seqAlgTimeStartMicroSec; 
    std::vector< std::vector< uint32_t > >* m_seqAlgTimeStopSec; 
    std::vector< std::vector< uint32_t > >* m_seqAlgTimeStopMicroSec; 

    
  }; // class TrigCostSequenceFillerTool

} // namespace D3PD

#endif // TRIGCOSTD3PDMAKER_SEQUENCEFILLERTOOL_H
