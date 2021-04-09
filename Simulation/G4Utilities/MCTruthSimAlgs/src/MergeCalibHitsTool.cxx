/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MergeCalibHitsTool.h"

#include "CaloSimEvent/CaloCalibrationHit.h"
#include "CaloSimEvent/CaloCalibrationHitContainer.h"

MergeCalibHitsTool::MergeCalibHitsTool(const std::string& type,
                                       const std::string& name,
                                       const IInterface* parent) :
  PileUpToolBase(type, name, parent)
{
}

StatusCode MergeCalibHitsTool::initialize() {
  ATH_MSG_VERBOSE ( "initialize()" );

  ATH_CHECK(m_pMergeSvc.retrieve());

  // list of CaloCalibrationHit containers
  if(m_oldFormat) {
    m_CalibrationHitContainer.reserve(5);
    m_CalibrationHitContainer.push_back("LArCalibrationHitActive");
    m_CalibrationHitContainer.push_back("LArCalibrationHitDeadMaterial");
    m_CalibrationHitContainer.push_back("LArCalibrationHitInactive");
    m_CalibrationHitContainer.push_back("TileCalibrationCellHitCnt");
    m_CalibrationHitContainer.push_back("TileCalibrationDMHitCnt");
  }
  else {
    m_CalibrationHitContainer.reserve(6);
    m_CalibrationHitContainer.push_back("LArCalibrationHitActive");
    m_CalibrationHitContainer.push_back("LArCalibrationHitDeadMaterial");
    m_CalibrationHitContainer.push_back("LArCalibrationHitInactive");
    m_CalibrationHitContainer.push_back("TileCalibHitActiveCell");
    m_CalibrationHitContainer.push_back("TileCalibHitInactiveCell");
    m_CalibrationHitContainer.push_back("TileCalibHitDeadMaterial");
  }
  m_outputContainers.reserve(m_CalibrationHitContainer.size());
  return StatusCode::SUCCESS;
}


StatusCode MergeCalibHitsTool::prepareEvent(const EventContext& /*ctx*/,unsigned int nInputEvents) {
  ATH_MSG_VERBOSE ( "Calling prepareEvent()" );
  ATH_MSG_VERBOSE( "prepareEvent: there are " << nInputEvents << " subevents in this event.");
  m_firstSubEvent = true;
  //clean up pointers from the last event
  m_outputContainers.clear();
  for (unsigned int iHitContainer=0;iHitContainer<m_CalibrationHitContainer.size(); ++iHitContainer) {
    m_outputContainers.push_back(NULL);
  }
  return StatusCode::SUCCESS;
}

