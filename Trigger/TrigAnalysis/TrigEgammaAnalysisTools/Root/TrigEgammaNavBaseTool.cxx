/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************
 * AsgTool: TrigEgammaNavZeeTPBaseTool
 * Authors:
 *      Ryan Mackenzie White <ryan.white@cern.ch>
 *      Denis Damazio <denis.damazio@cern.ch>
 * Contributors:
 *      Joao Victor Pinto <joao.victor.da.fonseca.pinto@cern.ch>
 * Date: Feb 2015
 * Description:
 *      Inherits from TrigEgammaAnalysisBaseTool.
 **********************************************************************/
#include "egammaMVACalib/IegammaMVATool.h"
#include "TrigEgammaAnalysisTools/TrigEgammaNavBaseTool.h"
#include "TrigConfxAOD/xAODConfigTool.h"

#include "string"
#include <algorithm>
#include "boost/algorithm/string.hpp"
#include <boost/tokenizer.hpp>
#include <boost/foreach.hpp>
//**********************************************************************
using namespace Trig;
using namespace TrigConf;
using namespace xAOD;
using namespace boost;
TrigEgammaNavBaseTool::
TrigEgammaNavBaseTool( const std::string& myname )
: TrigEgammaAnalysisBaseTool(myname),
    m_dir(myname)
{
  declareProperty("MVACalibTool", m_MVACalibTool);
  declareProperty("ApplyMVACalib", m_applyMVACalib=false);
  declareProperty("ElectronIsEMSelector", m_electronIsEMTool);
  declareProperty("ElectronLikelihoodTool", m_electronLHTool);
  declareProperty("TrigDecisionTool", m_trigdec, "iTool to access the trigger decision");
  declareProperty("TriggerList",m_trigInputList);
  declareProperty("CategoryList",m_categories);
  declareProperty("dR",m_dR=0.07); //new
  declareProperty("RemoveCrack", m_rmCrack=true); //new
  declareProperty("PhotonPid",m_photonPid = "Tight");
  declareProperty("doUnconverted", m_doUnconverted=false);
  declareProperty("OfflineProbeIsolation", m_offProbeIsolation="Loose");
  declareProperty("ForceProbeIsolation", m_forceProbeIsolation=false);
  declareProperty("ForcePidSelection", m_forcePidSelection=true);
  m_PidToolMap["Tight"]=0;
  m_PidToolMap["Medium"]=1;
  m_PidToolMap["Loose"]=2;

  m_offElectrons=nullptr;
  m_offPhotons=nullptr;
  m_jets=nullptr;
  // Maps should be static
  // Make a wrapper function to set map and return value
    
}

//**********************************************************************

StatusCode TrigEgammaNavBaseTool::childExecute() {

   ATH_MSG_VERBOSE( "child Execute tool " << name() );

   return StatusCode::SUCCESS;
}

StatusCode TrigEgammaNavBaseTool::childFinalize() {

   ATH_MSG_VERBOSE( "child Finalize tool " << name() );

   return StatusCode::SUCCESS;
}

//**********************************************************************
StatusCode
TrigEgammaNavBaseTool::childInitialize() {
    ATH_MSG_VERBOSE( "child Initialize tool " << name() );
    if ( (m_trigdec.retrieve()).isFailure() ){
        ATH_MSG_ERROR("Could not retrieve Trigger Decision Tool! Can't work");
        return StatusCode::FAILURE;
    }
    if ( (m_electronIsEMTool.retrieve()).isFailure() ){
        ATH_MSG_ERROR( "Could not retrieve Selector Tool! Can't work");
        return StatusCode::FAILURE;
    }
    if ( (m_electronLHTool.retrieve()).isFailure() ){
        ATH_MSG_ERROR( "Could not retrieve Selector Tool! Can't work");
        return StatusCode::FAILURE;
    }
    if( (m_MVACalibTool.retrieve()).isFailure()) {
        ATH_MSG_ERROR("Failed to retrieve " << m_MVACalibTool);
        m_applyMVACalib = false;
        m_MVACalibTool = ToolHandle<IegammaMVATool>();
        return StatusCode::FAILURE; 
    }
    else {
        ATH_MSG_DEBUG("Retrieved tool " << m_MVACalibTool);   
    }

    return StatusCode::SUCCESS;
}

StatusCode TrigEgammaNavBaseTool::childBook() {
    return StatusCode::SUCCESS;
}


StatusCode TrigEgammaNavBaseTool::eventWiseSelection( ){
    // Check Size of Electron Container
    m_offElectrons = 0;
    if ( (m_storeGate->retrieve(m_offElectrons,m_offElContKey)).isFailure() ){
        ATH_MSG_ERROR("Failed to retrieve offline Electrons ");
        return StatusCode::FAILURE; 
    }
    
    for(const auto& eg : *m_offElectrons ){
        if(ApplyElectronPid(eg,"Loose")) hist1("electrons")->AddBinContent(1);
        if(ApplyElectronPid(eg,"Medium")) hist1("electrons")->AddBinContent(2);
        if(ApplyElectronPid(eg,"Tight")) hist1("electrons")->AddBinContent(3); 
        if(ApplyElectronPid(eg,"LHLoose")) hist1("electrons")->AddBinContent(4);
        if(ApplyElectronPid(eg,"LHMedium")) hist1("electrons")->AddBinContent(5);
        if(ApplyElectronPid(eg,"LHTight")) hist1("electrons")->AddBinContent(6); 
    }

    // Check Size of Electron Container
    m_offPhotons = 0;
    if ( (m_storeGate->retrieve(m_offPhotons,m_offPhContKey)).isFailure() ){
        ATH_MSG_ERROR("Failed to retrieve offline Electrons ");
        return StatusCode::FAILURE;
    }

    return StatusCode::SUCCESS;
}

