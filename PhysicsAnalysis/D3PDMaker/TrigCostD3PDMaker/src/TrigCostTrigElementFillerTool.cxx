/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigCostTrigElementFillerTool.cxx tamartin $

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"

// Local include(s):
#include "TrigCostTrigElementFillerTool.h"
#include "TrigCostD3PDUtils.h"

// EDM include(s):
#include "TrigMonitoringEvent/TrigMonEvent.h"
#include "TrigMonitoringEvent/TrigMonTE.h"

namespace D3PD {

  TrigCostTrigElementFillerTool::TrigCostTrigElementFillerTool( const std::string& type,
                                                                const std::string& name,
                                                                const IInterface* parent )
      : BlockFillerTool< TrigMonEventCollection >( type, name, parent )
  {
    // Avoid coverity warnings
    book().ignore();
    
    declareProperty( "DataVectorEntryToSave", m_dataVectorEntryToSave = 0  );
    declareProperty( "FailGracefully", m_failGracefully = true );
  }

  StatusCode TrigCostTrigElementFillerTool::book() {
    //TEs
    CHECK( addVariable( "te_n", m_teN, "" ) );
    CHECK( addVariable( "te_id", m_teId, "" ) );
    CHECK( addVariable( "te_index", m_teIndex, "" ) );
    CHECK( addVariable( "te_isRegularTe", m_teIsRegularTE, "" ) );
    CHECK( addVariable( "te_isInitialTe", m_teIsInitialTE, "" ) );
    CHECK( addVariable( "te_isRoITe", m_teIsRoITE, "" ) );
    CHECK( addVariable( "te_isL1ThresholdTe", m_teIsL1ThreshTE, "" ) );
    CHECK( addVariable( "te_isActiveState", m_teActiveState, "" ) );
    CHECK( addVariable( "te_isErrorState", m_teErrorState, "" ) );
    CHECK( addVariable( "te_isTerminalNode", m_teIsTerminalNode, "" ) );
    CHECK( addVariable( "te_isOutputL2Node", m_teIsOutputL2Node, "" ) );
    CHECK( addVariable( "te_isOutputEFNode", m_teIsOutputEFNode, "" ) );
    CHECK( addVariable( "te_isTopologicalTe", m_teIsTopologicalTE, "" ) );
    CHECK( addVariable( "te_childIndex", m_teChildIndex, "" ) );
    CHECK( addVariable( "te_parentIndex", m_teParentIndex, "" ) );
    CHECK( addVariable( "te_roiId", m_teRoiId, "" ) );
    CHECK( addVariable( "te_clid", m_teClid, "" ) );
    
    return StatusCode::SUCCESS;
  }

  StatusCode TrigCostTrigElementFillerTool::fill( const TrigMonEventCollection& eventCollection ) {

    //Get requested event from vector
    const TrigMonEvent* event = getObjectFromVector(eventCollection, m_dataVectorEntryToSave);
    if ( event == 0 && m_failGracefully == true ) {
      REPORT_MESSAGE( MSG::DEBUG ) << "Requesting entry not contained in DataVector! Continuing run...";
      return StatusCode::SUCCESS;
    } else if ( event == 0 && m_failGracefully == false ) {
      REPORT_MESSAGE( MSG::FATAL ) << "Requesting entry not contained in DataVector! Could not run TrigCostD3PDMaker.";
      return StatusCode::FAILURE;
    }
    
    //TrigMonTE Flattener
    const std::vector<TrigMonTE> eventTEs = event->getVec<TrigMonTE>();
    
    if ( !eventTEs.size() ) {
      //No items to save
      return StatusCode::SUCCESS;
    }
  
    *m_teN = eventTEs.size();

    m_teId         ->resize( eventTEs.size() );
    m_teIndex      ->resize( eventTEs.size() );

    m_teIsRegularTE    ->resize( eventTEs.size() );
    m_teIsInitialTE    ->resize( eventTEs.size() );
    m_teIsRoITE        ->resize( eventTEs.size() );
    m_teIsL1ThreshTE   ->resize( eventTEs.size() );
    m_teActiveState    ->resize( eventTEs.size() );
    m_teErrorState     ->resize( eventTEs.size() );
    m_teIsTerminalNode ->resize( eventTEs.size() );
    m_teIsOutputL2Node ->resize( eventTEs.size() );
    m_teIsOutputEFNode ->resize( eventTEs.size() );
    m_teIsTopologicalTE->resize( eventTEs.size() );
    m_teChildIndex     ->resize( eventTEs.size() ); 
    m_teParentIndex    ->resize( eventTEs.size() );
    m_teRoiId          ->resize( eventTEs.size() );
    m_teClid           ->resize( eventTEs.size() ); 
    
    for(unsigned int i = 0; i < eventTEs.size(); ++i) {
      m_teId             ->at(i) = eventTEs.at(i).getId();
      m_teIndex          ->at(i) = eventTEs.at(i).getIndex();
      m_teIsRegularTE    ->at(i) = (uint8_t) (eventTEs.at(i).getType() == TrigMonTE::kELEM);
      m_teIsInitialTE    ->at(i) = (uint8_t) (eventTEs.at(i).getType() == TrigMonTE::kINIT);
      m_teIsRoITE        ->at(i) = (uint8_t) (eventTEs.at(i).getType() == TrigMonTE::kROI);
      m_teIsL1ThreshTE   ->at(i) = (uint8_t) (eventTEs.at(i).getType() == TrigMonTE::kL1TH);
      m_teActiveState    ->at(i) = (uint8_t) eventTEs.at(i).getActiveState();
      m_teErrorState     ->at(i) = (uint8_t) eventTEs.at(i).getErrorState();
      m_teIsTerminalNode ->at(i) = (uint8_t) eventTEs.at(i).isTerminalNode();
      m_teIsOutputL2Node ->at(i) = (uint8_t) eventTEs.at(i).isOutputL2Node();
      m_teIsOutputEFNode ->at(i) = (uint8_t) eventTEs.at(i).isOutputEFNode();
      m_teIsTopologicalTE->at(i) = (uint8_t) eventTEs.at(i).isTopologicalTE();
      //Get inner vectors direct from the TE
      m_teChildIndex ->at(i) = eventTEs.at(i).getChildIndex();
      m_teParentIndex->at(i) = eventTEs.at(i).getParentIndex();
      m_teRoiId      ->at(i) = eventTEs.at(i).getRoiId();
      m_teClid       ->at(i) = eventTEs.at(i).getClid();

      // Check and save additonal payload 
      if ( eventTEs.at(i).getVarKey().size() != eventTEs.at(i).getVarVal().size() ) {
        REPORT_MESSAGE( MSG::WARNING ) << "Trigger TE additional float payload size different to key size, skipping.";
      } else {
        for (unsigned j=0; j < eventTEs.at(i).getVarKey().size(); ++j) {
          REPORT_MESSAGE( MSG::WARNING ) << "TIMM Got a TE["<<i<<"] int=>float pair. " << eventTEs.at(i).getVarKey().at(j) << " => " << eventTEs.at(i).getVarVal().at(j);
        }
      }  
    }



    return StatusCode::SUCCESS;
  }

} // namespace D3PD
