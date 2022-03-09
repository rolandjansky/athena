/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/


#include "PrimaryDPDMaker/CollisionFilterAlg.h"
#include "PrimaryDPDMaker/CaloTimeFilterTool.h"
#include "PrimaryDPDMaker/MBTSTimeFilterTool.h"


CollisionFilterAlg::CollisionFilterAlg(const std::string& name, ISvcLocator* pSvcLocator) 
 : AthReentrantAlgorithm(name, pSvcLocator)
 , m_calofilter("CaloTimeFilterTool")
 , m_mbtsfilter("MBTSTimeFilterTool"){
  declareProperty( "CaloTimeFilterTool", m_calofilter, "The instance of the CaloTimeFilterTool" );
  declareProperty( "MBTSTimeFilterTool", m_mbtsfilter, "The instance of the MBTSTimeFilterTool" );
}

StatusCode CollisionFilterAlg::initialize() 
{
  ATH_MSG_DEBUG ("initialize()");

  /// Retrieve the CaloTimeFilter tool using the ToolHandles
  if (m_doCaloTimeFilter) {
    ATH_CHECK(m_calofilter.retrieve());
  }
  ATH_CHECK(m_LArTimeKey.initialize(m_doCaloTimeFilter));

  /// Retrieve the mbts timefilter tool using the ToolHandles
  if (m_doMBTSTimeFilter) {
    ATH_CHECK(m_mbtsfilter.retrieve());
  }
  ATH_CHECK(m_MBTS_key.initialize(m_doMBTSTimeFilter));

  ATH_MSG_DEBUG ("initialize() successful");

  return StatusCode::SUCCESS;
}


StatusCode 
CollisionFilterAlg::execute(const EventContext& ctx) const 
{
  StatusCode sc = StatusCode::SUCCESS;

  ATH_MSG_DEBUG ("execute()");

  if (m_counter % m_prescale == 0)
    ATH_MSG_DEBUG ("Now processing event : " << m_counter);
  ++m_counter;

  //////////////////////////////////////////////

  bool pass(false);
  
  TimingFilterInformation MBTS;
  TimingFilterInformation Calo;
 
  //////////////////////////////////////////////  

  if (m_doCaloTimeFilter) {
    sc = m_calofilter->getTimeDifference(Calo, m_LArTimeKey, ctx);
    if ( sc.isFailure() ) {
      ATH_MSG_WARNING (m_calofilter.propertyName() << ": Failed to execute tool " << m_calofilter.type());
      return StatusCode::SUCCESS;
    }
    if (Calo.passCut) {
      ATH_MSG_DEBUG ("Event accepted by CaloTimeFilter.");
      ++m_caloaccept;
    } else 
      ATH_MSG_DEBUG ("Event rejected by CaloTimeFilter.");
  }

  //////////////////////////////////////////////  

  if (m_doMBTSTimeFilter) {
    sc = m_mbtsfilter->getTimeDifference(MBTS, m_MBTS_key,ctx);
    if ( sc.isFailure() ) {
      ATH_MSG_WARNING (m_mbtsfilter.propertyName() << ": Failed to execute tool " << m_mbtsfilter.type());
      return StatusCode::SUCCESS;
    }
    if (MBTS.passCut) { 
      ATH_MSG_DEBUG ("Event accepted by MBTSTimeFilter.");
      ++m_mbtsaccept;
    } else
      ATH_MSG_DEBUG ("Event rejected by MBTSTimeFilter.");
  }

  //////////////////////////////////////////////  

  if (m_doCaloTimeFilter) pass = pass || Calo.passCut; 
  if (m_doMBTSTimeFilter) pass = pass || MBTS.passCut;
  if (m_doMBTSTimeFilter || m_doCaloTimeFilter) {
    ATH_MSG_DEBUG ("Event accepted as collision ? " << pass);
    setFilterPassed (pass,ctx); // This skips the execution of following algs for this event
  }

  if (m_doCaloTimeFilter && m_doMBTSTimeFilter)
    if (Calo.passCut && MBTS.passCut) ++m_overlap;

  //////////////////////////////////////////////  

  ATH_MSG_DEBUG ("execute() successful");

  return StatusCode::SUCCESS;
}


StatusCode CollisionFilterAlg::finalize() 
{
  ATH_MSG_DEBUG ("finalize()");

  if (m_doCaloTimeFilter) ATH_MSG_INFO ("CaloTimeFilter     : Processed " << m_counter << " events, accepted " << m_caloaccept << " events.");
  if (m_doMBTSTimeFilter) ATH_MSG_INFO ("MBTSTimeFilter     : Processed " << m_counter << " events, accepted " << m_mbtsaccept << " events.");

  if (m_doCaloTimeFilter && m_doMBTSTimeFilter)
    ATH_MSG_INFO ("Overlapping accept : " << m_overlap << " events.");

  return StatusCode::SUCCESS;
}


