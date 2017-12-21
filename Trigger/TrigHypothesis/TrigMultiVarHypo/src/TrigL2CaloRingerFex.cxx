/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


///Local include(s)
#include "TrigMultiVarHypo/TrigL2CaloRingerFex.h"
#include "TrigMultiVarHypo/preprocessor/TrigRingerPreprocessor.h"
#include "TrigMultiVarHypo/discriminator/MultiLayerPerceptron.h"

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
  HLT::FexAlgo(name, pSvcLocator)
{  
  declareProperty("HltFeature"        , m_hlt_feature = "TrigRingerNeuralFex" );  
  declareProperty("Feature"           , m_feature = "TrigT2CaloEgamma"        );  
  declareProperty("NormalisationRings", m_normRings                           );
  declareProperty("SectionRings"      , m_sectionRings                        );
  declareProperty("NRings"            , m_nRings                              );
  declareProperty("Nodes"             , m_nodes                               );
  declareProperty("Weights"           , m_weights                             );
  declareProperty("Bias"              , m_bias                                );
  declareProperty("Thresholds"        , m_threshold                           );
  declareProperty("EtaBins"           , m_etaBins                             );
  declareProperty("EtBins"            , m_etBins                              );

  declareMonitoredVariable("NeuralNetworkOutput", m_output                    );
  m_key       = "";
  m_nDiscr    = 0;
  m_nPreproc  = 0;
}

