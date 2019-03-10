/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///Local include(s)
#include "TrigMultiVarHypo/TrigL2CaloRingerFex.h"
#include "xAODTrigCalo/TrigEMCluster.h"
#include "xAODTrigRinger/TrigRNNOutput.h"
#include "xAODTrigRinger/TrigRNNOutputContainer.h"
#include "xAODTrigRinger/TrigRingerRings.h"
#include "xAODTrigRinger/TrigRingerRingsContainer.h"

#include "TrigMultiVarHypo/tools/procedures/IModel.h"
#include "TrigMultiVarHypo/tools/procedures/MultiLayerPerceptron.h"
#include "TrigMultiVarHypo/tools/procedures/Norm1.h"


///std include(s)
#include <new>
#include <cmath>
#include <iostream>
using namespace std;

//!===============================================================================================
TrigL2CaloRingerFex::TrigL2CaloRingerFex(const std::string& name, ISvcLocator* pSvcLocator):
  HLT::FexAlgo(name, pSvcLocator), 
  m_lumiBlockMuTool("LumiBlockMuTool/LumiBlockMuTool"),
  m_reader("TrigL2CaloRingerReader")
{  
  declareProperty("HltFeature"        , m_hlt_feature = "TrigRingerNeuralFex" );  
  declareProperty("Feature"           , m_feature = "TrigT2CaloEgamma"        );  
  declareProperty("CalibPath"         , m_calibPath = ""                      );
  declareProperty("LuminosityTool"    , m_lumiBlockMuTool, "Luminosity Tool"  );
  
  m_key       = "";
}

//!===============================================================================================
HLT::ErrorCode TrigL2CaloRingerFex::hltInitialize() 
{
  // Retrieve the NeuralNetwork list
  if(!m_calibPath.empty()){
    if(!m_reader.retrieve(m_calibPath, m_discriminators)){
      ATH_MSG_ERROR("Can not retrieve all information from " << m_calibPath);
      return HLT::BAD_JOB_SETUP;
    }
  }

  ///Monitoring hitograms
  if(doTiming()){
    m_totalTimer    = addTimer("Total");
    m_normTimer     = addTimer("Normalization");
    m_propagateTimer= addTimer("Propagation");
    m_storeTimer    = addTimer("StoreOutput");
  }///Only if time is set on python config

  m_useLumiTool=false;
  if (m_lumiBlockMuTool.retrieve().isFailure()) {
    ATH_MSG_WARNING("Unable to retrieve Luminosity Tool");
  } else {
    m_useLumiTool=true; // Tell to the execute that the LumiTool is available...
    ATH_MSG_INFO("Successfully retrieved Luminosity Tool");
  }

  // NOTE: Norm1 standard normalization used for Run2 (only for ringer shape)
  m_preproc.push_back( std::make_shared<Ringer::Norm1>(-999.,999.,-999,999,-999,999.,true) );

  ATH_MSG_INFO("Using the Luminosity tool? " <<  (m_useLumiTool ? "Yes":"No") );
  ATH_MSG_INFO( "TrigL2CaloRingerHypo initialization completed successfully." );

  return HLT::OK;
}
//!===============================================================================================
HLT::ErrorCode TrigL2CaloRingerFex::hltFinalize() {  
  ATH_MSG_INFO( "TrigL2CaloRingerHypo finalization completed successfully." );
  return HLT::OK;
}
//!===============================================================================================
HLT::ErrorCode TrigL2CaloRingerFex::hltExecute(const HLT::TriggerElement* /*inputTE*/, HLT::TriggerElement* outputTE){

  if(doTiming())  m_totalTimer->start();
  // For now, this must be [avgmu, rnnOutputWithTansig, rnnOutputWithoutTansig] 
  m_output=-999;
  std::vector<float> output;

  ///Retrieve rings pattern information
  const xAOD::TrigRingerRings *ringerShape = get_rings(outputTE);
  if(!ringerShape){
    ATH_MSG_WARNING( "Can not retrieve xADO::TrigRingerRings from storegate." );
    if(doTiming())  m_totalTimer->stop();
    return HLT::OK;
  }///protection

  ///Retrieve cluster information
  const xAOD::TrigEMCluster *emCluster = ringerShape->emCluster();

  ///Check if emCluster link exist
  if(!emCluster){
    ATH_MSG_WARNING( "Can not found the xAOD::TrigEMCluster link" );
    if(doTiming())  m_totalTimer->stop();
    return HLT::OK;
  }///protection

  ATH_MSG_DEBUG( "Event with roiword: 0x" << std::hex << emCluster->RoIword() << std::dec );



  std::shared_ptr<Ringer::IModel> discr;

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


  
  if(m_discriminators.size() > 0){
      
    for(unsigned i=0; i<m_discriminators.size(); ++i){
      if(avgmu > m_discriminators[i]->mumin() && avgmu <= m_discriminators[i]->mumax()){
        if(et > m_discriminators[i]->etmin() && et <= m_discriminators[i]->etmax()){
          if(eta > m_discriminators[i]->etamin() && eta <= m_discriminators[i]->etamax()){
            discr   = m_discriminators[i];
            break;
          }// mu conditions
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
      m_preproc.at(0)->execute(refRings);
      if(doTiming())  m_normTimer->stop();    
      if(doTiming())  m_propagateTimer->start();    
      auto answer = discr->propagate(refRings);
      if(doTiming())  m_propagateTimer->stop();    
      output.push_back(answer.output);
      output.push_back(answer.outputBeforeTheActivationFunction);
    }// has discr?
  }else{
    ATH_MSG_DEBUG( "There is no discriminator into this Fex." );
  }



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
  if(doTiming()){
    m_storeTimer->stop();
    m_totalTimer->stop();
  }
  if (hltStatus != HLT::OK) {
    ATH_MSG_WARNING( "Failed to record xAOD::TrigRNNOutput to StoreGate." );
    return HLT::OK;
  }

  return HLT::OK;
}
//!===============================================================================================
