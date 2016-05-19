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
#include <boost/dynamic_bitset.hpp>
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
    declareProperty("TrigList"                , m_trigList                );
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
     
    for(const auto trigName : m_trigList){
      ATH_MSG_DEBUG("Trigger " << trigName <<" to TrigInfo");
      if(m_trigInfo.count(trigName) != 0){
        ATH_MSG_INFO("Trigger already added");
      }else{
        setTrigInfo(trigName);
      }
    }//Loop over trigger list
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
  m_accept.clear();
  const HLT::TriggerElement *finalFC = NULL;
  m_matchTool->match(eg,trigger,finalFC);

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
  bool pass = false;
  
  if(m_trigInfo.count(trigger) != 0){
    TrigInfo info     = getTrigInfo(trigger);
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

    m_efElectronSelector->setTe(te); //Must be passed to config track isolation

    m_l1Selector->emulation( l1, passedL1Calo , info);
    m_l2Selector->emulation( emCluster, passedL2Calo , info);
    m_efCaloSelector->emulation( elCont, passedEFCalo, info);
    m_accept.setCutResult("EmTauRoI"            , passedL1Calo);
    m_accept.setCutResult("TrigEMCluster"       , passedL2Calo);
    m_accept.setCutResult("CaloClusterContainer", passedEFCalo);
    //execute object reconstructions combined (calo+ID)
    if (info.type == "electron") {
      m_l2Selector->emulation( trigElCont, passedL2, info);
      m_efElectronSelector->emulation( elCont, passedEF, info);
      m_accept.setCutResult("TrigElectronContainer", passedL2);
      m_accept.setCutResult("ElectronContainer"    , passedEF);
    }
    else if (info.type == "photon") {
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

    if(info.ringer || info.perf){//bypass L2 Electron/Photon Level
      if(passedL1Calo && passedEFCalo && passedEF) pass = true;
    }else{
      if(passedL1Calo && passedL2Calo && passedL2 && passedEFCalo && passedEF) pass = true;
    }

    m_accept.setCutResult("HLT", pass);

  }else{
    ATH_MSG_WARNING("Can not emulate " << trigger << ". This chain must be added into trigList before the creation.");
  }
  return m_accept;
}
//!==========================================================================
//! Emulation from xAOD containers not using TDT tools
const Root::TAccept& TrigEgammaEmulationTool::executeTool(const std::string &trigger) {
  clearDecorations();
  m_accept.clear();
  
  if( m_trigInfo.count(trigger) != 0){
    TrigInfo info     = getTrigInfo(trigger);
    bool passedL1Calo = false;  
    bool passedL2Calo = false; 
    bool passedEFCalo = false; 
    bool passedL2     = false;
    bool passedEF     = false;

    boost::dynamic_bitset<> bitL1Accept(m_emTauRois->size());
    boost::dynamic_bitset<> bitL2CaloAccept(m_trigEMClusters->size());

    //loop over objects
    unsigned bit=0;  bool pass=false;
    for(const auto& l1 : *m_emTauRois) {
      m_l1Selector->emulation(l1, pass, info);
      bitL1Accept.set(bit, pass);
      bit++;
    }

    bit=0; pass=false;
    //loop over objects
    for(const auto& emCluster : *m_trigEMClusters){
      m_l2Selector->emulation(emCluster, pass, info);
      bitL2CaloAccept.set(bit, pass);
      bit++;
    }

    if(bitL1Accept.count()>0)  passedL1Calo=true;
    if(bitL2CaloAccept.count()>0)  passedL2Calo=true;

    m_efCaloSelector->emulation(m_onlElectrons, passedEFCalo, info);
    m_accept.setCutResult("EmTauRoI"            , passedL1Calo);
    m_accept.setCutResult("TrigEMCluster"       , passedL2Calo);
    m_accept.setCutResult("CaloClusterContainer", passedEFCalo);
    //execute object reconstructions combined (calo+ID)
    if (info.type == "electron") {
      m_l2Selector->emulation(m_trigElectrons, passedL2, info);
      m_efElectronSelector->emulation(m_onlElectrons, passedEF, info);
      m_accept.setCutResult("TrigElectronContainer", passedL2);
      m_accept.setCutResult("ElectronContainer"    , passedEF);
    }
    else if (info.type == "photon") {
      //m_l2Selector->emulation( trigPhCont, passedL2, info);
      //m_efPhotonSelector->emulation( phCont, passedEF, info);
      m_accept.setCutResult("TrigPhotonContainer", passedL2);
      m_accept.setCutResult("PhotonContainer"    , passedEF);
    }
    else {
      ATH_MSG_DEBUG("Type (e/g) not found from " << trigger);
      m_accept.clear();
      return m_accept;
    }

    if(info.ringer || info.perf){//bypass L2 Electron/Photon Level
      if(passedL1Calo && passedEFCalo && passedEF) pass = true;
    }else{
      if(passedL1Calo && passedL2Calo && passedL2 && passedEFCalo && passedEF) pass = true;
    }

    m_accept.setCutResult("HLT", pass);
  }else{
    ATH_MSG_WARNING("Can not emulate. Trigger not configurated");
  }
  return m_accept;
}

