/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigCostSequenceFillerTool.cxx tamartin $

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"

// Local include(s):
#include "TrigCostSequenceFillerTool.h"
#include "TrigCostD3PDUtils.h"

// EDM include(s):
#include "TrigMonitoringEvent/TrigMonEvent.h"
#include "TrigMonitoringEvent/TrigMonSeq.h"
#include "TrigMonitoringEvent/TrigMonAlg.h"

#include<iostream>
namespace D3PD {

  TrigCostSequenceFillerTool::TrigCostSequenceFillerTool( const std::string& type,
                                                          const std::string& name,
                                                          const IInterface* parent )
      : BlockFillerTool< TrigMonEventCollection >( type, name, parent )
  {
    // Avoid coverity warnings
    book().ignore();
    
    declareProperty( "DataVectorEntryToSave", m_dataVectorEntryToSave = 0  );
    declareProperty( "FailGracefully", m_failGracefully = true );
  }

  StatusCode TrigCostSequenceFillerTool::book() {
    //SEQ
    CHECK( addVariable( "seq_n", m_seqN, "" ) );
    CHECK( addVariable( "seq_level", m_seqLevel, "" ) ); 
    CHECK( addVariable( "seq_isInitial", m_seqIsInitial, "" ) ); 
    CHECK( addVariable( "seq_isExecuted", m_seqIsExecuted, "" ) ); 
    CHECK( addVariable( "seq_isAlreadyExecuted", m_seqIsAlreadyExecuted, "" ) ); 
    CHECK( addVariable( "seq_isPrevious", m_seqIsPrevious, "" ) ); 
    CHECK( addVariable( "seq_channelCounter", m_seqChannelCounter, "" ) );         
    CHECK( addVariable( "seq_index", m_seqIndex, "" ) );         
    CHECK( addVariable( "seq_timer", m_seqSequenceTimer, "" ) );         
    CHECK( addVariable( "seq_algTotalTime", m_seqAlgTimerTot, "" ) );         
    CHECK( addVariable( "seq_alg_n", m_seqAlgN, "" ) );         
    CHECK( addVariable( "seq_alg_position", m_seqAlgPosition, "" ) );  
    CHECK( addVariable( "seq_alg_roi_n", m_seqAlgNRoIs, "" ) ); // Limit to storage depth, how many RoIs this alg has         
    CHECK( addVariable( "seq_alg_roi_index", m_seqAlgRoIIndex, "" ) ); // Index where to find these RoI IDs
    CHECK( addVariable( "seq_roi", m_seqRoI, "" ) ); // Look at this index to get the vector<RoI ID>
    CHECK( addVariable( "seq_alg_isCached", m_seqAlgIsCached, "" ) );       
    CHECK( addVariable( "seq_alg_isCalled", m_seqAlgIsCalled, "" ) );       
    CHECK( addVariable( "seq_alg_timer", m_seqAlgTimer, "" ) );    
    CHECK( addVariable( "seq_alg_timeStartSec", m_seqAlgTimeStartSec, "" ) );  
    CHECK( addVariable( "seq_alg_timeStartMicroSec", m_seqAlgTimeStartMicroSec, "" ) );  
    CHECK( addVariable( "seq_alg_timeStopSec", m_seqAlgTimeStopSec, "" ) );  
    CHECK( addVariable( "seq_alg_timeStopMicroSec", m_seqAlgTimeStopMicroSec, "" ) );     
        
    return StatusCode::SUCCESS;
  }

