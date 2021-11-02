/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MergeTruthJetsTool.h"

#include "AthenaKernel/errorcheck.h"

#include "GaudiKernel/SystemOfUnits.h"

MergeTruthJetsTool::MergeTruthJetsTool(const std::string& type,
                                       const std::string& name,
                                       const IInterface* parent)
  : PileUpToolBase(type, name, parent)
{
}

StatusCode MergeTruthJetsTool::initialize()
{
  ATH_MSG_DEBUG ( "Initializing " << name());
  ATH_CHECK(m_pMergeSvc.retrieve());
  return StatusCode::SUCCESS;
}

StatusCode MergeTruthJetsTool::prepareEvent(const EventContext& /*ctx*/, unsigned int nInputEvents)
{
  ATH_MSG_VERBOSE ( "prepareEvent()" );
  ATH_MSG_DEBUG ( "prepareEvent: there are " << nInputEvents << " subevents in this event." );
  m_first_event = true;

  m_inTimeOutputJetContainer = std::make_unique< xAOD::JetContainer >();
  m_inTimeOutputJetAuxContainer = std::make_unique< xAOD::JetAuxContainer >();
  m_inTimeOutputJetContainer->setStore( m_inTimeOutputJetAuxContainer.get() );

  m_outOfTimeOutputJetContainer = std::make_unique< xAOD::JetContainer >();
  m_outOfTimeOutputJetAuxContainer = std::make_unique< xAOD::JetAuxContainer >();
  m_outOfTimeOutputJetContainer->setStore( m_outOfTimeOutputJetAuxContainer.get() );

  return StatusCode::SUCCESS;
}

StatusCode MergeTruthJetsTool::processBunchXing(int bunchXing,
                                                SubEventIterator bSubEvents,
                                                SubEventIterator eSubEvents)
{
  ATH_MSG_VERBOSE ( "processBunchXing()" );
  SubEventIterator iEvt(bSubEvents);
  while (iEvt != eSubEvents) {
    const xAOD::JetContainer* inputJetContainer(nullptr);
    if (m_pMergeSvc->retrieveSingleSubEvtData(m_inputJetCollKey, inputJetContainer,
                                              bunchXing, iEvt).isSuccess()) {
      ATH_MSG_VERBOSE("Found an xAOD::JetContainer in storeGate.");
      if ( !inputJetContainer ) {
        ATH_MSG_ERROR("Unable to retrieve input jet container: " << m_inputJetCollKey);
        return StatusCode::FAILURE;
      }
      ATH_MSG_DEBUG ( "processBunchXing: bunch Crossing = " << bunchXing << " JetContainer size = " << inputJetContainer->size());
      const int eventNumber{iEvt->index()};
      if (bunchXing==0) {
        if (m_first_event) {//FIXME this may not be robust in the case that there is no TruthJet container from the signal event.
          m_signal_max_pT = this->processJetContainer(&(*inputJetContainer), nullptr, 0.0, 0.0, eventNumber);
          ATH_MSG_DEBUG ( "Setting m_signal_max_pT = " << m_signal_max_pT);
          if(m_includeSignalJets) {
            (void)this->processJetContainer(&(*inputJetContainer), m_inTimeOutputJetContainer.get(), m_inTimePtCut, 0.0, eventNumber);
          }
          else {
            ATH_MSG_VERBOSE ( "Don't include signal events in output Truth Jet Containers.");
          }
          m_first_event=false;
          ++iEvt;
          continue;
        }
        const double pileup_this_pT=this->processJetContainer(&(*inputJetContainer), m_inTimeOutputJetContainer.get(), m_inTimePtCut, 0.0, eventNumber);
        ATH_MSG_VERBOSE ( "highest jet pT in the current background event = " << pileup_this_pT);
        if (pileup_this_pT>m_pileup_max_pT) m_pileup_max_pT=pileup_this_pT;
        ATH_MSG_DEBUG ( "highest in-time background jet pT so far = " << m_pileup_max_pT);
      }
      else {
        const float timeOfBCID(static_cast<float>(iEvt->time()));
        (void)this->processJetContainer(&(*inputJetContainer), m_outOfTimeOutputJetContainer.get(), m_outOfTimePtCut, timeOfBCID, eventNumber);
      }
    }
    else {
      ATH_MSG_DEBUG ( "processBunchXing: No JetContainers found." );
    }
    ++iEvt;
  }
  //signal is always the first event, so even if we didn't see
  //anything should set this to false here.
  if(m_first_event) {m_first_event=false;}
  return StatusCode::SUCCESS;
}

