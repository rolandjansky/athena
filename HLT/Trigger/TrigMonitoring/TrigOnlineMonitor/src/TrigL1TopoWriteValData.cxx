/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigL1TopoWriteValData.h"

#include "TrigT1Interfaces/FrontPanelCTP.h"
#include "TrigT1Interfaces/TrigT1StoreGateKeys.h"

#include "GaudiKernel/ThreadGaudi.h"
#include "AthenaKernel/Timeout.h"

#include "TrigSteeringEvent/HLTResult.h"
#include "TrigSteeringEvent/HLTExtraData.h"

//----------------------------------------------------------
TrigL1TopoWriteValData::TrigL1TopoWriteValData(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator)
{
  declareProperty("SimTopoCTPLocation",
                  m_simTopoCTPLocation = LVL1::DEFAULT_L1TopoCTPLocation,
                  "StoreGate key of simulated topo decision output for CTP, defaults to default output key of L1TopoSimulation");
  declareProperty("SimTopoOverflowCTPLocation", 
		  m_simTopoOverflowCTPLocation = LVL1::DEFAULT_L1TopoOverflowCTPLocation,
		  "StoreGate key of simulated topo overflow output for CTP" );
  declareProperty("HLTResultName",
                  m_HltResultName = "HLTResult_HLT",
                  "StoreGate key of HLT result" );
}
//----------------------------------------------------------
StatusCode TrigL1TopoWriteValData::initialize(){
  ATH_MSG_INFO ("initialize");
  ATH_MSG_DEBUG ("Properties:" );
  ATH_MSG_DEBUG ("SimTopoCTPLocation : "<<m_simTopoCTPLocation );
  ATH_MSG_DEBUG ("SimTopoOverflowCTPLocation : "<<m_simTopoOverflowCTPLocation );
  ATH_MSG_DEBUG ("HLTResultName : "<<m_HltResultName );
  return StatusCode::SUCCESS;
}
//----------------------------------------------------------
StatusCode TrigL1TopoWriteValData::execute() {

  ATH_MSG_DEBUG ("execute");

  // check that there is still time left
  if (Athena::Timeout::instance().reached()) {
    ATH_MSG_INFO( " Time out reached in entry to execute." );
    return StatusCode::SUCCESS;
  }

  CHECK( doWriteValData() );

  return StatusCode::SUCCESS;
}
//----------------------------------------------------------
StatusCode TrigL1TopoWriteValData::finalize() {
  ATH_MSG_INFO ("finalize");
  return StatusCode::SUCCESS;
}
//----------------------------------------------------------
StatusCode TrigL1TopoWriteValData::doWriteValData(){
  ATH_MSG_DEBUG( "doWriteValData" );
  
  int num_words = 4;
  // Retrieve HLTResult
  DataHandle<HLT::HLTResult> hltResult; ///! HLTResult object
  if ( ! evtStore()->transientContains<HLT::HLTResult>(m_HltResultName.value()) ) {
    ATH_MSG_INFO("Could not find HLTResult with key " << m_HltResultName.value() << "in SG." );
  } else {
    CHECK_RECOVERABLE( evtStore()->retrieve(hltResult,m_HltResultName.value()) );
    if (!hltResult){
      ATH_MSG_INFO( "Retrieve of HLT::HLTResult failed. No data are written to HLTResult" );
      return StatusCode::RECOVERABLE;
    }
  }

  // Retrieve L1Topo CTP simulated decision if present
  const DataHandle< LVL1::FrontPanelCTP > simTopoCTP; ///! simulation output
  if ( ! evtStore()->contains<LVL1::FrontPanelCTP>(m_simTopoCTPLocation.value()) ){
    ATH_MSG_INFO("Could not find LVL1::FrontPanelCTP with key " << m_simTopoCTPLocation.value() << "in SG." );
  } else {
    CHECK_RECOVERABLE( evtStore()->retrieve(simTopoCTP,m_simTopoCTPLocation.value()) );
    if (!simTopoCTP){
      ATH_MSG_INFO( "Retrieve of LVL1::FrontPanelCTP failed. No data are written to HLTResult" );
      return StatusCode::RECOVERABLE;
    }
  }

  // Retrieve L1Topo CTP simulated overflows if present
  const DataHandle< LVL1::FrontPanelCTP > simTopoOverflowCTP; ///! simulated overflow output
  if ( ! evtStore()->contains<LVL1::FrontPanelCTP>(m_simTopoOverflowCTPLocation.value()) ){
    ATH_MSG_INFO("Could not find LVL1::FrontPanelCTP with key " << m_simTopoOverflowCTPLocation.value() << " in SG. No simulated overflow bits" );
  } else {
    CHECK_RECOVERABLE( evtStore()->retrieve(simTopoOverflowCTP,m_simTopoOverflowCTPLocation.value()) );
    if (!simTopoOverflowCTP){
      ATH_MSG_INFO( "Retrieve of LVL1::FrontPanelCTP sim overflows failed. No sim overflow data written to HLTResult" );
    } else {
      num_words = 8;
    }
  }

  // Write the L1Topo simulation data into the HLTResult
  hltResult->getExtraData().anonymous.push_back( num_words );                         // number of words to be written
  hltResult->getExtraData().anonymous.push_back( simTopoCTP->cableWord1(0) ); // L1Topo simulation words
  hltResult->getExtraData().anonymous.push_back( simTopoCTP->cableWord1(1) );
  hltResult->getExtraData().anonymous.push_back( simTopoCTP->cableWord2(0) );
  hltResult->getExtraData().anonymous.push_back( simTopoCTP->cableWord2(1) );

  if ( num_words == 8 ) {
    hltResult->getExtraData().anonymous.push_back( simTopoOverflowCTP->cableWord1(0) );// L1Topo simulated overflow words
    hltResult->getExtraData().anonymous.push_back( simTopoOverflowCTP->cableWord1(1) );
    hltResult->getExtraData().anonymous.push_back( simTopoOverflowCTP->cableWord2(0) );
    hltResult->getExtraData().anonymous.push_back( simTopoOverflowCTP->cableWord2(1) );
  }

  return StatusCode::SUCCESS;
}

