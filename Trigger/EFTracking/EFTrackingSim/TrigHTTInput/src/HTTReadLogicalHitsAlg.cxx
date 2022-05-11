#include "HTTReadLogicalHitsAlg.h"
#include "TrigHTTObjects/HTTTowerInputHeader.h"
#include "TrigHTTObjects/HTTLogicalEventInputHeader.h"
#include "TrigHTTObjects/HTTLogicalEventOutputHeader.h"


HTTReadLogicalHitsAlg::HTTReadLogicalHitsAlg (const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator){}

StatusCode HTTReadLogicalHitsAlg::initialize()
{
  ATH_CHECK( m_readOutputTool.retrieve());
  m_event=0;  
  return StatusCode::SUCCESS;
}


StatusCode HTTReadLogicalHitsAlg::execute() 
{
  HTTLogicalEventInputHeader  eventInputHeader_1st;
  HTTLogicalEventInputHeader  eventInputHeader_2nd;
  HTTLogicalEventOutputHeader eventOutputHeader;
  bool last = false;
  ATH_CHECK(m_readOutputTool->readData(&eventInputHeader_1st, &eventInputHeader_2nd, &eventOutputHeader, last) );
  if (last) return StatusCode::SUCCESS;
  ATH_MSG_DEBUG (eventInputHeader_1st);
  m_event++;

  return StatusCode::SUCCESS;
}

