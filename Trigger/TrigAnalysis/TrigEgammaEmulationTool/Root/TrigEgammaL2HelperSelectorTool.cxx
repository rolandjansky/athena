/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************
 * AsgTool: TrigEgammaL2HelperSelectorTool
 * Authors:
 *      Ryan Mackenzie White <ryan.white@cern.ch>
 *      Joao Victor Pinto    <jodafons@cern.ch>
 * Date: Feb 2015
 * Description:
 *
 **********************************************************************/
#include "TrigEgammaEmulationTool/TrigEgammaL2HelperSelectorTool.h"
#include <boost/foreach.hpp>
#include <boost/tokenizer.hpp>
#include "boost/algorithm/string.hpp"

using namespace std;
using namespace Trig;
//**********************************************************************

TrigEgammaL2HelperSelectorTool::
TrigEgammaL2HelperSelectorTool( const std::string& myname )
    : TrigEgammaSelectorBaseTool(myname)
{
  declareProperty("CaloSelectors"         , m_caloSelectors        );
  declareProperty("CaloRingerSelectors"   , m_caloRingerSelectors  );
}
//**********************************************************************
StatusCode TrigEgammaL2HelperSelectorTool::initialize() {

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

  return StatusCode::SUCCESS;
}

//!==========================================================================
StatusCode TrigEgammaL2HelperSelectorTool::finalize() {
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
bool TrigEgammaL2HelperSelectorTool::emulation(const xAOD::TrigEMCluster *emCluster,  bool &pass, const std::string &trigger)
{
  std::string trigItem = trigger;
  trigItem.erase(0, 4); //Removes HLT_ prefix from name
  bool isL1 = false;
  bool perf = false;
  bool etcut = false;
  bool idperf = false;
  bool hltcalo = false;
  bool ringer = false;
  float etthr = 0;
  float l1thr = 0;
  std::string type = "";
  std::string l1type = "";
  std::string pidname = "";
  if (boost::contains(trigItem,"idperf"))  idperf    = true;
  if (boost::contains(trigItem,"HTLcalo")) hltcalo   = true;
  if (boost::contains(trigItem,"ringer"))  ringer    = true;
  parseTriggerName(trigItem,"Loose",isL1,type,etthr,l1thr,l1type,pidname,etcut,perf); // Determines probe PID from trigger
  if (boost::contains(pidname,"1")) return false; // Not emulating Run1
  if(idperf || etcut || hltcalo || perf){
    pass=true;//bypass this level
    return true;
  }else if(ringer){
    for(unsigned itool=0; itool<m_caloRingerSelectors.size(); ++itool){
      ATH_MSG_DEBUG("loop over tools...");
      if(!m_caloRingerSelectors[itool]->emulation(emCluster, pass, trigger))  continue;
    }//loop over sub selectors  
  }else{//pid tools
    ATH_MSG_DEBUG("size of tools is: " << m_caloSelectors.size());
    for(unsigned itool=0; itool<m_caloSelectors.size(); ++itool){
      if(!m_caloSelectors[itool]->emulation(emCluster, pass, trigger))  continue;
    }//loop over sub selectors
  }

  dressDecision(emCluster, trigger, pass);

  ATH_MSG_DEBUG("trigger: " << trigger << " decision: " << int(pass));
  return true;
}
//!==========================================================================
bool TrigEgammaL2HelperSelectorTool::emulation( const xAOD::IParticleContainer*    , bool &, const std::string&)
{
  return true;
}






