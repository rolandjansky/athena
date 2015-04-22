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
: TrigEgammaAnalysisBaseTool(myname)
{
  declareProperty("MVACalibTool", m_MVACalibTool);
  declareProperty("ApplyMVACalib", m_applyMVACalib=false);
  declareProperty("ElectronIsEMSelector", m_electronPPCutIDTool);
  declareProperty("ElectronLikelihoodTool", m_electronLHTool);
  declareProperty("TrigDecisionTool", m_trigdec, "iTool to access the trigger decision");
  declareProperty("TriggerList",m_trigList);
  declareProperty("dR",m_dR=0.07); //new
  declareProperty("RemoveCrack", m_rmCrack=true); //new
  // just for compile

  HLT::TriggerElement* t = NULL;
  const xAOD::TrigElectronContainer* a = getFeature<xAOD::TrigElectronContainer>(t);
  const xAOD::ElectronContainer* b = getFeature<xAOD::ElectronContainer>(t);
  bool a1 = ancestorPassed<xAOD::TrigElectronContainer>(t);
  bool b1 = ancestorPassed<xAOD::ElectronContainer>(t);
  (void)a; (void)b;
  (void)a1; (void)b1;

  m_PidToolMap["Tight"]=0;
  m_PidToolMap["Medium"]=1;
  m_PidToolMap["Loose"]=2;


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
    if ( (m_electronPPCutIDTool.retrieve()).isFailure() ){
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

StatusCode TrigEgammaNavBaseTool::executeNavigation( std::string trigItem ){

  clearList();
  ATH_MSG_DEBUG("Apply navigation selection");
  // Check Size of Electron Container
  m_offElectrons = 0;
  if ( (m_storeGate->retrieve(m_offElectrons,m_offElContKey)).isFailure() ){
    ATH_MSG_ERROR("Failed to retrieve offline Electrons ");
  	return StatusCode::FAILURE;
  }

  for(const auto& el : *m_offElectrons ){
    
    const HLT::TriggerElement *te = NULL;
    if(!el->trackParticle()){
      ATH_MSG_DEBUG("No track Particle");
      continue;
    }
    if(!el->caloCluster()){
      ATH_MSG_DEBUG("No caloCluster");
      continue;
    } 
    if ( m_matchTool->match(el, trigItem, te)){
      std::pair< const xAOD::Electron*, const HLT::TriggerElement* > pair(el,te);
      m_electronTriggerElementList.push_back(pair);
    }
  }

  ATH_MSG_INFO("BaseTool::TEs " << m_electronTriggerElementList.size() << " found.");
  return StatusCode::SUCCESS;
}


void TrigEgammaNavBaseTool::clearList(){
    m_electronTriggerElementList.clear();
}

bool TrigEgammaNavBaseTool::passedTrigger(const HLT::TriggerElement* obj){
    bool passed=false;
    if ( obj->getActiveState() ) passed = true;
    return passed;
}

