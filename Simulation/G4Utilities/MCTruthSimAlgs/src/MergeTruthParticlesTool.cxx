/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MergeTruthParticlesTool.h"

#include "AthenaKernel/errorcheck.h"
#include "xAODTruth/TruthParticleAuxContainer.h"

#include "GaudiKernel/SystemOfUnits.h"

MergeTruthParticlesTool::MergeTruthParticlesTool(const std::string& type,
                                                 const std::string& name,
                                                 const IInterface* parent)
  : PileUpToolBase(type, name, parent)
{
}

StatusCode MergeTruthParticlesTool::initialize()
{
  ATH_MSG_DEBUG ( "Initializing " << name());
  ATH_CHECK(m_pMergeSvc.retrieve());
  return StatusCode::SUCCESS;
}

StatusCode MergeTruthParticlesTool::prepareEvent(const EventContext& /*ctx*/, unsigned int nInputEvents)
{
  ATH_MSG_VERBOSE ( "prepareEvent()" );
  ATH_MSG_DEBUG ( "prepareEvent: there are " << nInputEvents << " subevents in this event." );
  m_first_event = true;
  m_inTimeOutputTruthParticleContainer = new xAOD::TruthParticleContainer();
  m_inTimeOutputTruthParticleContainer->setStore(new xAOD::TruthParticleAuxContainer);
  return StatusCode::SUCCESS;
}

StatusCode MergeTruthParticlesTool::processBunchXing(int bunchXing,
                                                     SubEventIterator bSubEvents,
                                                     SubEventIterator eSubEvents)
{
  ATH_MSG_VERBOSE ( "processBunchXing()" );
  SubEventIterator iEvt(bSubEvents);
  while (iEvt != eSubEvents) {
    const xAOD::TruthParticleContainer* inputTruthParticleContainer{};
    if (m_pMergeSvc->retrieveSingleSubEvtData(m_inputTruthParticleCollKey, inputTruthParticleContainer,
                                              bunchXing, iEvt).isSuccess()) {
      ATH_MSG_VERBOSE("Found an xAOD::TruthParticleContainer in storeGate.");
      if ( !inputTruthParticleContainer ) {
        ATH_MSG_ERROR("Unable to retrieve input xAOD::TruthParticleContainer: " << m_inputTruthParticleCollKey);
        return StatusCode::FAILURE;
      }
      ATH_MSG_DEBUG ( "processBunchXing: bunch Crossing = " << bunchXing << " xAOD::TruthParticleContainer size = " << inputTruthParticleContainer->size());
      const int eventNumber{iEvt->index()};
      // const float timeOfBCID{static_cast<float>(iEvt->time())};
      if ( !m_first_event || m_includeSignalTruthParticles ) {
        ATH_CHECK(this->processTruthParticleContainer(&(*inputTruthParticleContainer), m_inTimeOutputTruthParticleContainer, eventNumber));
      }
    }
    else {
      ATH_MSG_DEBUG ( "processBunchXing: No TruthParticleContainer found." );
    }
    m_first_event=false;
    ++iEvt;
  }
  //signal is always the first event, so even if we didn't see
  //anything should set this to false here.
  if(m_first_event) {m_first_event=false;}
  return StatusCode::SUCCESS;
}

StatusCode MergeTruthParticlesTool::mergeEvent(const EventContext& /*ctx*/)
{
  ATH_MSG_VERBOSE ( "mergeEvent" );

  if(this->record(m_inTimeOutputTruthParticleContainer, m_inTimeOutputTruthParticleCollKey).isFailure()) { // This call also records the xAOD::TruthParticleAuxContainer.
    ATH_MSG_ERROR("mergeEvent: Failed to record InTimeOutputTruthParticleContainer");
    return StatusCode::FAILURE;
  }
  else {
    ATH_MSG_DEBUG ( "mergeEvent: Recorded " << m_inTimeOutputTruthParticleCollKey << " TruthParticleContainer with "
                    << m_inTimeOutputTruthParticleContainer->size() <<" entries." );
  }
  return StatusCode::SUCCESS;
}