StatusCode MergeCalibHitsTool::processBunchXing(int bunchXing,
                                                SubEventIterator bSubEvents,
                                                SubEventIterator eSubEvents)
{
  SubEventIterator iEvt(bSubEvents);
  while (iEvt != eSubEvents) {
    // MergeCalibHitsTool should only write out the hits from the signal event.
    if(!m_firstSubEvent) {
      ATH_MSG_VERBOSE ( " Have already seen a SubEvent marked as Signal! Don't need to look at any others! " );
      break;
    }

    if( iEvt->type()==xAOD::EventInfo_v1::PileUpType::Signal ) {
      ATH_MSG_DEBUG ( " Found the Signal event! " );
    }
    else {
      // this is not the signal event, so we should skip it.
      ++iEvt;
      continue;
    }

    // loop over containers
    for (unsigned int iHitContainer=0;iHitContainer<m_CalibrationHitContainer.size();++iHitContainer) {
      ATH_MSG_VERBOSE ( " Bunch Crossing: " <<bunchXing << ". Process CalibrationHit container " << m_CalibrationHitContainer[iHitContainer] );
      const CaloCalibrationHitContainer* hitCont;
      if (!m_pMergeSvc->retrieveSingleSubEvtData(m_CalibrationHitContainer[iHitContainer], hitCont,
                                                 bunchXing, iEvt).isSuccess()){
        ATH_MSG_VERBOSE("CaloCalibrationHitContainer not found for event key " << m_CalibrationHitContainer[iHitContainer]);
        continue;
      }

      //if this is the first SubEvent for this Event then create the hitContainers;
      if(m_firstSubEvent) {
        m_outputContainers[iHitContainer] = new CaloCalibrationHitContainer(m_CalibrationHitContainer[iHitContainer]);
      }
      // loop over this container, perform a deep brute-force copy
      int nhit=0;
      ATH_MSG_VERBOSE ( " Start loop over calibration hits of signal event " );
      CaloCalibrationHitContainer::const_iterator calibhititer = hitCont->begin();
      CaloCalibrationHitContainer::const_iterator calibhitend  = hitCont->end();
      while(calibhititer != calibhitend) {
        ++nhit;
        m_outputContainers[iHitContainer]->push_back(new CaloCalibrationHit(**calibhititer));
        ++calibhititer;
      }   // end of loop over calibration hits

      ATH_MSG_DEBUG ( " Number of hits found in this " << m_CalibrationHitContainer[iHitContainer]
                      << " container = " << nhit );
    }
    //we must have seen at least one event by now...
    m_firstSubEvent = false;
    ++iEvt;
  }

  return StatusCode::SUCCESS;
}
StatusCode MergeCalibHitsTool::mergeEvent(const EventContext& /*ctx*/) {
  for (unsigned int iHitContainer=0;iHitContainer<m_CalibrationHitContainer.size();++iHitContainer) {
    if ( m_outputContainers[iHitContainer]==0 ) { continue; } //don't bother recording unused containers!
    if (!(evtStore()->record(m_outputContainers[iHitContainer],m_CalibrationHitContainer[iHitContainer]).isSuccess())) {
      ATH_MSG_ERROR ( " Cannot record new calibration hit in overlayed event " );
      return StatusCode::FAILURE;
    }
  }   // end of loop over the various types of calo calibration hits
  return StatusCode::SUCCESS;
}

StatusCode MergeCalibHitsTool::processAllSubEvents(const EventContext& /*ctx*/) {
  ATH_MSG_VERBOSE ( "processAllSubEvents()" );

  // loop over containers
  for (unsigned int iHitContainer=0;iHitContainer<m_CalibrationHitContainer.size();iHitContainer++) {

    ATH_MSG_DEBUG ( " Process CalibrationHit container " << m_CalibrationHitContainer[iHitContainer] );

    const CaloCalibrationHitContainer* hitCont;

    // if container not there from pileupservice, do nothing
    if (!(m_pMergeSvc->retrieveOriginal(m_CalibrationHitContainer[iHitContainer]
                                        ,hitCont).isSuccess())  ||  !hitCont) {
      ATH_MSG_DEBUG ( " Cannot find calibrationHit from PileUp service " );
      continue;
    }

    // create new container for overlayed event
    CaloCalibrationHitContainer* newContainer = new CaloCalibrationHitContainer(m_CalibrationHitContainer[iHitContainer]);

    // loop over this container, perform a deep brute-force copy
    CaloCalibrationHitContainer::const_iterator calibhititer;
    int nhit=0;
    ATH_MSG_DEBUG ( " Start loop over calibration hits of signal event " );
    for(calibhititer=hitCont->begin();
        calibhititer != hitCont->end();calibhititer++) {
      nhit++;
      newContainer->push_back(new CaloCalibrationHit(**calibhititer));
    }   // end of loop over calibration hits

    ATH_MSG_DEBUG ( " Number of hits found in this container " << nhit );

    // record new container in overlayed event
    if (!(evtStore()->record(newContainer,m_CalibrationHitContainer[iHitContainer]).isSuccess())) {
      ATH_MSG_ERROR ( " Cannot record new calibration hit in overlayed event " );
      return StatusCode::FAILURE;
    }

  }   // end of loop over the various types of calo calibration hits

  return StatusCode::SUCCESS;
}
