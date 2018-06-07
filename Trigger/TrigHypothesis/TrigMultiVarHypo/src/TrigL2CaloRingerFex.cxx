/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///Local include(s)
#include "TrigMultiVarHypo/TrigL2CaloRingerFex.h"
#include "TrigMultiVarHypo/preproc/TrigRingerPreprocessor.h"
#include "TrigMultiVarHypo/tools/MultiLayerPerceptron.h"

///xAOD include(s)
#include "xAODTrigCalo/TrigEMCluster.h"
#include "xAODTrigRinger/TrigRNNOutput.h"
#include "xAODTrigRinger/TrigRNNOutputContainer.h"
#include "xAODTrigRinger/TrigRingerRings.h"
#include "xAODTrigRinger/TrigRingerRingsContainer.h"

///std include(s)
#include <new>
#include <cmath>

#define SIZEOF_NODES            3 
#define SIZEOF_RINGSETS         7

#include <iostream>
using namespace std;

//!===============================================================================================
TrigL2CaloRingerFex::TrigL2CaloRingerFex(const std::string& name, ISvcLocator* pSvcLocator):
  HLT::FexAlgo(name, pSvcLocator), m_lumiBlockMuTool("LumiBlockMuTool/LumiBlockMuTool"),
  m_reader("TrigL2CaloRingerReader")
{  
  declareProperty("HltFeature"        , m_hlt_feature = "TrigRingerNeuralFex" );  
  declareProperty("Feature"           , m_feature = "TrigT2CaloEgamma"        );  
  declareProperty("NormalisationRings", m_normRings                           );
  declareProperty("SectionRings"      , m_sectionRings                        );
  declareProperty("NRings"            , m_nRings                              );
  declareProperty("CalibPath"         , m_calibPath = ""                      );
  declareProperty("LuminosityTool"    , m_lumiBlockMuTool, "Luminosity Tool"  );
  declareMonitoredVariable("NeuralNetworkOutput", m_output                    );
  
  m_reader.setMsgStream(msg());
  m_useLumiVar=false;
  m_useEtaVar=false;
  m_key       = "";
}

