/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************
 * AsgTool: TrigEgammaL2CaloRingerSelectorTool
 * Authors:
 *      Ryan Mackenzie White <ryan.white@cern.ch>
 *      Joao Victor Pinto    <jodafons@cern.ch>
 * Date: Feb 2015
 * Description:
 *
 **********************************************************************/
#include "TrigEgammaEmulationTool/TrigEgammaL2CaloRingerSelectorTool.h"
#include <boost/foreach.hpp>
#include <boost/tokenizer.hpp>
#include "boost/algorithm/string.hpp"
#include <boost/lexical_cast.hpp>
#include <cmath>
#define SIZEOF_NODES            3
#define SIZEOF_RINGSETS         7

using namespace std;
using namespace Trig;
//**********************************************************************

TrigEgammaL2CaloRingerSelectorTool::
TrigEgammaL2CaloRingerSelectorTool( const std::string& myname )
    : TrigEgammaSelectorBaseTool(myname)
{
  declareProperty("Signature"         , m_signature                           );
  declareProperty("Pidname"           , m_pidname                             );
  declareProperty("EtCut"             , m_etCut                               );
  declareProperty("NormalisationRings", m_normRings                           );
  declareProperty("SectionRings"      , m_sectionRings                        );
  declareProperty("NRings"            , m_nRings                              );
  declareProperty("Nodes"             , m_nodes                               );
  declareProperty("Weights"           , m_weights                             );
  declareProperty("Bias"              , m_bias                                );
  declareProperty("Thresholds"        , m_threshold                           );
  declareProperty("EtaBins"           , m_etaBins                             );
  declareProperty("EtBins"            , m_etBins                              );


  m_nDiscr    = 0;
  m_nPreproc  = 0;
}
//**********************************************************************
StatusCode TrigEgammaL2CaloRingerSelectorTool::initialize() {

  boost::algorithm::to_lower(m_signature);
  //What is the number of discriminators?
  m_nDiscr   = m_nodes.size()/SIZEOF_NODES;
  m_nPreproc = m_normRings.size()/SIZEOF_RINGSETS;
  m_str_etthr = boost::lexical_cast<std::string>(m_etCut/1e-3);

  ///check configuration
  if(m_weights.size() != m_nDiscr){
    ATH_MSG_ERROR("Weight list dont match with the number of discriminators found" );
    return StatusCode::FAILURE;
  }

  if(m_bias.size() != m_nDiscr){
    ATH_MSG_ERROR("Bias list dont match with the number of discriminators found" );
    return StatusCode::FAILURE;
  }

  if((m_etaBins.size() != m_nDiscr) || (m_etBins.size() != m_nDiscr)){
    ATH_MSG_ERROR("Eta/Et list dont match with the number of discriminators found" );
    return StatusCode::FAILURE;
  }

  if((m_threshold.size() != m_nDiscr)){
    ATH_MSG_ERROR("Threshold list dont match with the number of discriminators found" );
    return StatusCode::FAILURE;
  }


 
  if(m_nRings.size() != m_normRings.size()){
    ATH_MSG_ERROR("Preproc nRings list dont match with the number of discriminators found" );
    return StatusCode::FAILURE;
  }

  if(m_sectionRings.size() != m_normRings.size()){
    ATH_MSG_ERROR("Preproc section rings list dont match with the number of discriminators found" );
    return StatusCode::FAILURE;
  }
 

  ///Initialize all discriminators
  for(unsigned i=0; i<m_nDiscr; ++i)
  {
    MultiLayerPerceptron   *discr   = nullptr;
    TrigRingerPreprocessor *preproc = nullptr;
   
    ATH_MSG_INFO("Create multi layer perceptron discriminator using configuration:" );
    ATH_MSG_INFO("   Input layer   :   " << m_nodes[i*SIZEOF_NODES+0] );
    ATH_MSG_INFO("   Hidden layer  :   " << m_nodes[i*SIZEOF_NODES+1] );
    ATH_MSG_INFO("   Output layer  :   " << m_nodes[i*SIZEOF_NODES+2] );
    ATH_MSG_INFO("   Eta range     :   " << m_etaBins[i][0] << " < |eta|   <=" << m_etaBins[i][1] );
    ATH_MSG_INFO("   Et range      :   " << m_etBins[i][0] << "  < Et[GeV] <=" << m_etBins[i][1]  );
    try{
      ///Alloc discriminator
      ///TODO: find best way to parse this vector. The athena don't accept vector<vector<unsigned int>>
      std::vector<unsigned int> nodes(SIZEOF_NODES);
      for(unsigned k=0; k<SIZEOF_NODES;++k) nodes[k]= m_nodes[i*SIZEOF_NODES+k]; ///Parser
     
      discr = new MultiLayerPerceptron(nodes, m_weights[i], m_bias[i], m_threshold[i],
                                       m_etBins[i][0], m_etBins[i][1], m_etaBins[i][0],
                                       m_etaBins[i][1]);

    }
    catch(std::bad_alloc xa){
      ATH_MSG_ERROR("Weight vector size is not compatible with nodes vector." );
      return StatusCode::FAILURE;
    }
    catch(int e){
      if (e == BAD_WEIGHT_SIZE)
      {
        ATH_MSG_ERROR("Weight vector size is not compatible with nodes vector." );
        return StatusCode::FAILURE;
      }
      if (e == BAD_BIAS_SIZE)
      {
        ATH_MSG_ERROR("Bias vector size is not compatible with nodes vector." );
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

      ATH_MSG_INFO("Creating pre-processing tool..." );
      preproc = new TrigRingerPreprocessor(nrings,normrings,sectionrings);
    }
    catch(std::bad_alloc xa){
      ATH_MSG_ERROR("Bad alloc for TrigRingerPrepoc." );
      return StatusCode::FAILURE;
    }

    ///Hold the pointer configuration
    m_preproc.push_back(preproc);
  }///Loop over discriminators
  
  ATH_MSG_INFO("TrigL2CaloRingerHypo initialization completed successfully." );
  return StatusCode::SUCCESS;
}

//!==========================================================================
StatusCode TrigEgammaL2CaloRingerSelectorTool::finalize() {
  
  //release memory
  for(unsigned i=0; i<m_nDiscr;++i){
    if(m_preproc.at(i))         delete m_preproc.at(i);
    if(m_discriminators.at(i))  delete m_discriminators.at(i);
  }//Loop over all discriminators and prepoc objects

  ATH_MSG_DEBUG("TrigL2CaloRingerHypo finalization completed successfully.");
  return StatusCode::SUCCESS;
}

//!==========================================================================
bool TrigEgammaL2CaloRingerSelectorTool::is_correct_trigger(const std::string &trigger){
  
  std::string trigItem = trigger;
  trigItem.erase(0, 4); //Removes HLT_ prefix from name
  bool isL1 = false;
  bool perf = false;
  bool etcut = false;
  bool ringer=false;
  float etthr = 0;
  float l1thr = 0;
  std::string type = "";
  std::string l1type = "";
  std::string pidname = "";
  if (boost::contains(trigItem,"ringer")) ringer = true;
  if(!ringer)  return false;
  parseTriggerName(trigItem,"Loose",isL1,type,etthr,l1thr,l1type,pidname,etcut,perf); 
  boost::algorithm::to_lower(pidname);
  std::string str_etthr = boost::lexical_cast<std::string>(etthr);
  if(m_str_etthr != str_etthr)  return false;
  if(pidname != m_pidname)  return false;
  if(type != m_signature)  return false;
  return true;
}
//!==========================================================================
bool TrigEgammaL2CaloRingerSelectorTool::emulation(const xAOD::TrigEMCluster* emCluster, bool &pass, const std::string &trigger)
{
  pass = false;

  if(!emCluster){
    ATH_MSG_DEBUG("emCluster is nullptr");
    return false;
  }

  if(!is_correct_trigger(trigger)){
    return false;
  }//is good tool?

  //for(const auto emCluster : *container){
  //retrieve rings
  const xAOD::TrigRingerRings *ringer = getTrigCaloRings(emCluster);
  
  if(!ringer){
    ATH_MSG_DEBUG("Can not found TrigRingerRings object matched with the current TrigEMCluster.");
    return false;
  }//protection

  m_output = 999;

  ///It's ready to select the correct eta/et bin
  MultiLayerPerceptron    *discr  = nullptr;
  TrigRingerPreprocessor  *preproc = nullptr;
  
  float eta = std::fabs(emCluster->eta());
  if(eta>2.50) eta=2.50;///fix for events out of the ranger
  float et  = emCluster->et()*1e-3; ///in GeV

  if(et < m_etCut){
    ATH_MSG_DEBUG("Event reproved by Et threshold. Et = " << et << ", EtCut = "<< m_etCut);
    return true;
  }

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
    const std::vector<float> rings = ringer->rings();
    ATH_MSG_DEBUG( "ringer->rings().size() is: " <<rings.size());
    std::vector<float> refRings(rings.size());
    refRings.assign(rings.begin(), rings.end());
    ATH_MSG_DEBUG("Et = " << et << " GeV, |eta| = " << eta);
    
    if(preproc)     preproc->ppExecute(refRings);

    ///Apply the discriminator
    if(discr)  m_output = discr->propagate(refRings);

    //Apply cut
    if(m_output < discr->threshold()){
      ATH_MSG_DEBUG("Event reproved by discriminator.");
      return true;
    }

  }else{
    ATH_MSG_DEBUG("There is no discriminator into this Fex.");
  }//

  ATH_MSG_DEBUG("Et = " << et << " GeV, |eta| = " << eta << " and rnnoutput = " << m_output );
  pass=true;
  return true;
}
//!==========================================================================
