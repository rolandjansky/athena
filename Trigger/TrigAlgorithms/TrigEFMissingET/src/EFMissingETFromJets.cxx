/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     EFMissingETFromJets.cxx
PACKAGE:  Trigger/TrigAlgorithms/TrigEFMissingET

AUTHORS:  Florian U. Bernlochner, Doug Schaefer, Justin Chiu
CREATED:  May 20, 2014

PURPOSE:  Updates TrigMissingETHelper using info from jets

 ********************************************************************/
#include "TrigEFMissingET/EFMissingETFromJets.h"

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

EFMissingETFromJets::EFMissingETFromJets(const std::string& type,
    const std::string& name,
    const IInterface* parent) :
  EFMissingETBaseTool(type, name, parent)
{
  declareProperty("EtaSeparation", m_etacut = 2.2 ,"Cut to split into forward and central jets -- needs to be positive");
  declareProperty("CentralpTCut", m_central_ptcut = 0.0 ,"pT Cut for central jets");
  declareProperty("ForwardpTCut", m_forward_ptcut = 0.0 ,"pT Cut for forward jets");
  declareProperty("ApplyTileGap3Correction", m_applyTileGap3Correction = false);
  // declare configurables

  m_fextype = FexType::JET; m_etacut = fabs(m_etacut);

  m_methelperposition = 8;
}


EFMissingETFromJets::~EFMissingETFromJets()
{
}


StatusCode EFMissingETFromJets::initialize()
{
  ATH_MSG_DEBUG( "called EFMissingETFromJets::initialize()" );

  /// timers
  if( service( "TrigTimerSvc", m_timersvc).isFailure() )
    ATH_MSG_WARNING( name() << ": Unable to locate TrigTimer Service" );

  if (m_timersvc) {
    // global time
    std::string basename=name()+".TotalTime";
    m_glob_timer = m_timersvc->addItem(basename);
  } // if timing service

  return StatusCode::SUCCESS;
}


StatusCode EFMissingETFromJets::execute()
{

  ATH_MSG_DEBUG( name() << ": Executing Jet algorithm for ETMiss" );
  return StatusCode::SUCCESS;
}

StatusCode EFMissingETFromJets::finalize()
{
  ATH_MSG_DEBUG( "called EFMissingETFromJets::finalize()" );

  return StatusCode::SUCCESS;
}

StatusCode EFMissingETFromJets::execute(xAOD::TrigMissingET *,
                                        TrigEFMissingEtHelper *metHelper,
                                        const xAOD::CaloClusterContainer * /* caloCluster */,
                                        const xAOD::JetContainer *MHTJetContainer,
                                        const xAOD::TrackParticleContainer * /*trackContainer*/,
                                        const xAOD::VertexContainer * /*vertexContainer*/,
                                        const xAOD::MuonContainer * /*muonContainer*/ )
{

  ATH_MSG_DEBUG( "called EFMissingETFromJets::execute()" ); // EFMissingET_Fex_Jets

  if(m_timersvc)
    m_glob_timer->start(); // total time

  ATH_MSG_DEBUG( "started MET jet CPU timer" );

  TrigEFMissingEtComponent* metComp = metHelper->GetComponent(metHelper->GetElements() - m_methelperposition); // fetch Jet component

  if (metComp==0) {  ATH_MSG_ERROR( "cannot fetch Topo. cluster component!" );  return StatusCode::FAILURE; }
  if(string(metComp->m_name).substr(0,3)!="JET"){ ATH_MSG_ERROR( "fetched " << metComp->m_name << " instead of the Jet component!" ); return StatusCode::FAILURE; }

  std::vector<const xAOD::Jet*> MHTJetsVec(MHTJetContainer->begin(), MHTJetContainer->end());

  ATH_MSG_DEBUG( "num of jets: " << MHTJetsVec.size() );

 //--- fetching the topo. cluster component
 float upperlim[4] = {m_etacut,0,5,-m_etacut}; float lowerlim[4] = {0,-m_etacut,m_etacut,-5};

 for(int i = 0; i < 5; i++) {

  metComp = metHelper->GetComponent(metHelper->GetElements() - m_methelperposition + i); // fetch Cluster component

  for (const xAOD::Jet* aJet : MHTJetsVec) {

    TLorentzVector p4 = aJet->p4();
    float scale = 1.;
    if (m_applyTileGap3Correction) {
      // get the uncalibrated energy and tile gap 3 fractions
      static const xAOD::JetAttributeAccessor::AccessorWrapper< std::vector<float> >& acc_ePerSample =
        *xAOD::JetAttributeAccessor::accessor< std::vector<float> >(xAOD::JetAttribute::EnergyPerSampling);
      static xAOD::JetAttributeAccessor::AccessorWrapper<xAOD::JetFourMom_t> acc_uncalibrated("JetConstitScaleMomentum");
      const std::vector<float>& eInSampling = acc_ePerSample.getAttribute(*aJet);
      float e_tileGap3 = eInSampling.at(CaloSampling::TileGap3);
      scale = 1 - e_tileGap3/acc_uncalibrated.getAttribute(*aJet).E();
    }
    p4*=scale;

    if(i == 0) {

          metComp->m_ex -= p4.Px();
          metComp->m_ey -= p4.Py();
          metComp->m_ez -= p4.Pz();
          metComp->m_sumEt += p4.Pt();
          metComp->m_sumE  += p4.E();
          metComp->m_usedChannels += 1;
          metComp->m_sumOfSigns += copysign(1.0, p4.Pt() );


    } else if (i > 0) {

       float eta = p4.Eta(); float ptCut = 0.;

       // Set pT cut depending on region
       if(i == 1 || i == 2) ptCut = m_central_ptcut;
        else ptCut = m_forward_ptcut;

       // only sum jets that have a pt above the cut value
       if(p4.Pt() < ptCut) continue;

       if( eta >= lowerlim[i-1] && eta <= upperlim[i-1]) {
          metComp->m_ex -= p4.Px();
          metComp->m_ey -= p4.Py();
          metComp->m_ez -= p4.Pz();
          metComp->m_sumEt += p4.Pt();
          metComp->m_sumE  += p4.E();
          metComp->m_usedChannels += 1;
          metComp->m_sumOfSigns += copysign(1.0, p4.Pt() );
       }

     }

    } // End loop over all jets

    // move from "processing" to "processed" state
    metComp->m_status ^= m_maskProcessing; // switch off bit
    metComp->m_status |= m_maskProcessed;  // switch on bit

  }

  metComp = metHelper->GetComponent(metHelper->GetElements() - m_methelperposition); // fetch Cluster component

  ATH_MSG_DEBUG( " calculated MET: " << sqrt((metComp->m_ex)*(metComp->m_ex)+(metComp->m_ey)*(metComp->m_ey)) );


  if(m_timersvc)
    m_glob_timer->stop(); // total time

  return StatusCode::SUCCESS;
}

