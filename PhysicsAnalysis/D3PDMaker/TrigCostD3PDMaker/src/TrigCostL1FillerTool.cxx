/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigCostBaseFillerTool.cxx tamartin $

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"

// Local include(s):
#include "TrigCostL1FillerTool.h"
#include "TrigCostD3PDUtils.h"

// EDM include(s):
#include "TrigMonitoringEvent/TrigMonEvent.h"
#include "TrigMonitoringEvent/TrigMonL1Item.h"


namespace D3PD {

  TrigCostL1FillerTool::TrigCostL1FillerTool( const std::string& type,
                                              const std::string& name,
                                              const IInterface* parent )
    : BlockFillerTool< TrigMonEventCollection >( type, name, parent )
  {
    // Avoid coverity warnings
    book().ignore();
    
    declareProperty( "DataVectorEntryToSave", m_dataVectorEntryToSave = 0  );
    declareProperty( "FailGracefully", m_failGracefully = true );
  }

  StatusCode TrigCostL1FillerTool::book() {
    //Chains
    CHECK( addVariable( "l1_n", m_l1N, "" ) );
    CHECK( addVariable( "l1_ctpId", m_l1CtpId, "" ) );
    CHECK( addVariable( "l1_passedBeforePrescale", m_l1PassedBeforePrescale, "" ) );
    CHECK( addVariable( "l1_passedAfterPrescale", m_l1PassedAfterPrescale, "" ) );
    CHECK( addVariable( "l1_passedAfterVeto", m_l1PassedAfterVeto, "" ) );
    CHECK( addVariable( "l1_passed", m_l1Passed, "" ) );
    CHECK( addVariable( "l1_isPrescaled", m_l1Prescaled, "" ) );
    CHECK( addVariable( "l1_isVetoed", m_l1Vetoed, "" ) );
    
    return StatusCode::SUCCESS;
   }

  StatusCode TrigCostL1FillerTool::fill( const TrigMonEventCollection& eventCollection ) {

    //Get requested event from vector
    const TrigMonEvent* event = getObjectFromVector(eventCollection, m_dataVectorEntryToSave);
    if ( event == 0 && m_failGracefully == true ) {
      REPORT_MESSAGE( MSG::DEBUG ) << "Requesting entry not contained in DataVector! Continuing run...";
      return StatusCode::SUCCESS;
    } else if ( event == 0 && m_failGracefully == false ) {
      REPORT_MESSAGE( MSG::FATAL ) << "Requesting entry not contained in DataVector! Could not run TrigCostD3PDMaker.";
      return StatusCode::FAILURE;
    }
    
    //TrigMonL1Item Flattener
    const std::vector<TrigMonL1Item> eventL1Items = event->getL1Items();

    if ( !eventL1Items.size() ) {
      //No items to save
      return StatusCode::SUCCESS;
    }
        
    *m_l1N = eventL1Items.size();
    m_l1CtpId               ->resize( eventL1Items.size() );
    m_l1PassedBeforePrescale->resize( eventL1Items.size() );    
    m_l1PassedAfterPrescale ->resize( eventL1Items.size() );    
    m_l1PassedAfterVeto     ->resize( eventL1Items.size() );
    m_l1Passed              ->resize( eventL1Items.size() );
    m_l1Prescaled           ->resize( eventL1Items.size() );
    m_l1Vetoed              ->resize( eventL1Items.size() );  
    for (unsigned int i = 0; i < eventL1Items.size(); ++i) {
      m_l1CtpId               ->at(i) = eventL1Items.at(i).getCtpId();
      m_l1PassedBeforePrescale->at(i) = (uint8_t) eventL1Items.at(i).isPassedBeforePrescale();
      m_l1PassedAfterPrescale ->at(i) = (uint8_t) eventL1Items.at(i).isPassedAfterPrescale();
      m_l1PassedAfterVeto     ->at(i) = (uint8_t) eventL1Items.at(i).isPassedAfterVeto();
      m_l1Passed              ->at(i) = (uint8_t) eventL1Items.at(i).isPassed();
      m_l1Prescaled           ->at(i) = (uint8_t) eventL1Items.at(i).isPrescaled();
      m_l1Vetoed              ->at(i) = (uint8_t) eventL1Items.at(i).isVeto();
    }
 
    return StatusCode::SUCCESS;
  }

} // namespace D3PD
