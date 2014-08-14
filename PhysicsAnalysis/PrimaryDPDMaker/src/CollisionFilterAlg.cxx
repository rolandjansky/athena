/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "PrimaryDPDMaker/CollisionFilterAlg.h"
#include "PrimaryDPDMaker/CaloTimeFilterTool.h"
#include "PrimaryDPDMaker/MBTSTimeFilterTool.h"


CollisionFilterAlg::CollisionFilterAlg(const std::string& name, ISvcLocator* pSvcLocator) 
 : AthFilterAlgorithm(name, pSvcLocator)
 , m_calofilter("CaloTimeFilterTool")
 , m_mbtsfilter("MBTSTimeFilterTool")
 , m_counter(0)
 , m_caloaccept(0)
 , m_mbtsaccept(0)
 , m_overlap(0)
 , m_doCaloTimeFilter(true)
 , m_doMBTSTimeFilter(true)
{
  declareProperty( "CounterPrescale",    m_prescale = 1000 );
  declareProperty( "CaloTimeFilterTool", m_calofilter, "The instance of the CaloTimeFilterTool" );
  declareProperty( "DoCaloTimeFilter",   m_doCaloTimeFilter = true );
  declareProperty( "MBTSTimeFilterTool", m_mbtsfilter, "The instance of the MBTSTimeFilterTool" );
  declareProperty( "DoMBTSTimeFilter",   m_doMBTSTimeFilter = true );
}


CollisionFilterAlg::~CollisionFilterAlg()
{ 
} 


StatusCode CollisionFilterAlg::initialize() 
{
  ATH_MSG_DEBUG ("initialize()");

  /// Retrieve the CaloTimeFilter tool using the ToolHandles
  if (m_doCaloTimeFilter) {
    if ( m_calofilter.retrieve().isFailure() ) {
      ATH_MSG_FATAL
        (m_calofilter.propertyName() << ": Failed to retrieve tool "
         << m_calofilter.type());
      return StatusCode::FAILURE;
    } else {
      ATH_MSG_DEBUG
        (m_calofilter.propertyName() << ": Retrieved tool " 
         << m_calofilter.type());
    }
  }

  /// Retrieve the mbts timefilter tool using the ToolHandles
  if (m_doMBTSTimeFilter) {
    if ( m_mbtsfilter.retrieve().isFailure() ) {
      ATH_MSG_FATAL
        (m_mbtsfilter.propertyName() << ": Failed to retrieve tool "
         << m_mbtsfilter.type());
      return StatusCode::FAILURE;
    } else {
      ATH_MSG_DEBUG
        (m_mbtsfilter.propertyName() << ": Retrieved tool " 
         << m_mbtsfilter.type());
    }
  }

  ATH_MSG_DEBUG ("initialize() successful");

  return StatusCode::SUCCESS;
}


StatusCode 
CollisionFilterAlg::execute() 
{
  StatusCode sc = StatusCode::SUCCESS;

  ATH_MSG_DEBUG ("execute()");

  if (m_counter % m_prescale == 0)
    ATH_MSG_DEBUG ("Now processing event : " << m_counter);
  m_counter++ ;

  //////////////////////////////////////////////

  bool pass(false), passCalo(false), passMBTS(false);
  double timeDiff(999.), timeA(0.), timeC(0.);
  int countA(0), countC(0);

  //////////////////////////////////////////////  

  if (m_doCaloTimeFilter) {
    sc = m_calofilter->getTimeDifference(passCalo,timeDiff,timeA,timeC,countA,countC);
    if ( sc.isFailure() ) {
      ATH_MSG_WARNING (m_calofilter.propertyName() << ": Failed to execute tool " << m_calofilter.type());
      return StatusCode::SUCCESS;
    }
    if (passCalo) {
      ATH_MSG_DEBUG ("Event accepted by CaloTimeFilter.");
      ++m_caloaccept;
    } else 
      ATH_MSG_DEBUG ("Event rejected by CaloTimeFilter.");
  }

  //////////////////////////////////////////////  

  if (m_doMBTSTimeFilter) {
    sc = m_mbtsfilter->getTimeDifference(passMBTS,timeDiff,timeA,timeC,countA,countC);
    if ( sc.isFailure() ) {
      ATH_MSG_WARNING (m_mbtsfilter.propertyName() << ": Failed to execute tool " << m_mbtsfilter.type());
      return StatusCode::SUCCESS;
    }
    if (passMBTS) { 
      ATH_MSG_DEBUG ("Event accepted by MBTSTimeFilter.");
      ++m_mbtsaccept;
    } else
      ATH_MSG_DEBUG ("Event rejected by MBTSTimeFilter.");
  }

  //////////////////////////////////////////////  

  if (m_doCaloTimeFilter) pass = pass || passCalo; 
  if (m_doMBTSTimeFilter) pass = pass || passMBTS;
  if (m_doMBTSTimeFilter || m_doCaloTimeFilter) {
    ATH_MSG_DEBUG ("Event accepted as collision ? " << pass);
    this->setFilterPassed (pass); // This skips the execution of following algs for this event
  }

  if (m_doCaloTimeFilter && m_doMBTSTimeFilter)
    if (passCalo && passMBTS) ++m_overlap;

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


