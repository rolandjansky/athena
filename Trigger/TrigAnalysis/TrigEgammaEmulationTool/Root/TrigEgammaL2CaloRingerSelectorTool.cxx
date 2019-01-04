/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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

using namespace std;
using namespace Trig;
//**********************************************************************

TrigEgammaL2CaloRingerSelectorTool::
TrigEgammaL2CaloRingerSelectorTool( const std::string& myname )
    : TrigEgammaSelectorBaseTool(myname),
      m_reader("TrigCaloRingerReader")
{
  declareProperty("NormalisationRings", m_normRings                           );
  declareProperty("SectionRings"      , m_sectionRings                        );
  declareProperty("NRings"            , m_nRings                              );
  declareProperty("CalibPathConstants", m_calibPath_constants=""              );
  declareProperty("CalibPathThresholds", m_calibPath_thresholds=""            );

  m_output    = 0;
  m_reader.setMsgStream(msg());
  m_useLumiVar=false;
  m_useEtaVar=false;

}
//**********************************************************************
StatusCode TrigEgammaL2CaloRingerSelectorTool::initialize() {

  StatusCode sc = TrigEgammaSelectorBaseTool::initialize();
  if(sc.isFailure()){
    ATH_MSG_WARNING("TrigEgammaSelectorBaseTool::initialize() failed");
    return StatusCode::FAILURE;
  }

  if(!m_calibPath_thresholds.empty()){
    if(!m_reader.retrieve(m_calibPath_thresholds, m_cutDefs)){
      ATH_MSG_ERROR("Can not retrieve the information from " << m_calibPath_thresholds);
      return StatusCode::FAILURE;
    }
    // retrieve metadata
    m_useNoActivationFunctionInTheLastLayer = m_reader.useNoActivationFunctionInTheLastLayer();
    m_doPileupCorrection = m_reader.doPileupCorrection();
    m_lumiCut  = m_reader.lumiCut();
  }

  // Retrieve the NeuralNetwork list
  if(!m_calibPath_constants.empty()){
    if(!m_reader.retrieve(m_calibPath_constants, m_discriminators)){
      ATH_MSG_ERROR("Can not retrieve all information from " << m_calibPath_constants);
      return StatusCode::FAILURE;
    }
    m_useLumiVar = m_reader.useLumiVar();
    m_useEtaVar  = m_reader.useEtaVar();
  }

  if(m_nRings.size() != m_normRings.size()){
    ATH_MSG_ERROR("Preproc nRings list dont match with the number of discriminators found");
    return StatusCode::FAILURE;
  }

  if(m_sectionRings.size() != m_normRings.size()){
    ATH_MSG_ERROR("Preproc section rings list dont match with the number of discriminators found");
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
  

  ATH_MSG_INFO("Using the Luminosity tool? " <<  (m_useLumiTool ? "Yes":"No") );
  ATH_MSG_INFO("Using lumiVar?             " <<  (m_useLumiVar ? "Yes":"No") );
  ATH_MSG_INFO("Using etaVar?              " <<  (m_useEtaVar ? "Yes":"No") );
  ATH_MSG_INFO("Using the activation function in the last layer? " <<  (m_useNoActivationFunctionInTheLastLayer ? "No":"Yes") );
  ATH_MSG_INFO("Using the Correction?                            " <<  (m_doPileupCorrection ? "Yes":"No") );
  ATH_MSG_INFO("Using lumi threshold equal: "  <<  m_lumiCut);
  ATH_MSG_INFO("TrigL2CaloRingerHypo initialization completed successfully." );
  return StatusCode::SUCCESS;
}

//!==========================================================================
StatusCode TrigEgammaL2CaloRingerSelectorTool::finalize() {
  
  //release memory
  for(unsigned i=0; i<m_discriminators.size();++i){
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
  m_output = -999;
  emCluster->auxdecor<float>("TrigEgammaL2CaloRingerSelectorTool_rnnOutput")=m_output;
  
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
          refRings.push_back(preproc->normalize_eta(emCluster->eta(), discr->etamin(), discr->etamax()));
        }
      }

      if(m_useLumiVar){
        if(preproc){
          refRings.push_back(preproc->normalize_mu(avgmu, m_lumiCut));
        }
      }

      m_output = discr->propagate(refRings);
      if(m_useNoActivationFunctionInTheLastLayer){
        // overwrite the output value to before the tansig function
        m_output = discr->getOutputBeforeTheActivationFunction();
      }

      //Apply cut
      for(unsigned i=0; i < m_cutDefs.size(); ++i){
        if((et  > m_cutDefs[i]->etmin()) && (et  <= m_cutDefs[i]->etmax())){
          if((eta > m_cutDefs[i]->etamin()) && (eta <= m_cutDefs[i]->etamax())){
            
            if(m_doPileupCorrection){
              // Limited Pileup
              if(avgmu>m_lumiCut)
                avgmu=m_lumiCut;
              ATH_MSG_DEBUG("Apply avgmu == " << avgmu);
              threshold = m_cutDefs[i]->threshold(avgmu);
              ATH_MSG_DEBUG("With correction, thr = "<<threshold);
            }else{
              threshold = m_cutDefs[i]->threshold();
              ATH_MSG_DEBUG("Without correction, thr = "<<threshold);
            }
          }
        }
      }// Loop over cutDefs
      
      emCluster->auxdecor<float>("TrigEgammaL2CaloRingerSelectorTool_rnnOutput")=m_output;

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

