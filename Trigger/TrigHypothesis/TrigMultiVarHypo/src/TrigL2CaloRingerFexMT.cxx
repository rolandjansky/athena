/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/


#include "xAODTrigCalo/TrigEMClusterContainer.h"
#include "xAODTrigCalo/TrigEMClusterAuxContainer.h"
#include "AthenaMonitoring/MonitoredScope.h"

#include "TrigMultiVarHypo/TrigL2CaloRingerFexMT.h"
#include "TrigMultiVarHypo/preprocessor/TrigRingerPreprocessor.h"
#include "TrigMultiVarHypo/discriminator/MultiLayerPerceptron.h"

///xAOD include(s)
#include "xAODTrigCalo/TrigEMCluster.h"
#include "xAODTrigRinger/TrigRNNOutput.h"
#include "xAODTrigRinger/TrigRNNOutputContainer.h"
#include "xAODTrigRinger/TrigRingerRings.h"
#include "xAODTrigRinger/TrigRingerRingsContainer.h"



#define SIZEOF_NODES            3
#define SIZEOF_RINGSETS         7


    TrigL2CaloRingerFexMT:: TrigL2CaloRingerFexMT(const std::string & name, ISvcLocator* pSvcLocator)
        :AthAlgorithm(name, pSvcLocator){

                ATH_MSG_DEBUG( "start RingerMT const:" );     
	} TrigL2CaloRingerFexMT:: ~TrigL2CaloRingerFexMT(){}


StatusCode TrigL2CaloRingerFexMT::initialize(){

  ATH_MSG_DEBUG( "start RingerMT init:" );     

  ///What is the number of discriminators?
  m_nDiscr   = m_nodes.size()/SIZEOF_NODES;
  m_nPreproc = m_normRings.size()/SIZEOF_RINGSETS;

  ATH_CHECK(m_ringerShape.initialize());
  //ATH_CHECK(m_TrigRingerRingsContainer.initialize());
  ATH_CHECK(m_outputKey.initialize());

  if(configurationInvalid()){
	return StatusCode::FAILURE;
  }
  ///Initialize all discriminators
  for(unsigned i=0; i<m_nDiscr; ++i)
  {
    MultiLayerPerceptron   *discr   = nullptr;
    TrigRingerPreprocessor *preproc = nullptr;

    ATH_MSG_INFO("Create multi layer perceptron discriminator using configuration:");     
    ATH_MSG_INFO( "   Input layer   :   " << m_nodes[i*SIZEOF_NODES+0] );
    ATH_MSG_INFO( "   Output layer  :   " << m_nodes[i*SIZEOF_NODES+2] );
    ATH_MSG_INFO( "   Eta range     :   " << m_etaBins[i][0] << " < |eta|   <=" << m_etaBins[i][1] );
    ATH_MSG_INFO( "   Et range      :   " << m_etBins[i][0] << "  < Et[GeV] <=" << m_etBins[i][1] );

   
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
      ATH_MSG_ERROR( "Weight vector size is not compatible with nodes vector." );
      return StatusCode::FAILURE;
    }
    catch(int e){
      if (e == BAD_WEIGHT_SIZE)
      { 
        ATH_MSG_ERROR( "Weight vector size is not compatible with nodes vector." );
	return StatusCode::FAILURE;
      }
      if (e == BAD_BIAS_SIZE)
      {
        
        ATH_MSG_ERROR( "Bias vector size is not compatible with nodes vector." );
	return StatusCode::FAILURE;
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
     
      ATH_MSG_ERROR(  "Bad alloc for TrigRingerPrepoc." );
      return StatusCode::FAILURE;
    }

    ///Hold the pointer configuration
    m_preproc.push_back(preproc);
  }///Loop over discriminators

    ATH_MSG_DEBUG( "TrigL2CaloRingerHypo initialization completed successful" );

  return StatusCode::SUCCESS;
}


StatusCode TrigL2CaloRingerFexMT::finalize(){
    ///release memory
    for(unsigned i=0; i<m_nDiscr;++i){
      if(m_preproc.at(i))         delete m_preproc.at(i);
      if(m_discriminators.at(i))  delete m_discriminators.at(i);
     }///Loop over all discriminators and prepoc objects

    ATH_MSG_DEBUG( "TrigL2CaloRingerHypo finalization completed successfully." );
    return StatusCode::SUCCESS;
}

StatusCode TrigL2CaloRingerFexMT::execute(){
  ATH_MSG_DEBUG("start RingerMT");
  
  m_output = 999;
  auto ctx = getContext();

  ///Retrieve rings pattern information
  auto ringerShapeHandle = SG::makeHandle(m_ringerShape, ctx);
  const xAOD::TrigRingerRings * ringerShape =  ringerShapeHandle.cptr();
  //G::ReadHandle<xAOD::TrigRingerRings_v2>
 
  
  //It's ready to select the correct eta/et bin
  ATH_MSG_DEBUG("executing"); 

  MultiLayerPerceptron    *discr  = nullptr;
  TrigRingerPreprocessor  *preproc = nullptr;
  
  //TODO read parameters from EM Cluster
  //float eta = std::fabs(emCluster->eta());
  //if(eta>2.50) eta=2.50;///fix for events out of the ranger
  //float et  = emCluster->et()*1e-3; ///in GeV
  
  float et = 10;
  float eta = 1;
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

    ///get shape
    const std::vector<float> rings = ringerShape->rings();
     
    ATH_MSG_DEBUG( "ringerShape->rings().size() is: " <<rings.size() );

    std::vector<float> refRings(rings.size());
    refRings.assign(rings.begin(), rings.end());

    
    ATH_MSG_DEBUG("Et = " << et << " GeV, |eta| = " << eta);

    ///pre-processing shape
    if(preproc)     preproc->ppExecute(refRings);

    ATH_MSG_DEBUG("after preproc refRings.size() is: " << rings.size());

    ///Apply the discriminator
    if(discr)  m_output = discr->propagate(refRings);
  }else{
     ATH_MSG_DEBUG("There is no discriminator into this Fex.");
  }


  ATH_MSG_DEBUG( "Et = " << et << " GeV, |eta| = " << eta << " and rnnoutput = " << m_output );
  ///Store outout information for monitoring and studys
  std::unique_ptr<xAOD::TrigRNNOutput> rnnOutput( new xAOD::TrigRNNOutput());
  rnnOutput->makePrivateStore();
  rnnOutput->setRnnDecision(m_output);


  auto outputHandle =   SG::makeHandle (m_outputKey, ctx);  
  ATH_CHECK( outputHandle.record(std::move(rnnOutput)) );


  return StatusCode::SUCCESS;
}

bool TrigL2CaloRingerFexMT::configurationInvalid(){

  if(m_weights.size() != m_nDiscr){
    ATH_MSG_ERROR( "Weight list dont match with the number of discriminators found" );
    return true;
  }

  if(m_bias.size() != m_nDiscr){
    ATH_MSG_ERROR( "Bias list dont match with the number of discriminators found" );
    return true;
  }

  if((m_etaBins.size() != m_nDiscr) || (m_etBins.size() != m_nDiscr)){ 
    ATH_MSG_ERROR( "Eta/Et list dont match with the number of discriminators found" );
    return true;
  }


  if(m_nRings.size() != m_normRings.size()){ 
    ATH_MSG_ERROR( "Preproc nRings list dont match with the number of discriminators found" );
    return true;
  }

  if(m_sectionRings.size() != m_normRings.size()){
    ATH_MSG_ERROR("Preproc section rings list dont match with the number of discriminators found");
    return true;	
  }

  return false;
	
}
