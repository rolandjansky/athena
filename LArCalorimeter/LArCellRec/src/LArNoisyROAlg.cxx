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
  m_noisyROTool("LArNoisyROTool",this)
{
  declareProperty( "CaloCellContainer", m_CaloCellContainerName = "AllCalo" );
  declareProperty( "OutputKey", m_outputKey="LArNoisyROSummary");
  declareProperty( "Tool",m_noisyROTool);
}

StatusCode LArNoisyROAlg::initialize()
{
  CHECK(m_noisyROTool.retrieve());
  return StatusCode::SUCCESS;
}

StatusCode LArNoisyROAlg::execute() 
{

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
  bool MNBLooseCut=noisyRO->MNBLooseFlaggedPartitions();
  bool MNBTightCut=noisyRO->MNBTightFlaggedPartitions();
  


  if ( badFEBFlag || badFEBFlag_W || badSaturatedTightCut || MNBLooseCut || MNBTightCut) 
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


    bool failSetWARN=false;
    bool failSetWARNREASON=false;
    // set warning flag except if the error flag has been already set
    if ( eventInfo &&  eventInfo->errorState(EventInfo::LAr) != EventInfo::Error) {
      if ( badFEBFlag ) {
	failSetWARN |= (!eventInfo->setErrorState(EventInfo::LAr,EventInfo::Warning));
	failSetWARNREASON |= (!eventInfo->setEventFlagBit(EventInfo::LAr,LArEventBitInfo::BADFEBS));
      }//endif badFEBFlag

      if ( badFEBFlag_W ) {
	//Set WARNING Flag
	failSetWARN |=(!eventInfo->setErrorState(EventInfo::LAr,EventInfo::Warning));
	// Set reason why event was flagged
	failSetWARNREASON |=(!eventInfo->setEventFlagBit(EventInfo::LAr,LArEventBitInfo::BADFEBS_W));
      }// end if badFEBFlag_W

      if ( badSaturatedTightCut ){
	failSetWARNREASON |= (!eventInfo->setEventFlagBit(EventInfo::LAr,LArEventBitInfo::TIGHTSATURATEDQ));
      }
    }

    if ( MNBTightCut ) {
      failSetWARN |=(!eventInfo->setErrorState(EventInfo::LAr,EventInfo::Warning));
      // Set reason why event was flagged
      failSetWARNREASON |=(!eventInfo->setEventFlagBit(EventInfo::LAr,LArEventBitInfo::MININOISEBURSTTIGHT));
    }

    if ( MNBLooseCut ) { //FIXME Tight cut actually implies loose cut too
      failSetWARNREASON |=(!eventInfo->setEventFlagBit(EventInfo::LAr,LArEventBitInfo::MININOISEBURSTLOOSE));
    }

    if (failSetWARN) msg(MSG::WARNING) << "Failure during EventInfo::setEventErrorState(EventInfo::LAR,EventInfo::WARNING)" << endreq;
    if (failSetWARNREASON) msg(MSG::WARNING) << "Failure during setEventFlagBit(EventInfo::LAr,...)" << endreq;
  
  }

  CHECK(evtStore()->record(std::move(noisyRO),m_outputKey));

  return StatusCode::SUCCESS;
}

StatusCode LArNoisyROAlg::finalize()
{  
  return StatusCode::SUCCESS;
}

