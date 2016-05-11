/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************
 * AsgTool: TrigEgammaEmulationTool
 * Authors:
 *      Ryan Mackenzie White <ryan.white@cern.ch>
 *      Joao Victor Pinto    <jodafons@cern.ch>
 *      Jorge Andres Lopez   <jorge.lopez@cern.ch>
 * Date: Feb 2015
 * Description:
 *      Generic Base tool class for trigger egamma analysis. Inherits from AsgTool.
 *      Used for handling histograms and TTrees in derived classes
 *      Relies on THistSvc. For dual-use need to find alternative
 *      Class not entirely generic
 *      Method that maps HLT trigger to L1 trigger name
 *      Methods that maps trigger pid name to egamma Pid string
 *      Helper methods for plotting Egamma variables
 **********************************************************************/
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include "boost/algorithm/string.hpp"
#include <boost/tokenizer.hpp>
#include <boost/foreach.hpp>
#include "StoreGate/StoreGateSvc.h"
#include "TrigEgammaEmulationTool/TrigEgammaEmulationTool.h"
#include "TrigSteeringEvent/TrigPassBits.h"
#include "TrigSteeringEvent/TrigPassBitsCollection.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"
#include "TrigDecisionTool/TrigDecisionTool.h"


using namespace std;
using namespace Trig;
//**********************************************************************

TrigEgammaEmulationTool::
TrigEgammaEmulationTool( const std::string& myname )
    : AsgTool(myname),
    m_accept("TrigEgammaEmulationTool"),
    m_trigdec("Trig::TrigDecisionTool/TrigDecisionTool"),
    m_matchTool("Trig::TrigEgammaMatchingTool/TrigEgammaMatchingTool")