StatusCode MergeTruthJetsTool::mergeEvent(const EventContext& /*ctx*/)
{
  ATH_MSG_VERBOSE ( "mergeEvent" );

  // Veto event when m_pileup_max_pT>m_signal_max_pT
  if (m_activateFilter && m_pileup_max_pT>m_signal_max_pT) {
    ATH_MSG_INFO ( "Highest pT Jet in Hard-scatter event = " << m_signal_max_pT
                   << ", highest pT jet in a background event = " << m_pileup_max_pT
                   << ". Therefore filtering this hard-scatter event." );
    m_filterPassed = false;
  }

  ATH_CHECK( evtStore()->record( std::move( m_inTimeOutputJetContainer ),
                                 m_inTimeOutputJetCollKey ) );
  ATH_CHECK( evtStore()->record( std::move( m_inTimeOutputJetAuxContainer
                                            ), m_inTimeOutputJetCollKey + "Aux." ) );

  ATH_CHECK( evtStore()->record( std::move( m_outOfTimeOutputJetContainer
                                            ), m_outOfTimeOutputJetCollKey ) );
  ATH_CHECK( evtStore()->record( std::move(
                                           m_outOfTimeOutputJetAuxContainer ), m_outOfTimeOutputJetCollKey + "Aux."
                                 ) );

  return StatusCode::SUCCESS;
}

StatusCode MergeTruthJetsTool::processAllSubEvents(const EventContext& /*ctx*/)
{
  ATH_MSG_VERBOSE ( "processAllSubEvents()" );

  m_first_event = true;

  m_inTimeOutputJetContainer = std::make_unique< xAOD::JetContainer >();
  m_inTimeOutputJetAuxContainer = std::make_unique< xAOD::JetAuxContainer >();
  m_inTimeOutputJetContainer->setStore( m_inTimeOutputJetAuxContainer.get() );

  m_outOfTimeOutputJetContainer = std::make_unique< xAOD::JetContainer >();
  m_outOfTimeOutputJetAuxContainer = std::make_unique< xAOD::JetAuxContainer >();
  m_outOfTimeOutputJetContainer->setStore( m_outOfTimeOutputJetAuxContainer.get() );

  typedef PileUpMergeSvc::TimedList<xAOD::JetContainer>::type TruthJetList;
  TruthJetList truthList;
  if ( (m_pMergeSvc->retrieveSubEvtsData(m_inputJetCollKey, truthList)).isSuccess() ) {
    if (!truthList.empty()) {
      //now merge all collections into one
      TruthJetList::const_iterator jetColl_iter(truthList.begin());
      const TruthJetList::const_iterator endOfJetColls(truthList.end());
      while (jetColl_iter!=endOfJetColls) {
        const int eventNumber = (jetColl_iter)->first.index();
        //FIXME we are forced to do a deep copy
        if (static_cast<int>((jetColl_iter)->first.time())==0) {
          if (m_first_event) {
            //FIXME this may not be robust in the case that there is no TruthJet container from the signal event.
            m_signal_max_pT = this->processJetContainer(&(*((jetColl_iter)->second)), nullptr, 0.0, 0.0, eventNumber);
            if(m_includeSignalJets) {
              (void)this->processJetContainer(&(*((jetColl_iter)->second)), m_inTimeOutputJetContainer.get(), m_inTimePtCut, 0.0, eventNumber);
            }
            else {
              ATH_MSG_VERBOSE ( "Don't include signal events in output Truth Jet Containers.");
            }
            m_first_event=false;
            ++jetColl_iter;
            continue;
          }
          const double pileup_this_pT=this->processJetContainer(&(*((jetColl_iter)->second)), m_inTimeOutputJetContainer.get(), m_inTimePtCut, 0.0, eventNumber);
          ATH_MSG_VERBOSE ( "highest jet pT in the current background event = " << pileup_this_pT);
          if (pileup_this_pT>m_pileup_max_pT) m_pileup_max_pT=pileup_this_pT;
          ATH_MSG_DEBUG ( "highest in-time background jet pT so far = " << m_pileup_max_pT);
        }
        else {
          const float timeOfBCID(static_cast<float>((jetColl_iter)->first.time()));
          (void)this->processJetContainer(&(*((jetColl_iter)->second)), m_outOfTimeOutputJetContainer.get(), m_outOfTimePtCut, timeOfBCID, eventNumber);
        }
        //signal is always the first event, so if the first event
        //wasn't in-time, then the signal collection was missing and
        //we should skip further checks.
        if(m_first_event) {m_first_event=false;}
        ++jetColl_iter;
      }
    }
    else {
      ATH_MSG_DEBUG ( "processAllSubEvents: TruthJetList is empty" );
    }
  }
  else {
    ATH_MSG_ERROR ( "processAllSubEvents: Can not find TruthJetList" );
  }
  // Veto event when m_pileup_max_pT>m_signal_max_pT
  if (m_activateFilter && m_pileup_max_pT>m_signal_max_pT) {
    ATH_MSG_INFO ( "Highest pT Jet in Hard-scatter event = " << m_signal_max_pT
                   << ", highest pT jet in a background event = " << m_pileup_max_pT
                   << ". Therefore filtering this hard-scatter event." );
    m_filterPassed = false;
  }
  ATH_CHECK( evtStore()->record( std::move( m_inTimeOutputJetContainer ),
                                 m_inTimeOutputJetCollKey ) );
  ATH_CHECK( evtStore()->record( std::move( m_inTimeOutputJetAuxContainer
                                            ), m_inTimeOutputJetCollKey + "Aux." ) );

  ATH_CHECK( evtStore()->record( std::move( m_outOfTimeOutputJetContainer
                                            ), m_outOfTimeOutputJetCollKey ) );
  ATH_CHECK( evtStore()->record( std::move(
                                           m_outOfTimeOutputJetAuxContainer ), m_outOfTimeOutputJetCollKey + "Aux."
                                 ) );

  return StatusCode::SUCCESS;
}

