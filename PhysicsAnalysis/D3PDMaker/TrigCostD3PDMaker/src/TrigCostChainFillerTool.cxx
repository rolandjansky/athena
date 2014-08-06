/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigCostChainFillerTool.cxx tamartin $

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"

// Local include(s):
#include "TrigCostChainFillerTool.h"
#include "TrigCostD3PDUtils.h"

// EDM include(s):
#include "TrigMonitoringEvent/TrigMonEvent.h"
#include "TrigMonitoringEvent/TrigMonChain.h"

namespace D3PD {

  TrigCostChainFillerTool::TrigCostChainFillerTool( const std::string& type,
                                                    const std::string& name,
                                                    const IInterface* parent )
      : BlockFillerTool< TrigMonEventCollection >( type, name, parent )
  {
    // Avoid coverity warnings
    book().ignore();
    
    declareProperty( "DataVectorEntryToSave", m_dataVectorEntryToSave = 0  );
    declareProperty( "FailGracefully", m_failGracefully = true );
  }

  StatusCode TrigCostChainFillerTool::book() {
    //Chains
    CHECK( addVariable( "chain_n", m_chainN, "" ) );
    CHECK( addVariable( "chain_counter", m_chainCounter, "" ) );
    CHECK( addVariable( "chain_level", m_chainLevel, "" ) );
    CHECK( addVariable( "chain_timer", m_chainTimer, "" ) );
    CHECK( addVariable( "chain_isPassed", m_chainIsPassed, "" ) );
    CHECK( addVariable( "chain_isPassedRaw", m_chainIsPassedRaw, "" ) );
    CHECK( addVariable( "chain_isPassthrough", m_chainIsPassthrough, "" ) );
    CHECK( addVariable( "chain_isResurrected", m_chainIsResurrected, "" ) ); 
    CHECK( addVariable( "chain_isPrescaled", m_chainIsPrescaled, "" ) );
    CHECK( addVariable( "chain_wasL1AfterVeto", m_chainWasL1AfterVeto, "" ) );     
    CHECK( addVariable( "chain_wasL1BeforePrescale", m_chainWasL1BeforePrescale, "" ) );  
    CHECK( addVariable( "chain_wasL1AfterPrescale", m_chainWasL1AfterPrescale, "" ) );  
    CHECK( addVariable( "chain_isExpressStream", m_chainIsExpressStream, "" ) );  
    
    return StatusCode::SUCCESS;
  }

  StatusCode TrigCostChainFillerTool::fill( const TrigMonEventCollection& eventCollection ) {

    //Get requested event from vector
    const TrigMonEvent* event = getObjectFromVector(eventCollection, m_dataVectorEntryToSave);
    if ( event == 0 && m_failGracefully == true ) {
      REPORT_MESSAGE( MSG::DEBUG ) << "Requesting entry not contained in DataVector! Continuing run...";
      return StatusCode::SUCCESS;
    } else if ( event == 0 && m_failGracefully == false ) {
      REPORT_MESSAGE( MSG::FATAL ) << "Requesting entry not contained in DataVector! Could not run TrigCostD3PDMaker.";
      return StatusCode::FAILURE;
    }

    //TrigMonChain Flattener
    const std::vector<TrigMonChain> eventChains = event->getChains();
    if ( !eventChains.size() ) {
      //No items to save
      return StatusCode::SUCCESS;
    }
    
    *m_chainN = eventChains.size();

    //Save details about the chain
    m_chainCounter            ->resize( eventChains.size() );
    m_chainLevel              ->resize( eventChains.size() );         
    m_chainTimer              ->resize( eventChains.size() );         
    m_chainIsPassed           ->resize( eventChains.size() );
    m_chainIsPassedRaw        ->resize( eventChains.size() );
    m_chainIsPassthrough      ->resize( eventChains.size() );    
    m_chainIsResurrected      ->resize( eventChains.size() );
    m_chainIsPrescaled        ->resize( eventChains.size() );
    m_chainWasL1AfterVeto     ->resize( eventChains.size() );
    m_chainWasL1BeforePrescale->resize( eventChains.size() );
    m_chainWasL1AfterPrescale ->resize( eventChains.size() );
    m_chainIsExpressStream    ->resize( eventChains.size() );
    
    for(unsigned int i = 0; i < eventChains.size(); ++i) {
      m_chainCounter            ->at(i) = eventChains.at(i).getCounter();     
      m_chainLevel              ->at(i) = eventChains.at(i).getLevel();
      m_chainTimer              ->at(i) = eventChains.at(i).getTimer();
      m_chainIsPassed           ->at(i) = (uint8_t) eventChains.at(i).isPassed( TrigMonChain::kPassed );
      m_chainIsPassedRaw        ->at(i) = (uint8_t) eventChains.at(i).isPassed( TrigMonChain::kPassedRaw );
      m_chainIsPassthrough      ->at(i) = (uint8_t) eventChains.at(i).isPassed( TrigMonChain::kPassedThrough );
      m_chainIsResurrected      ->at(i) = (uint8_t) eventChains.at(i).isPassed( TrigMonChain::kResurrected );
      m_chainIsPrescaled        ->at(i) = (uint8_t) eventChains.at(i).isPassed( TrigMonChain::kPrescaled );
      m_chainWasL1AfterVeto     ->at(i) = (uint8_t) eventChains.at(i).isPassed( TrigMonChain::kL1AfterVeto );
      m_chainWasL1BeforePrescale->at(i) = (uint8_t) eventChains.at(i).isPassed( TrigMonChain::kL1BeforePrescale );
      m_chainWasL1AfterPrescale ->at(i) = (uint8_t) eventChains.at(i).isPassed( TrigMonChain::kL1AfterPrescale );
      m_chainIsExpressStream    ->at(i) = (uint8_t) eventChains.at(i).isPassed( TrigMonChain::kExpressStream );

      // Check and save additonal payload 
      if ( eventChains.at(i).getVarKey().size() != eventChains.at(i).getVarVal().size() ) {
        REPORT_MESSAGE( MSG::WARNING ) << "Trigger chain float payload size different to key size, skipping.";
      } else {
        for (unsigned j=0; j < eventChains.at(i).getVarKey().size(); ++j) {
          REPORT_MESSAGE( MSG::DEBUG ) << "Got a CHAIN["<<i<<"] int=>float pair. Currently not saved. " << eventChains.at(i).getVarKey().at(j) << " => " << eventChains.at(i).getVarVal().at(j);
        }
      }  
    }

    return StatusCode::SUCCESS;
  }

} // namespace D3PD
