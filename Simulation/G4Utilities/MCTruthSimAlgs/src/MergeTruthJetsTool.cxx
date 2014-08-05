/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MergeTruthJetsTool.h"

#include "xAODJet/JetAuxContainer.h"
#include "PileUpTools/PileUpMergeSvc.h"
#include "StoreGate/StoreGateSvc.h"

#include "GaudiKernel/SystemOfUnits.h"

MergeTruthJetsTool::MergeTruthJetsTool(const std::string& type,
                                       const std::string& name,
                                       const IInterface* parent) :
  PileUpToolBase(type, name, parent),m_intool(""),
  m_pMergeSvc("PileUpMergeSvc", name),
  m_inTimeOutputJetContainer(NULL),
  m_outOfTimeOutputJetContainer(NULL)
{
  declareInterface<IPileUpTool>(this);
  declareProperty("InputTool", m_intool);
  declareProperty("InputTruthJetCollKey", m_inputJetCollKey="AntiKt4TruthJets");
  declareProperty("InTimeOutputTruthJetCollKey", m_inTimeOutputJetCollKey="InTimeAntiKt4TruthJets");
  declareProperty("OutOfTimeTruthJetCollKey", m_outOfTimeOutputJetCollKey="OutOfTimeAntiKt4TruthJets");
  declareProperty("InTimePtCut", m_inTimePtCut=10.0*Gaudi::Units::GeV);
  declareProperty("OutOfTimePtCut", m_outOfTimePtCut=15.0*Gaudi::Units::GeV);
}
StatusCode MergeTruthJetsTool::initialize() {
  ATH_MSG_DEBUG ( "Initializing " << name() << " - package version " << PACKAGE_VERSION );
  return StatusCode::SUCCESS;
}
StatusCode MergeTruthJetsTool::prepareEvent(unsigned int nInputEvents) {
  ATH_MSG_VERBOSE ( "prepareEvent()" );
  ATH_MSG_DEBUG ( "prepareEvent: there are " << nInputEvents << " subevents in this event." );
  m_inTimeOutputJetContainer = new xAOD::JetContainer();
  m_inTimeOutputJetContainer->setStore(new xAOD::JetAuxContainer);
  m_outOfTimeOutputJetContainer = new xAOD::JetContainer();
  m_outOfTimeOutputJetContainer->setStore(new xAOD::JetAuxContainer);
  return StatusCode::SUCCESS;
}
StatusCode MergeTruthJetsTool::processBunchXing(int bunchXing,
                                                PileUpEventInfo::SubEvent::const_iterator bSubEvents,
                                                PileUpEventInfo::SubEvent::const_iterator eSubEvents) {
  ATH_MSG_VERBOSE ( "processBunchXing()" );
  PileUpEventInfo::SubEvent::const_iterator iEvt(bSubEvents);
  while (iEvt != eSubEvents) {
    StoreGateSvc& seStore(*iEvt->pSubEvtSG);
    const xAOD::JetContainer* inputJetContainer = 0;
    if (seStore.contains<xAOD::JetContainer>(m_inputJetCollKey)) {
      inputJetContainer = seStore.retrieve<const xAOD::JetContainer>(m_inputJetCollKey);
      if ( inputJetContainer==0 && !m_intool.empty() ) {
        ATH_MSG_DEBUG("Excuting input tool.");
        if ( m_intool->execute() ) { //NEED TO BE ABLE TO SET THE STOREGATE FOR m_intool AT THIS POINT.
          ATH_MSG_WARNING("Input tool execution failed.");
        }
        inputJetContainer = seStore.retrieve<const xAOD::JetContainer>(m_inputJetCollKey);
        if(inputJetContainer == 0) {
          ATH_MSG_ERROR("Unable to retrieve input jet container: " << m_inputJetCollKey);
          return StatusCode::FAILURE;
        }
        ATH_MSG_DEBUG ( "processBunchXing: bunch Crossing = " << bunchXing << " JetContainer size = " << inputJetContainer->size());
        if (bunchXing==0) {
          processJetContainer(&(*inputJetContainer), m_inTimeOutputJetContainer, m_inTimePtCut, 0.0);
        }
        else {
          const float timeOfBCID(static_cast<float>(iEvt->time()));
          processJetContainer(&(*inputJetContainer), m_outOfTimeOutputJetContainer, m_outOfTimePtCut, timeOfBCID);
        }
      }
    } else {
      ATH_MSG_DEBUG ( "processBunchXing: No JetContainers found." );
    }
    ++iEvt;
  }
  return StatusCode::SUCCESS;
}