StatusCode TrigEgammaNavBaseTool::executeNavigation( std::string trigItem ){

  clearList();
  ATH_MSG_DEBUG("Apply navigation selection");
  
  std::string type="";
  bool isL1=false;
  float etthr=0;
  float l1thr=0;
  std::string l1type="";
  std::string pidname="";
  bool perf=false;
  bool etcut=false;
  parseTriggerName(trigItem,m_defaultProbePid,isL1,type,etthr,l1thr,l1type,pidname,perf,etcut); // Determines probe PID from trigger
  std::string trigName="";
  if(isL1) trigName= trigItem;
  else trigName="HLT_"+trigItem;
  if(type == "electron") return executeElectronNavigation( trigName,etthr,pidname );
  else if(type=="photon") return executePhotonNavigation( trigName,etthr);

  ATH_MSG_DEBUG("BaseTool::TEs " << m_objTEList.size() << " found.");
  return StatusCode::SUCCESS;
}

bool TrigEgammaNavBaseTool::ApplyElectronPid(const xAOD::Electron *eg, const std::string pidname){
  ATH_MSG_DEBUG("Apply Pid " << pidname);
  if (pidname == "Tight"){
    return m_electronIsEMTool[0]->accept(eg);
  }
  else if (pidname == "Medium"){
    return m_electronIsEMTool[1]->accept(eg);
  }
  else if (pidname == "Loose"){
    return m_electronIsEMTool[2]->accept(eg);
  }
  else if (pidname == "LHTight"){
    return m_electronLHTool[0]->accept(eg);
  }
  else if (pidname == "LHMedium"){
    return m_electronLHTool[1]->accept(eg);
  }
  else if (pidname == "LHLoose"){
    return m_electronLHTool[2]->accept(eg);
  }
  else ATH_MSG_DEBUG("No Pid tool, continue without PID");
  return false;
}

StatusCode TrigEgammaNavBaseTool::executeElectronNavigation( std::string trigItem,float etthr,std::string pidname ){

  clearList(); //Clear Probe list before each execution -- not in derived class
  ATH_MSG_DEBUG("Apply navigation selection "); 


  for(const auto& eg : *m_offElectrons ){
      const HLT::TriggerElement *te = NULL;
      if(!eg->trackParticle()){
          ATH_MSG_DEBUG("No track Particle");
          continue;
      }
      if(!eg->caloCluster()){
          ATH_MSG_DEBUG("No caloCluster");
          continue;
      }

      if(m_forcePidSelection){
        if( !( getEt(eg)  > (etthr-5.)*1.e3) ) continue;
        if ( (fabs(eg->eta())>1.37 && fabs(eg->eta())<1.52) || fabs(eg->eta())>2.47 )
            continue; 
        // if(!eg->passSelection(pidname)) continue;
        // Rerun offline selection
        if(!ApplyElectronPid(eg,pidname)) continue;
        if (m_forceProbeIsolation) {
            if (!isIsolated(eg, m_offProbeIsolation)) {
                continue;
            }
        }
      }

      if ( m_matchTool->match(eg, trigItem, te)){
          std::pair< const xAOD::Electron*, const HLT::TriggerElement* > pair(eg,te);
          m_objTEList.push_back(pair);
      }
      else {
          std::pair< const xAOD::Electron*, const HLT::TriggerElement* > pair(eg,NULL);
          m_objTEList.push_back(pair);
      }
  }

  ATH_MSG_DEBUG("BaseTool::Electron TEs " << m_objTEList.size() << " found.");
  return StatusCode::SUCCESS;
}

StatusCode TrigEgammaNavBaseTool::executePhotonNavigation( std::string trigItem, float etthr){

  clearList();
  ATH_MSG_DEBUG("Apply navigation selection");
  


  for(const auto& eg : *m_offPhotons ){
      const HLT::TriggerElement *te = NULL;
      if(!eg->caloCluster()){
          ATH_MSG_DEBUG("No caloCluster");
          continue;
      } 
      if( !(getCluster_et(eg) > (etthr-5.)*1.e3)) continue; //Take 2GeV above threshold
      if(!eg->passSelection(m_photonPid)) continue;
      if(m_doUnconverted)
          if (eg->vertex()){
              ATH_MSG_DEBUG("Removing converted photons, continuing...");
              continue;
          }
      if ( m_matchTool->match(eg, trigItem, te)){
          std::pair< const xAOD::Photon*, const HLT::TriggerElement* > pair(eg,te);
          m_objTEList.push_back(pair);
      }
      else {
          std::pair< const xAOD::Photon*, const HLT::TriggerElement* > pair(eg,NULL);
          m_objTEList.push_back(pair);
      }

  }

  ATH_MSG_DEBUG("BaseTool::Photon TEs " << m_objTEList.size() << " found.");
  return StatusCode::SUCCESS;
}

void TrigEgammaNavBaseTool::clearList(){
    m_objTEList.clear();
}

bool TrigEgammaNavBaseTool::passedTrigger(const HLT::TriggerElement* obj){
    bool passed=false;
    if ( obj->getActiveState() ) passed = true;
    return passed;
}

