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

#include "TrigMultiVarHypo/tools/procedures/Thresholds.h"
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
  
  m_removeOutputTansigTF=false;
  m_doPileupCorrection=false;
  m_lumiCut=50;
}
//!===============================================================================================
HLT::ErrorCode TrigL2CaloRingerHypo::hltInitialize() 
{
  if(!m_calibPath.empty()){
    if(!m_reader.retrieve(m_calibPath, m_cutDefs)){
      ATH_MSG_ERROR("Can not retrieve the information from " << m_calibPath);
      return HLT::BAD_JOB_SETUP;
    }
    // retrieve metadata
    m_removeOutputTansigTF = m_reader.removeOutputTansigTF();
    m_doPileupCorrection = m_reader.doPileupCorrection();
    m_lumiCut  = m_reader.lumiCut();
  }
  ATH_MSG_INFO("Using the activation function in the last layer? " <<  (!m_removeOutputTansigTF ? "Yes":"No") );
  ATH_MSG_INFO("Using pileup correction?                         " <<  (m_doPileupCorrection ? "Yes":"No") );
  ATH_MSG_INFO("Using lumi threshold equal:                      "  <<  m_lumiCut); 
  ATH_MSG_INFO( "TrigL2CaloRingerHypo initialization completed successfully." );
  
  ///Monitoring hitograms
  if(doTiming()){
    m_totalTimer    = addTimer("Total");
  }///Only if time is set on python config


  return HLT::OK;
}
//!===============================================================================================
HLT::ErrorCode TrigL2CaloRingerHypo::hltFinalize() {  
  
  ATH_MSG_INFO( "TrigL2CaloRingerHypo finalization completed successfully." );
  return HLT::OK;
}
//!===============================================================================================

HLT::ErrorCode TrigL2CaloRingerHypo::hltExecute(const HLT::TriggerElement* outputTE, bool& pass){

  pass = false;

  if(doTiming())  m_totalTimer->start();
  
  if (m_acceptAll){
    pass = true;
    ATH_MSG_DEBUG( "AcceptAll property is set: taking all events"  );
    if(doTiming())  m_totalTimer->stop();
    return HLT::OK;
  }

  const xAOD::TrigRNNOutput* rnnOutput = get_rnnOutput(outputTE);
  if(!rnnOutput){
    ATH_MSG_WARNING( "There is no xAO::TrigRNNOutput into the TriggerElement." );
    if(doTiming())  m_totalTimer->stop();
    return HLT::OK;
  }

  // Start to retrieve all information that I need...
  const xAOD::TrigEMCluster *emCluster = 0;
  const xAOD::TrigRingerRings *ringerShape = rnnOutput->ringer();
  if(ringerShape){
    emCluster = ringerShape->emCluster();
    if(!emCluster){
      ATH_MSG_WARNING( "There is no link to xAOD::TrigEMCluster into the Ringer object." );
      if(doTiming())  m_totalTimer->stop();
      return HLT::OK;
    }
  }else{
    ATH_MSG_WARNING( "There is no xAOD::TrigRingerRings link into the rnnOutput object." );
    if(doTiming())  m_totalTimer->stop();
    return HLT::OK;
  }

  float eta     = std::fabs(emCluster->eta());
  float et      = emCluster->et()*1e-3;//GeV
  if(eta>2.50) eta=2.50;///fix for events out of the ranger

  ///Et threshold
  if(et < m_emEtCut*1e-3){
    ATH_MSG_DEBUG( "Event reproved by Et threshold. Et = " << et << ", EtCut = " << m_emEtCut*1e-3 );
    if(doTiming())  m_totalTimer->stop();
    return HLT::OK;
  }

 
  if(m_cutDefs.size() > 0){
    // TODO: Maybe this will expanded for future...
    // This was define as [avgmu, rnnOtput, rnnOutputWithoutTansig]
    if(rnnOutput->rnnDecision().size() != 3){
      ATH_MSG_INFO( "Event reproved because we can not retrieve the completed information from RnnOutput to run this hypo!" );
      if(doTiming())  m_totalTimer->stop();
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

            if(m_removeOutputTansigTF)
              output=rnnOutput->rnnDecision().at(2);
            else
              output=rnnOutput->rnnDecision().at(1);

            if(output >= threshold){
              ATH_MSG_DEBUG( "Event information:" );
              ATH_MSG_DEBUG( "   " << m_cutDefs[i]->etmin()  << " < Et ("<<et<<") GeV " << " <=" << m_cutDefs[i]->etmax() );
              ATH_MSG_DEBUG( "   " << m_cutDefs[i]->etamin() << " < |Eta| ("<<eta<<") " << " <=" << m_cutDefs[i]->etamax() );
              ATH_MSG_DEBUG( "   " << m_cutDefs[i]->mumin()  << " < Mu ("<<avgmu<<")  " << " <=" << m_cutDefs[i]->mumax() );
              ATH_MSG_DEBUG( "   rnnOutput: " << output <<  " and threshold: " << m_cutDefs[i]->threshold() );
              pass=true;
            }else{
              ATH_MSG_DEBUG( "Event reproved by discriminator threshold" );
            }///Threshold condition
            
            break;
          }///Loop ove eta
        }///Loop over et
      }///Loop over mu
    }///Loop over cutDefs
  }else{
    ATH_MSG_DEBUG( "There is no discriminator. Event approved by Et threshold." );
    ///Only for EtCut
    pass=true; 
  }///protection

  if(doTiming())  m_totalTimer->stop();
  return HLT::OK;
}
//!===============================================================================================
