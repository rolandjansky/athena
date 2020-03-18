/*
 *   Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 *   */


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
#include <boost/algorithm/string/replace.hpp>
#include "StoreGate/StoreGateSvc.h"
#include "TrigEgammaEmulationTool/TrigEgammaEmulationTool.h"
#include "TrigSteeringEvent/TrigPassBits.h"
#include "TrigSteeringEvent/TrigPassBitsCollection.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"
#include "TrigDecisionTool/TrigDecisionTool.h"
//**********************************************************************
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
    declareProperty("TriggerList"             , m_trigList                );
    declareProperty("SupportingTriggerList"   , m_supportingTrigList      );
    declareProperty("ElectronKey"             , m_offElContKey="Electrons");
    declareProperty("MatchTool"               , m_matchTool               );
    declareProperty("L1SelectorTool"          , m_l1Selector              );
    declareProperty("L2SelectorTool"          , m_l2Selector              );
    declareProperty("EFCaloSelectorTool"      , m_efCaloSelector          );
    declareProperty("EFElectronSelectorTools" , m_efElectronSelector      );
    declareProperty("EFPhotonSelectorTools"   , m_efPhotonSelector        );

    // for expert: use this flags to switch and custo your menu
    declareProperty("DoL2ElectronFex"         , m_doL2ElectronFex=false   );
    declareProperty("DoRinger"                , m_doRinger=false          );
    declareProperty("DoRingerBelow15GeV"      , m_doRingerBelow15GeV=false);
    declareProperty("DoEFCaloPid"             , m_doEFCaloPid=false       );


    // If this is enable, the emulator will emulate the TE using all features available.
    // And all levels will be emulate!
    m_experimentalAndExpertMethods=false;
    m_offElectrons   =nullptr;
    m_onlElectrons   =nullptr;
    m_onlPhotons     =nullptr;
    m_trigElectrons  =nullptr;
    m_caloClusters   =nullptr;
    m_trigEMClusters =nullptr;
    m_emTauRois      =nullptr;
    m_storeGate      =nullptr;
    m_l1Cont         =nullptr;

    // just for compile
    HLT::TriggerElement* t = nullptr;
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

    ATH_MSG_INFO("Initialising Selectors tool...");
    //Enable expert methods
    m_trigdec->ExperimentalAndExpertMethods()->enable();

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
    for(auto& tool : m_efElectronSelector){
      tool->setParents(m_trigdec, m_storeGate);
      sc = tool->initialize();
      if(sc.isFailure()){
        ATH_MSG_ERROR( "Unable to initialize EF Electron selector tool." );
        return sc;
      }
    }

    ATH_MSG_INFO("Initialising EF Photon Selectors tool...");
    for(auto& tool : m_efPhotonSelector){
      tool->setParents(m_trigdec, m_storeGate);
      sc = tool->initialize();
      if(sc.isFailure()){
        ATH_MSG_ERROR( "Unable to initialize EF Photon selector tool." );
        return sc;
      }
    }

    ATH_MSG_INFO("Initialising accept...");
    //add cuts into TAccept
    m_accept.addCut("L1Calo"  , "Trigger L1Calo step"     );
    m_accept.addCut("L2Calo"  , "Trigger L2Calo step"     );
    m_accept.addCut("L2"      , "Trigger L2Electron step" );
    m_accept.addCut("EFCalo"  , "Trigger EFCalo step"     );
    m_accept.addCut("EFTrack" , "Trigger EFTrack step"    );
    m_accept.addCut("HLT"     , "Trigger HLT decision"    );
    m_accept.addCut("L1_matched"  , "L1 Trigger object Matched"                                   );
    m_accept.addCut("TE_full"     , "The Trigger Element has all objects needed by the emulation"  );
 
    ATH_MSG_INFO("Initialising trigMap...");
    auto trigList = m_trigList;

    for(const auto trigName : trigList){
      ATH_MSG_DEBUG("Trigger " << trigName <<" to TrigInfo");
      if(m_trigInfo.count(trigName) != 0){
      }else{
        setTrigInfo(trigName);
      }
    }//Loop over trigger list

    ATH_MSG_INFO("Initialising supporting trigger...");

    std::vector<std::string> supportingTrigList;
    auto chains = m_trigdec->getListOfTriggers("HLT_e.*");


    ATH_MSG_INFO("Supporting trigger before cleaner...");
    for(const auto trigName:m_supportingTrigList){
      ATH_MSG_INFO("    Suppot: " << trigName);
    }

    // All support triggers must be inside of the xaod
    for(const auto trigName:m_supportingTrigList){
      if (std::find(chains.begin(), chains.end(), trigName) != chains.end()){
        if(m_trigInfo.count(trigName) == 0){
          ATH_MSG_INFO("Append " << trigName << " as support trigger");
          supportingTrigList.push_back(trigName);
          setTrigInfo(trigName);
        }
        // To avoid that loose some chain in support trigger list
        else{
          supportingTrigList.push_back(trigName);
        }
      }
    }

    // Overwrite all support list
    m_supportingTrigList.clear();
    m_supportingTrigList.insert(m_supportingTrigList.end(), supportingTrigList.begin(), supportingTrigList.end());

    ATH_MSG_INFO("Supporting trigger after cleaner...");
    for(const auto trigName:m_supportingTrigList){
      ATH_MSG_INFO("    Suppot: " << trigName);
    }


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

    for(auto& tool : m_efElectronSelector){
      sc = tool->finalize();
      if(sc.isFailure()){
        ATH_MSG_ERROR( "Unable to finalize EF Electron selector tool." );
        return sc;
      }
    }

    for(auto& tool : m_efPhotonSelector){
      sc = tool->finalize();
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
//! Emulation from Trigger Element
asg::AcceptData TrigEgammaEmulationTool::executeTool(const HLT::TriggerElement *te_external, const std::string &trigger) {

  ATH_MSG_DEBUG("TrigEgammaEmulationTool::executeTool(te, trigger)");
  asg::AcceptData acceptData (&m_accept);
  
  if(m_trigInfo.count(trigger) != 0){

    Trig::Info info     = getTrigInfo(trigger);
    bool passedL1Calo = false;
    bool passedL2Calo = false;
    bool passedL2     = false;
    bool passedEFCalo = false;
    bool passedEFTrack= false;
    bool passedHLT    = false;

    // Trigger Element holder
    const HLT::TriggerElement *te=nullptr;
    bool isGoodToGo=false;

    // Here, if the te_external does not have the Electron object means that this te is not completed.
    // We need to match objects with this RoI to complete the chain.
    if(!getFeature<xAOD::ElectronContainer>(te_external)){

      const auto* l1 = getFeature<xAOD::EmTauRoI>(te_external);
      if(!l1){// Its not possible to match because this TE does not have EmTauRoI, abort
        ATH_MSG_WARNING("Can not retrieve the support element because the current TE does not has xAOD::EmTauRoI object!");
        return acceptData;
      }

      // There is L1 object in this RoI
      acceptData.setCutResult("L1_matched", true);

      // This object is not fully completed, try to found other.
      for (const auto &fctrigger : m_supportingTrigList){
        ATH_MSG_DEBUG("Searching for EL objects in "<< fctrigger);
        // Retrieve all trigger elements for this fctrigger
        auto fc = m_trigdec->features(fctrigger, TrigDefs::alsoDeactivateTEs);
        // Only TEs with all objects
        auto vec = fc.get<xAOD::ElectronContainer>();
        // Try to match by roiword
        ATH_MSG_DEBUG("VEC SIZE IS "<< vec.size());
        for(const auto &feat : vec){
          if(l1->roiWord() == getFeature<xAOD::EmTauRoI>(feat.te())->roiWord()){
            ATH_MSG_DEBUG("Searching for EL objects in "<< fctrigger << ": FOUND");
            te = feat.te();
            isGoodToGo=true;
            break;
          }
        }// Loop over Trigger elements
        if(isGoodToGo)
          break;
      }// Loop over support trigger
    }else{
      // The current TE has all objects needed by the emulation
      acceptData.setCutResult("L1_matched", true);
      isGoodToGo=true;
      te=te_external;
      setTEMatched(te);
    }

    if(!isGoodToGo){
      if (m_experimentalAndExpertMethods){
        ATH_MSG_WARNING("The experimental method is enable. The trigger element is not completed and the emulation will proced until the end and by pass all levels.");
        te=te_external;
        setTEMatched(te);
      }else{
        ATH_MSG_WARNING("This Trigger Element does not have all features needed by the emulation tool. The external match is " <<
                     " not possible! Maybe the support trigger list not attend all requirements.");
        setTEMatched(nullptr);
        return acceptData;
      }
    }



    ATH_MSG_DEBUG("getFeatures from TE...");

    // The TE has RoI and now have all objects needed by the emulatio.
    acceptData.setCutResult("TE_full", true);

    const auto* l1            = getFeature<xAOD::EmTauRoI>(te);
    const auto* emCluster     = getFeature<xAOD::TrigEMCluster>(te);
    const auto* trigElCont    = getFeature<xAOD::TrigElectronContainer>(te);
    //const auto* clusCont    = getFeature<xAOD::CaloClusterContainer>(te);
    const auto* elCont        = getFeature<xAOD::ElectronContainer>(te);
    //const auto* phCont        = getFeature<xAOD::PhotonContainer>(te);

    if(elCont){
      for(auto& tool : m_efElectronSelector)
        tool->setTe(te); //Must be passed to config track isolation
    }

    // Level 1
    m_l1Selector->emulation( l1, passedL1Calo , info);
    // passedL1Calo = ancestorPassed<xAOD::EmTauRoI>(te_external);
    acceptData.setCutResult("L1Calo", passedL1Calo);

    if( (passedL1Calo ) && !info.isL1 ){

      m_l2Selector->emulation( emCluster, passedL2Calo , info);
      acceptData.setCutResult("L2Calo", passedL2Calo);

      if(passedL2Calo ){
        if(info.perf){//bypass L2 Electron/Photon Level
          passedL2=true;
          acceptData.setCutResult("L2", passedL2);
        }else{
          if (info.type == "electron") {
            if(m_doL2ElectronFex)
              m_l2Selector->emulation( trigElCont, passedL2, info);
            else
              passedL2=true;
            acceptData.setCutResult("L2", passedL2);
          }
          else if (info.type == "photon") {
            //m_l2Selector->emulation( trigPhCont, passedL2, trigger);
            //m_efPhotonSelector->emulation( phCont, passedEF, trigger);
            passedL2=true;
            acceptData.setCutResult("L2", passedL2);
          }
        }//bypass L2

        if (passedL2){
          m_efCaloSelector->emulation( elCont, passedEFCalo, info);
          acceptData.setCutResult("EFCalo", passedEFCalo);

          if(passedEFCalo){
            passedEFTrack=true;
            acceptData.setCutResult("EFTrack"    , passedEFTrack);

            if(passedEFTrack){
              if(!emulationHLT(elCont, passedHLT, info)){
                acceptData.clear();
                return acceptData;
              }else{
                acceptData.setCutResult("HLT"    , passedHLT);
              }
            }//EFTrack
          }//EFCalo
        }//L2
      }//L2Calo
    }//L1Calo
  }else{
    ATH_MSG_WARNING("Can not emulate " << trigger << ". This chain must be added into trigList before the creation.");
  }

  return acceptData;
}
//!==========================================================================
//! Emulation from xAOD containers not using TDT tools
asg::AcceptData TrigEgammaEmulationTool::executeTool(const std::string &trigger) {
  clearDecorations();
  asg::AcceptData acceptData (&m_accept);

  if( m_trigInfo.count(trigger) != 0){
    Trig::Info info     = getTrigInfo(trigger);
    bool passedL1Calo = false;
    bool passedL2Calo = false;
    bool passedL2     = false;
    bool passedEFCalo = false;
    bool passedEFTrack= false;
    bool passedHLT    = false;

    boost::dynamic_bitset<> bitL1Accept(m_emTauRois->size());
    boost::dynamic_bitset<> bitL2CaloAccept(m_trigEMClusters->size());

    //loop over objects
    unsigned bit=0;  bool pass=false;
    for(const auto& l1 : *m_emTauRois) {
      m_l1Selector->emulation(l1, pass, info);
      bitL1Accept.set(bit, pass);
      bit++;
    }
    if(bitL1Accept.count()>0)  passedL1Calo=true;
    acceptData.setCutResult("L1Calo", passedL1Calo);

    if(passedL1Calo  && !info.isL1){
      bit=0; pass=false;
      //loop over objects
      for(const auto& emCluster : *m_trigEMClusters){
        m_l2Selector->emulation(emCluster, pass, info);
        bitL2CaloAccept.set(bit, pass);
        bit++;
      }

      if(bitL2CaloAccept.count()>0)  passedL2Calo=true;
      acceptData.setCutResult("L2Calo", passedL2Calo);

      if(passedL2Calo) {
        if(info.perf){//bypass L2 Electron/Photon Level
          passedL2=true;
          acceptData.setCutResult("L2", passedL2);
        }else{
          if (info.type == "electron") {
            if(m_doL2ElectronFex)
              m_l2Selector->emulation(m_trigElectrons, passedL2, info);
            else
              passedL2=true;
            acceptData.setCutResult("L2", passedL2);
          }
          else if (info.type == "photon") {
            //m_l2Selector->emulation( trigPhCont, passedL2, trigger);
            //m_efPhotonSelector->emulation( phCont, passedEF, trigger);
            passedL2=true;
            acceptData.setCutResult("L2", passedL2);
          }
        }//bypass L2

        if (passedL2){

          m_efCaloSelector->emulation(m_onlElectrons, passedEFCalo, info);
          acceptData.setCutResult("EFCalo", passedEFCalo);

          if(passedEFCalo){
            //TODO: running the EF track step
            passedEFTrack=true;
            acceptData.setCutResult("EFTrack", passedEFTrack);

            if(passedEFTrack){
              if(!emulationHLT(m_onlElectrons, passedHLT, info)){
                acceptData.clear();
                return acceptData;
              }else{
                acceptData.setCutResult("HLT"    , passedHLT);
              }
            }//EFTrack

          }//EFCalo
        }//L2
      }//L2Calo
    }//L1Calo
  }else{
    ATH_MSG_WARNING("Can not emulate. Trigger not configurated");
  }
  return acceptData;
}


//!==========================================================================
bool TrigEgammaEmulationTool::isPassed(const std::string &trigger) {
  asg::AcceptData acceptData = executeTool(trigger);
  ATH_MSG_DEBUG("Trigger = "<< trigger );
  ATH_MSG_DEBUG("isPassed()::L1Calo = " << acceptData.getCutResult("L1"));
  ATH_MSG_DEBUG("isPassed()::L2Calo = " << acceptData.getCutResult("L2Calo"));
  ATH_MSG_DEBUG("isPassed()::L2     = " << acceptData.getCutResult("L2"));
  ATH_MSG_DEBUG("isPassed()::EFCalo = " << acceptData.getCutResult("EFCalo"));
  ATH_MSG_DEBUG("isPassed()::EFTrack= " << acceptData.getCutResult("EFTrack"));
  ATH_MSG_DEBUG("isPassed()::HLT    = " << acceptData.getCutResult("HLT"));
  return acceptData.getCutResult("HLT");
}

//!==========================================================================
bool TrigEgammaEmulationTool::isPassed(const std::string &trigger, const std::string &fctrigger) {
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
    asg::AcceptData acceptData = executeTool(finalFC, trigger);
    if(acceptData.getCutResult("HLT"))  bitAccept.set(bit-1,true);
    ATH_MSG_DEBUG("isPassed()::L1Calo = " << acceptData.getCutResult("L1"));
    ATH_MSG_DEBUG("isPassed()::L2Calo = " << acceptData.getCutResult("L2Calo"));
    ATH_MSG_DEBUG("isPassed()::L2     = " << acceptData.getCutResult("L2"));
    ATH_MSG_DEBUG("isPassed()::EFCalo = " << acceptData.getCutResult("EFCalo"));
    ATH_MSG_DEBUG("isPassed()::EFTrack= " << acceptData.getCutResult("EFTrack"));
    ATH_MSG_DEBUG("isPassed()::HLT    = " << acceptData.getCutResult("HLT"));
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
bool TrigEgammaEmulationTool::emulationHLT(const xAOD::IParticleContainer *container, bool &pass, const Trig::Info &info){

  if(info.type == "electron"){
    for( auto& tool : m_efElectronSelector){
      if( tool->emulation(container, pass, info) )
        break;
    }// Loop over electron tools
  }else if(info.type == "photon"){
    for(auto& tool : m_efPhotonSelector){
      if( tool->emulation(container, pass, info) )
        break;
    }// Loop ever Photon tools
  }else{
    ATH_MSG_WARNING("Type (e/g) not found from " << info.trigName);
    return false;
  }
  return true;
}

bool TrigEgammaEmulationTool::emulationL2Calo(const xAOD::TrigEMCluster *emCluster, std::string pidname)
{
  bool pass=false;
  Trig::Info info;
  info.ringer=false;
  info.etcut=false;
  info.perf = false;
  info.hltcalo=false;
  info.idperf=false;
  info.thrL2Calo=0.0;
  info.thrHLT=emCluster->et()*1e-3;
  info.pidname=pidname;
  m_l2Selector->emulation(emCluster, pass, info);
  return pass;
}
  
 
//!==========================================================================
void TrigEgammaEmulationTool::setTrigInfo(const std::string trigger){
  ATH_MSG_DEBUG("setTrigInfo::trigger = "<< trigger);
  std::string type="";
  bool isL1=false;
  float etthr=0;
  float l1thr=0;
  std::string l1type="";
  std::string pidname="";
  std::string lhinfo;
  bool perf=false;
  bool idperf=false;
  bool etcut=false;
  bool isolation=false;
  bool hltcalo=false;
  bool ringer=false;
  std::string l1item = "";
  parseTriggerName(trigger,"Loose",isL1,type,etthr,l1thr,l1type,l1item,pidname,etcut,perf); // Determines probe PID from trigger


  std::string decorator="is"+pidname;
  // isolation types
  if (boost::contains(trigger,"iloose") || boost::contains(trigger, "ivarloose") ||
      boost::contains(trigger,"ivarmedium") || boost::contains(trigger, "ivartight") ||
      boost::contains(trigger,"icaloloose") || boost::contains(trigger, "icalomedium") ||
      boost::contains(trigger,"icalotight") ) isolation = true;
  if (boost::contains(trigger,"idperf") ) idperf    = true;
  if (boost::contains(trigger,"HLTCalo")) hltcalo   = true;

  // Apply L2Calo ringer?
  if (boost::contains(trigger,"noringer") )
    ringer = false;
  else if ( boost::contains(trigger,"ringer") )
    ringer = true;
  else if ( etthr >= 15.0 && m_doRinger )
    ringer = true;
  else if ( etthr < 15.0 && m_doRinger && m_doRingerBelow15GeV )
    ringer = true;
  else
    ringer  =false;


  //Likelihood special tunings
  if (boost::contains(trigger,"nod0"))                lhinfo = "nod0";
  else if(boost::contains(trigger, "cutd0dphideta"))  lhinfo = "cutd0dphideta";
  else if(boost::contains(trigger, "nodeta"))         lhinfo = "nodeta";
  else if(boost::contains(trigger, "nodphires"))      lhinfo = "nodphires";
  else if(boost::contains(trigger, "smooth"))         lhinfo = "smooth";
  else{
    lhinfo="";
  }

  if(isL1) etthr=l1thr; // Should be handled elsewhere
  std::string strEtthr = boost::lexical_cast<std::string>(etthr);
  Trig::Info info{trigger,type,strEtthr,l1item,l1type,pidname,decorator,lhinfo,isolation,isL1,perf,
    idperf,hltcalo,ringer,etcut,m_doL2ElectronFex,m_doEFCaloPid,etthr,l1thr,etthr};
  m_trigInfo.insert(std::pair<std::string,Trig::Info>(trigger, info));
  ATH_MSG_DEBUG("Inserting trigger: " << trigger << ", completed.");
}

//!==========================================================================
Trig::Info TrigEgammaEmulationTool::getTrigInfo(const std::string &trigger){
  return m_trigInfo[trigger];
}
//!==========================================================================
void TrigEgammaEmulationTool::parseTriggerName(const std::string trigger, std::string defaultPid,bool &isL1,std::string &type,
        float &threshold, float &l1threshold, std::string &l1type, std::string &l1item, std::string &pidname, bool &etcut, bool &perf){

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
      l1item=l1info;
      pidname = defaultPid;
      type = "electron"; // for now only electron L1 studies
  }
  else {
      std::string hltinfo=trigger;
      if(boost::contains(hltinfo,"HLT")) hltinfo.erase(0,4);
      l1item = getL1Item(trigger);
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
      if(!boost::contains(strs.back(),"L1"))  strs.push_back(l1item);


      if(boost::contains(strs.back(),"L1")){
        std::string l1info = strs.back();
        l1info.erase(0,4);
        l1type = boost::trim_copy_if(l1info, boost::is_digit());
        std::string l1cut = boost::trim_copy_if(l1info, !boost::is_digit());
        l1threshold = atof(l1cut.c_str());
        l1item=l1info;
        ATH_MSG_DEBUG("L1 item " << l1item << " " << l1threshold << " " << l1type);
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
    if(trig_conf != nullptr){
        ATH_MSG_DEBUG("TrigConf available");
        L1_seed = trig_conf->lower_chain_name(); //L1 trigger seed
        boost::replace_all( L1_seed, "_", "" );
    }else{
      ATH_MSG_WARNING("There is no L1 Seed for this trigger "<< trigger);
    }
    return L1_seed;
}

//!==========================================================================
std::string TrigEgammaEmulationTool::getPid(const std::string pidtype){
  static std::map<std::string,std::string> PidMap; //no longer class member but static
  if(PidMap.empty()){
      PidMap["vloose"]="VLoose";
      PidMap["loose"]="Loose";
      PidMap["medium"]="Medium";
      PidMap["tight"]="Tight";
      PidMap["loose1"]="Loose";
      PidMap["medium1"]="Medium";
      PidMap["tight1"]="Tight";
      PidMap["lhvloose"]="LHVLoose";
      PidMap["lhloose"]="LHLoose";
      PidMap["lhmedium"]="LHMedium";
      PidMap["lhtight"]="LHTight";
  }
  return PidMap[pidtype];
}

//!==========================================================================

void TrigEgammaEmulationTool::match(const xAOD::Egamma *el,  const  HLT::TriggerElement* &te_external )
{
  const HLT::TriggerElement *feat=nullptr;
  te_external=nullptr;
  unsigned nobj1,nobj2;
  nobj1=nobj2=0;


  for(auto& trigItem : m_supportingTrigList){
    // Trigger match using Matching tool
    m_matchTool->match(el, trigItem, feat);
    if(feat){// If has a match!

      ATH_MSG_DEBUG("AKI, match with " << trigItem );

      if(te_external){
        nobj2=0;
        if(getFeature<xAOD::ElectronContainer>(feat)    ) nobj2++; // HLT
        ATH_MSG_DEBUG("Electron " << nobj2);
        if(getFeature<xAOD::CaloClusterContainer>(feat) ) nobj2++; // EFCalo
        ATH_MSG_DEBUG("Calo " << nobj2);
        if(getFeature<xAOD::TrigElectronContainer>(feat)) nobj2++; // L2
        ATH_MSG_DEBUG("TrigElectron " << nobj2);
        if(getFeature<xAOD::TrigEMCluster>(feat)        ) nobj2++; // L2Calo
        ATH_MSG_DEBUG("Cluster " << nobj2);
        if(getFeature<xAOD::EmTauRoI>(feat)             ) nobj2++; // L2Calo
        ATH_MSG_DEBUG("L1 " << nobj2);
        if(nobj2>nobj1){
          te_external=feat;
          nobj1=nobj2;
        }
        //ATH_MSG_INFO("ET  = " << getFeature<xAOD::TrigEMCluster>(te_external)->et());
      }else{
        if(getFeature<xAOD::ElectronContainer>(feat)    ) nobj1++; // HLT
        ATH_MSG_DEBUG("Electron " << nobj1);
        if(getFeature<xAOD::CaloClusterContainer>(feat) ) nobj1++; // EFCalo
        ATH_MSG_DEBUG("Calo " << nobj1);
        if(getFeature<xAOD::TrigElectronContainer>(feat)) nobj1++; // L2
        ATH_MSG_DEBUG("TrigElectron " << nobj1);
        if(getFeature<xAOD::TrigEMCluster>(feat)        ) nobj1++; // L2Calo
        ATH_MSG_DEBUG("Cluster " << nobj1);
        if(getFeature<xAOD::EmTauRoI>(feat)             ) nobj1++; // L2Calo
        ATH_MSG_DEBUG("L1 " << nobj1);
        te_external=feat;
      }

    }
    ATH_MSG_DEBUG("Electron with ET = " << el->pt()*1e-3 << " GeV match with " << trigItem
                  << " is good? " << (te_external?"yes":"no") << " and has " << nobj1 << " features.");

    // All objects is needed
    if(nobj1 == 5)  break;
  }// Loop over support triggers

}



