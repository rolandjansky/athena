/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MergeGenericMuonSimHitCollTool.h"

#include "PileUpTools/PileUpMergeSvc.h"
#include "StoreGate/StoreGateSvc.h"

MergeGenericMuonSimHitCollTool::MergeGenericMuonSimHitCollTool(const std::string& type,
                                                               const std::string& name,
                                                               const IInterface* parent) :
  PileUpToolBase(type, name, parent),
  m_pMergeSvc("PileUpMergeSvc", name),
  m_firstSubEvent(true)
{
  declareInterface<IPileUpTool>(this);
  declareProperty("SimHitContainerNames",  m_SimHitContainerNamesProp);
}

StatusCode MergeGenericMuonSimHitCollTool::initialize() {
  ATH_MSG_VERBOSE ( "initialize()" );
  // list of CaloCalibrationHit containers
  if (m_SimHitContainerNamesProp.value().empty()) {
    m_SimHitContainerNames.reserve(2);
    m_SimHitContainerNames.push_back("MicromegasSensitiveDetector");
    m_SimHitContainerNames.push_back("sTGCSensitiveDetector");
  }
  else {
    m_SimHitContainerNames = m_SimHitContainerNamesProp.value();
  }
  m_outputContainers.reserve(m_SimHitContainerNames.size());
  return StatusCode::SUCCESS;
}

StatusCode MergeGenericMuonSimHitCollTool::prepareEvent(unsigned int nInputEvents) {
  ATH_MSG_DEBUG ( "Calling prepareEvent(): " << name() << " - package version " << PACKAGE_VERSION );
  ATH_MSG_DEBUG( "prepareEvent: there are " << nInputEvents << " subevents in this event.");
  m_firstSubEvent = true;
  //clean up pointers from the last event
  m_outputContainers.clear();
  for (unsigned int iHitContainer=0;iHitContainer<m_SimHitContainerNames.size(); ++iHitContainer) {
    m_outputContainers.push_back(NULL);
  }
  return StatusCode::SUCCESS;
}
StatusCode MergeGenericMuonSimHitCollTool::processBunchXing(int bunchXing,
                                                            SubEventIterator bSubEvents,
                                                            SubEventIterator eSubEvents)
{
  StatusCode sc(StatusCode::SUCCESS);
  //if this is the first SubEvent for this Event then create the hitContainers;
  ATH_MSG_VERBOSE ( "processBunchXing()" );
  SubEventIterator iEvt(bSubEvents);
  while (iEvt != eSubEvents) {
    StoreGateSvc& seStore(*iEvt->ptr()->evtStore());
    // loop over containers
    for (unsigned int iHitContainer=0;iHitContainer<m_SimHitContainerNames.size();++iHitContainer) {
      ATH_MSG_VERBOSE ( " Bunch Crossing: " <<bunchXing << ". Process GenericMuonSimHitCollection " << m_SimHitContainerNames[iHitContainer] );
      const GenericMuonSimHitCollection* hitCont;
      // if container not there, do nothing
      if (seStore.contains<GenericMuonSimHitCollection>(m_SimHitContainerNames[iHitContainer])) {
        if( !seStore.retrieve(hitCont, m_SimHitContainerNames[iHitContainer]).isSuccess()) {
          ATH_MSG_ERROR ( " Failed to retrieve GenericMuonSimHitCollection called " << m_SimHitContainerNames[iHitContainer] );
        }
      }
      else {
        ATH_MSG_VERBOSE ( " Cannot find GenericMuonSimHitCollection called " << m_SimHitContainerNames[iHitContainer] );
        continue;
      }
      //if this is the first SubEvent for this Event then create the hitContainers;
      if(m_firstSubEvent) {
        m_outputContainers[iHitContainer] = new GenericMuonSimHitCollection(m_SimHitContainerNames[iHitContainer]);
      }
      const double timeOfBCID(static_cast<double>(iEvt->time()));
      this->processGenericMuonSimHitColl(hitCont, m_outputContainers[iHitContainer], timeOfBCID);
    }
    ++iEvt;
    m_firstSubEvent=false;
  }
  return sc;
}

