/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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
  ATH_CHECK(m_eventInfoKey.initialize());
  ATH_CHECK(m_knownBadFEBsVecKey.initialize(!m_isMC) );
  ATH_CHECK(m_knownMNBFEBsVecKey.initialize(!m_isMC) );

  return StatusCode::SUCCESS;
}

StatusCode LArNoisyROAlg::execute (const EventContext& ctx) const
{
  SG::ReadHandle<CaloCellContainer> cellContainer(m_CaloCellContainerName, ctx);
  if (!cellContainer.isValid()) { 
    ATH_MSG_WARNING( " Can not retrieve CaloCellContainer: " << m_CaloCellContainerName.key());
    ATH_MSG_WARNING( " Empty object recorded !");

    const std::set<unsigned int> knownBadFEBs;
    const std::vector<HWIdentifier> knownMNBFEBs;
    SG::WriteHandle<LArNoisyROSummary> noisyRO(m_outputKey, ctx);
    ATH_CHECK(noisyRO.record(m_noisyROTool->process(nullptr, &knownBadFEBs, &knownMNBFEBs)));

    return StatusCode::SUCCESS;      
  } 
  
  std::set<unsigned int> bf;
  std::vector<HWIdentifier> MNBfeb;
  if (! m_isMC) {
    SG::ReadCondHandle<LArBadFebCont> badHdl(m_knownBadFEBsVecKey, ctx);
    const LArBadFebCont* badCont=*badHdl;
    if(badCont) {
      for(LArBadFebCont::BadChanVec::const_iterator i = badCont->begin(); i!=badCont->end(); ++i) {
        bf.insert(i->first);
      }
      if(bf.size() == 0) {
        if(m_isMC) {
          ATH_MSG_DEBUG("Empty ist of known Bad FEBs as expected ");
        } else {   
          ATH_MSG_WARNING("List of known Bad FEBs empty !? ");
        }
      }
    }
  
    SG::ReadCondHandle<LArBadFebCont> MNBHdl(m_knownMNBFEBsVecKey, ctx);
    const LArBadFebCont* MNBCont=*MNBHdl;
    if(MNBCont) {
      for(LArBadFebCont::BadChanVec::const_iterator i = MNBCont->begin(); i!=MNBCont->end(); ++i) {
        MNBfeb.push_back(HWIdentifier(i->first));
      } 
      if(MNBfeb.size() == 0) {
        if(m_isMC) {
          ATH_MSG_DEBUG("Empty ist of known Bad FEBs as expected ");
        } else {   
          ATH_MSG_WARNING("List of known MNB FEBs empty !? ");
        }
      } 
    }
  }

  ATH_MSG_DEBUG("Number of known Bad FEBs: "<<bf.size());
  ATH_MSG_DEBUG("Number of known MNB FEBs: "<<MNBfeb.size());



  SG::WriteHandle<LArNoisyROSummary> noisyRO(m_outputKey, ctx);
  ATH_CHECK(noisyRO.record(m_noisyROTool->process(cellContainer.cptr(), &bf, &MNBfeb)));


  bool badFEBFlag=noisyRO->BadFEBFlaggedPartitions();
  bool badFEBFlag_W=noisyRO->BadFEB_WFlaggedPartitions();
  bool badSaturatedTightCut=noisyRO->SatTightFlaggedPartitions();
  bool MNBLooseCut=noisyRO->MNBLooseFlaggedPartitions();
  bool MNBTightCut=noisyRO->MNBTightFlaggedPartitions();
  bool MNBTight_PsVetoCut=noisyRO->MNBTight_PsVetoFlaggedPartitions();
  
  if ( badFEBFlag || badFEBFlag_W || badSaturatedTightCut || MNBLooseCut || MNBTightCut || MNBTight_PsVetoCut) 
  {
    // retrieve EventInfo
    SG::ReadHandle<xAOD::EventInfo> eventInfo (m_eventInfoKey, ctx); 

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

    if ( MNBTight_PsVetoCut ) {
      failSetWARN |=(!eventInfo->updateErrorState(xAOD::EventInfo::LAr,xAOD::EventInfo::Warning));
      // Set reason why event was flagged
      failSetWARNREASON |=(!eventInfo->updateEventFlagBit(xAOD::EventInfo::LAr,LArEventBitInfo::MININOISEBURSTTIGHT_PSVETO));
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

