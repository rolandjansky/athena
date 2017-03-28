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
  // declare configurables

  m_fextype = FexType::JET; m_etacut = fabs(m_etacut);

  m_methelperposition = 8;
}


EFMissingETFromJets::~EFMissingETFromJets()
{
}


StatusCode EFMissingETFromJets::initialize()
{
  if(msgLvl(MSG::DEBUG))
    msg(MSG::DEBUG) << "called EFMissingETFromJets::initialize()" << endmsg;

  /// timers
  if( service( "TrigTimerSvc", m_timersvc).isFailure() )
    msg(MSG::WARNING) << name() << ": Unable to locate TrigTimer Service" << endmsg;

  if (m_timersvc) {
    // global time
    std::string basename=name()+".TotalTime";
    m_glob_timer = m_timersvc->addItem(basename);
  } // if timing service

  return StatusCode::SUCCESS;
}


StatusCode EFMissingETFromJets::execute()
{

  msg(MSG::DEBUG) << name() << ": Executing Jet algorithm for ETMiss" << endmsg;
  return StatusCode::SUCCESS;
}

StatusCode EFMissingETFromJets::finalize()
{
  if(msgLvl(MSG::DEBUG))
    msg(MSG::DEBUG) << "called EFMissingETFromJets::finalize()" << endmsg;

  return StatusCode::SUCCESS;
}

StatusCode EFMissingETFromJets::execute(xAOD::TrigMissingET *,
                                        TrigEFMissingEtHelper *metHelper,
                                        const xAOD::CaloClusterContainer * /* caloCluster */,
                                        const xAOD::JetContainer *MHTJetContainer,
                                        const xAOD::TrackParticleContainer * /*trackContainer*/,
                                        const xAOD::VertexContainer * /*vertexContainer*/ )
{

  if(msgLvl(MSG::DEBUG))
    msg(MSG::DEBUG) << "called EFMissingETFromJets::execute()" << endmsg; // EFMissingET_Fex_Jets

  if(m_timersvc)
    m_glob_timer->start(); // total time

  if(msgLvl(MSG::DEBUG))
    msg(MSG::DEBUG) << "started MET jet CPU timer" << endmsg;

  TrigEFMissingEtComponent* metComp = metHelper->GetComponent(metHelper->GetElements() - m_methelperposition); // fetch Jet component

  if (metComp==0) {  msg(MSG::ERROR) << "cannot fetch Topo. cluster component!" << endmsg;  return StatusCode::FAILURE; }
  if(string(metComp->m_name).substr(0,3)!="JET"){ msg(MSG::ERROR) << "fetched " << metComp->m_name << " instead of the Jet component!" << endmsg; return StatusCode::FAILURE; }

  std::vector<const xAOD::Jet*> MHTJetsVec(MHTJetContainer->begin(), MHTJetContainer->end());

  msg(MSG::DEBUG) << "num of jets: " << MHTJetsVec.size() << endmsg;

 //--- fetching the topo. cluster component
 float upperlim[4] = {m_etacut,0,5,-m_etacut}; float lowerlim[4] = {0,-m_etacut,m_etacut,-5};

 for(int i = 0; i < 5; i++) {

  metComp = metHelper->GetComponent(metHelper->GetElements() - m_methelperposition + i); // fetch Cluster component

  for (const xAOD::Jet* aJet : MHTJetsVec) {

    if(i == 0) {

          metComp->m_ex -= aJet->px();
          metComp->m_ey -= aJet->py();
          metComp->m_ez -= aJet->pz();
          metComp->m_sumEt += aJet->pt();
          metComp->m_sumE  += aJet->e();
          metComp->m_usedChannels += 1;
          metComp->m_sumOfSigns += static_cast<short int>(floor(copysign(1.0,aJet->pt())+0.5));


    } else if (i > 0) {

       float eta = aJet->eta(); float ptCut = 0.;

       // Set pT cut depending on region
       if(i == 1 || i == 2) ptCut = m_central_ptcut;
        else ptCut = m_forward_ptcut;

       // only sum jets that have a pt above the cut value
       if(aJet->pt() < ptCut) continue;

       if( eta >= lowerlim[i-1] && eta <= upperlim[i-1]) {
          metComp->m_ex -= aJet->px();
          metComp->m_ey -= aJet->py();
          metComp->m_ez -= aJet->pz();
          metComp->m_sumEt += aJet->pt();
          metComp->m_sumE  += aJet->e();
          metComp->m_usedChannels += 1;
          metComp->m_sumOfSigns += static_cast<short int>(floor(copysign(1.0,aJet->pt())+0.5));
       }

     }

    } // End loop over all jets

    // move from "processing" to "processed" state
    metComp->m_status ^= m_maskProcessing; // switch off bit
    metComp->m_status |= m_maskProcessed;  // switch on bit

  }

  metComp = metHelper->GetComponent(metHelper->GetElements() - m_methelperposition); // fetch Cluster component

  msg(MSG::DEBUG) << " calculated MET: " << sqrt((metComp->m_ex)*(metComp->m_ex)+(metComp->m_ey)*(metComp->m_ey)) << endmsg;


  if(m_timersvc)
    m_glob_timer->stop(); // total time

  return StatusCode::SUCCESS;
}

