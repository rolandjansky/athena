/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "LArCellRec/LArNoisyROAlg.h"

#include "CaloEvent/CaloCellContainer.h"
#include "CaloEvent/CaloCell.h"
#include "Identifier/Identifier.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "LArIdentifier/LArOnlineID.h" 
#include "LArTools/LArCablingService.h"
#include "GeoModelInterfaces/IGeoModelSvc.h"
#include "LArRecEvent/LArNoisyROSummary.h"
#include "LArRecEvent/LArEventBitInfo.h"
#include "xAODEventInfo/EventInfo.h"
//#include "xAODEventInfo/EventID.h"

#include <cmath>

using xAOD::EventInfo;

LArNoisyROAlg::LArNoisyROAlg(const std::string &name,ISvcLocator *pSvcLocator):
  AthAlgorithm (name, pSvcLocator), 
  m_event_counter(0),
  m_noisyROTool("LArNoisyROTool",this)
{
  declareProperty( "CaloCellContainer", m_CaloCellContainerName = "AllCalo" );
  declareProperty( "OutputKey", m_outputKey="LArNoisyROSummary");
  declareProperty( "Tool",m_noisyROTool);
}

StatusCode LArNoisyROAlg::initialize()
{
  m_event_counter = 0;

  CHECK(m_noisyROTool.retrieve());
  return StatusCode::SUCCESS;
}

StatusCode LArNoisyROAlg::execute() 
{

  m_event_counter++;

  const CaloCellContainer* cellContainer(0);
  StatusCode sc = evtStore()->retrieve(cellContainer, m_CaloCellContainerName);
  if (sc.isFailure() || !cellContainer ) {
    msg(MSG::WARNING) << " Could not retreive the CaloCellContainer with name " << m_CaloCellContainerName << endreq;
    return StatusCode::RECOVERABLE;
  }

  std::unique_ptr<LArNoisyROSummary> noisyRO=m_noisyROTool->process(cellContainer);


  bool badFEBFlag=noisyRO->BadFEBFlaggedPartitions();
  bool badFEBFlag_W=noisyRO->BadFEB_WFlaggedPartitions();
  bool badSaturatedTightCut=noisyRO->SatTightFlaggedPartitions();

  if ( badFEBFlag || badFEBFlag_W || badSaturatedTightCut ) 
  {
    // retrieve EventInfo
    const xAOD::EventInfo* eventInfo_c=0;
    sc = evtStore()->retrieve(eventInfo_c);
    if (sc.isFailure()) 
    {
      msg(MSG::WARNING) << " cannot retrieve EventInfo, will not set LAr bit information " << endreq;
    }
    xAOD::EventInfo* eventInfo=0;
    if (eventInfo_c)
    {
      eventInfo = const_cast<xAOD::EventInfo*>(eventInfo_c);
    }

    if ( eventInfo )
    {
      if ( badFEBFlag )
      {
	// set warning flag except if the error flag has been already set
	if ( eventInfo->errorState(EventInfo::LAr) != EventInfo::Error )
	{
	  if (!eventInfo->setErrorState(EventInfo::LAr,EventInfo::Warning)) 
	  {
	    msg(MSG::WARNING) << " cannot set error state for LAr " << endreq;
	  }
	}
	// set reason why event was flagged
	if (!eventInfo->setEventFlagBit(EventInfo::LAr,LArEventBitInfo::BADFEBS) )
	{
	    msg(MSG::WARNING) << " cannot set flag bit for LAr " << endreq;
	}
      }

      if ( badFEBFlag_W )
      {
	// set warning flag except if the error flag has been already set
	if ( eventInfo->errorState(EventInfo::LAr) != EventInfo::Error )
	{
	  if (!eventInfo->setErrorState(EventInfo::LAr,EventInfo::Warning)) 
	  {
	    msg(MSG::WARNING) << " cannot set error state for LAr " << endreq;
	  }
	}
	// set reason why event was flagged
	if (!eventInfo->setEventFlagBit(EventInfo::LAr,LArEventBitInfo::BADFEBS_W) )
	{
	    msg(MSG::WARNING) << " cannot set flag bit for LAr " << endreq;
	}
      }

      if ( badSaturatedTightCut )
      {
	//msg(MSG::INFO) << "Too many saturated Q cells (tight) "  << eventInfo->event_ID()->run_number() << " " << eventInfo->event_ID()->event_number() << " " << eventInfo->event_ID()->lumi_block() << endreq;
	// set reason why event is supicious but not the error state
	if (!eventInfo->setEventFlagBit(EventInfo::LAr,LArEventBitInfo::TIGHTSATURATEDQ)) 
	{
	    msg(MSG::WARNING) << " cannot set flag bit for LAr " << endreq;
	}
      }

    }
  }

  CHECK(evtStore()->record(std::move(noisyRO),m_outputKey));

  return StatusCode::SUCCESS;
}

StatusCode LArNoisyROAlg::finalize()
{
  /*
  msg(MSG::INFO) << "List of bad FEBs found in all events " << endreq;
  for ( SG::unordered_map<unsigned int, unsigned int>::const_iterator it = m_badFEB_counters.begin(); it != m_badFEB_counters.end(); it++ )
  {
    msg(MSG::INFO) << "FEB " << it->first << " declared noisy in " << it->second << " events " << endreq; 
  }

  msg(MSG::INFO) << "List of bad preamps found in at least max(2,0.1%) events" << endreq;
  unsigned int cut = static_cast<unsigned int>(0.001*static_cast<float>(m_event_counter));
  if ( cut < 2 ) cut = 2;
  uint64_t PAfactor = 1000000000L;
  for ( std::map<uint64_t, unsigned int>::const_iterator it = m_badPA_counters.begin(); it != m_badPA_counters.end(); it++ )
  {
    if ( it->second > cut ) msg(MSG::INFO) << "Preamplifier " << (it->first)/PAfactor << " of FEB " << (it->first)%PAfactor << " declared noisy in " << it->second << " events " << endreq; 
  }

  msg(MSG::INFO) << "Number of events with too many saturated QFactor cells (Tight cuts): " << m_SaturatedCellTightCutEvents << endreq;
  */
  return StatusCode::SUCCESS;
}