//!===============================================================================================
HLT::ErrorCode TrigL2CaloRingerFex::hltInitialize() 
{
  ///What is the number of discriminators?
  m_nDiscr   = m_nodes.size()/SIZEOF_NODES;
  m_nPreproc = m_normRings.size()/SIZEOF_RINGSETS;


  ///check configuration
  if(m_weights.size() != m_nDiscr){
    msg() << MSG::ERROR << "Weight list dont match with the number of discriminators found" << endmsg;
    return HLT::BAD_JOB_SETUP;
  }

  if(m_bias.size() != m_nDiscr){
    msg() << MSG::ERROR << "Bias list dont match with the number of discriminators found" << endmsg;
    return HLT::BAD_JOB_SETUP;
  }

  if((m_etaBins.size() != m_nDiscr) || (m_etBins.size() != m_nDiscr)){
    msg() << MSG::ERROR << "Eta/Et list dont match with the number of discriminators found" << endmsg;
    return HLT::BAD_JOB_SETUP;
  }

  
  if(m_nRings.size() != m_normRings.size()){
    msg() << MSG::ERROR << "Preproc nRings list dont match with the number of discriminators found" << endmsg;
    return HLT::BAD_JOB_SETUP;
  }

  if(m_sectionRings.size() != m_normRings.size()){
    msg() << MSG::ERROR << "Preproc section rings list dont match with the number of discriminators found" << endmsg;
    return HLT::BAD_JOB_SETUP;
  }
  

  ///Initialize all discriminators
  for(unsigned i=0; i<m_nDiscr; ++i)
  {
    MultiLayerPerceptron   *discr   = nullptr;
    TrigRingerPreprocessor *preproc = nullptr;
    
    if ( msgLvl() <= MSG::INFO ) {
      msg() << MSG::INFO << "Create multi layer perceptron discriminator using configuration:" << endmsg; 
      msg() << MSG::INFO << "   Input layer   :   " << m_nodes[i*SIZEOF_NODES+0] << endmsg;
      msg() << MSG::INFO << "   Hidden layer  :   " << m_nodes[i*SIZEOF_NODES+1] << endmsg;
      msg() << MSG::INFO << "   Output layer  :   " << m_nodes[i*SIZEOF_NODES+2] << endmsg;
      msg() << MSG::INFO << "   Eta range     :   " << m_etaBins[i][0] << " < |eta|   <=" << m_etaBins[i][1] << endmsg;
      msg() << MSG::INFO << "   Et range      :   " << m_etBins[i][0] << "  < Et[GeV] <=" << m_etBins[i][1]  << endmsg;
    }
    try{
      ///Alloc discriminator
      ///TODO: find best way to parse this vector. The athena don't accept vector<vector<unsigned int>>
      std::vector<unsigned int> nodes(SIZEOF_NODES);
      for(unsigned k=0; k<SIZEOF_NODES;++k) nodes[k]= m_nodes[i*SIZEOF_NODES+k]; ///Parser
      
      discr = new MultiLayerPerceptron(nodes, m_weights[i], m_bias[i], 0,
                                       m_etBins[i][0], m_etBins[i][1], m_etaBins[i][0],
                                       m_etaBins[i][1]);
    }
    catch(std::bad_alloc xa){
      msg() << MSG::ERROR << "Weight vector size is not compatible with nodes vector." << endmsg;
      return HLT::BAD_JOB_SETUP;
    }
    catch(int e){
      if (e == BAD_WEIGHT_SIZE)
      {
        msg() << MSG::ERROR << "Weight vector size is not compatible with nodes vector." << endmsg;
        return HLT::BAD_JOB_SETUP;
      }
      if (e == BAD_BIAS_SIZE)
      {
        msg() << MSG::ERROR << "Bias vector size is not compatible with nodes vector." << endmsg;
        return HLT::BAD_JOB_SETUP;
      }
    }///try and catch alloc protection
    
    ///hold the pointer configuration
    m_discriminators.push_back(discr);

    try{
      ///TODO: find best way to parse this vector. The athena don't accept vector<vector<unsigned int>>
      std::vector<unsigned int> nrings(SIZEOF_RINGSETS), normrings(SIZEOF_RINGSETS), sectionrings(SIZEOF_RINGSETS);
      for(unsigned rs=0;rs<SIZEOF_RINGSETS;++rs){
        nrings[rs]=m_nRings[rs+i*SIZEOF_RINGSETS];
        normrings[rs]=m_normRings[rs+i*SIZEOF_RINGSETS];
        sectionrings[rs]=m_sectionRings[rs+i*SIZEOF_RINGSETS];
      }///parser

      preproc = new TrigRingerPreprocessor(nrings,normrings,sectionrings);
    }
    catch(std::bad_alloc xa){
      msg() << MSG::ERROR << "Bad alloc for TrigRingerPrepoc." << endmsg;
      return HLT::BAD_JOB_SETUP;
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

  if ( msgLvl() <= MSG::DEBUG )
    msg() << MSG::DEBUG << "TrigL2CaloRingerHypo initialization completed successfully." << endmsg;

  return HLT::OK;
}
//!===============================================================================================
HLT::ErrorCode TrigL2CaloRingerFex::hltFinalize() {  
  ///release memory
  for(unsigned i=0; i<m_nDiscr;++i){
    if(m_preproc.at(i))         delete m_preproc.at(i);
    if(m_discriminators.at(i))  delete m_discriminators.at(i);
  }///Loop over all discriminators and prepoc objects
  if ( msgLvl() <= MSG::DEBUG ) 
    msg() << MSG::DEBUG << "TrigL2CaloRingerHypo finalization completed successfully." << endmsg;
  return HLT::OK;
}
//!===============================================================================================
HLT::ErrorCode TrigL2CaloRingerFex::hltExecute(const HLT::TriggerElement* /*inputTE*/, HLT::TriggerElement* outputTE){

  m_output = 999;

  ///Retrieve rings pattern information
  const xAOD::TrigRingerRings *ringerShape = get_rings(outputTE);
  if(!ringerShape){
    msg() << MSG::WARNING << "Can not retrieve xADO::TrigRingerRings from storegate." << endmsg;
    return HLT::OK;
  }///protection

  ///Retrieve cluster information
  const xAOD::TrigEMCluster *emCluster = ringerShape->emCluster();

  ///Check if emCluster link exist
  if(!emCluster){
    msg() << MSG::WARNING << "Can not found the xAOD::TrigEMCluster link" << endmsg;
    return HLT::OK;
  }///protection

  if(msgLvl() <= MSG::DEBUG){
     msg() << MSG::DEBUG << "Event with roiword: 0x" << std::hex << emCluster->RoIword() << std::dec <<endmsg;
  }

  ///It's ready to select the correct eta/et bin
  MultiLayerPerceptron    *discr  = nullptr;
  TrigRingerPreprocessor  *preproc = nullptr;
  float eta = std::fabs(emCluster->eta());
  if(eta>2.50) eta=2.50;///fix for events out of the ranger

  float et  = emCluster->et()*1e-3; ///in GeV
  
  if(m_discriminators.size() > 0){
    if(doTiming())  m_decisionTimer->start();
    for(unsigned i=0; i<m_discriminators.size(); ++i){
      if(et > m_discriminators[i]->etmin() && et <= m_discriminators[i]->etmax()){
        if(eta > m_discriminators[i]->etamin() && eta <= m_discriminators[i]->etamax()){
          discr   = m_discriminators[i];
          preproc = m_preproc[i];
          break;
        }///eta conditions
      }///Et conditions
    }///Loop over discriminators

    ///get shape
    const std::vector<float> rings = ringerShape->rings();
    if(msgLvl() <= MSG::DEBUG)
      msg() << MSG::DEBUG << "ringerShape->rings().size() is: " <<rings.size() << endmsg;
 
    std::vector<float> refRings(rings.size());
    refRings.assign(rings.begin(), rings.end());

    if(msgLvl() <= MSG::DEBUG)
      msg() << MSG::DEBUG << "Et = " << et << " GeV, |eta| = " << eta << endmsg;

    ///pre-processing shape
    if(doTiming())  m_normTimer->start();
    if(preproc)     preproc->ppExecute(refRings);
    if(doTiming())  m_normTimer->stop();

    if(msgLvl() <= MSG::DEBUG)
      msg() << MSG::DEBUG << "after preproc refRings.size() is: " <<rings.size() << endmsg;
 
    ///Apply the discriminator
    if(discr)  m_output = discr->propagate(refRings);

  }else{
    if(msgLvl() <= MSG::DEBUG)
      msg() << MSG::DEBUG << "There is no discriminator into this Fex." << endmsg;
  }


  if(doTiming())  m_decisionTimer->stop();

  if(msgLvl() <= MSG::DEBUG)
    msg() << MSG::DEBUG << "Et = " << et << " GeV, |eta| = " << eta << " and rnnoutput = " << m_output << endmsg;

  if(doTiming())  m_storeTimer->start();
  ///Store outout information for monitoring and studys
  xAOD::TrigRNNOutput *rnnOutput = new xAOD::TrigRNNOutput();
  rnnOutput->makePrivateStore(); 
  rnnOutput->setRnnDecision(m_output);

  ///Get the ringer link to store into TrigRNNOuput  
  HLT::ErrorCode hltStatus;
  ElementLink<xAOD::TrigRingerRingsContainer> ringer_link;
  hltStatus = getFeatureLink<xAOD::TrigRingerRingsContainer,xAOD::TrigRingerRings>(outputTE, ringer_link);

  if( (hltStatus != HLT::OK) || (!ringer_link.isValid())){
    msg() << MSG::WARNING << "Failed to access ElementLink to TrigRingerRings" << endmsg;
  }else{
    rnnOutput->setRingerLink( ringer_link );
  }
  hltStatus = recordAndAttachFeature<xAOD::TrigRNNOutput>(outputTE, rnnOutput, m_key, m_hlt_feature);
  if(doTiming())  m_storeTimer->stop();

  if (hltStatus != HLT::OK) {
    msg() << MSG::WARNING << "Failed to record xAOD::TrigRNNOutput to StoreGate." << endmsg;
    return HLT::OK;
  }

  return HLT::OK;
}
//!===============================================================================================
