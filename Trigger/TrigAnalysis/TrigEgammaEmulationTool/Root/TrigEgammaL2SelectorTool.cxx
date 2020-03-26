/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
  declareProperty("CaloCutIDSelectors"  , m_caloCutIDSelectors   );
  declareProperty("CaloRingerSelectors" , m_caloRingerSelectors  );
  declareProperty("ElectronSelector"    , m_electronSelector     );
  declareProperty("PhotonSelector"      , m_photonSelector       );
}
//**********************************************************************
StatusCode TrigEgammaL2SelectorTool::initialize() {

  StatusCode sc = TrigEgammaSelectorBaseTool::initialize();
  
  if(sc.isFailure()){
    ATH_MSG_WARNING("TrigEgammaSelectorBaseTool::initialize() failed");
    return StatusCode::FAILURE;
  }
  
  for(auto& tool : m_caloCutIDSelectors){
    tool->setParents(m_trigdec, m_storeGate);
    sc = tool->initialize();
    if(sc.isFailure()){
      ATH_MSG_WARNING("TrigEgammaL2CaloSelectorTool::initialize() failed");
      return StatusCode::FAILURE;
    }
  }// loop over pids: Tight, Medium, Loose and VeryLoose
  
  for(auto& tool : m_caloRingerSelectors){
    tool->setParents(m_trigdec, m_storeGate);
    sc = tool->initialize();
    if(sc.isFailure()){
      ATH_MSG_WARNING("TrigEgammaL2CaloRingerSelectorTool::initialize() failed");
      return StatusCode::FAILURE;
    }
  }// loop over pids: Tight, Medium, Loose and VeryLoose
 

  ATH_MSG_INFO("Initialising L2(Track) Electron Selector tool...");
  m_electronSelector->setParents(m_trigdec, m_storeGate);
  sc = m_electronSelector->initialize();
  if(sc.isFailure()){
    ATH_MSG_WARNING("TrigEgammaL2ElectronSelectorTool::initialize() failed");
    return StatusCode::FAILURE;
  }
 

  return StatusCode::SUCCESS;
}

//!==========================================================================
StatusCode TrigEgammaL2SelectorTool::finalize() {
  return StatusCode::SUCCESS;
}

//!==========================================================================
bool TrigEgammaL2SelectorTool::emulation(const xAOD::TrigEMCluster *emCluster,  bool &pass, const Trig::Info &info)
{
  pass=false;
  if (boost::contains(info.pidname,"1")) return false; // Not emulating Run1
  std::string pidname = (boost::contains(info.pidname,"LH")) ? info.pidname.substr(2, info.pidname.size()) : info.pidname;
  
  ATH_MSG_DEBUG("L2 Selector pidname : " << pidname );
  if(info.hltcalo || info.perf){
    pass=true;//bypass this level
    return true;
  }else if(info.etcut){
    if(emCluster->et()*1e-3 >= info.thrHLT-5) pass=true;
    return true;
  }else if(info.ringer){
    ATH_MSG_DEBUG("Ringer chain...");
    if(pidname == "Tight" )
      m_caloRingerSelectors[0]->emulation(emCluster, pass, info);
    else if(pidname == "Medium" )
      m_caloRingerSelectors[1]->emulation(emCluster, pass, info);
    else if(pidname == "Loose" ) 
      m_caloRingerSelectors[2]->emulation(emCluster, pass, info);
    else if(pidname == "VLoose")
      m_caloRingerSelectors[3]->emulation(emCluster, pass, info);
    else{
      ATH_MSG_WARNING("No pidname found");
      return false;
    }
  }else{//pid tools
    if(pidname == "Tight" )
      m_caloCutIDSelectors[0]->emulation(emCluster, pass, info);
    else if(pidname == "Medium" )
      m_caloCutIDSelectors[1]->emulation(emCluster, pass, info);
    else if(pidname == "Loose" ) // should be this order because of the "else if"
      m_caloCutIDSelectors[2]->emulation(emCluster, pass, info);
    else if(pidname == "VLoose" )
      m_caloCutIDSelectors[3]->emulation(emCluster, pass, info);
    else{
      ATH_MSG_WARNING("No pidname found");
      return false;
    }
  }

  dressDecision(emCluster, info.trigName, pass);
  ATH_MSG_DEBUG("trigger: " << info.trigName << " decision: " << int(pass));
  return true;
}
//!==========================================================================
bool TrigEgammaL2SelectorTool::emulation( const xAOD::IParticleContainer *container, bool &pass, const Trig::Info &info)
{
  pass=false;
  if(info.idperf || info.etcut || info.hltcalo || info.perf){
    pass=true;
    return true;
  }else if(info.type == "electron"){
    m_electronSelector->emulation(container, pass, info);
  }else if (info.type == "photon"){
    //m_photonSelector->emulation(container, pass, info);
    pass=true;
  }else{
    ATH_MSG_DEBUG("Type (e/g) not found from " << info.trigName);
    return false;
  }
  return true;
}






