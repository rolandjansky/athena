/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************
 * AsgTool: TrigEgammaL2SelectorTool
 * Authors:
 *      Ryan Mackenzie White <ryan.white@cern.ch>
 *      Joao Victor Pinto    <jodafons@cern.ch>
 * Date: Feb 2015
 * Description:
 *
 **********************************************************************/
#include "TrigEgammaEmulationTool/TrigEgammaL2SelectorTool.h"
#include <boost/foreach.hpp>
#include <boost/tokenizer.hpp>
#include "boost/algorithm/string.hpp"

using namespace std;
using namespace Trig;
//**********************************************************************

TrigEgammaL2SelectorTool::
TrigEgammaL2SelectorTool( const std::string& myname )
    : TrigEgammaSelectorBaseTool(myname)
{
  declareProperty("CaloSelectors"         , m_caloSelectors        );
  declareProperty("CaloRingerSelectors"   , m_caloRingerSelectors  );
  declareProperty("ElectronSelectors"     , m_electronSelectors    );
}
//**********************************************************************
StatusCode TrigEgammaL2SelectorTool::initialize() {

  //StatusCode sc;
  for(unsigned i=0; i<m_caloSelectors.size(); ++i){
    m_caloSelectors[i]->setParents(m_trigdec, m_storeGate);
    /*
    //sc = m_caloSelectors[i]->initialize();
    if(sc.isFailure()){
      ATH_MSG_ERROR( "Unable to initialize L2 selector tool." );
      return sc;
    }*/
  }

  ATH_MSG_INFO("Initialising L2CaloRingerSelector tool...");
  for(unsigned i=0; i<m_caloRingerSelectors.size(); ++i){
    m_caloRingerSelectors[i]->setParents(m_trigdec, m_storeGate);
    /*
    //sc = m_caloRingerSelectors[i]->initialize();
    if(sc.isFailure()){
      ATH_MSG_ERROR( "Unable to initialize L2 CaloRings selector tool." );
      return sc;
    }*/
  }

  ATH_MSG_INFO("Initialising L2ElectronSelector tool...");
  for(unsigned i=0; i<m_electronSelectors.size(); ++i){
    m_electronSelectors[i]->setParents(m_trigdec, m_storeGate);
    /*
    //sc = m_caloRingerSelectors[i]->initialize();
    if(sc.isFailure()){
      ATH_MSG_ERROR( "Unable to initialize L2 CaloRings selector tool." );
      return sc;
    }*/
  }


  return StatusCode::SUCCESS;
}

//!==========================================================================
StatusCode TrigEgammaL2SelectorTool::finalize() {
  /*
  StatusCode sc;
  ATH_MSG_INFO("Finalising L2Selector tool...");
  for(unsigned i=0; i<m_caloSelectors.size(); ++i){
    //sc = m_caloSelectors[i]->finalize();
    if(sc.isFailure()){
      ATH_MSG_ERROR( "Unable to finalize L2 selector tool." );
      return sc;
    }
  }
   
  ATH_MSG_INFO("Finalising L2CaloRingerSelector tool...");
  for(unsigned i=0; i<m_caloRingerSelectors.size(); ++i){
    //sc = m_caloRingerSelectors[i]->finalize();
    if(sc.isFailure()){
      ATH_MSG_ERROR( "Unable to finalize L2 CaloRings selector tool." );
      return sc;
    }
  }*/
  return StatusCode::SUCCESS;
}

//!==========================================================================
bool TrigEgammaL2SelectorTool::emulation(const xAOD::TrigEMCluster *emCluster,  bool &pass, const TrigInfo &info)
{

  if (boost::contains(info.pidname,"1")) return false; // Not emulating Run1
  if(info.etcut || info.hltcalo || info.perf){
    pass=true;//bypass this level
    return true;
  }else if(info.ringer){
    ATH_MSG_DEBUG("Ringer chain...");
    for(unsigned itool=0; itool<m_caloRingerSelectors.size(); ++itool){
      if(!m_caloRingerSelectors[itool]->emulation(emCluster, pass, info))  continue;
    }//loop over sub selectors  
  }else{//pid tools
    for(unsigned itool=0; itool<m_caloSelectors.size(); ++itool){
      if(!m_caloSelectors[itool]->emulation(emCluster, pass, info))  continue;
    }//loop over sub selectors
  }

  dressDecision(emCluster, info.trigName, pass);
  ATH_MSG_DEBUG("trigger: " << info.trigName << " decision: " << int(pass));
  return true;
}
//!==========================================================================
bool TrigEgammaL2SelectorTool::emulation( const xAOD::IParticleContainer *container, bool &pass, const TrigInfo &info)
{
  pass=false;
  
  if(info.idperf || info.etcut || info.hltcalo || info.perf){
    pass=true;
    return true;
  }

  if(info.type == "electron"){
    for(unsigned itool=0; itool<m_electronSelectors.size(); ++itool){
      if(!m_electronSelectors[itool]->emulation(container, pass, info))  continue;
    }//loop over sub selectors  
  }else if (info.type == "photon"){
    pass=true;
    return true;
  }else{
    ATH_MSG_DEBUG("Type (e/g) not found from " << info.trigName);
    return false;
  }
  return true;
}






