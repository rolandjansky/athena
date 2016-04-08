/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MergeTruthJetsTool.h"

#include "AthenaKernel/errorcheck.h"
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
  m_outOfTimeOutputJetContainer(NULL),
  m_first_event(true),
  m_signal_max_pT(-1.),
  m_pileup_max_pT(-1.)
{
  declareInterface<IPileUpTool>(this);
  declareProperty("InputTool", m_intool);
  declareProperty("InputTruthJetCollKey", m_inputJetCollKey="AntiKt4TruthJets");
  declareProperty("InTimeOutputTruthJetCollKey", m_inTimeOutputJetCollKey="InTimeAntiKt4TruthJets");
  declareProperty("OutOfTimeTruthJetCollKey", m_outOfTimeOutputJetCollKey="OutOfTimeAntiKt4TruthJets");
  declareProperty("InTimePtCut", m_inTimePtCut=10.0*Gaudi::Units::GeV);
  declareProperty("OutOfTimePtCut", m_outOfTimePtCut=15.0*Gaudi::Units::GeV);
  declareProperty("VetoOnInTime", m_vetoOnInTime=false);
  declareProperty("IncludeSignalJets", m_includeSignalJets=false, "Include signal jets in the pileup truth jet collections, if they exist");
}

StatusCode MergeTruthJetsTool::initialize()
{
  ATH_MSG_DEBUG ( "Initializing " << name() << " - package version " << PACKAGE_VERSION );
  return StatusCode::SUCCESS;
}

StatusCode MergeTruthJetsTool::prepareEvent(unsigned int nInputEvents)
{
  ATH_MSG_VERBOSE ( "prepareEvent()" );
  ATH_MSG_DEBUG ( "prepareEvent: there are " << nInputEvents << " subevents in this event." );
  m_inTimeOutputJetContainer = new xAOD::JetContainer();
  m_inTimeOutputJetContainer->setStore(new xAOD::JetAuxContainer);
  m_outOfTimeOutputJetContainer = new xAOD::JetContainer();
  m_outOfTimeOutputJetContainer->setStore(new xAOD::JetAuxContainer);
  return StatusCode::SUCCESS;
}

StatusCode MergeTruthJetsTool::processBunchXing(int bunchXing,
                                                SubEventIterator bSubEvents,
                                                SubEventIterator eSubEvents)
{
  ATH_MSG_VERBOSE ( "processBunchXing()" );
  SubEventIterator iEvt(bSubEvents);
  while (iEvt != eSubEvents)
    {
      StoreGateSvc& seStore(*iEvt->ptr()->evtStore());
      const xAOD::JetContainer* inputJetContainer = 0;
      if (seStore.contains<xAOD::JetContainer>(m_inputJetCollKey))
        {
          ATH_MSG_VERBOSE("Found an xAOD::JetContainer in storeGate.");
          inputJetContainer = seStore.retrieve<const xAOD::JetContainer>(m_inputJetCollKey);
          //Back-compatibility with inputs which contain old JetCollections
          if ( inputJetContainer==0 )
            {
              if (!m_intool.empty() )
                {
                  ATH_MSG_VERBOSE("Excuting input tool.");
                  if ( m_intool->execute() ) //NEED TO BE ABLE TO SET THE STOREGATE FOR m_intool AT THIS POINT.
                    {
                      ATH_MSG_WARNING("Input tool execution failed.");
                    }
                  inputJetContainer = seStore.retrieve<const xAOD::JetContainer>(m_inputJetCollKey);
                }
              if(inputJetContainer == 0)
                {
                  ATH_MSG_ERROR("Unable to retrieve input jet container: " << m_inputJetCollKey);
                  return StatusCode::FAILURE;
                }
            }
          ATH_MSG_DEBUG ( "processBunchXing: bunch Crossing = " << bunchXing << " JetContainer size = " << inputJetContainer->size());
          double pileup_this_pT=-1.;
          if (bunchXing==0)
            {
              if (m_first_event)//FIXME this may not be robust in the case that there is no TruthJet container from the signal event.
                {
                  m_signal_max_pT = processJetContainer(&(*inputJetContainer), 0, 0.0, 0.0);
                  ATH_MSG_DEBUG ( "Setting m_signal_max_pT = " << m_signal_max_pT);
                  m_first_event=false;
                  if (!m_includeSignalJets)
                    {
                      ATH_MSG_VERBOSE ( "Don't include signal events in output Truth Jet Containers.");
                      ++iEvt;
                      continue;
                    }
                }
              pileup_this_pT=processJetContainer(&(*inputJetContainer), m_inTimeOutputJetContainer, m_inTimePtCut, 0.0);
            }
          else
            {
              const float timeOfBCID(static_cast<float>(iEvt->time()));
              pileup_this_pT=processJetContainer(&(*inputJetContainer), m_outOfTimeOutputJetContainer, m_outOfTimePtCut, timeOfBCID);
            }
          if (pileup_this_pT>m_pileup_max_pT) m_pileup_max_pT=pileup_this_pT;
        }
      else
        {
          ATH_MSG_DEBUG ( "processBunchXing: No JetContainers found." );
        }
      ++iEvt;
    }
  if(m_first_event) {m_first_event=false;}//signal is always the first event, so even if we didn't see anything should set this to false here.
  return StatusCode::SUCCESS;
}

