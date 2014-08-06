/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigCostROBFillerTool.cxx tamartin $

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"

// Local include(s):
#include "TrigCostROBFillerTool.h"
#include "TrigCostD3PDUtils.h"

// EDM include(s):
#include "TrigMonitoringEvent/TrigMonEvent.h"
#include "TrigMonitoringEvent/TrigMonROB.h"
#include "TrigMonitoringEvent/TrigMonROBData.h"
#include "TrigMonitoringEvent/TrigMonROBSum.h"

namespace D3PD {

  TrigCostROBFillerTool::TrigCostROBFillerTool( const std::string& type,
                                                const std::string& name,
                                                const IInterface* parent )
      : BlockFillerTool< TrigMonEventCollection >( type, name, parent )
  {
    // Avoid coverity warnings
    book().ignore();
    
    declareProperty( "DataVectorEntryToSave", m_dataVectorEntryToSave = 0  );
    declareProperty( "FailGracefully", m_failGracefully = true );
  }

  StatusCode TrigCostROBFillerTool::book() {
    CHECK( addVariable( "rob_n", m_robN, "" ) );
    CHECK( addVariable( "rob_requestorId", m_robRequestorId, "" ) );
    CHECK( addVariable( "rob_timer", m_robTimer, "" ) );
    CHECK( addVariable( "rob_timeStartSec", m_robTimeStartSec, "" ) );
    CHECK( addVariable( "rob_timeStartMicroSec", m_robTimeStartMicroSec, "" ) );
    CHECK( addVariable( "rob_timeStopSec", m_robTimeStopSec, "" ) );
    CHECK( addVariable( "rob_timeStopMicroSec", m_robTimeStopMicroSec, "" ) );
    CHECK( addVariable( "rob_data_n", m_robDataN, "" ) );
    CHECK( addVariable( "rob_data_id", m_robDataRobId, "" ) );
    CHECK( addVariable( "rob_data_size", m_robDataRobSize, "" ) );
    CHECK( addVariable( "rob_data_isUnclassified", m_robDataIsUnclassified, "" ) );
    CHECK( addVariable( "rob_data_isCached", m_robDataIsCached, "" ) );
    CHECK( addVariable( "rob_data_isRetrieved", m_robDataIsRetrieved, "" ) );
    CHECK( addVariable( "rob_data_isIgnored", m_robDataIsIgnored, "" ) );
    CHECK( addVariable( "rob_data_isDisabled", m_robDataIsDisabled, "" ) );
    CHECK( addVariable( "rob_data_isStatusOk", m_robDataIsStatusOk, "" ) );
    CHECK( addVariable( "rob_data_isStatusPrefetched", m_robDataIsPrefetched, "" ) );
    CHECK( addVariable( "rob_sum_n", m_robSumDataN, "" ) );
    CHECK( addVariable( "rob_sum_size", m_robSumDataSize, "" ) );
    CHECK( addVariable( "rob_sum_nRob", m_robSumDataNRob, "" ) );
    CHECK( addVariable( "rob_sum_subDet", m_robSumDataSubDet, "" ) );
    CHECK( addVariable( "rob_sum_isUnclassified", m_robSumDataIsUnclassified, "" ) );
    CHECK( addVariable( "rob_sum_isCached", m_robSumDataIsCached, "" ) );
    CHECK( addVariable( "rob_sum_isRetrieved", m_robSumDataIsRetrieved, "" ) );
    CHECK( addVariable( "rob_sum_isIgnored", m_robSumDataIsIgnored, "" ) );
    CHECK( addVariable( "rob_sum_isDisabled", m_robSumDataIsDisabled, "" ) );
    
    return StatusCode::SUCCESS;
  }