{
    declareProperty("MatchTool"               , m_matchTool               );
    declareProperty("L1SelectorTool"          , m_l1Selector              );
    declareProperty("L2SelectorTool"          , m_l2Selector              );
    declareProperty("EFCaloSelectorTool"      , m_efCaloSelector          );
    declareProperty("EFElectronSelectorTool"  , m_efElectronSelector      );
    declareProperty("EFPhotonSelectorTool"    , m_efPhotonSelector        );
    declareProperty("ElectronKey"             , m_offElContKey="Electrons");

    m_offElectrons   =nullptr;
    m_onlElectrons   =nullptr;
    m_onlPhotons     =nullptr;
    m_trigElectrons  =nullptr;
    m_caloClusters   =nullptr;
    m_trigEMClusters =nullptr;
    m_emTauRois      =nullptr;
    m_storeGate      =nullptr;

    // just for compile
    HLT::TriggerElement* t = NULL;
    const xAOD::TrigElectronContainer* a = getFeature<xAOD::TrigElectronContainer>(t);
    const xAOD::ElectronContainer* b = getFeature<xAOD::ElectronContainer>(t);
    (void)a; (void)b;
}
//**********************************************************************
StatusCode TrigEgammaEmulationTool::initialize() {

    ATH_MSG_VERBOSE( "Initialising tool " << name() );

    StatusCode sc;
    sc = service("StoreGateSvc", m_storeGate);
    if(sc.isFailure()) {
        ATH_MSG_ERROR( "Unable to locate Service StoreGateSvc" );
        return sc;
    }

    if ( (m_trigdec.retrieve()).isFailure() ){
      ATH_MSG_ERROR("Could not retrieve Trigger Decision Tool! Can't work");
      return StatusCode::FAILURE;
    }    
    //Enable expert methods
    m_trigdec->ExperimentalAndExpertMethods()->enable();

    ATH_MSG_INFO("Initialising Selectors tool...");

    ATH_MSG_INFO("Initialising L1 Selectors tool...");
    if(m_l1Selector){
      m_l1Selector->setParents(m_trigdec, m_storeGate);
      sc = m_l1Selector->initialize();
      if(sc.isFailure()){
        ATH_MSG_ERROR( "Unable to initialize L1 selector tool." );
        return sc;
      }
    }
    
    ATH_MSG_INFO("Initialising L2 Selectors tool...");
    if(m_l2Selector){
      m_l2Selector->setParents(m_trigdec, m_storeGate);
      sc = m_l2Selector->initialize();
      if(sc.isFailure()){
        ATH_MSG_ERROR( "Unable to initialize L2 selector tool." );
        return sc;
      }
    }
    
    ATH_MSG_INFO("Initialising EFCalo Selectors tool...");
    if(m_efCaloSelector){
      m_efCaloSelector->setParents(m_trigdec, m_storeGate);
      sc = m_efCaloSelector->initialize();
      if(sc.isFailure()){
        ATH_MSG_ERROR( "Unable to initialize EF Calo selector tool." );
        return sc;
      }
    }

    ATH_MSG_INFO("Initialising EF Electron Selectors tool...");
    if(m_efElectronSelector){
      m_efElectronSelector->setParents(m_trigdec, m_storeGate);
      sc = m_efElectronSelector->initialize();
      if(sc.isFailure()){
        ATH_MSG_ERROR( "Unable to initialize EF Electron selector tool." );
        return sc;
      }
    }

    ATH_MSG_INFO("Initialising EF Photon Selectors tool...");
    if(m_efPhotonSelector){
      m_efPhotonSelector->setParents(m_trigdec, m_storeGate);
      sc = m_efPhotonSelector->initialize();
      if(sc.isFailure()){
        ATH_MSG_ERROR( "Unable to initialize EF Photon selector tool." );
        return sc;
      }
    }
    
    //add cuts into TAccept
    m_accept.addCut("EmTauRoi"             , "Trigger L1Calo step"     );
    m_accept.addCut("TrigEMCluster"        , "Trigger L2Calo step"     );
    m_accept.addCut("TrigElectronContainer", "Trigger L2Electron step" );
    m_accept.addCut("TrigPhotonContainer"  , "Trigger L2Photon step"   );
    m_accept.addCut("CaloClusterContainer" , "Trigger EFCalo step"     );
    m_accept.addCut("ElectronContainer"    , "Trigger EFElectron step" );
    m_accept.addCut("PhotonContainer"      , "Trigger EFPhoton step"   );
    m_accept.addCut("HLT"                  , "Trigger HLT decision"    );
     


    return sc;
}
//!==========================================================================
StatusCode TrigEgammaEmulationTool::execute() {
    return StatusCode::SUCCESS;
}
//!==========================================================================
StatusCode TrigEgammaEmulationTool::finalize() {
    StatusCode sc;
    ATH_MSG_INFO("Finalising L1Selector tool...");
    if(m_l1Selector){
      sc = m_l1Selector->finalize();
      if(sc.isFailure()){
        ATH_MSG_ERROR( "Unable to finalize L1 selector tool." );
        return sc;
      }
    }
    
    if(m_l2Selector){
      sc = m_l2Selector->finalize();
      if(sc.isFailure()){
        ATH_MSG_ERROR( "Unable to finalize L2 selector tool." );
        return sc;
      }
    }
    
    if(m_efCaloSelector){
      sc = m_efCaloSelector->finalize();
      if(sc.isFailure()){
        ATH_MSG_ERROR( "Unable to finalize EF Calo selector tool." );
        return sc;
      }
    }
    if(m_efElectronSelector){
      sc = m_efElectronSelector->finalize();
      if(sc.isFailure()){
        ATH_MSG_ERROR( "Unable to finalize EF Electron selector tool." );
        return sc;
      }
    }
    if(m_efPhotonSelector){
      sc = m_efPhotonSelector->finalize();
      if(sc.isFailure()){
        ATH_MSG_ERROR( "Unable to finalize EF Photon selector tool." );
        return sc;
      }
    }


    return StatusCode::SUCCESS;
}
//!==========================================================================
bool TrigEgammaEmulationTool::EventWiseContainer(){

  m_offElectrons=0;  //Offline
  m_onlElectrons=0;  //EFElectron
  m_caloClusters=0;  //EFCalo
  m_trigElectrons=0; //L2Electron
  m_trigEMClusters=0;//L2Calo
  m_emTauRois=0;     //L1

  if ((m_storeGate->retrieve(m_offElectrons,m_offElContKey)).isFailure() ) {
      ATH_MSG_ERROR("Failed to retrieve offline Electrons ");
      return false;
  }
  ATH_MSG_DEBUG("Offline Electron container size " << m_offElectrons->size());
  if ((m_storeGate->retrieve(m_onlElectrons,"HLT_xAOD__ElectronContainer_egamma_Electrons")).isFailure() ) {
      ATH_MSG_ERROR("Failed to retrieve offline Electrons ");
      return false;
  }
  else ATH_MSG_DEBUG("Online Electron container size " << m_onlElectrons->size());
  if ((m_storeGate->retrieve(m_onlPhotons,"HLT_xAOD__PhotonContainer_egamma_Photons")).isFailure() ) {
      ATH_MSG_ERROR("Failed to retrieve offline Photons ");
      return false;
  }
  else ATH_MSG_DEBUG("Online Photon container size " << m_onlPhotons->size());
  if ((m_storeGate->retrieve(m_trigElectrons,"HLT_xAOD__TrigElectronContainer_L2ElectronFex")).isFailure() ) {
      ATH_MSG_ERROR("Failed to retrieve offline Electrons ");
      return false;
  }
  else ATH_MSG_DEBUG("Trig Electron container size " << m_trigElectrons->size());
  if ((m_storeGate->retrieve(m_emTauRois,"LVL1EmTauRoIs")).isFailure() ) {
      ATH_MSG_ERROR("Failed to retrieve offline Electrons ");
      return false;
  }
  else ATH_MSG_DEBUG("L1 EM container size " << m_emTauRois->size());
  if ((m_storeGate->retrieve(m_caloClusters,"HLT_xAOD__CaloClusterContainer_TrigEFCaloCalibFex")).isFailure() ) {
      ATH_MSG_ERROR("Failed to retrieve CaloClusters");
      return false;
  }
  else ATH_MSG_DEBUG("CaloCluster size " << m_caloClusters->size());
  if ((m_storeGate->retrieve(m_trigEMClusters,"HLT_xAOD__TrigEMClusterContainer_TrigT2CaloEgamma")).isFailure() ) {
      ATH_MSG_ERROR("Failed to retrieve TrigEMClusters");
      return false;
  }else ATH_MSG_DEBUG("TrigEMCluser size " << m_trigEMClusters->size());

  


  return true;
}
//!==========================================================================
//! Emulation from Egamma Offline object. Must be a match between online and offline.
const Root::TAccept& TrigEgammaEmulationTool::executeTool(const xAOD::Egamma *eg, const std::string &trigger, const std::string &fctrigger) {
  std::string trigItem = fctrigger;
  m_accept.clear();
  const HLT::TriggerElement *finalFC = NULL;
  if(!boost::contains(trigItem,"HLT")) trigItem = "HLT_"+trigItem;
  m_matchTool->match(eg,trigItem,finalFC);

  if (!finalFC) {
    ATH_MSG_DEBUG("TE is NULL");
    return false;
  }
  return executeTool(finalFC, trigger);
}
//!==========================================================================
//! Emulation from Trigger Element
const Root::TAccept& TrigEgammaEmulationTool::executeTool(const HLT::TriggerElement *te, const std::string &trigger) {
  ATH_MSG_DEBUG("TrigEgammaEmulationTool::executeTool(te, trigger)");
  m_accept.clear(); 
  std::string trigItem = trigger;
  bool pass = false;
  std::string type = "Electron"; //electron type as default
  std::vector<std::string> strs;
  if(boost::contains(trigItem,"HLT")) trigItem.erase(0,4);
  if(boost::contains(trigItem,"L1")) trigItem.erase(0,4);
  boost::split(strs,trigItem,boost::is_any_of("_"));
  if (boost::contains(strs.at(0),"e")) type = "Electron";
  if (boost::contains(strs.at(0),"g")) type = "Photon";
  bool passedL1Calo = false;  
  bool passedL2Calo = false; 
  bool passedEFCalo = false; 
  bool passedL2     = false;
  bool passedEF     = false;
  ATH_MSG_DEBUG("getFeatures from TE...");
  const auto* l1            = getFeature<xAOD::EmTauRoI>(te);
  const auto* emCluster     = getFeature<xAOD::TrigEMCluster>(te);
  const auto* trigElCont    = getFeature<xAOD::TrigElectronContainer>(te);
  //const auto* clusCont    = getFeature<xAOD::CaloClusterContainer>(te);
  const auto* elCont        = getFeature<xAOD::ElectronContainer>(te);
  //const auto* phCont        = getFeature<xAOD::PhotonContainer>(te);
  m_l1Selector->emulation( l1, passedL1Calo ,trigger);
  m_l2Selector->emulation( emCluster, passedL2Calo ,trigger);
  m_efCaloSelector->emulation( elCont, passedEFCalo, trigger);
  m_accept.setCutResult("EmTauRoI"            , passedL1Calo);
  m_accept.setCutResult("TrigEMCluster"       , passedL2Calo);
  m_accept.setCutResult("CaloClusterContainer", passedEFCalo);
  //execute object reconstructions combined (calo+ID)
  if (type == "Electron") {
    m_l2Selector->emulation( trigElCont, passedL2, trigger);
    m_efElectronSelector->emulation( elCont, passedEF, trigger);
    m_accept.setCutResult("TrigElectronContainer", passedL2);
    m_accept.setCutResult("ElectronContainer"    , passedEF);
  }
  else if (type == "Photon") {
    //m_l2Selector->emulation( trigPhCont, passedL2, trigger);
    //m_efPhotonSelector->emulation( phCont, passedEF, trigger);
    m_accept.setCutResult("TrigPhotonContainer", passedL2);
    m_accept.setCutResult("PhotonContainer"    , passedEF);
  }
  else {
    ATH_MSG_DEBUG("Type (e/g) not found from " << trigger);
    m_accept.clear();
    return m_accept;
  }
  // if(passedL1Calo && passedL2Calo && passedL2 && passedEFCalo && passedEF) pass = true;
  if (passedL1Calo && passedEFCalo && passedEF) pass = true;
  m_accept.setCutResult("HLT", pass);
  return m_accept;
}
//!==========================================================================
//! Emulation from xAOD containers not using TDT tools
const Root::TAccept& TrigEgammaEmulationTool::executeTool(const std::string &trigger) {
  clearDecorations();
  m_accept.clear();
  std::string trigItem = trigger;
  bool pass=false;
  std::string type = "Electron";//electron type as default
  std::vector<std::string> strs;
  if(boost::contains(trigItem,"HLT")) trigItem.erase(0,4);
  if(boost::contains(trigItem,"L1")) trigItem.erase(0,4);
  boost::split(strs,trigItem,boost::is_any_of("_"));
  if (boost::contains(strs.at(0),"e")) type = "Electron";
  if (boost::contains(strs.at(0),"g")) type = "Photon";
  bool passedL1Calo = false;  
  bool passedL2Calo = false; 
  bool passedEFCalo = false; 
  bool passedL2     = false;
  bool passedEF     = false;
  //loop over objects
  for(const auto& l1 : *m_emTauRois) {
    m_l1Selector->emulation(l1, passedL1Calo ,trigger);
    //ATH_MSG_DEBUG("L1 decoration is : " << int(l1->auxdecor<bool>(trigger)) );
    if(passedL1Calo)  break;
  }
  //loop over objects
  for(const auto& emCluster : *m_trigEMClusters){
    m_l2Selector->emulation(emCluster, passedL2Calo ,trigger);
    if(passedL2Calo)  break;
  }
  m_efCaloSelector->emulation(m_onlElectrons, passedEFCalo, trigger);
  m_accept.setCutResult("EmTauRoI"            , passedL1Calo);
  m_accept.setCutResult("TrigEMCluster"       , passedL2Calo);
  m_accept.setCutResult("CaloClusterContainer", passedEFCalo);
  //execute object reconstructions combined (calo+ID)
  if (type == "Electron") {
    m_l2Selector->emulation(m_trigElectrons, passedL2, trigger);
    m_efElectronSelector->emulation(m_onlElectrons, passedEF, trigger);
    m_accept.setCutResult("TrigElectronContainer", passedL2);
    m_accept.setCutResult("ElectronContainer"    , passedEF);
  }
  else if (type == "Photon") {
    //m_l2Selector->emulation( trigPhCont, passedL2, trigger);
    //m_efPhotonSelector->emulation( phCont, passedEF, trigger);
    m_accept.setCutResult("TrigPhotonContainer", passedL2);
    m_accept.setCutResult("PhotonContainer"    , passedEF);
  }
  else {
    ATH_MSG_DEBUG("Type (e/g) not found from " << trigger);
    m_accept.clear();
    return m_accept;
  }
  // if(passedL1Calo && passedL2Calo && passedL2 && passedEFCalo && passedEF) pass = true;
  if (passedL1Calo && passedEFCalo && passedEF) pass = true;
  m_accept.setCutResult("HLT", pass);
  return m_accept;
}

