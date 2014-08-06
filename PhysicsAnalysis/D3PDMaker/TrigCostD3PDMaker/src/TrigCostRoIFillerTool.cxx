/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigCostRoIFillerTool.cxx tamartin $

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"

// Local include(s):
#include "TrigCostRoIFillerTool.h"
#include "TrigCostD3PDUtils.h"

// EDM include(s):
#include "TrigMonitoringEvent/TrigMonEvent.h"
#include "TrigMonitoringEvent/TrigMonRoi.h"

namespace D3PD {

  TrigCostRoIFillerTool::TrigCostRoIFillerTool( const std::string& type,
                                                const std::string& name,
                                                const IInterface* parent )
      : BlockFillerTool< TrigMonEventCollection >( type, name, parent )
  {
    // Avoid coverity warnings
    book().ignore();
    
    declareProperty( "DataVectorEntryToSave", m_dataVectorEntryToSave = 0  );
    declareProperty( "FailGracefully", m_failGracefully = true );
  }

  StatusCode TrigCostRoIFillerTool::book() {
    //RoIs
    CHECK( addVariable( "roi_n", m_roiN, "" ) );
    CHECK( addVariable( "roi_isTypeNone", m_roiIsNoneType, "" ) );
    CHECK( addVariable( "roi_isTypeMuon", m_roiIsMuonType, "" ) );
    CHECK( addVariable( "roi_isTypeEmTau", m_roiIsEmTauType, "" ) );
    CHECK( addVariable( "roi_isTypeJet", m_roiIsJetType, "" ) );
    CHECK( addVariable( "roi_isTypeJetEt", m_roiIsJetEtType, "" ) );
    CHECK( addVariable( "roi_isTypeEnergy", m_roiIsEnergyType, "" ) );
    CHECK( addVariable( "roi_id", m_roiId, "" ) );
    CHECK( addVariable( "roi_nL1Thresholds", m_roiNL1thresholds, "" ) );
    CHECK( addVariable( "roi_eta", m_roiEta, "" ) );
    CHECK( addVariable( "roi_phi", m_roiPhi, "" ) );
    CHECK( addVariable( "roi_area", m_roiArea, "" ) );  
    return StatusCode::SUCCESS;
  }
  
  StatusCode TrigCostRoIFillerTool::fill( const TrigMonEventCollection& eventCollection ) {

    //Get requested event from vector
    const TrigMonEvent* event = getObjectFromVector(eventCollection, m_dataVectorEntryToSave);
    if ( event == 0 && m_failGracefully == true ) {
      REPORT_MESSAGE( MSG::DEBUG ) << "Requesting entry not contained in DataVector! Continuing run...";
      return StatusCode::SUCCESS;
    } else if ( event == 0 && m_failGracefully == false ) {
      REPORT_MESSAGE( MSG::FATAL ) << "Requesting entry not contained in DataVector! Could not run TrigCostD3PDMaker.";
      return StatusCode::FAILURE;
    }

    const std::vector<TrigMonRoi> eventRoIs = event->getVec<TrigMonRoi>();
    if ( !eventRoIs.size() ) {
      //No items to save
      return StatusCode::SUCCESS;
    }
    
    *m_roiN = eventRoIs.size();

    m_roiIsNoneType     ->resize( eventRoIs.size() );
    m_roiIsMuonType     ->resize( eventRoIs.size() );
    m_roiIsEmTauType    ->resize( eventRoIs.size() );
    m_roiIsJetType      ->resize( eventRoIs.size() );
    m_roiIsJetEtType    ->resize( eventRoIs.size() );
    m_roiIsEnergyType   ->resize( eventRoIs.size() );
    m_roiId             ->resize( eventRoIs.size() );
    m_roiNL1thresholds  ->resize( eventRoIs.size() );
    m_roiEta            ->resize( eventRoIs.size() );
    m_roiPhi            ->resize( eventRoIs.size() );
    m_roiArea           ->resize( eventRoIs.size() );

    for(unsigned int i = 0; i < eventRoIs.size(); ++i) {
      m_roiIsNoneType   ->at(i) = (uint8_t) (eventRoIs.at(i).getRoiType() == TrigMonRoi::kNone);
      m_roiIsMuonType   ->at(i) = (uint8_t) (eventRoIs.at(i).getRoiType() == TrigMonRoi::kMuon);
      m_roiIsEmTauType  ->at(i) = (uint8_t) (eventRoIs.at(i).getRoiType() == TrigMonRoi::kEmTau);
      m_roiIsJetType    ->at(i) = (uint8_t) (eventRoIs.at(i).getRoiType() == TrigMonRoi::kJet);
      m_roiIsJetEtType  ->at(i) = (uint8_t) (eventRoIs.at(i).getRoiType() == TrigMonRoi::kJetEt);
      m_roiIsEnergyType ->at(i) = (uint8_t) (eventRoIs.at(i).getRoiType() == TrigMonRoi::kEnergy);
      m_roiId           ->at(i) = eventRoIs.at(i).getRoiId();
      m_roiNL1thresholds->at(i) = eventRoIs.at(i).getNL1th();
      m_roiEta          ->at(i) = eventRoIs.at(i).getEta();
      m_roiPhi          ->at(i) = eventRoIs.at(i).getPhi();
      m_roiArea         ->at(i) = eventRoIs.at(i).getRoIArea();
    }


    return StatusCode::SUCCESS;
  }

} // namespace D3PD