//!==========================================================================
bool TrigEgammaEmulationTool::isPassed(const std::string &trigger) {
  m_accept.clear();
  m_accept = executeTool(trigger);
  ATH_MSG_DEBUG("Trigger = "<< trigger );
  ATH_MSG_DEBUG("isPassed()::L1Calo = " << m_accept.getCutResult("EmTauRoI"));
  ATH_MSG_DEBUG("isPassed()::L2Calo = " << m_accept.getCutResult("TrigEMCluster"));
  ATH_MSG_DEBUG("isPassed()::L2     = " << m_accept.getCutResult("TrigElectronContainer"));
  ATH_MSG_DEBUG("isPassed()::EFCalo = " << m_accept.getCutResult("CaloClusterContainer"));
  ATH_MSG_DEBUG("isPassed()::EF     = " << m_accept.getCutResult("ElectronContainer"));
  return m_accept.getCutResult("HLT");
}

//!==========================================================================
bool TrigEgammaEmulationTool::isPassed(const std::string &trigger, const std::string &fctrigger) {
  m_accept.clear();
  const HLT::TriggerElement *finalFC = NULL;
  auto fc = m_trigdec->features(fctrigger, TrigDefs::alsoDeactivateTEs);
  auto vec = fc.get<xAOD::ElectronContainer>();
  boost::dynamic_bitset<> bitAccept(vec.size());
  unsigned bit=0;
  ATH_MSG_DEBUG("isPassed(trigger,fc)::ElectronContainer->size() = " << vec.size());
  for (const auto &feat : vec) {
    bit++;
    finalFC = feat.te();
    if (!finalFC) continue;
    m_accept = executeTool(finalFC, trigger);
    if(m_accept.getCutResult("HLT"))  bitAccept.set(bit-1,true);
    ATH_MSG_DEBUG("Trigger = "<< trigger << " fctrigger = " << fctrigger);
    ATH_MSG_DEBUG("isPassed()::L1Calo = " << m_accept.getCutResult("EmTauRoI"));
    ATH_MSG_DEBUG("isPassed()::L2Calo = " << m_accept.getCutResult("TrigEMCluster"));
    ATH_MSG_DEBUG("isPassed()::L2     = " << m_accept.getCutResult("TrigElectronContainer"));
    ATH_MSG_DEBUG("isPassed()::EFCalo = " << m_accept.getCutResult("CaloClusterContainer"));
    ATH_MSG_DEBUG("isPassed()::EF     = " << m_accept.getCutResult("ElectronContainer"));
  }
  bool pass=false;
  if(bitAccept.count()>0)  pass=true;

  return pass;
}

//!==========================================================================
void TrigEgammaEmulationTool::clearDecorations(){
  /*
  m_offElectrons->clearDecorations();  //Offline
  m_onlElectrons->clearDecorations();  //EFElectron
  m_caloClusters->clearDecorations();  //EFCalo
  m_trigElectrons->clearDecorations(); //L2Electron
  m_trigEMClusters->clearDecorations();//L2Calo
  m_emTauRois->clearDecorations();     //L1
  */
}
//!==========================================================================
//!==========================================================================
//!==========================================================================
//!==========================================================================
//!==========================================================================
//!==========================================================================
//!==========================================================================
//!==========================================================================
void TrigEgammaEmulationTool::setTrigInfo(const std::string &trigger){
  std::string type="";
  bool isL1=false;
  float etthr=0;
  float l1thr=0;
  std::string l1type="";
  std::string pidname="";
  bool perf=false;
  bool idperf=false;
  bool etcut=false;
  bool isolation=false;
  bool hltcalo=false;
  bool ringer=false;
  parseTriggerName(trigger,"Loose",isL1,type,etthr,l1thr,l1type,pidname,etcut,perf); // Determines probe PID from trigger
  std::string l1item = "";
  if(isL1) l1item=trigger;
  else getL1Item(trigger);
  std::string decorator="is"+pidname; 
  if (boost::contains(trigger,"iloose") ) isolation = true;
  if (boost::contains(trigger,"idperf") ) idperf    = true;
  if (boost::contains(trigger,"HLTCalo")) hltcalo   = true;
  if (boost::contains(trigger,"ringer") ) ringer    = true;
  if(isL1) etthr=l1thr; // Should be handled elsewhere
  std::string strEtthr = boost::lexical_cast<std::string>(etthr);
  TrigInfo info{trigger,type,strEtthr,l1item,l1type,pidname,decorator,isolation,isL1,perf,idperf,hltcalo,ringer,etcut,etthr,l1thr};
  m_trigInfo[trigger] = info;
}

