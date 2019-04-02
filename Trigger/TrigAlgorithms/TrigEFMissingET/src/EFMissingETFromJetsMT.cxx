/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     EFMissingETFromJetsMT.cxx
PACKAGE:  Trigger/TrigAlgorithms/TrigEFMissingET
AUTHOR:   Gabriel Gallardo
CREATED:  Feb 19, 2018

BASED ON: EFMissingETFromJets.cxx
AUTHORS:  Florian U. Bernlochner, Doug Schaefer, Justin Chiu

PURPOSE:  Updates TrigMissingETHelper using info from jets

 ********************************************************************/

#include "EFMissingETFromJetsMT.h"

#include "TrigTimeAlgs/TrigTimerSvc.h"
#include "CxxUtils/sincosf.h"

#include "JetEvent/JetCollection.h"
#include "JetEvent/Jet.h"
//#include "FourMomUtils/P4DescendingSorters.h"
#include "xAODJet/JetContainer.h"
#include "xAODJet/Jet.h"
#include "CaloGeoHelpers/CaloSampling.h"
#include "xAODJet/JetAccessorMap.h"

#include "EventKernel/ISignalState.h"
#include "EventKernel/SignalStateHelper.h"

#include <cmath>
#include <string>
using namespace std;

EFMissingETFromJetsMT::EFMissingETFromJetsMT(const std::string& type,
    const std::string& name,
    const IInterface* parent) : base_class(type, name, parent)
{
  m_etacut = fabs(m_etacut);
}


EFMissingETFromJetsMT::~EFMissingETFromJetsMT()
{
}


StatusCode EFMissingETFromJetsMT::initialize()
{
  ATH_MSG_DEBUG( "called EFMissingETFromJetsMT::initialize()" );

  /// timers
  if( service( "TrigTimerSvc", m_timersvc).isFailure() )
    ATH_MSG_WARNING( name() << ": Unable to locate TrigTimer Service" );

  if (m_timersvc) {
    // global time
    std::string basename=name()+".TotalTime";
    m_glob_timer = m_timersvc->addItem(basename);
  } // if timing service

  CHECK( m_jetsKey.initialize() );

  return StatusCode::SUCCESS;
}


StatusCode EFMissingETFromJetsMT::update( xAOD::TrigMissingET *met,
             TrigEFMissingEtHelper *metHelper, const EventContext& ctx ) const
{

  ATH_MSG_DEBUG( "called EFMissingETFromJetsMT::update()" ); // 

  const std::vector<std::string> vComp = {"Jets", "Muon"};
  met->defineComponents(vComp);

  if(m_timersvc)
    m_glob_timer->start(); // total time
  ATH_MSG_DEBUG( "started MET jet CPU timer" );

  TrigEFMissingEtComponent* metComp = metHelper->GetComponent(TrigEFMissingEtComponent::JET); // fetch Jet component
  if (metComp==0) {  ATH_MSG_ERROR( "Could not fetch jet component!" );  return StatusCode::FAILURE; }
  else ATH_MSG_DEBUG( "fetched metHelper component \"" << metComp->m_name << "\"" );

  if ( (metComp->m_status & m_maskProcessed)==0 ){ // not yet processed
    metComp->Reset();  // reset component...
  } else { // skip if processed
    return StatusCode::SUCCESS;
  }

  auto jetsHandle = SG::makeHandle( m_jetsKey, ctx );
  std::vector<const xAOD::Jet*> MHTJetsVec(jetsHandle->begin(), jetsHandle->end());
  ATH_MSG_DEBUG( "num of jets: " << MHTJetsVec.size() );

  //--- fetching the jet components
  const std::map<TrigEFMissingEtComponent::Component, std::pair<float, float > > jetComponents = {
                                          {TrigEFMissingEtComponent::JET  , {       20, -20       }},
                                          {TrigEFMissingEtComponent::JETB1, { m_etacut,  0       }},
                                          {TrigEFMissingEtComponent::JETB2, {        0, -m_etacut}},
                                          {TrigEFMissingEtComponent::JETE1, {        5,  m_etacut}},
                                          {TrigEFMissingEtComponent::JETE2, {-m_etacut, -5       }}  };

  for(auto const& [jetComp, etaLimits] : jetComponents) {
    metComp = metHelper->GetComponent(jetComp); 

    if(!metComp) {
      ATH_MSG_ERROR("Could not fetch jet component "
                    << TrigEFMissingEtComponent::ComponentToName(jetComp) << "!");
      return StatusCode::FAILURE;
    }

    for (const xAOD::Jet* aJet : MHTJetsVec) {
      TLorentzVector p4 = aJet->p4();
      float scale = 1.;
      p4*=scale;

      /*
      // TileGap3Correction obsolete? Will remove in future MR if this is the case.
      // Commented out for the time being because this block of code is not thread-safe and is raising warnings. @ggallard

      if (m_applyTileGap3Correction) {
        // get the uncalibrated energy and tile gap 3 fractions
        static const xAOD::JetAttributeAccessor::AccessorWrapper< std::vector<float> >& acc_ePerSample =
          *xAOD::JetAttributeAccessor::accessor< std::vector<float> >(xAOD::JetAttribute::EnergyPerSampling);
        static xAOD::JetAttributeAccessor::AccessorWrapper<xAOD::JetFourMom_t> acc_uncalibrated("JetConstitScaleMomentum");
        const std::vector<float>& eInSampling = acc_ePerSample.getAttribute(*aJet);
        float e_tileGap3 = eInSampling.at(CaloSampling::TileGap3);
        scale = 1 - e_tileGap3/acc_uncalibrated.getAttribute(*aJet).E();
      }
      */

      switch (jetComp)
      {
        case TrigEFMissingEtComponent::JETB1:
        case TrigEFMissingEtComponent::JETB2:
          if (p4.Pt() < m_central_ptcut) continue;
          break;

        case TrigEFMissingEtComponent::JETE1:
        case TrigEFMissingEtComponent::JETE2:
          if (p4.Pt() < m_forward_ptcut) continue;
          break;
        case TrigEFMissingEtComponent::JET:
          break;
        default:
          ATH_MSG_ERROR("You are somehow iterating over this non-jet component: " 
                          << TrigEFMissingEtComponent::ComponentToName(jetComp) 
                          << ". This is not okay, because this is supposed to be MET from jets!");
          return StatusCode::FAILURE;
      }

      if (p4.Eta() < etaLimits.first && p4.Eta() > etaLimits.second)
      {
        metComp->m_ex           -= p4.Px();
        metComp->m_ey           -= p4.Py();
        metComp->m_ez           -= p4.Pz();
        metComp->m_sumEt        += p4.Pt();
        metComp->m_sumE         += p4.E();
        metComp->m_usedChannels += 1;
        metComp->m_sumOfSigns   += copysign(1.0, p4.Pt() );
      }

    } // End loop over all jets

    // move from "processing" to "processed" state
    metComp->m_status ^= m_maskProcessing; // switch off bit
    metComp->m_status |= m_maskProcessed;  // switch on bit

  }

  // fetch jet component and output MET value
  metComp = metHelper->GetComponent(TrigEFMissingEtComponent::JET); 
  ATH_MSG_DEBUG( " calculated MET: " << sqrt((metComp->m_ex)*(metComp->m_ex)+(metComp->m_ey)*(metComp->m_ey)) );


  if(m_timersvc)
    m_glob_timer->stop(); // total time

  return StatusCode::SUCCESS;
}