StatusCode MergeTruthJetsTool::mergeEvent() {
  ATH_MSG_VERBOSE ( "mergeEvent" );
  if(this->record(m_inTimeOutputJetContainer, m_inTimeOutputJetCollKey).isFailure()) { //This call also records the xAOD::JetAuxContainer.
    ATH_MSG_ERROR("mergeEvent: Failed to record InTimeOutputJetContainer");
    return StatusCode::FAILURE;
  }
  else { ATH_MSG_DEBUG ( "mergeEvent: Recorded " << m_inTimeOutputJetCollKey << " JetContainer with "
                         << m_inTimeOutputJetContainer->size() <<" entries." );
  }
  if(this->record(m_outOfTimeOutputJetContainer, m_outOfTimeOutputJetCollKey).isFailure()) { //This call also records the xAOD::JetAuxContainer.
    ATH_MSG_ERROR("mergeEvent: Failed to record OutOfTimeOutputJetContainer");
    return StatusCode::FAILURE;
  }
  else { ATH_MSG_DEBUG ( "mergeEvent: Recorded " << m_outOfTimeOutputJetCollKey << " JetContainer with "
                         << m_outOfTimeOutputJetContainer->size() <<" entries." );
  }
  return StatusCode::SUCCESS;
}

StatusCode MergeTruthJetsTool::record(const xAOD::JetContainer* pjets, std::string jetcontainername) const {
  const xAOD::JetAuxContainer* pjetsaux =
    dynamic_cast<const xAOD::JetAuxContainer*>(pjets->getConstStore());
  ATH_MSG_DEBUG("Check Aux store: " << pjets << " ... " << &pjets->auxbase() << " ... " << pjetsaux );
  if ( pjetsaux == 0 ) {
    ATH_MSG_ERROR("Unable to retrieve Aux container");
    return StatusCode::FAILURE;
  }
  ATH_MSG_VERBOSE("Recording new Jet Aux container.");
  if ( evtStore()->record(pjetsaux, jetcontainername+"Aux.").isFailure() ) {
    ATH_MSG_ERROR("Unable to write new Aux Jet collection to event store: " << jetcontainername);
    return StatusCode::FAILURE;
  }
  ATH_MSG_VERBOSE("Recording new Jet container.");
  if ( evtStore()->record(pjets, jetcontainername).isFailure() ) {
    ATH_MSG_ERROR("Unable to write new Jet collection to event store: " << jetcontainername);
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG("Created new Jet collection in event store: " << jetcontainername);
  return StatusCode::SUCCESS;
}


StatusCode MergeTruthJetsTool::processAllSubEvents()
{
  ATH_MSG_VERBOSE ( "processAllSubEvents()" );
  if(!m_pMergeSvc) {
    if (!(m_pMergeSvc.retrieve()).isSuccess()) {
      ATH_MSG_FATAL ( "processAllSubEvents: Could not find PileUpMergeSvc" );
      return StatusCode::FAILURE;
    }
  }

  m_inTimeOutputJetContainer = new xAOD::JetContainer();
  m_inTimeOutputJetContainer->setStore(new xAOD::JetAuxContainer);
  m_outOfTimeOutputJetContainer = new xAOD::JetContainer();
  m_outOfTimeOutputJetContainer->setStore(new xAOD::JetAuxContainer);

  typedef PileUpMergeSvc::TimedList<xAOD::JetContainer>::type TruthJetList;
  TruthJetList truthList;
  if ( (m_pMergeSvc->retrieveSubEvtsData(m_inputJetCollKey, truthList)).isSuccess() ) {
    if (!truthList.empty()) {

      //now merge all collections into one
      TruthJetList::const_iterator jetColl_iter(truthList.begin());
      const TruthJetList::const_iterator endOfJetColls(truthList.end());
      while (jetColl_iter!=endOfJetColls) {
        //FIXME we are forced to do a deep copy
        if (static_cast<int>((jetColl_iter)->first.time())==0) {
          processJetContainer(&(*((jetColl_iter)->second)), m_inTimeOutputJetContainer, m_inTimePtCut, 0.0);
        }
        else {
          const float timeOfBCID(static_cast<float>((jetColl_iter)->first.time()));
          processJetContainer(&(*((jetColl_iter)->second)), m_outOfTimeOutputJetContainer, m_outOfTimePtCut, timeOfBCID);
        }
        ++jetColl_iter;
      }

    } else ATH_MSG_DEBUG ( "processAllSubEvents: TruthJetList is empty" );
  } else ATH_MSG_ERROR ( "processAllSubEvents: Can not find TruthJetList" );

  if(this->record(m_inTimeOutputJetContainer, m_inTimeOutputJetCollKey).isFailure()) { //This call also records the JetMomentMap.
    ATH_MSG_ERROR ( "processAllSubEvents: Failed to record InTimeOutputJetContainer" );
    return StatusCode::FAILURE;
  }
  else { ATH_MSG_DEBUG ( "processAllSubEvents: Recorded " << m_inTimeOutputJetCollKey << " JetContainer with "
                         << m_inTimeOutputJetContainer->size() <<" entries." );
  }

  if(this->record(m_outOfTimeOutputJetContainer, m_outOfTimeOutputJetCollKey).isFailure()) { //This call also records the JetMomentMap.
    ATH_MSG_ERROR ( "processAllSubEvents: Failed to record OutOfTimeOutputJetContainer" );
    return StatusCode::FAILURE;
  }
  else { ATH_MSG_DEBUG ( "processAllSubEvents: Recorded " << m_outOfTimeOutputJetCollKey << " JetContainer with "
                         << m_outOfTimeOutputJetContainer->size() <<" entries." );
  }

  return StatusCode::SUCCESS;
}

//use a float for timeOfBCID as Jet moments are stored as floats.
void MergeTruthJetsTool::processJetContainer(const xAOD::JetContainer* inputJetContainer, xAOD::JetContainer *outputJetContainer, const double& ptCut, const float& timeOfBCID)
{
  const xAOD::JetContainer::const_iterator endOfJets(inputJetContainer->end());
  for (xAOD::JetContainer::const_iterator jetIter(inputJetContainer->begin()); jetIter != endOfJets; ++jetIter) {
    if (!(*jetIter) || (*jetIter)->pt()<ptCut) {
      ATH_MSG_VERBOSE( "processJetContainer: Jet with pT = " << (*jetIter)->pt() << " GeV failed ptCut of " << ptCut << "GeV." );
      continue;
    }
    ATH_MSG_VERBOSE( "processJetContainer: Jet with pT = " << (*jetIter)->pt() << " GeV passed ptCut of " << ptCut << "GeV." );
    xAOD::Jet* pjet = new xAOD::Jet();
    outputJetContainer->push_back(pjet);
    *pjet = **jetIter;
    //FIXME: Would be nice to check if the Timing attribute is already
    //set and add that time to the BCID time, but I can't see a way of
    //doing that which doesn't involve lots of string comparison, so
    //we will have to live without it.
    outputJetContainer->back()->setAttribute("Timing", timeOfBCID);
  }
  return;
}
