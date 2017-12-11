/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


///Local include(s)
#include "TrigMultiVarHypo/TrigL2CaloRingerHypo.h"

///xAOD include(s)
#include "xAODTrigCalo/TrigEMCluster.h"
#include "xAODTrigRinger/TrigRNNOutput.h"
#include "xAODTrigRinger/TrigRNNOutputContainer.h"
#include "xAODTrigRinger/TrigRingerRings.h"
#include "xAODTrigRinger/TrigRingerRingsContainer.h"

///std include(s)
#include <new>
#include <cmath>


//!===============================================================================================
TrigL2CaloRingerHypo::TrigL2CaloRingerHypo(const std::string& name, ISvcLocator* pSvcLocator):
  HLT::HypoAlgo(name, pSvcLocator)
{  
  declareProperty("AcceptAll"         , m_acceptAll = false                   );
  declareProperty("EmEtCut"           , m_emEtCut = 0.0                       );
  declareProperty("HltFeature"        , m_hlt_feature = "TrigRingerNeuralFex" );  
  declareProperty("Thresholds"        , m_thresholds                          );
  declareProperty("EtaBins"           , m_etaBins                             );
  declareProperty("EtBins"            , m_etBins                              );

  m_nThresholds = 0;
}
//!===============================================================================================
HLT::ErrorCode TrigL2CaloRingerHypo::hltInitialize() 
{
  ///What is the number of discriminators?
  m_nThresholds = m_thresholds.size();

  if((m_etaBins.size() != m_nThresholds) && (m_etBins.size() != m_nThresholds)){
    msg() << MSG::ERROR << "Eta/Et list dont match with the number of thesholds found" << endmsg;
    return HLT::BAD_JOB_SETUP;
  }

  ///Initialize all discriminators
  for(unsigned i=0; i<m_nThresholds; ++i)
  {
    ///Hold the pointer configuration
    try{
      m_cutDefs.push_back(new TrigL2CaloRingerHypo::CutDefsHelper(m_thresholds[i],m_etaBins[i][0],/*
                                              */m_etaBins[i][1], m_etBins[i][0],m_etBins[i][1]));
    }catch(std::bad_alloc xa){
      msg() << MSG::ERROR << "Can not alloc cutDefs on memory." << endmsg;
      return HLT::BAD_JOB_SETUP;
    }
  }///Loop over discriminators
  
  if ( msgLvl() <= MSG::INFO )
    msg() << MSG::INFO << "TrigL2CaloRingerHypo initialization completed successfully." << endmsg;

  return HLT::OK;
}
//!===============================================================================================
HLT::ErrorCode TrigL2CaloRingerHypo::hltFinalize() {  
  
  for(unsigned i=0; i<m_nThresholds;++i){
    if(m_cutDefs[i])  delete m_cutDefs[i];
  } 
  if ( msgLvl() <= MSG::INFO ) 
    msg() << MSG::INFO << "TrigL2CaloRingerHypo finalization completed successfully." << endmsg;
  return HLT::OK;
}
//!===============================================================================================

HLT::ErrorCode TrigL2CaloRingerHypo::hltExecute(const HLT::TriggerElement* outputTE, bool& pass){

  pass = false;

  if (m_acceptAll){
    pass = true;
    if ( msgLvl() <= MSG::DEBUG ){
      msg() << MSG::DEBUG << "AcceptAll property is set: taking all events"  << endmsg;
    }
    return HLT::OK;
  }

  const xAOD::TrigRNNOutput* rnnOutput = get_rnnOutput(outputTE);
  if(!rnnOutput){
    msg() << MSG::WARNING << "There is no xAO::TrigRNNOutput into the TriggerElement." << endmsg;
    return HLT::OK;
  }

  const xAOD::TrigEMCluster *emCluster = 0;
  const xAOD::TrigRingerRings *ringerShape = rnnOutput->ringer();
  if(ringerShape){
    emCluster = ringerShape->emCluster();
    if(!emCluster){
      msg() << MSG::WARNING << "There is no link to xAOD::TrigEMCluster into the Ringer object." << endmsg;
      return HLT::OK;
    }
  }else{
    msg() << MSG::WARNING << "There is no xAOD::TrigRingerRings link into the rnnOutput object." << endmsg;
    return HLT::OK;
  }

  float eta     = std::fabs(emCluster->eta());
  float et      = emCluster->et()*1e-3;//GeV
  if(eta>2.50) eta=2.50;///fix for events out of the ranger
  float output  = rnnOutput->rnnDecision().at(0);


  ///Et threshold
  if(et < m_emEtCut*1e-3){
    if(msgLvl() <= MSG::DEBUG){
      msg() << MSG::DEBUG<< "Event reproved by Et threshold. Et = " << et << ", EtCut = " << m_emEtCut*1e-3 << endmsg;
    }
    return HLT::OK;
  }

  if(m_nThresholds > 0){
    ///Select the correct threshold for each eta/Et region
    for(unsigned i=0; i<m_nThresholds;++i){
      if((et  > m_cutDefs[i]->etmin()) && (et  <= m_cutDefs[i]->etmax())){
        if((eta > m_cutDefs[i]->etamin()) && (eta <= m_cutDefs[i]->etamax())){
          if(output >= m_cutDefs[i]->threshold()){

            if(msgLvl() <= MSG::DEBUG){///print event information
              msg() << MSG::DEBUG << "Event information:" << endmsg;
              msg() << MSG::DEBUG << "   " << m_cutDefs[i]->etmin() << "< Et ("<<et<<") GeV" << " <=" << m_cutDefs[i]->etmax() << endmsg;
              msg() << MSG::DEBUG << "   " << m_cutDefs[i]->etamin() << "< |Eta| ("<<eta<<") " << " <=" << m_cutDefs[i]->etamax() << endmsg;
              msg() << MSG::DEBUG << "   rnnOutput: " << output <<  " and threshold: " << m_cutDefs[i]->threshold() <<endmsg;
            }

            pass=true;
          }else{
            if(msgLvl() <= MSG::DEBUG){
              msg() << MSG::DEBUG << "Event reproved by discriminator threshold" << endmsg;
            }
          }///Threshold condition
          break;
        }///Loop over eta
      }///Loop over et
    }///Loop over cutDefs
  }else{
    if(msgLvl() <= MSG::DEBUG){
      msg() << MSG::DEBUG<< "There is no discriminator. Event approved by Et threshold." << endmsg;
    }
    ///Only for EtCut
    pass=true; 
  }///protection

  return HLT::OK;
}
//!===============================================================================================