StatusCode MergeTruthJetsTool::mergeEvent()
{
  ATH_MSG_VERBOSE ( "mergeEvent" );
  if(this->record(m_inTimeOutputJetContainer, m_inTimeOutputJetCollKey).isFailure())
    { //This call also records the xAOD::JetAuxContainer.
      ATH_MSG_ERROR("mergeEvent: Failed to record InTimeOutputJetContainer");
      return StatusCode::FAILURE;
    }
  else
    {
      ATH_MSG_DEBUG ( "mergeEvent: Recorded " << m_inTimeOutputJetCollKey << " JetContainer with "
                      << m_inTimeOutputJetContainer->size() <<" entries." );
    }
  if(this->record(m_outOfTimeOutputJetContainer, m_outOfTimeOutputJetCollKey).isFailure())
    { //This call also records the xAOD::JetAuxContainer.
      ATH_MSG_ERROR("mergeEvent: Failed to record OutOfTimeOutputJetContainer");
      return StatusCode::FAILURE;
    }
  else
    {
      ATH_MSG_DEBUG ( "mergeEvent: Recorded " << m_outOfTimeOutputJetCollKey << " JetContainer with "
                      << m_outOfTimeOutputJetContainer->size() <<" entries." );
    }
  return StatusCode::SUCCESS;
}

StatusCode MergeTruthJetsTool::record(const xAOD::JetContainer* pjets, std::string jetcontainername) const
{
  const xAOD::JetAuxContainer* pjetsaux =
    dynamic_cast<const xAOD::JetAuxContainer*>(pjets->getConstStore());
  ATH_MSG_DEBUG("Check Aux store: " << pjets << " ... " << &pjets->auxbase() << " ... " << pjetsaux );
  if ( pjetsaux == 0 )
    {
      ATH_MSG_ERROR("Unable to retrieve Aux container");
      return StatusCode::FAILURE;
    }
  ATH_MSG_VERBOSE("Recording new Jet Aux container.");
  if ( evtStore()->record(pjetsaux, jetcontainername+"Aux.").isFailure() )
    {
      ATH_MSG_ERROR("Unable to write new Aux Jet collection to event store: " << jetcontainername);
      return StatusCode::FAILURE;
    }
  ATH_MSG_VERBOSE("Recording new Jet container.");
  if ( evtStore()->record(pjets, jetcontainername).isFailure() )
    {
      ATH_MSG_ERROR("Unable to write new Jet collection to event store: " << jetcontainername);
      return StatusCode::FAILURE;
    }
  ATH_MSG_DEBUG("Created new Jet collection in event store: " << jetcontainername);
  return StatusCode::SUCCESS;
}