StatusCode MergeTruthParticlesTool::record(const xAOD::TruthParticleContainer* pTruthParticles, const std::string& truthParticleContainerName) const
{
  const xAOD::TruthParticleAuxContainer* pTruthParticlesAux =
    dynamic_cast<const xAOD::TruthParticleAuxContainer*>(pTruthParticles->getConstStore());
  ATH_MSG_DEBUG("Check Aux store: " << pTruthParticles << " ... " << &pTruthParticles->auxbase() << " ... " << pTruthParticlesAux );
  if ( !pTruthParticlesAux ) {
    ATH_MSG_ERROR("Unable to retrieve xAOD::TruthParticleAuxContainer");
    return StatusCode::FAILURE;
  }
  ATH_MSG_VERBOSE("Recording new xAOD::TruthParticleAuxContainer.");
  if ( evtStore()->record(pTruthParticlesAux, truthParticleContainerName+"Aux.").isFailure() ) {
    ATH_MSG_ERROR("Unable to write new xAOD::TruthParticleAuxContainer to event store: " << truthParticleContainerName);
    return StatusCode::FAILURE;
  }
  ATH_MSG_VERBOSE("Recording new xAOD::TruthParticleContainer.");
  if ( evtStore()->record(pTruthParticles, truthParticleContainerName).isFailure() ) {
    ATH_MSG_ERROR("Unable to write new xAOD::TruthParticleContainer to event store: " << truthParticleContainerName);
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG("Created new xAOD::TruthParticleContainer in event store: " << truthParticleContainerName);
  return StatusCode::SUCCESS;
}


StatusCode MergeTruthParticlesTool::processAllSubEvents(const EventContext& /*ctx*/)
{
  ATH_MSG_VERBOSE ( "processAllSubEvents()" );

  m_first_event = true;
  m_inTimeOutputTruthParticleContainer = new xAOD::TruthParticleContainer();
  m_inTimeOutputTruthParticleContainer->setStore(new xAOD::TruthParticleAuxContainer);

  typedef PileUpMergeSvc::TimedList<xAOD::TruthParticleContainer>::type TruthParticleList;
  TruthParticleList truthList;
  if ( (m_pMergeSvc->retrieveSubEvtsData(m_inputTruthParticleCollKey, truthList)).isSuccess() ) {
    if (!truthList.empty()) {
      //now merge all collections into one
      TruthParticleList::const_iterator truthParticleColl_iter(truthList.begin());
      const TruthParticleList::const_iterator endOfTruthParticleColls(truthList.end());
      while (truthParticleColl_iter!=endOfTruthParticleColls) {
        const int eventNumber{static_cast<int>((truthParticleColl_iter)->first.index())}; // casting from long unsigned int to int
        //const float timeOfBCID(static_cast<float>((truthParticleColl_iter)->first.time()));
        if( static_cast<int>((truthParticleColl_iter)->first.time())!=0 || !m_first_event || m_includeSignalTruthParticles ) {
          ATH_CHECK(this->processTruthParticleContainer(&(*((truthParticleColl_iter)->second)), m_inTimeOutputTruthParticleContainer, eventNumber));
        }
        //signal is always the first event, so if the first event
        //wasn't in-time, then the signal collection was missing and
        //we should skip further checks.
        if(m_first_event) {m_first_event=false;}
        ++truthParticleColl_iter;
      }
    }
    else {
      ATH_MSG_DEBUG ( "processAllSubEvents: TruthParticleList is empty" );
    }
  }
  else {
    ATH_MSG_ERROR ( "processAllSubEvents: Can not find TruthParticleList" );
  }

  if( this->record(m_inTimeOutputTruthParticleContainer, m_inTimeOutputTruthParticleCollKey).isFailure() ) { // This call also records the xAOD::TruthParticleAuxContainer.
    ATH_MSG_ERROR ( "processAllSubEvents: Failed to record InTimeOutputTruthParticleContainer" );
    return StatusCode::FAILURE;
  }
  else {
    ATH_MSG_DEBUG ( "processAllSubEvents: Recorded " << m_inTimeOutputTruthParticleCollKey << " xAOD::TruthParticleContainer with "
                    << m_inTimeOutputTruthParticleContainer->size() <<" entries." );
  }

  return StatusCode::SUCCESS;
}

//use a float for timeOfBCID as TruthParticle moments are stored as floats.
StatusCode MergeTruthParticlesTool::processTruthParticleContainer(const xAOD::TruthParticleContainer* inputTruthParticleContainer, xAOD::TruthParticleContainer *outputTruthParticleContainer, int eventNumber)
{
  if (!inputTruthParticleContainer || !outputTruthParticleContainer) { return StatusCode::FAILURE; }

  // Set up decorators
  const static SG::AuxElement::Decorator< unsigned int > originDecorator("classifierParticleOrigin");
  const static SG::AuxElement::Decorator< unsigned int > typeDecorator("classifierParticleType");
  const static SG::AuxElement::Decorator< unsigned int > outcomeDecorator("classifierParticleOutCome");
  const static SG::AuxElement::Decorator< unsigned int > classificationDecorator("Classification");
  const static SG::AuxElement::Decorator< int > parentHadronIDDecorator("parentHadronID");  // check type
  const static SG::AuxElement::Decorator< int > eventNumberDecorator("pileupEventNumber");
  const static SG::AuxElement::Decorator< float > pVzDecorator("PVz");

  const xAOD::TruthParticleContainer::const_iterator endOfTruthParticles(inputTruthParticleContainer->end());
  for (xAOD::TruthParticleContainer::const_iterator truthParticleIter(inputTruthParticleContainer->begin()); truthParticleIter != endOfTruthParticles; ++truthParticleIter) {
    const xAOD::TruthParticle* theParticle = *truthParticleIter;
    xAOD::TruthParticle* xTruthParticle = new xAOD::TruthParticle();
    outputTruthParticleContainer->push_back(xTruthParticle);
    // Fill with numerical content
    xTruthParticle->setPdgId(theParticle->pdgId());
    xTruthParticle->setBarcode(theParticle->barcode());
    xTruthParticle->setStatus(theParticle->status());
    xTruthParticle->setM(theParticle->m());
    xTruthParticle->setPx(theParticle->px());
    xTruthParticle->setPy(theParticle->py());
    xTruthParticle->setPz(theParticle->pz());
    xTruthParticle->setE(theParticle->e());
    // Copy over the decorations if they are available
    if (theParticle->isAvailable<unsigned int>("classifierParticleType")) {
      typeDecorator(*xTruthParticle) = theParticle->auxdata< unsigned int >( "classifierParticleType" );
    } else {typeDecorator(*xTruthParticle) = 0;}
    if (theParticle->isAvailable<unsigned int>("classifierParticleOrigin")) {
      originDecorator(*xTruthParticle) = theParticle->auxdata< unsigned int >( "classifierParticleOrigin" );
    } else {originDecorator(*xTruthParticle) = 0;}
    if (theParticle->isAvailable<unsigned int>("classifierParticleOutCome")) {
      outcomeDecorator(*xTruthParticle) = theParticle->auxdata< unsigned int >( "classifierParticleOutCome" );
    } else {outcomeDecorator(*xTruthParticle) = 0;}
    if (theParticle->isAvailable<unsigned int>("Classification")) {
      classificationDecorator(*xTruthParticle) = theParticle->auxdata< unsigned int >( "Classification" );
    } else {classificationDecorator(*xTruthParticle) = 0;}
    if (theParticle->isAvailable<int>("parentHadronID")) {
      parentHadronIDDecorator(*xTruthParticle) = theParticle->auxdata< int >( "parentHadronID" );
    } else {parentHadronIDDecorator(*xTruthParticle) = 0;}
    if (theParticle->isAvailable<float>("PVz")) {
      pVzDecorator(*xTruthParticle) = theParticle->auxdata< float >( "PVz" );
    } else {pVzDecorator(*xTruthParticle) = 0.f;}
    // add the pile-up event number
    eventNumberDecorator(*xTruthParticle) = eventNumber;
  }
  return StatusCode::SUCCESS;
}