//!===============================================================================================
HLT::ErrorCode TrigL2CaloRingerFex::hltInitialize() 
{
  // Retrieve the NeuralNetwork list
  if(!m_calibPath.empty()){
    if(!m_reader.retrieve(m_calibPath, m_discriminators)){
      ATH_MSG_ERROR("Can not retrieve all information from " << m_calibPath);
      return StatusCode::FAILURE;
    }
    m_useLumiVar = m_reader.useLumiVar();
    m_useEtaVar  = m_reader.useEtaVar();
  }

  if(m_nRings.size() != m_normRings.size()){
    ATH_MSG_ERROR( "Preproc nRings list dont match with the number of discriminators found" );
    return StatusCode::FAILURE;
  }

  if(m_sectionRings.size() != m_normRings.size()){
    ATH_MSG_ERROR( "Preproc section rings list dont match with the number of discriminators found" );
    return StatusCode::FAILURE;
  }

  ///TODO: This is default for now, apply this into the conf file for future
  ///Initialize all discriminators
  for(unsigned i=0; i<m_discriminators.size(); ++i)
  {
    TrigRingerPreprocessor *preproc;
    try{
      preproc = new TrigRingerPreprocessor(m_nRings,m_normRings,m_sectionRings);
    }catch(std::bad_alloc xa){
      ATH_MSG_ERROR( "Bad alloc for TrigRingerPrepoc." );
      return StatusCode::FAILURE;
    }
    ///Hold the pointer configuration
    m_preproc.push_back(preproc);
  }///Loop over discriminators
  

  ///Monitoring hitograms
  if(doTiming()){
    m_normTimer     = addTimer("Normalization");
    m_decisionTimer = addTimer("Decision");
    m_storeTimer    = addTimer("StoreOutput");
  }///Only if time is set on python config

  m_useLumiTool=false;
  if (m_lumiBlockMuTool.retrieve().isFailure()) {
    ATH_MSG_WARNING("Unable to retrieve Luminosity Tool");
  } else {
    m_useLumiTool=true; // Tell to the execute that the LumiTool is available...
    ATH_MSG_INFO("Successfully retrieved Luminosity Tool");
  }
 
  ATH_MSG_INFO("Using the Luminosity tool? " <<  (m_useLumiTool ? "Yes":"No") );
  ATH_MSG_INFO("Using lumiVar?             " <<  (m_useLumiVar ? "Yes":"No") );
  ATH_MSG_INFO("Using etaVar?              " <<  (m_useEtaVar ? "Yes":"No") );
  
  ATH_MSG_INFO( "TrigL2CaloRingerHypo initialization completed successfully." );

  return HLT::OK;
}
//!===============================================================================================
HLT::ErrorCode TrigL2CaloRingerFex::hltFinalize() {  
  ///release memory
  for(unsigned i=0; i<m_discriminators.size();++i){
    if(m_preproc.at(i))         delete m_preproc.at(i);
    if(m_discriminators.at(i))  delete m_discriminators.at(i);
  }///Loop over all discriminators and prepoc objects
  ATH_MSG_INFO( "TrigL2CaloRingerHypo finalization completed successfully." );

  return HLT::OK;
}
//!===============================================================================================
HLT::ErrorCode TrigL2CaloRingerFex::hltExecute(const HLT::TriggerElement* /*inputTE*/, HLT::TriggerElement* outputTE){

  // For now, this must be [avgmu, rnnOutputWithTansig, rnnOutputWithoutTansig] 
  m_output=-999;
  std::vector<float> output;

  ///Retrieve rings pattern information
  const xAOD::TrigRingerRings *ringerShape = get_rings(outputTE);
  if(!ringerShape){
    ATH_MSG_WARNING( "Can not retrieve xADO::TrigRingerRings from storegate." );
    return HLT::OK;
  }///protection

  ///Retrieve cluster information
  const xAOD::TrigEMCluster *emCluster = ringerShape->emCluster();

  ///Check if emCluster link exist
  if(!emCluster){
    ATH_MSG_WARNING( "Can not found the xAOD::TrigEMCluster link" );
    return HLT::OK;
  }///protection

  ATH_MSG_DEBUG( "Event with roiword: 0x" << std::hex << emCluster->RoIword() << std::dec );


  ///It's ready to select the correct eta/et bin
  MultiLayerPerceptron    *discr  = nullptr;
  TrigRingerPreprocessor  *preproc = nullptr;

  float eta = std::fabs(emCluster->eta());
  float et  = emCluster->et()*1e-3; ///in GeV 
  float avgmu, mu = 0.0;

  if(m_useLumiTool){
    if(m_lumiBlockMuTool){
      mu = m_lumiBlockMuTool->actualInteractionsPerCrossing(); // (retrieve mu for the current BCID)
      avgmu = m_lumiBlockMuTool->averageInteractionsPerCrossing();
      ATH_MSG_DEBUG("Retrieved Mu Value : " << mu);
      ATH_MSG_DEBUG("Average Mu Value   : " << avgmu);   
    }
  }

  // Fix eta range
  if(eta>2.50) eta=2.50;///fix for events out of the ranger
  // Add avgmu!
  output.push_back(avgmu);
    

  if(doTiming())  m_decisionTimer->start();
  
  if(m_discriminators.size() > 0){
      
    for(unsigned i=0; i<m_discriminators.size(); ++i){
      if(et > m_discriminators[i]->etmin() && et <= m_discriminators[i]->etmax()){
        if(eta > m_discriminators[i]->etamin() && eta <= m_discriminators[i]->etamax()){
          discr   = m_discriminators[i];
          preproc = m_preproc[i];
          break;
        }///eta conditions
      }///Et conditions
    }///Loop over discriminators

    ATH_MSG_DEBUG( "Et = " << et << " GeV, |eta| = " << eta );
    ///Apply the discriminator
    if(discr){

      const std::vector<float> rings = ringerShape->rings();
      std::vector<float> refRings(rings.size());
      refRings.assign(rings.begin(), rings.end());

      ///pre-processing ringer shape (default is Norm1...)
      if(doTiming())  m_normTimer->start();
      if(preproc)     preproc->ppExecute(refRings);
      if(doTiming())  m_normTimer->stop();


      float eta_norm=0.0;
      float avgmu_norm=0.0;

      // Add extra variables in this order! Do not change this!!!
      if(m_useEtaVar){
        if(preproc){
          eta_norm = preproc->normalize_eta(emCluster->eta(), discr->etamin(), discr->etamax());
          refRings.push_back(eta_norm);
        }
      }

      if(m_useLumiVar){
        if(preproc){
          avgmu_norm = preproc->normalize_mu(avgmu, m_lumiCut);
          refRings.push_back(avgmu_norm);
        }
      }

      m_output=discr->propagate(refRings);
      output.push_back(m_output);
      output.push_back(discr->getOutputBeforeTheActivationFunction());
    }// has discr?
  }else{
    ATH_MSG_DEBUG( "There is no discriminator into this Fex." );
  }

  if(doTiming())  m_decisionTimer->stop();

  ATH_MSG_DEBUG( "Et = " << et << " GeV, |eta| = " << eta << " and rnnoutput = " << m_output );

  if(doTiming())  m_storeTimer->start();
  ///Store outout information for monitoring and studys
  xAOD::TrigRNNOutput *rnnOutput = new xAOD::TrigRNNOutput();
  rnnOutput->makePrivateStore(); 
  rnnOutput->setRnnDecision(output);

  ///Get the ringer link to store into TrigRNNOuput  
  HLT::ErrorCode hltStatus;
  ElementLink<xAOD::TrigRingerRingsContainer> ringer_link;
  hltStatus = getFeatureLink<xAOD::TrigRingerRingsContainer,xAOD::TrigRingerRings>(outputTE, ringer_link);

  if( (hltStatus != HLT::OK) || (!ringer_link.isValid())){
    ATH_MSG_WARNING( "Failed to access ElementLink to TrigRingerRings" );
  }else{
    rnnOutput->setRingerLink( ringer_link );
  }
  hltStatus = recordAndAttachFeature<xAOD::TrigRNNOutput>(outputTE, rnnOutput, m_key, m_hlt_feature);
  if(doTiming())  m_storeTimer->stop();

  if (hltStatus != HLT::OK) {
    ATH_MSG_WARNING( "Failed to record xAOD::TrigRNNOutput to StoreGate." );
    return HLT::OK;
  }

  return HLT::OK;
}
//!===============================================================================================