StatusCode MergeTruthJetsTool::processAllSubEvents()
{
  ATH_MSG_VERBOSE ( "processAllSubEvents()" );
  if(!m_pMergeSvc)
    {
      if (!(m_pMergeSvc.retrieve()).isSuccess())
        {
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
  double pileup_this_pT=-1.;
  if ( (m_pMergeSvc->retrieveSubEvtsData(m_inputJetCollKey, truthList)).isSuccess() )
    {
      if (!truthList.empty())
        {

          //now merge all collections into one
          TruthJetList::const_iterator jetColl_iter(truthList.begin());
          const TruthJetList::const_iterator endOfJetColls(truthList.end());
          while (jetColl_iter!=endOfJetColls)
            {
              //FIXME we are forced to do a deep copy
              if (static_cast<int>((jetColl_iter)->first.time())==0)
                {
                  if (m_first_event)
                    {//FIXME this may not be robust in the case that there is no TruthJet container from the signal event.
                      m_signal_max_pT = this->processJetContainer(&(*((jetColl_iter)->second)), 0, 0.0, 0.0);
                      m_first_event=false;
                      if (!m_includeSignalJets)
                        {
                          ATH_MSG_VERBOSE ( "Don't include signal events in output Truth Jet Containers.");
                          ++jetColl_iter;
                          continue;
                        }
                    }
                  pileup_this_pT=this->processJetContainer(&(*((jetColl_iter)->second)), m_inTimeOutputJetContainer, m_inTimePtCut, 0.0);
                }
              else
                {
                  const float timeOfBCID(static_cast<float>((jetColl_iter)->first.time()));
                  pileup_this_pT=this->processJetContainer(&(*((jetColl_iter)->second)), m_outOfTimeOutputJetContainer, m_outOfTimePtCut, timeOfBCID);
                }
              if (pileup_this_pT>m_pileup_max_pT) m_pileup_max_pT=pileup_this_pT;
              //signal is always the first event, so if the first event
              //wasn't in-time, then the signal collection was missing and
              //we should skip further checks.
              if(m_first_event) {m_first_event=false;}
              ++jetColl_iter;
            }
      
        }
      else
        {
          ATH_MSG_DEBUG ( "processAllSubEvents: TruthJetList is empty" );
        }
    }
  else
    {
      ATH_MSG_ERROR ( "processAllSubEvents: Can not find TruthJetList" );
    }
  // Veto event when m_pileup_max_pT>m_signal_max_pT
  if (m_pileup_max_pT>m_signal_max_pT) m_filterPassed = false;

  if(this->record(m_inTimeOutputJetContainer, m_inTimeOutputJetCollKey).isFailure())
    { //This call also records the JetMomentMap.
      ATH_MSG_ERROR ( "processAllSubEvents: Failed to record InTimeOutputJetContainer" );
      return StatusCode::FAILURE;
    }
  else
    {
      ATH_MSG_DEBUG ( "processAllSubEvents: Recorded " << m_inTimeOutputJetCollKey << " JetContainer with "
                      << m_inTimeOutputJetContainer->size() <<" entries." );
    }

  if(this->record(m_outOfTimeOutputJetContainer, m_outOfTimeOutputJetCollKey).isFailure())
    { //This call also records the JetMomentMap.
      ATH_MSG_ERROR ( "processAllSubEvents: Failed to record OutOfTimeOutputJetContainer" );
      return StatusCode::FAILURE;
    }
  else
    {
      ATH_MSG_DEBUG ( "processAllSubEvents: Recorded " << m_outOfTimeOutputJetCollKey << " JetContainer with "
                      << m_outOfTimeOutputJetContainer->size() <<" entries." );
    }
  
  return StatusCode::SUCCESS;
}

//use a float for timeOfBCID as Jet moments are stored as floats.
double MergeTruthJetsTool::processJetContainer(const xAOD::JetContainer* inputJetContainer, xAOD::JetContainer *outputJetContainer, const double& ptCut, const float& timeOfBCID)
{
  double max_pT=-1.;
  const xAOD::JetContainer::const_iterator endOfJets(inputJetContainer->end());
  for (xAOD::JetContainer::const_iterator jetIter(inputJetContainer->begin()); jetIter != endOfJets; ++jetIter)
    {
      try
        {
          if (!(*jetIter) || (*jetIter)->pt()<ptCut)
            {
              ATH_MSG_VERBOSE( "processJetContainer: Jet with pT = " << (*jetIter)->pt() << " GeV failed ptCut of " << ptCut << "GeV." );
              continue;
            }
          if (max_pT<(*jetIter)->pt()) max_pT=(*jetIter)->pt();
          if (!outputJetContainer) continue;
          ATH_MSG_VERBOSE( "processJetContainer: Jet with pT = " << (*jetIter)->pt() << " GeV passed ptCut of " << ptCut << "GeV." );
        }
      catch (...)
        {
          ATH_MSG_ERROR ( "Failed to find Truth jet pT for Jet in a BCID at time = " << timeOfBCID );
        }
      xAOD::Jet* pjet = new xAOD::Jet();
      outputJetContainer->push_back(pjet);
      *pjet = **jetIter;
      //FIXME: Would be nice to check if the Timing attribute is already
      //set and add that time to the BCID time, but I can't see a way of
      //doing that which doesn't involve lots of string comparison, so
      //we will have to live without it.
      outputJetContainer->back()->setAttribute("Timing", timeOfBCID);
    }
  return max_pT;
}
