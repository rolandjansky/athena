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

using xAOD::EventInfo;

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
  ATH_CHECK(m_knownBadFEBsVecKey.initialize() );
  ATH_CHECK(m_knownMNBFEBsVecKey.initialize() );
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
  
  
  SG::ReadCondHandle<LArBadFebCont> badHdl(m_knownBadFEBsVecKey);
  const LArBadFebCont* badCont=*badHdl;
  std::set<unsigned int> bf;
  if(badCont) {
     for(LArBadFebCont::BadChanVec::const_iterator i = badCont->begin(); i!=badCont->end(); i++) {
        bf.insert(i->first);
     }
     if(bf.size() == 0) {
        ATH_MSG_WARNING("List of known Bad FEBs empty !? ");
     }
  }
  const std::set<unsigned int> knownBadFEBs(bf);
  ATH_MSG_DEBUG("Number of known Bad FEBs: "<<knownBadFEBs.size());

  SG::ReadCondHandle<LArBadFebCont> MNBHdl(m_knownMNBFEBsVecKey);
  std::vector<HWIdentifier> MNBfeb;
  const LArBadFebCont* MNBCont=*MNBHdl;
  if(MNBCont) {
     for(LArBadFebCont::BadChanVec::const_iterator i = MNBCont->begin(); i!=MNBCont->end(); i++) {
        MNBfeb.push_back(HWIdentifier(i->first));
     } 
     if(MNBfeb.size() == 0) {
        ATH_MSG_WARNING("List of known MNB FEBs empty !? ");
     } 
  }
  const std::vector<HWIdentifier> knownMNBFEBs(MNBfeb);
  ATH_MSG_INFO("Number of known MNB FEBs: "<<knownMNBFEBs.size());



  SG::WriteHandle<LArNoisyROSummary> noisyRO(m_outputKey, ctx);
  ATH_CHECK(noisyRO.record(m_noisyROTool->process(cellContainer.cptr(), &knownBadFEBs, &knownMNBFEBs)));


  bool badFEBFlag=noisyRO->BadFEBFlaggedPartitions();
  bool badFEBFlag_W=noisyRO->BadFEB_WFlaggedPartitions();
  bool badSaturatedTightCut=noisyRO->SatTightFlaggedPartitions();
  bool MNBLooseCut=noisyRO->MNBLooseFlaggedPartitions();
  bool MNBTightCut=noisyRO->MNBTightFlaggedPartitions();
  
  if ( badFEBFlag || badFEBFlag_W || badSaturatedTightCut || MNBLooseCut || MNBTightCut) 
  {
    // retrieve EventInfo
    const xAOD::EventInfo* eventInfo_c=0;
    StatusCode sc = evtStore()->retrieve(eventInfo_c);
    if (sc.isFailure()) 
    {
      ATH_MSG_WARNING( " cannot retrieve EventInfo, will not set LAr bit information "  );
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

    if (failSetWARN) ATH_MSG_WARNING( "Failure during EventInfo::setEventErrorState(EventInfo::LAR,EventInfo::WARNING)"  );
    if (failSetWARNREASON) ATH_MSG_WARNING( "Failure during setEventFlagBit(EventInfo::LAr,...)"  );
  
  }

  return StatusCode::SUCCESS;
}

StatusCode LArNoisyROAlg::finalize()
{  
  return StatusCode::SUCCESS;
}