//use a float for timeOfBCID as Jet moments are stored as floats.
double MergeTruthJetsTool::processJetContainer(const xAOD::JetContainer* inputJetContainer, xAOD::JetContainer *outputJetContainer, const double& ptCut, const float& timeOfBCID, int eventNumber)
{
  const static SG::AuxElement::Accessor< float > timingAccessor("Timing");
  const static SG::AuxElement::Accessor< int > eventNumberAccessor("pileupEventNumber");
  double max_pT=-1.;
  for (const xAOD::Jet *origTruthJet : *inputJetContainer) {
    try {
      if (!origTruthJet || origTruthJet->pt()<ptCut) {
        ATH_MSG_VERBOSE( "processJetContainer: Jet with pT = " << origTruthJet->pt() << " GeV failed ptCut of " << ptCut << "GeV." );
        continue;
      }
      if (max_pT<origTruthJet->pt()) max_pT=origTruthJet->pt();
      if (!outputJetContainer) continue;
      ATH_MSG_VERBOSE( "processJetContainer: Jet with pT = " << origTruthJet->pt() << " GeV passed ptCut of " << ptCut << "GeV." );
    }
    catch (...) {
      ATH_MSG_ERROR ( "Failed to find Truth jet pT for Jet in a BCID at time = " << timeOfBCID );
    }
    xAOD::Jet* outputTruthJet = new xAOD::Jet();
    outputJetContainer->push_back(outputTruthJet);
    *outputTruthJet = *origTruthJet; // deep-copy
    if (timingAccessor.isAvailable(*origTruthJet)) {
      timingAccessor(*outputTruthJet) = timeOfBCID + timingAccessor(*origTruthJet);
    } else {
      timingAccessor(*outputTruthJet) = timeOfBCID;
    }
    // add the pile-up event number
    eventNumberAccessor(*outputTruthJet) = eventNumber;
  }
  return max_pT;
}
