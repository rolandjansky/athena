/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "MergeMMSimHitCollTool.h"

#include "PileUpTools/PileUpMergeSvc.h"
#include "StoreGate/StoreGateSvc.h"

MergeMMSimHitCollTool::MergeMMSimHitCollTool(const std::string& type,
                                                               const std::string& name,
                                                               const IInterface* parent) :
  PileUpToolBase(type, name, parent),
  m_pMergeSvc("PileUpMergeSvc", name),
  m_firstSubEvent(true)
{
  declareProperty("SimHitContainerNames",  m_SimHitContainerNamesProp);
}

MergeMMSimHitCollTool::~MergeMMSimHitCollTool() {
}

StatusCode MergeMMSimHitCollTool::initialize() {
  ATH_MSG_VERBOSE ( "initialize()" );
  // list of MMSimHit container
  if (m_SimHitContainerNamesProp.value().empty()) {
    m_SimHitContainerNames = "MicromegasSensitiveDetector";
  }
  else {
    m_SimHitContainerNames = m_SimHitContainerNamesProp.value();
  }
  return StatusCode::SUCCESS;
}

StatusCode MergeMMSimHitCollTool::prepareEvent(unsigned int nInputEvents) {
  ATH_MSG_DEBUG ( "Calling prepareEvent(): " << name() << " - package version " << PACKAGE_VERSION );
  ATH_MSG_DEBUG( "prepareEvent: there are " << nInputEvents << " subevents in this event.");
  m_firstSubEvent = true;
  //clean up pointers from the last event
  m_outputContainers=nullptr;
  return StatusCode::SUCCESS;
}
StatusCode MergeMMSimHitCollTool::processBunchXing(int bunchXing,
                                                            SubEventIterator bSubEvents,
                                                            SubEventIterator eSubEvents)
{
  StatusCode sc(StatusCode::SUCCESS);
  //if this is the first SubEvent for this Event then create the hitContainers;
  ATH_MSG_VERBOSE ( "processBunchXing()" );
  SubEventIterator iEvt(bSubEvents);
  while (iEvt != eSubEvents) {
    StoreGateSvc& seStore(*iEvt->ptr()->evtStore());

    ATH_MSG_VERBOSE ( " Bunch Crossing: " <<bunchXing << ". Process MMSimHitCollection " << m_SimHitContainerNames );
    const MMSimHitCollection* hitCont;
    // if container not there, do nothing
    if (seStore.contains<MMSimHitCollection>(m_SimHitContainerNames)) {
      if( !seStore.retrieve(hitCont, m_SimHitContainerNames).isSuccess()) {
        ATH_MSG_ERROR ( " Failed to retrieve MMSimHitCollection called " << m_SimHitContainerNames );
      }
    }
    else {
      ATH_MSG_VERBOSE ( " Cannot find MMSimHitCollection called " << m_SimHitContainerNames );
      continue;
    }
    //if this is the first SubEvent for this Event then create the hitContainers;
    if(m_firstSubEvent) {
      m_outputContainers = new MMSimHitCollection(m_SimHitContainerNames);
    }
    const double timeOfBCID(static_cast<double>(iEvt->time()));
    this->processMMSimHitColl(hitCont, m_outputContainers, timeOfBCID);

    ++iEvt;
    m_firstSubEvent=false;
  }
  return sc;
}

StatusCode MergeMMSimHitCollTool::mergeEvent() {
  if (!(evtStore()->record(m_outputContainers,m_SimHitContainerNames).isSuccess())) {
    ATH_MSG_ERROR ( " Cannot record new MMSimHitCollection in overlayed event " );
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

StatusCode MergeMMSimHitCollTool::processAllSubEvents()
{
  ATH_MSG_VERBOSE ( "processAllSubEvents()" );
  if(!m_pMergeSvc) {
    if (!(m_pMergeSvc.retrieve()).isSuccess()) {
      ATH_MSG_FATAL ( "processAllSubEvents: Could not find PileUpMergeSvc" );
      return StatusCode::FAILURE;
    }
  }


  ATH_MSG_DEBUG(" Process CalibrationHit container " << m_SimHitContainerNames);
  typedef PileUpMergeSvc::TimedList<MMSimHitCollection>::type SimHitList;
  SimHitList simHitList;
  if ( (m_pMergeSvc->retrieveSubEvtsData(m_SimHitContainerNames, simHitList)).isSuccess() ) {
    MMSimHitCollection* newContainer = new MMSimHitCollection(m_SimHitContainerNames);
    if (!simHitList.empty()) {
      //now merge all collections into one
      SimHitList::const_iterator simHitColl_iter(simHitList.begin());
      const SimHitList::const_iterator endOfSimHitColls(simHitList.end());
      while (simHitColl_iter!=endOfSimHitColls) {
        const double timeOfBCID(static_cast<double>((simHitColl_iter)->first.time()));
        this->processMMSimHitColl(&(*((simHitColl_iter)->second)), m_outputContainers, timeOfBCID);
      }
    }
    // record new container in overlayed event
    if (!(evtStore()->record(newContainer,m_SimHitContainerNames).isSuccess())) {
      ATH_MSG_ERROR(" Cannot record new MMSimHitCollection in overlayed event ");
      delete newContainer;
      return StatusCode::FAILURE;
    }
  }
  return StatusCode::SUCCESS;
}

void MergeMMSimHitCollTool::processMMSimHitColl(const MMSimHitCollection *inputCollection, MMSimHitCollection *outputCollection, const double& timeOfBCID)
{
  for (MMSimHitCollection::const_iterator simhitIter = inputCollection->begin(); simhitIter != inputCollection->end(); ++simhitIter) {
    const int idHit = simhitIter->MMId();
    outputCollection->Emplace(idHit, simhitIter->globalTime()+timeOfBCID, simhitIter->globalPosition(), simhitIter->particleEncoding(),simhitIter->kineticEnergy(), simhitIter->globalDirection(),simhitIter->depositEnergy(), simhitIter->trackNumber() );
  }
  return;
}
