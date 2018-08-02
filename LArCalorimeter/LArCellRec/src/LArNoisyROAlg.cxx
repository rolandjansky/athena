/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "LArNoisyROAlg.h"

#include "CaloEvent/CaloCellContainer.h"
#include "CaloEvent/CaloCell.h"
#include "Identifier/Identifier.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "LArIdentifier/LArOnlineID.h" 
#include "LArRecEvent/LArEventBitInfo.h"
#include "xAODEventInfo/EventInfo.h"

#include <cmath>

LArNoisyROAlg::LArNoisyROAlg(const std::string &name,ISvcLocator *pSvcLocator):
  AthReentrantAlgorithm (name, pSvcLocator), 
  m_noisyROTool("LArNoisyROTool",this)
{
  declareProperty( "Tool",m_noisyROTool);
}

StatusCode LArNoisyROAlg::initialize() {
  ATH_CHECK(m_noisyROTool.retrieve());
  ATH_CHECK(m_CaloCellContainerName.initialize());
  ATH_CHECK(m_outputKey.initialize());
  return StatusCode::SUCCESS;
}

StatusCode LArNoisyROAlg::execute_r (const EventContext& ctx) const
{
  SG::ReadHandle<CaloCellContainer> cellContainer(m_CaloCellContainerName, ctx);
  if (!cellContainer.isValid()) { 
    ATH_MSG_ERROR( " Can not retrieve CaloCellContainer: "
                   << m_CaloCellContainerName.key());
    return StatusCode::FAILURE;      
  } 
  

  SG::WriteHandle<LArNoisyROSummary> noisyRO(m_outputKey, ctx);
  ATH_CHECK(noisyRO.record(m_noisyROTool->process(cellContainer.cptr())));


  bool badFEBFlag=noisyRO->BadFEBFlaggedPartitions();
  bool badFEBFlag_W=noisyRO->BadFEB_WFlaggedPartitions();
  bool badSaturatedTightCut=noisyRO->SatTightFlaggedPartitions();
  bool MNBLooseCut=noisyRO->MNBLooseFlaggedPartitions();
  bool MNBTightCut=noisyRO->MNBTightFlaggedPartitions();
  
  if ( badFEBFlag || badFEBFlag_W || badSaturatedTightCut || MNBLooseCut || MNBTightCut) 
  {
    // retrieve EventInfo
    SG::ReadHandle<xAOD::EventInfo> eventInfo (m_eventInfoKey); 

    bool failSetWARN=false;
    bool failSetWARNREASON=false;
    // set warning flag except if the error flag has been already set
    if ( eventInfo->errorState(xAOD::EventInfo::LAr) != xAOD::EventInfo::Error) {
      if ( badFEBFlag ) {
	failSetWARN |= (!eventInfo->updateErrorState(xAOD::EventInfo::LAr,xAOD::EventInfo::Warning));
	failSetWARNREASON |= (!eventInfo->updateEventFlagBit(xAOD::EventInfo::LAr,LArEventBitInfo::BADFEBS));
      }//endif badFEBFlag

      if ( badFEBFlag_W ) {
	//Set WARNING Flag
	failSetWARN |=(!eventInfo->updateErrorState(xAOD::EventInfo::LAr,xAOD::EventInfo::Warning));
	// Set reason why event was flagged
	failSetWARNREASON |=(!eventInfo->updateEventFlagBit(xAOD::EventInfo::LAr,LArEventBitInfo::BADFEBS_W));
      }// end if badFEBFlag_W

      if ( badSaturatedTightCut ){
	failSetWARNREASON |= (!eventInfo->updateEventFlagBit(xAOD::EventInfo::LAr,LArEventBitInfo::TIGHTSATURATEDQ));
      }
    }

    if ( MNBTightCut ) {
      failSetWARN |=(!eventInfo->updateErrorState(xAOD::EventInfo::LAr,xAOD::EventInfo::Warning));
      // Set reason why event was flagged
      failSetWARNREASON |=(!eventInfo->updateEventFlagBit(xAOD::EventInfo::LAr,LArEventBitInfo::MININOISEBURSTTIGHT));
    }

    if ( MNBLooseCut ) { //FIXME Tight cut actually implies loose cut too
      failSetWARNREASON |=(!eventInfo->updateEventFlagBit(xAOD::EventInfo::LAr,LArEventBitInfo::MININOISEBURSTLOOSE));
    }

    if (failSetWARN) ATH_MSG_WARNING( "Failure during EventInfo::setEventErrorState(EventInfo::LAR,EventInfo::WARNING)"  );
    if (failSetWARNREASON) ATH_MSG_WARNING( "Failure during setEventFlagBit(EventInfo::LAr,...)"  );
  
  }

  return StatusCode::SUCCESS;
}

StatusCode LArNoisyROAlg::finalize()
{  
  return StatusCode::SUCCESS;
}