  StatusCode TrigCostROBFillerTool::fill( const TrigMonEventCollection& eventCollection ) {

    //Get requested event from vector
    const TrigMonEvent* event = getObjectFromVector(eventCollection, m_dataVectorEntryToSave);
    if ( event == 0 && m_failGracefully == true ) {
      REPORT_MESSAGE( MSG::DEBUG ) << "Requesting entry not contained in DataVector! Continuing run...";
      return StatusCode::SUCCESS;
    } else if ( event == 0 && m_failGracefully == false ) {
      REPORT_MESSAGE( MSG::FATAL ) << "Requesting entry not contained in DataVector! Could not run TrigCostD3PDMaker.";
      return StatusCode::FAILURE;
    }

    //TrigMonROB Flattner
    const std::vector<TrigMonROB> eventROBs = event->getVec<TrigMonROB>();
    if ( !eventROBs.size() ) {
      //No items to save
      return StatusCode::SUCCESS;
    }
    
    *m_robN = eventROBs.size();
    
    m_robRequestorId          ->resize( eventROBs.size() );
    m_robTimer                ->resize( eventROBs.size() ); 
    m_robTimeStartSec         ->resize( eventROBs.size() );
    m_robTimeStartMicroSec    ->resize( eventROBs.size() );
    m_robTimeStopSec           ->resize( eventROBs.size() );
    m_robTimeStopMicroSec      ->resize( eventROBs.size() );
    m_robDataN                ->resize( eventROBs.size() );
    m_robDataRobId            ->resize( eventROBs.size() );
    m_robDataRobSize          ->resize( eventROBs.size() );
    m_robDataIsUnclassified   ->resize( eventROBs.size() );
    m_robDataIsCached         ->resize( eventROBs.size() );
    m_robDataIsRetrieved      ->resize( eventROBs.size() );
    m_robDataIsIgnored        ->resize( eventROBs.size() );
    m_robDataIsDisabled       ->resize( eventROBs.size() );
    m_robDataIsStatusOk       ->resize( eventROBs.size() );
    m_robDataIsPrefetched     ->resize( eventROBs.size() );   
    m_robSumDataN             ->resize( eventROBs.size() );
    m_robSumDataSize          ->resize( eventROBs.size() );
    m_robSumDataNRob          ->resize( eventROBs.size() );
    m_robSumDataSubDet        ->resize( eventROBs.size() );   
    m_robSumDataIsUnclassified->resize( eventROBs.size() );
    m_robSumDataIsCached      ->resize( eventROBs.size() );
    m_robSumDataIsRetrieved   ->resize( eventROBs.size() );
    m_robSumDataIsIgnored     ->resize( eventROBs.size() );
    m_robSumDataIsDisabled    ->resize( eventROBs.size() );
    
    
    for(unsigned int i = 0; i < eventROBs.size(); ++i) { 
      m_robRequestorId      ->at(i) = eventROBs.at(i).getRequestorId();
      m_robTimer            ->at(i) = eventROBs.at(i).getTimer();
      m_robTimeStartSec     ->at(i) = eventROBs.at(i).start().getSec();
      m_robTimeStartMicroSec->at(i) = eventROBs.at(i).start().getMicroSec();
      m_robTimeStopSec      ->at(i) = eventROBs.at(i).stop().getSec();
      m_robTimeStopMicroSec ->at(i) = eventROBs.at(i).stop().getMicroSec();
      //Loop over ROB data items
      const std::vector<TrigMonROBData> robData = eventROBs.at(i).getData(); 
      m_robDataN->at(i) = robData.size();
      (m_robDataRobId         ->at(i)).resize( robData.size() );
      (m_robDataRobSize       ->at(i)).resize( robData.size() );
      (m_robDataIsUnclassified->at(i)).resize( robData.size() );
      (m_robDataIsCached      ->at(i)).resize( robData.size() );
      (m_robDataIsRetrieved   ->at(i)).resize( robData.size() );
      (m_robDataIsIgnored     ->at(i)).resize( robData.size() );
      (m_robDataIsDisabled    ->at(i)).resize( robData.size() );
      (m_robDataIsStatusOk    ->at(i)).resize( robData.size() );
      (m_robDataIsPrefetched  ->at(i)).resize( robData.size() ); 
      for (unsigned int j = 0; j < robData.size(); ++j) {
        (m_robDataRobId         ->at(i)).at(j) = robData.at(j).getROBId();
        (m_robDataRobSize       ->at(i)).at(j) = robData.at(j).getROBSize();
        (m_robDataIsUnclassified->at(i)).at(j) = (uint8_t) robData.at(j).isUnclassified();
        (m_robDataIsCached      ->at(i)).at(j) = (uint8_t) robData.at(j).isCached();
        (m_robDataIsRetrieved   ->at(i)).at(j) = (uint8_t) robData.at(j).isRetrieved();
        (m_robDataIsIgnored     ->at(i)).at(j) = (uint8_t) robData.at(j).isIgnored();
        (m_robDataIsDisabled    ->at(i)).at(j) = (uint8_t) robData.at(j).isDisabled();
        (m_robDataIsStatusOk    ->at(i)).at(j) = (uint8_t) robData.at(j).isStatusOk();
        (m_robDataIsPrefetched  ->at(i)).at(j) = (uint8_t) robData.at(j).isStatusPrefetched(); 
      }
      //Loop over ROB SUM data items
      const std::vector<TrigMonROBSum> robSum = eventROBs.at(i).getSum();
      m_robSumDataN->at(i) = robSum.size();
      (m_robSumDataSize          ->at(i)).resize( robSum.size() );
      (m_robSumDataNRob          ->at(i)).resize( robSum.size() );
      (m_robSumDataSubDet        ->at(i)).resize( robSum.size() );   
      (m_robSumDataIsUnclassified->at(i)).resize( robSum.size() );
      (m_robSumDataIsCached      ->at(i)).resize( robSum.size() );
      (m_robSumDataIsRetrieved   ->at(i)).resize( robSum.size() );
      (m_robSumDataIsIgnored     ->at(i)).resize( robSum.size() );
      (m_robSumDataIsDisabled    ->at(i)).resize( robSum.size() );
      for (unsigned int j = 0; j < robSum.size(); ++j) {
        (m_robSumDataSize          ->at(i)).at(j) = robSum.at(j).getSize();
        (m_robSumDataNRob          ->at(i)).at(j) = robSum.at(j).getNROB();
        (m_robSumDataSubDet        ->at(i)).at(j) = robSum.at(j).getSubDet();
        (m_robSumDataIsUnclassified->at(i)).at(j) = (uint8_t) (robSum.at(j).getHistory() == TrigMonROBData::kUNCLASSIFIED);
        (m_robSumDataIsCached      ->at(i)).at(j) = (uint8_t) (robSum.at(j).getHistory() == TrigMonROBData::kCACHED);
        (m_robSumDataIsRetrieved   ->at(i)).at(j) = (uint8_t) (robSum.at(j).getHistory() == TrigMonROBData::kRETRIEVED);
        (m_robSumDataIsIgnored     ->at(i)).at(j) = (uint8_t) (robSum.at(j).getHistory() == TrigMonROBData::kIGNORED);
        (m_robSumDataIsDisabled    ->at(i)).at(j) = (uint8_t) (robSum.at(j).getHistory() == TrigMonROBData::kDISABLED);
      }
    }
 
    return StatusCode::SUCCESS;
  }

} // namespace D3PD