//!==========================================================================

TrigInfo TrigEgammaEmulationTool::getTrigInfo(const std::string &trigger){
  return m_trigInfo[trigger];
}
//!==========================================================================
void TrigEgammaEmulationTool::parseTriggerName(const std::string trigger, std::string defaultPid,bool &isL1,std::string &type,
        float &threshold, float &l1threshold, std::string &l1type, std::string &pidname, bool &etcut, bool &perf){
  

  // Analyze L1 or HLT item
  bool result = boost::starts_with( trigger , "L1" );
  if (result) {
      std::string l1info = trigger;
      l1info.erase(0,4);
      l1type = boost::trim_copy_if(l1info, boost::is_digit());
      std::string l1cut = boost::trim_copy_if(l1info, !boost::is_digit());
      l1threshold = atof(l1cut.c_str());
      threshold = l1threshold;
      isL1=true;
      pidname = defaultPid;
      type = "electron"; // for now only electron L1 studies
  }
  else {
      std::string hltinfo=trigger;
      if(boost::contains(hltinfo,"HLT")) hltinfo.erase(0,4);
      std::string l1item = getL1Item(trigger);
      ATH_MSG_DEBUG("Trigger L1item " << trigger << " " << l1item << " " << hltinfo);
      std::vector<std::string> strs;
      boost::split(strs,hltinfo,boost::is_any_of("_"));
      for (std::vector<std::string>::iterator it = strs.begin(); it != strs.end(); ++it)
      {
          ATH_MSG_DEBUG("Trigger parse "  << *it);
      }
      // Set probe Pid from second part of trigger name
      // Non pid triggers use default Probe which is set as a property


      if(boost::contains(strs.at(0),"e")) type = "electron";
      else if(boost::contains(strs.at(0),"g")) type = "photon";
      else ATH_MSG_ERROR("Cannot set trigger type from name");
      if(boost::contains(strs.at(1),"perf")){
          //pidname = defaultPid;
          pidname = "";
          perf=true;
          ATH_MSG_DEBUG("Perf " << perf << " " << pidname );
      }
      else if(boost::contains(strs.at(1),"L2Star")){
          pidname = defaultPid;
          perf=true;
          ATH_MSG_DEBUG("L2Star " << perf << " " << pidname );
      }
      else if(boost::contains(strs.at(1),"hiptrt")){
          pidname = defaultPid;
          perf=true;
          ATH_MSG_DEBUG("hiptrt " << perf << " " << pidname );
      }
      else if( strs.at(1)== "etcut"){
          pidname = defaultPid;
          etcut=true;
      }
      else pidname = getPid(strs.at(1));

      //Get the L1 information

      if(boost::contains(strs.back(),"L1")){
          std::string l1info = strs.back();
          l1info.erase(0,4);
          l1type = boost::trim_copy_if(l1info, boost::is_digit());
          std::string l1cut = boost::trim_copy_if(l1info, !boost::is_digit());
          l1threshold = atof(l1cut.c_str());

          ATH_MSG_DEBUG("L1 item " << l1info << " " << l1threshold << " " << l1type);
      }

      // Get the threshold
      std::string str_thr = strs.at(0);
      str_thr.erase( 0, 1);
      threshold = atof(str_thr.c_str());

      isL1=false;
      ATH_MSG_DEBUG(trigger << " " << type << " " << pidname << " " << threshold);
  }

}
//!==========================================================================
std::string TrigEgammaEmulationTool::getL1Item(std::string trigger){
    auto trig_conf = m_trigdec->ExperimentalAndExpertMethods()->getChainConfigurationDetails(trigger);
    std::string L1_seed = "";
    if(trig_conf != NULL){
        ATH_MSG_DEBUG("TrigConf available");
        L1_seed = trig_conf->lower_chain_name(); //L1 trigger seed             
    }
    return L1_seed;
}

//!==========================================================================
std::string TrigEgammaEmulationTool::getPid(const std::string pidtype){
  static std::map<std::string,std::string> PidMap; //no longer class member but static
  if(PidMap.empty()){
      PidMap["vloose"]="Loose";
      PidMap["loose"]="Loose";
      PidMap["medium"]="Medium";
      PidMap["tight"]="Tight";
      PidMap["loose1"]="Loose";
      PidMap["medium1"]="Medium";
      PidMap["tight1"]="Tight";
      PidMap["lhvloose"]="LHLoose";
      PidMap["lhloose"]="LHLoose";
      PidMap["lhmedium"]="LHMedium";
      PidMap["lhtight"]="LHTight";
  }
  return PidMap[pidtype];
}
//!==========================================================================

