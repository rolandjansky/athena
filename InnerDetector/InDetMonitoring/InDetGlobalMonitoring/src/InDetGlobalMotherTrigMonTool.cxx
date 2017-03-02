/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** @file InDetGlobalTestMonTool.cxx
 * Implementation of inner detector global hits monitoring tool
 *
 * @author Heidi Sandaker <Heidi.Sandaker@cern.ch> @n
 * Arshak Tonoyan <Arshak.Tonyoan@cern.ch> @n
 * Thomas Burgess <Thomas.Burgess@cern.ch> @n
 * Alex Kastanas <Alex.Kastanas@cern.ch>       
 *
 * $Id: InDetGlobalTestTool.cxx,v 1.7 2008/12/17 17:31:15 sandaker Exp $
 *
 *********************************************************************************/


//Local
#include "InDetGlobalMotherTrigMonTool.h"
//Framework
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/StatusCode.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/TriggerInfo.h"
#include "LWHists/TH1I_LW.h"
#include "LWHists/TH2I_LW.h"

InDetGlobalMotherTrigMonTool::InDetGlobalMotherTrigMonTool(
    const std::string & type, 
    const std::string & name,
    const IInterface* parent)
  : InDetGlobalMotherMonTool(type, name, parent),
    doTrigger(true)
{
  declareProperty("doTrigger",doTrigger);
}

StatusCode InDetGlobalMotherTrigMonTool::initialize(){
  if ( ManagedMonitorToolBase::initialize().isFailure() )
    {
      if ( msgLvl(MSG::DEBUG) ) msg(MSG::DEBUG) << "Could not initialize Monitor tool base!" << endmsg;
      return StatusCode::FAILURE;
    }
  m_triggerNames[0] = "RNDM";
  m_triggerNames[1] = "BPTX";
  m_triggerNames[2] = "L1CAL";
  m_triggerNames[3] = "TGC";
  m_triggerNames[4] = "RPC";
  m_triggerNames[5] = "MBTS";
  m_triggerNames[6] = "COSM";
  m_triggerNames[7] = "Calib";
  return StatusCode::SUCCESS;
}
  
//---------------------------------------------------------
StatusCode InDetGlobalMotherTrigMonTool::CheckTriggers()
{
    m_activeMenuItems.clear();
    const EventInfo * evtInfo;
    if ( evtStore()->contains<EventInfo>("ByteStreamEventInfo") ){
	evtStore()->retrieve(evtInfo, "ByteStreamEventInfo");
	if ( evtInfo->trigger_info() == 0) // Trigger info not available, will not do trigger aware plots
	    return StatusCode::FAILURE;
	std::vector<unsigned int> level1TriggerInfo = evtInfo->trigger_info()->level1TriggerInfo();
	if ( level1TriggerInfo.size() != 24 ) // LVL1 trigger info not of correct size, will not do trigger aware plots
	    return StatusCode::FAILURE;

	for ( int i = 0; i < 8; i++){
	    m_firedTriggers[i] = evtInfo->trigger_info()->level1TriggerType() >> i & 1;
	}    
	for (unsigned int i = 16; i <24; i++ ){
	    for (unsigned int j = 0;j< 32; j++){
		if ( (level1TriggerInfo[i] >> j) & 0x1 )  
		    m_activeMenuItems.push_back(j+(i%8)*32);
	    }
	}
	return StatusCode::SUCCESS;
    }else{
	return StatusCode::FAILURE;
    }
}

StatusCode InDetGlobalMotherTrigMonTool::PrintTriggers()
{
  for ( int i = 0; i < 8; i++){
    if ( msgLvl(MSG::INFO) ) msg(MSG::INFO) << std::setw(7) << m_triggerNames[i] << " : " << m_firedTriggers[i] << endmsg;
  }
  return StatusCode::SUCCESS;
}

bool InDetGlobalMotherTrigMonTool::hasTriggerFired(int trigger)
{
  if ( m_firedTriggers[trigger] == 1 ) return true;
  else return false;
}



StatusCode InDetGlobalMotherTrigMonTool::fillTrigAwareHisto(TH1I_LW * hist, int value = 1)
{
    std::vector<unsigned int>::const_iterator trig_itr = m_activeMenuItems.begin();
    std::vector<unsigned int>::const_iterator trig_itr_end = m_activeMenuItems.end();
    for ( ; trig_itr != trig_itr_end ; ++trig_itr) {
	hist->Fill( (*trig_itr), value );
    }
    return StatusCode::SUCCESS;
}

StatusCode InDetGlobalMotherTrigMonTool::fillTrigAwareHisto(TH2I_LW * hist, int value)
{
    std::vector<unsigned int>::const_iterator trig_itr = m_activeMenuItems.begin();
    std::vector<unsigned int>::const_iterator trig_itr_end = m_activeMenuItems.end();
    for ( ; trig_itr != trig_itr_end ; ++trig_itr) {
	hist->Fill( (*trig_itr), value );
    }
    return StatusCode::SUCCESS;
}
