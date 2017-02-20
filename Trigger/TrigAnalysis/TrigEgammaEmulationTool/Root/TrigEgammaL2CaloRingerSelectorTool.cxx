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
#include <math.h>
#define SIZEOF_NODES            3
#define SIZEOF_RINGSETS         7

using namespace std;
using namespace Trig;
//**********************************************************************

TrigEgammaL2CaloRingerSelectorTool::
TrigEgammaL2CaloRingerSelectorTool( const std::string& myname )
    : TrigEgammaSelectorBaseTool(myname)
{
  declareProperty("NormalisationRings", m_normRings                           );
  declareProperty("SectionRings"      , m_sectionRings                        );
  declareProperty("NRings"            , m_nRings                              );
  declareProperty("Nodes"             , m_nodes                               );
  declareProperty("Weights"           , m_weights                             );
  declareProperty("Bias"              , m_bias                                );
  declareProperty("EtaBins"           , m_etaBins                             );
  declareProperty("EtBins"            , m_etBins                              );
  declareProperty("Thresholds"        , m_thresholds                          );
  declareProperty("ThresholdEtaBins"  , m_thr_etaBins                         );
  declareProperty("ThresholdEtBins"   , m_thr_etBins                         );
  declareProperty("LuminosityCut"     , m_lumiCut=60                          );
  declareProperty("UseEtaVar"         , m_useEtaVar=false                     );  
  declareProperty("UseLumiVar"        , m_useLumiVar=false                    );  
  declareProperty("DoPileupCorrection", m_doPileupCorrection=false            );
  declareProperty("UseNoActivationFunctionInTheLastLayer", m_useNoActivationFunctionInTheLastLayer=false);

  m_nDiscr    = 0;
  m_nPreproc  = 0;
  m_output    = 0;

}
//**********************************************************************
StatusCode TrigEgammaL2CaloRingerSelectorTool::initialize() {

  StatusCode sc = TrigEgammaSelectorBaseTool::initialize();
  if(sc.isFailure()){
    ATH_MSG_WARNING("TrigEgammaSelectorBaseTool::initialize() failed");
    return StatusCode::FAILURE;
  }

  //What is the number of discriminators?
  m_nDiscr   = m_nodes.size()/SIZEOF_NODES;
  m_nPreproc = m_normRings.size()/SIZEOF_RINGSETS;

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

  if(m_nRings.size() != m_normRings.size()){
    ATH_MSG_ERROR("Preproc nRings list dont match with the number of discriminators found" );
    return StatusCode::FAILURE;
  }

  if(m_sectionRings.size() != m_normRings.size()){
    ATH_MSG_ERROR("Preproc section rings list dont match with the number of discriminators found" );
    return StatusCode::FAILURE;
  }

  if((m_thr_etaBins.size() != m_thresholds.size()) || (m_thr_etBins.size() != m_thresholds.size())){
    ATH_MSG_ERROR("Eta/Et list dont match with the number of thresholds found" );
    return StatusCode::FAILURE;
  }

  ATH_MSG_INFO("Using the activation function in the last layer? " <<  (m_useNoActivationFunctionInTheLastLayer ? "No":"Yes") );
  ATH_MSG_INFO("Using the Correction?                            " <<  (m_doPileupCorrection ? "Yes":"No") );
  
  ///Initialize all discriminators
  for(unsigned i=0; i<m_nDiscr; ++i)
  {
    MultiLayerPerceptron   *discr              = nullptr;
    TrigRingerPreprocessor *preproc            = nullptr;
   
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
     
      discr = new MultiLayerPerceptron(nodes, m_weights[i], m_bias[i],
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

  // initialize the cuts...
  for(unsigned i=0; i<m_thresholds.size(); ++i)
  {
    ///Hold the pointer configuration
    try{
      m_cutDefs.push_back(new TrigCaloRingsHelper::CutDefsHelper(m_thresholds[i],m_etaBins[i][0],/*
                                                  */m_etaBins[i][1], m_etBins[i][0],m_etBins[i][1]));
    }catch(std::bad_alloc xa){
      msg() << MSG::ERROR << "Can not alloc cutDefs on memory." << endreq;
      return StatusCode::FAILURE;
    }
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

  for(unsigned i=0; i < m_cutDefs.size();++i){
    if(m_cutDefs.at(i))         delete m_cutDefs.at(i);
  }//Loop over all cutDefs objects

  ATH_MSG_DEBUG("TrigL2CaloRingerHypo finalization completed successfully.");
  return StatusCode::SUCCESS;
}

//!==========================================================================

bool TrigEgammaL2CaloRingerSelectorTool::emulation(const xAOD::TrigEMCluster* emCluster, bool &pass, const Trig::Info &info)
{

  pass = false;

  if(!emCluster){
    ATH_MSG_DEBUG("emCluster is nullptr");
    return false;
  }

  //retrieve rings
  const xAOD::TrigRingerRings *ringer = getTrigCaloRings(emCluster);
  
  if(!ringer){
    ATH_MSG_DEBUG("Can not found TrigRingerRings object matched with the current TrigEMCluster.");
    return false;
  }//protection

  setEtThr((info.thrHLT-3));
  m_output = -999;

  ///It's ready to select the correct eta/et bin
  MultiLayerPerceptron    *discr  = nullptr;
  TrigRingerPreprocessor  *preproc = nullptr;

  float eta = std::fabs(emCluster->eta());
  if(eta>2.50) eta=2.50;///fix for events out of the ranger
  float et  = emCluster->et()*1e-3; ///in GeV

  ATH_MSG_DEBUG("Et = " << et << " <  EtCut = "<< m_etCut);
  if(et < m_etCut){
    ATH_MSG_DEBUG("Event reproved by Et threshold. Et = " << et << ", EtCut = "<< m_etCut);
    return true;
  }

  if(m_discriminators.size() > 0){
    unsigned discr_index=0;
    for(unsigned i=0; i<m_discriminators.size(); ++i){
      discr_index++;
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
    if(discr){

      float avgmu = getOnlAverageMu();
      float threshold=0.0;
      
      // Add extra variables in this order! Do not change this!!!
      if(m_useEtaVar){
        if(preproc){
          ATH_MSG_INFO( "AKI JOAO, ETA = "<<preproc->normalize_eta(emCluster->eta(), discr->etamin(), discr->etamax()));
          refRings.push_back(preproc->normalize_eta(emCluster->eta(), discr->etamin(), discr->etamax()));
        }
      }

      if(m_useLumiVar){
        if(preproc){
          ATH_MSG_INFO( "AKI JOAO, MU = "<<preproc->normalize_mu(avgmu, m_lumiCut));
          refRings.push_back(preproc->normalize_mu(avgmu, m_lumiCut));
        }
      }

      m_output = discr->propagate(refRings);
      if(m_useNoActivationFunctionInTheLastLayer){
        // overwrite the output value to before the tansig function
        m_output = discr->getOutputBeforeTheActivationFunction();
      }

      // Hold this value into the store
      //(*m_decorations)["TrigEgammaL2CaloRingerSelectorTool__NNOutput"] = m_output;

      //Apply cut
      for(unsigned i=0; i < m_cutDefs.size(); ++i){
        if((et  > m_cutDefs[i]->etmin()) && (et  <= m_cutDefs[i]->etmax())){
          if((eta > m_cutDefs[i]->etamin()) && (eta <= m_cutDefs[i]->etamax())){
            
            if(m_doPileupCorrection){
              // Limited Pileup
              if(avgmu>m_lumiCut)
                avgmu=m_lumiCut;
              ATH_MSG_INFO("Apply avgmu == " << avgmu);
              threshold = m_cutDefs[i]->threshold(avgmu);
            }else{
              threshold = m_cutDefs[i]->threshold();
              ATH_MSG_INFO("AKI JOAO, thr = "<<threshold);
            }
          }
        }
      }// Loop over cutDefs
      
      ATH_MSG_INFO("AKI JOAO, out = "<< m_output);

      if(m_output < threshold){
        ATH_MSG_DEBUG("Event reproved by discriminator.");
        return true;
      }
    }
  }else{
    ATH_MSG_DEBUG("There is no discriminator into this Fex.");
  }//

  ATH_MSG_DEBUG("Et = " << et << " GeV, |eta| = " << eta << " and rnnoutput = " << m_output );
  pass=true;
  return true;
}
//!===========================================================================