StatusCode MergeGenericMuonSimHitCollTool::mergeEvent() {
  for (unsigned int iHitContainer=0;iHitContainer<m_SimHitContainerNames.size();++iHitContainer) {
    if ( m_outputContainers[iHitContainer]==0 ) { continue; } //don't bother recording unused containers!
    if (!(evtStore()->record(m_outputContainers[iHitContainer],m_SimHitContainerNames[iHitContainer]).isSuccess())) {
      ATH_MSG_ERROR ( " Cannot record new GenericMuonSimHitCollection in overlayed event " );
      return StatusCode::FAILURE;
    }
  }   // end of loop over the various types of calo calibration hits
  return StatusCode::SUCCESS;
}

StatusCode MergeGenericMuonSimHitCollTool::processAllSubEvents()
{
  ATH_MSG_VERBOSE ( "processAllSubEvents()" );
  if(!m_pMergeSvc) {
    if (!(m_pMergeSvc.retrieve()).isSuccess()) {
      ATH_MSG_FATAL ( "processAllSubEvents: Could not find PileUpMergeSvc" );
      return StatusCode::FAILURE;
    }
  }

  // loop over containers
  for (unsigned int iHitContainer=0;iHitContainer<m_SimHitContainerNames.size();iHitContainer++) {

    ATH_MSG_DEBUG(" Process CalibrationHit container " << m_SimHitContainerNames[iHitContainer]);
    typedef PileUpMergeSvc::TimedList<GenericMuonSimHitCollection>::type SimHitList;
    SimHitList simHitList;
    if ( (m_pMergeSvc->retrieveSubEvtsData(m_SimHitContainerNames[iHitContainer], simHitList)).isSuccess() ) {
      GenericMuonSimHitCollection* newContainer = new GenericMuonSimHitCollection(m_SimHitContainerNames[iHitContainer]);
      if (!simHitList.empty()) {
        //now merge all collections into one
        SimHitList::const_iterator simHitColl_iter(simHitList.begin());
        const SimHitList::const_iterator endOfSimHitColls(simHitList.end());
        while (simHitColl_iter!=endOfSimHitColls) {
          const double timeOfBCID(static_cast<double>((simHitColl_iter)->first.time()));
          this->processGenericMuonSimHitColl(&(*((simHitColl_iter)->second)), m_outputContainers[iHitContainer], timeOfBCID);
        }
      }
      // record new container in overlayed event
      if (!(evtStore()->record(newContainer,m_SimHitContainerNames[iHitContainer]).isSuccess())) {
        ATH_MSG_ERROR(" Cannot record new GenericMuonSimHitCollection in overlayed event ");
        delete newContainer;
        return StatusCode::FAILURE;
      }
    }
  }   // end of loop over the various types of calo calibration hits
  return StatusCode::SUCCESS;
}

void MergeGenericMuonSimHitCollTool::processGenericMuonSimHitColl(const GenericMuonSimHitCollection *inputCollection, GenericMuonSimHitCollection *outputCollection, const double& timeOfBCID)
{
  for (GenericMuonSimHitCollection::const_iterator simhitIter = inputCollection->begin(); simhitIter != inputCollection->end(); ++simhitIter) {
    const int idHit = simhitIter->GenericId();
    outputCollection->Emplace(idHit, simhitIter->globalTime()+timeOfBCID, simhitIter->globalpreTime()+timeOfBCID, simhitIter->globalPosition(), simhitIter->localPosition(), simhitIter->globalPrePosition(),simhitIter->localPrePosition(), simhitIter->particleEncoding(),simhitIter->kineticEnergy(), simhitIter->globalDirection(),simhitIter->depositEnergy(), simhitIter->StepLength(),simhitIter->trackNumber() );
  }
  return;
}