//!==========================================================================
bool TrigEgammaEmulationTool::isPassed(const std::string &trigger) {
  Root::TAccept accept = executeTool(trigger);
  return accept.getCutResult("HLT");
}

//!==========================================================================
bool TrigEgammaEmulationTool::isPassed(const std::string &trigger, const std::string &fctrigger) {
  bool pass = false;
  std::string trigItem = fctrigger;
  const HLT::TriggerElement *finalFC = NULL;
  if(!boost::contains(trigItem,"HLT")) trigItem = "HLT_"+trigItem;
  auto fc = m_trigdec->features(trigItem, TrigDefs::alsoDeactivateTEs);
  auto vec = fc.get<xAOD::ElectronContainer>();
  for (const auto &feat : vec) {
    finalFC = feat.te();
    if (!finalFC) continue;
    Root::TAccept accept = executeTool(finalFC, trigger);
    pass = accept.getCutResult("HLT");
    if (pass == true) break;
  }
  return pass;
}

//!==========================================================================
void TrigEgammaEmulationTool::clearDecorations(){
  m_offElectrons->clearDecorations();  //Offline
  m_onlElectrons->clearDecorations();  //EFElectron
  m_caloClusters->clearDecorations();  //EFCalo
  m_trigElectrons->clearDecorations(); //L2Electron
  m_trigEMClusters->clearDecorations();//L2Calo
  m_emTauRois->clearDecorations();     //L1
}
