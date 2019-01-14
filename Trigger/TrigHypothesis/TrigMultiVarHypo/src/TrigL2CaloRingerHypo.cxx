/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
  HLT::HypoAlgo(name, pSvcLocator), m_reader("TrigCaloRingerReader")
{  
  declareProperty("AcceptAll"         , m_acceptAll = false                   );
  declareProperty("EmEtCut"           , m_emEtCut = 0.0                       );
  declareProperty("CalibPath"         , m_calibPath = ""                      );
  declareProperty("HltFeature"        , m_hlt_feature = "TrigRingerNeuralFex" );  
  
  m_reader.setMsgStream(msg());
  m_useNoActivationFunctionInTheLastLayer=false;
  m_doPileupCorrection=false;
  m_lumiCut=50;
}
//!===============================================================================================
HLT::ErrorCode TrigL2CaloRingerHypo::hltInitialize() 
{
  if(!m_calibPath.empty()){
    if(!m_reader.retrieve(m_calibPath, m_cutDefs)){
      msg() << MSG::ERROR << "Can not retrieve the information from " << m_calibPath << endmsg;
      return HLT::BAD_JOB_SETUP;
    }
    // retrieve metadata
    m_useNoActivationFunctionInTheLastLayer = m_reader.useNoActivationFunctionInTheLastLayer();
    m_doPileupCorrection = m_reader.doPileupCorrection();
    m_lumiCut  = m_reader.lumiCut();
  }
  msg() << MSG::INFO << "Using the activation function in the last layer? " <<  (m_useNoActivationFunctionInTheLastLayer ? "No":"Yes")  << endmsg;
  msg() << MSG::INFO << "Using pileup correction?                         " <<  (m_doPileupCorrection ? "Yes":"No")  << endmsg;
  msg() << MSG::INFO << "Using lumi threshold equal: "  <<  m_lumiCut << endmsg;
  
  msg() << MSG::INFO <<  "TrigL2CaloRingerHypo initialization completed successfully."  << endmsg;

  return HLT::OK;
}
//!===============================================================================================
HLT::ErrorCode TrigL2CaloRingerHypo::hltFinalize() {  
  
  for(unsigned i=0; i<m_cutDefs.size();++i){
    if(m_cutDefs[i])  delete m_cutDefs[i];
  } 
  msg() << MSG::INFO <<  "TrigL2CaloRingerHypo finalization completed successfully."  << endmsg;
  return HLT::OK;
}
//!===============================================================================================

HLT::ErrorCode TrigL2CaloRingerHypo::hltExecute(const HLT::TriggerElement* outputTE, bool& pass){

  pass = false;

  if (m_acceptAll){
    pass = true;
    msg() << MSG::DEBUG << "AcceptAll property is set: taking all events"  << endmsg;
    return HLT::OK;
  }

  const xAOD::TrigRNNOutput* rnnOutput = get_rnnOutput(outputTE);
  if(!rnnOutput){
    msg() << MSG::WARNING <<  "There is no xAO::TrigRNNOutput into the TriggerElement."  << endmsg;
    return HLT::OK;
  }

  // Start to retrieve all information that I need...
  const xAOD::TrigEMCluster *emCluster = 0;
  const xAOD::TrigRingerRings *ringerShape = rnnOutput->ringer();
  if(ringerShape){
    emCluster = ringerShape->emCluster();
    if(!emCluster){
      msg() << MSG::WARNING <<  "There is no link to xAOD::TrigEMCluster into the Ringer object."  << endmsg;
      return HLT::OK;
    }
  }else{
    msg() << MSG::WARNING <<  "There is no xAOD::TrigRingerRings link into the rnnOutput object."  << endmsg;
    return HLT::OK;
  }

  float eta     = std::fabs(emCluster->eta());
  float et      = emCluster->et()*1e-3;//GeV
  if(eta>2.50) eta=2.50;///fix for events out of the ranger

  ///Et threshold
  if(et < m_emEtCut*1e-3){
    msg() << MSG::DEBUG <<  "Event reproved by Et threshold. Et = " << et << ", EtCut = " << m_emEtCut*1e-3  << endmsg;
    return HLT::OK;
  }

 
  if(m_cutDefs.size() > 0){
    // TODO: Maybe this will expanded for future...
    // This was define as [avgmu, rnnOtput, rnnOutputWithoutTansig]
    if(rnnOutput->rnnDecision().size() != 3){
      msg() << MSG::INFO <<  "Event reproved because we can not retrieve the completed information from RnnOutput to run this hypo!"  << endmsg;
      return HLT::OK;
    }

    // get pileup average value
    float avgmu   = rnnOutput->rnnDecision().at(0);
    
    ///Select the correct threshold for each eta/Et region
    for(unsigned i=0; i<m_cutDefs.size();++i){
      if((avgmu > m_cutDefs[i]->mumin()) && (avgmu <= m_cutDefs[i]->mumax())){
        if((et  > m_cutDefs[i]->etmin()) && (et  <= m_cutDefs[i]->etmax())){
          if((eta > m_cutDefs[i]->etamin()) && (eta <= m_cutDefs[i]->etamax())){    

      
            float threshold=0.0;
            float output=0.0;

            // Retrieve the correct threshold
            if(m_doPileupCorrection){
              // Limited Pileup
              if(avgmu>m_lumiCut)
                avgmu=m_lumiCut;
              threshold =  m_cutDefs[i]->threshold(avgmu);
            }else{
              threshold = m_cutDefs[i]->threshold();
            }

            if(m_useNoActivationFunctionInTheLastLayer)
              output=rnnOutput->rnnDecision().at(2);
            else
              output=rnnOutput->rnnDecision().at(1);

            if(output >= threshold){
              msg() << MSG::DEBUG <<  "Event information:"  << endmsg;
              msg() << MSG::DEBUG <<  "   " << m_cutDefs[i]->etmin() << "< Et ("<<et<<") GeV" << " <=" << m_cutDefs[i]->etmax()  << endmsg;
              msg() << MSG::DEBUG <<  "   " << m_cutDefs[i]->etamin() << "< |Eta| ("<<eta<<") " << " <=" << m_cutDefs[i]->etamax()  << endmsg;
              msg() << MSG::DEBUG <<  "   " << m_cutDefs[i]->mumin() << "< Mu ("<<avgmu<<") " << " <=" << m_cutDefs[i]->mumax()  << endmsg;
              msg() << MSG::DEBUG <<  "   rnnOutput: " << output <<  " and threshold: " << m_cutDefs[i]->threshold()  << endmsg;
              pass=true;
            }else{
              msg() << MSG::DEBUG <<  "Event reproved by discriminator threshold"  << endmsg;
            }///Threshold condition
            
            break;
          }///Loop ove eta
        }///Loop over et
      }///Loop over mu
    }///Loop over cutDefs
  }else{
    msg() << MSG::DEBUG <<  "There is no discriminator. Event approved by Et threshold."  << endmsg;
    ///Only for EtCut
    pass=true; 
  }///protection

  return HLT::OK;
}
//!===============================================================================================