  StatusCode TrigCostSequenceFillerTool::fill( const TrigMonEventCollection& eventCollection ) {

    //Get requested event from vector
    const TrigMonEvent* event = getObjectFromVector(eventCollection, m_dataVectorEntryToSave);
    if ( event == 0 && m_failGracefully == true ) {
      REPORT_MESSAGE( MSG::DEBUG ) << "Requesting entry not contained in DataVector! Continuing run...";
      return StatusCode::SUCCESS;
    } else if ( event == 0 && m_failGracefully == false ) {
      REPORT_MESSAGE( MSG::FATAL ) << "Requesting entry not contained in DataVector! Could not run TrigCostD3PDMaker."; 
      return StatusCode::FAILURE;
    }
    
    //TrigMonSeq Flattner
    const std::vector<TrigMonSeq> eventSeqs = event->getVec<TrigMonSeq>();
    if ( !eventSeqs.size() ) {
      //No items to save
      return StatusCode::SUCCESS;
    }
    
    int _RoIIndex = 0;
    *m_seqN = eventSeqs.size();
    
    m_seqLevel               ->resize( eventSeqs.size() );
    m_seqIsInitial           ->resize( eventSeqs.size() );
    m_seqIsExecuted          ->resize( eventSeqs.size() );
    m_seqIsAlreadyExecuted   ->resize( eventSeqs.size() );
    m_seqIsPrevious          ->resize( eventSeqs.size() );
    m_seqChannelCounter      ->resize( eventSeqs.size() ); 
    m_seqIndex               ->resize( eventSeqs.size() );
    m_seqSequenceTimer       ->resize( eventSeqs.size() ); 
    m_seqAlgTimerTot         ->resize( eventSeqs.size() );
    m_seqAlgN                ->resize( eventSeqs.size() ); 
    m_seqAlgPosition         ->resize( eventSeqs.size() );
    m_seqAlgNRoIs            ->resize( eventSeqs.size() );
    m_seqAlgRoIIndex         ->resize( eventSeqs.size() );
    // We do not resize m_seqRoI as we don't yet know how big it will be
    m_seqAlgIsCached         ->resize( eventSeqs.size() );
    m_seqAlgIsCalled         ->resize( eventSeqs.size() );
    m_seqAlgTimer            ->resize( eventSeqs.size() );
    m_seqAlgTimeStartSec     ->resize( eventSeqs.size() );
    m_seqAlgTimeStartMicroSec->resize( eventSeqs.size() );
    m_seqAlgTimeStopSec      ->resize( eventSeqs.size() );
    m_seqAlgTimeStopMicroSec ->resize( eventSeqs.size() ); 
    
    for(unsigned int i = 0; i < eventSeqs.size(); ++i) {
      m_seqLevel            ->at(i) = eventSeqs.at(i).getLevel();
      m_seqIsInitial        ->at(i) = (uint8_t) eventSeqs.at(i).isInitial();
      m_seqIsExecuted       ->at(i) = (uint8_t) eventSeqs.at(i).isExecuted();
      m_seqIsAlreadyExecuted->at(i) = (uint8_t) eventSeqs.at(i).isAlreadyExecuted();
      m_seqIsPrevious       ->at(i) = (uint8_t) eventSeqs.at(i).isPrevious();
      m_seqChannelCounter   ->at(i) = eventSeqs.at(i).getChnCounter();
      m_seqIndex            ->at(i) = eventSeqs.at(i).getSeqIndex();
      m_seqSequenceTimer    ->at(i) = eventSeqs.at(i).getSeqTimer();
      m_seqAlgTimerTot      ->at(i) = eventSeqs.at(i).getAlgTimer();
      //Loop over sequence algorithms
      const std::vector<TrigMonAlg> seqAlgs = eventSeqs.at(i).getAlg();
      m_seqAlgN->at(i) = seqAlgs.size();
      (m_seqAlgPosition         ->at(i)).resize( seqAlgs.size() );
      (m_seqAlgNRoIs            ->at(i)).resize( seqAlgs.size() );
      (m_seqAlgRoIIndex         ->at(i)).resize( seqAlgs.size() );
      (m_seqAlgIsCached         ->at(i)).resize( seqAlgs.size() );
      (m_seqAlgIsCalled         ->at(i)).resize( seqAlgs.size() );
      (m_seqAlgTimer            ->at(i)).resize( seqAlgs.size() );
      (m_seqAlgTimeStartSec     ->at(i)).resize( seqAlgs.size() );
      (m_seqAlgTimeStartMicroSec->at(i)).resize( seqAlgs.size() );
      (m_seqAlgTimeStopSec      ->at(i)).resize( seqAlgs.size() );
      (m_seqAlgTimeStopMicroSec ->at(i)).resize( seqAlgs.size() );
      for (unsigned int j = 0; j < seqAlgs.size(); ++j) {
        (m_seqAlgPosition   ->at(i)).at(j) = seqAlgs.at(j).getPosition();
        // We shall not go deeper than vector<vector< bla > >, so store this vector<uint8_t> "up one level"
        (m_seqAlgNRoIs            ->at(i)).at(j) = seqAlgs.at(j).getNRoi();
        (m_seqAlgRoIIndex         ->at(i)).at(j) = _RoIIndex++;
        // We need to go via a vector, but this class gives us a set. Need to convert
        std::vector< uint8_t > _RoIIdSet;
        _RoIIdSet.clear();
        _RoIIdSet.resize( seqAlgs.at(j).getNRoi() );
        for (unsigned int _roi = 0; _roi < seqAlgs.at(j).getNRoi(); ++_roi) {
          _RoIIdSet.at(_roi) = seqAlgs.at(j).getRoiId( _roi );
        }
        m_seqRoI                  ->push_back( _RoIIdSet );
        (m_seqAlgIsCached         ->at(i)).at(j) = (uint8_t) seqAlgs.at(j).isCached();
        (m_seqAlgIsCalled         ->at(i)).at(j) = (uint8_t) seqAlgs.at(j).isCalled();
        (m_seqAlgTimer            ->at(i)).at(j) = seqAlgs.at(j).getTimer();
        (m_seqAlgTimeStartSec     ->at(i)).at(j) = seqAlgs.at(j).start().getSec();
        (m_seqAlgTimeStartMicroSec->at(i)).at(j) = seqAlgs.at(j).start().getMicroSec();
        (m_seqAlgTimeStopSec      ->at(i)).at(j) = seqAlgs.at(j).stop().getSec();
        (m_seqAlgTimeStopMicroSec ->at(i)).at(j) = seqAlgs.at(j).stop().getMicroSec();
      }

      // Check and save additonal payload 
      if ( eventSeqs.at(i).getVarKey().size() != eventSeqs.at(i).getVarVal().size() ) {
        REPORT_MESSAGE( MSG::WARNING ) << "Trigger sequences additional float payload size different to key size, skipping.";
      } else {
        for (unsigned j=0; j < eventSeqs.at(i).getVarKey().size(); ++j) {
          REPORT_MESSAGE( MSG::DEBUG ) << "Got a SEQ["<<i<<"] int=>float pair. " << eventSeqs.at(i).getVarKey().at(j) << " => " << eventSeqs.at(i).getVarVal().at(j);
        }
      }  
    }
   
    return StatusCode::SUCCESS;
  }

} // namespace D3PD
