/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id: JetObjectCollectionMaker.cxx 809674 2017-08-23 14:10:24Z iconnell $
#include "TopSystematicObjectMaker/JetObjectCollectionMaker.h"
#include "TopConfiguration/TopConfig.h"
#include "TopEvent/EventTools.h"

#include "xAODJet/JetContainer.h"
#include "xAODJet/JetAuxContainer.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODCore/ShallowCopy.h"
#include "xAODBase/IParticleHelpers.h"
#include "xAODMissingET/MissingETContainer.h"
#include "PATInterfaces/SystematicsUtil.h"

#include "TopJetSubstructure/TopJetSubstructure.h"
#include "TopJetSubstructure/LargeJetTrimmer.h"
#include "TopJetSubstructure/SubjetMaker.h"

namespace top{

JetObjectCollectionMaker::JetObjectCollectionMaker( const std::string& name ) :
  asg::AsgTool( name ),
  m_config(nullptr),
  m_doJER(false),
  m_doFull_JER(false),
  m_doFull_JER_Pseudodata(false),
  m_isMC(false),
  m_doMultipleJES(false),

  m_specifiedSystematics(),
  m_specifiedSystematicsLargeR(),
  m_specifiedSystematicsTrackJets(),
  m_recommendedSystematics(),
  m_nominalSystematicSet(),

  m_jetCalibrationTool("JetCalibrationTool"),
  m_jetCalibrationToolLargeR("JetCalibrationToolLargeR"),

  m_jetUncertaintiesTool("JetUncertaintiesTool"),
  m_jetUncertaintiesToolFrozenJMS("JetUncertaintiesToolFrozenJMS"),
  m_jetUncertaintiesToolReducedNPScenario1("JetUncertaintiesToolReducedNPScenario1"),
  m_jetUncertaintiesToolReducedNPScenario2("JetUncertaintiesToolReducedNPScenario2"),
  m_jetUncertaintiesToolReducedNPScenario3("JetUncertaintiesToolReducedNPScenario3"),
  m_jetUncertaintiesToolReducedNPScenario4("JetUncertaintiesToolReducedNPScenario4"),
  m_jetUncertaintiesToolLargeR("JetUncertaintiesToolLargeR"),

  m_jetUpdateJvtTool("JetUpdateJvtTool"),
  m_fjvtTool("fJVTTool"),

  m_jetSubstructure(nullptr),

  m_systMap_AllNP(),
  m_systMap_AllNP_FrozenJMS(),
  m_systMap_ReducedNPScenario1(),
  m_systMap_ReducedNPScenario2(),
  m_systMap_ReducedNPScenario3(),
  m_systMap_ReducedNPScenario4(),
  m_systMap_LargeR()
{
  declareProperty( "config" , m_config );

  declareProperty( "JetCalibrationTool" , m_jetCalibrationTool );
  declareProperty( "JetCalibrationToolLargeR" , m_jetCalibrationToolLargeR );

  declareProperty( "JetUncertaintiesTool" , m_jetUncertaintiesTool);
  declareProperty( "JetUncertaintiesToolFrozenJMS" , m_jetUncertaintiesToolFrozenJMS);
  declareProperty( "JetUncertaintiesToolReducedNPScenario1" , m_jetUncertaintiesToolReducedNPScenario1 );
  declareProperty( "JetUncertaintiesToolReducedNPScenario2" , m_jetUncertaintiesToolReducedNPScenario2 );
  declareProperty( "JetUncertaintiesToolReducedNPScenario3" , m_jetUncertaintiesToolReducedNPScenario3 );
  declareProperty( "JetUncertaintiesToolReducedNPScenario4" , m_jetUncertaintiesToolReducedNPScenario4 );
  declareProperty( "JetUncertaintiesToolLargeR" , m_jetUncertaintiesToolLargeR);

  declareProperty( "JetUpdateJvtTool" , m_jetUpdateJvtTool );

  declareProperty( "TruthJetCollectionForHSTagging" , m_truthJetCollForHS = "AntiKt4TruthJets" );
}

StatusCode JetObjectCollectionMaker::initialize() {
  ATH_MSG_INFO(" top::JetObjectCollectionMaker initialize" );

  ///-- Lets do the nominal systematics --///
  ///-- JetObjectCollectionMaker is a little different from the others --///
  m_specifiedSystematics.push_back(CP::SystematicSet());
  m_specifiedSystematicsLargeR.push_back(CP::SystematicSet());
  m_specifiedSystematicsTrackJets.push_back(CP::SystematicSet());
  m_recommendedSystematics.push_back(CP::SystematicSet());


  top::check( m_jetCalibrationTool.retrieve() , "Failed to retrieve JetCalibrationTool" );
  if (m_config->useLargeRJets()) {
    top::check( m_jetCalibrationToolLargeR.retrieve(),
                "Failed to retrieve JetCalibrationToolLargeR" );
    top::check( m_jetUncertaintiesToolLargeR.retrieve(),
                "Failed to retrieve JetUncertaintiesToolLargeR" );
  }

  ///-- JER uncertainties model --///
  if (m_config->jetJERSmearingModel() == "Full" || m_config->jetJERSmearingModel() == "All")
    m_doFull_JER = true;
  if (m_config->jetJERSmearingModel() == "Simple")
    m_doFull_JER = false;
  if (m_config->jetJERSmearingModel() == "Full_PseudoData" || m_config->jetJERSmearingModel() == "All_PseudoData")
    m_doFull_JER_Pseudodata=true;
  else m_doFull_JER_Pseudodata=false;


  m_isMC=m_config->isMC();
  m_doMultipleJES=m_config->doMultipleJES();

  ///-- Are we doing JER? Let's not repeat this logic over and over --///
  if (m_isMC)
    m_doJER = true;
  if (!m_isMC && m_doFull_JER)
    m_doJER = true;
  /// NB: for "Full_PseudoData", no need to smear the data, so m_doJER is false on data unless it's "Full"

  if(m_isMC || m_doFull_JER){
    if (!m_doMultipleJES) {
      top::check( m_jetUncertaintiesTool.retrieve() , "Failed to retrieve JetUncertaintiesTool" );
      if ( m_config->jetCalibSequence() == "JMS" ){
	top::check( m_jetUncertaintiesToolFrozenJMS.retrieve() , "Failed to retrieve JetUncertaintiesTool (FrozenJMS)" );
      }
    }
    else{
      top::check( m_jetUncertaintiesToolReducedNPScenario1.retrieve() , "Failed to retrieve JetUncertaintiesToolReducedNPScenario1" );
      top::check( m_jetUncertaintiesToolReducedNPScenario2.retrieve() , "Failed to retrieve JetUncertaintiesToolReducedNPScenario2" );
      top::check( m_jetUncertaintiesToolReducedNPScenario3.retrieve() , "Failed to retrieve JetUncertaintiesToolReducedNPScenario3" );
      top::check( m_jetUncertaintiesToolReducedNPScenario4.retrieve() , "Failed to retrieve JetUncertaintiesToolReducedNPScenario4" );
    }
  }
  
  




  top::check( m_jetUpdateJvtTool.retrieve() , "Failed to retrieve JetUpdateJvtTool" );
  top::check(m_fjvtTool.retrieve(), "Failed to retrieve fJVTTool");

  // Take this from the TopConfiguration
  // A blank vector will setup all systematics

  const std:: string& syststr = m_config->systematics();
  std::set<std::string> syst,systLargeR;

  if( !m_config->isSystNominal(syststr) && !m_config->isSystAll(syststr)){
    bool ok = m_config->getSystematicsList(syststr, syst);
    bool okLargeR = m_config->getSystematicsList(syststr, systLargeR);
    if (!ok || !okLargeR) {
      ATH_MSG_ERROR(" top::JetObjectCollectionMaker could not determine systematic list" );
      return StatusCode::FAILURE;
    }
    if (m_config->contains(syst, "AllJets")) {
      syst.clear();
      systLargeR.clear();
    }
    if (m_config->contains(syst, "AllSmallRJets")) {
      syst.clear();
    }
    if (m_config->contains(syst, "AllLargeRJets")) {
      systLargeR.clear();
    }
  }

  
  ///-- JES systematics --///
  if (m_isMC || m_doFull_JER) {
    std::string allNP(m_config->jetUncertainties_NPModel()+"_"),
      np1("SR_Scenario1_"),np2("SR_Scenario2_"),np3("SR_Scenario3_"),np4("SR_Scenario4_");
    std::string allNP_FrozenJMS(m_config->jetUncertainties_NPModel()+"_FrozenJMS_");
    
    bool onlyJER = ( (!m_isMC) && m_doFull_JER ) || ( m_isMC && m_doFull_JER_Pseudodata );

    if (!m_doMultipleJES) {
      specifiedSystematics( syst , m_jetUncertaintiesTool , m_systMap_AllNP , allNP, false, onlyJER );
      if ( m_config->jetCalibSequence() == "JMS" ){
	specifiedSystematics( syst , m_jetUncertaintiesToolFrozenJMS , m_systMap_AllNP_FrozenJMS , allNP_FrozenJMS, false, onlyJER );
      }
    }
    else {
      specifiedSystematics( syst , m_jetUncertaintiesToolReducedNPScenario1 , m_systMap_ReducedNPScenario1 , np1 , false , onlyJER );
      specifiedSystematics( syst , m_jetUncertaintiesToolReducedNPScenario2 , m_systMap_ReducedNPScenario2 , np2 , false , onlyJER );
      specifiedSystematics( syst , m_jetUncertaintiesToolReducedNPScenario3 , m_systMap_ReducedNPScenario3 , np3 , false , onlyJER );
      specifiedSystematics( syst , m_jetUncertaintiesToolReducedNPScenario4 , m_systMap_ReducedNPScenario4 , np4 , false , onlyJER );
    }
  }
  ///-- Large-R JES systematics --///
  if (m_config->useLargeRJets() && m_config->isMC()) { //No JES uncertainties for Data at the moment
    if(m_config->largeRJESJMSConfig() == "CombMass"){
      std::string largeR(m_config->largeRJetUncertainties_NPModel()+"_");
      specifiedSystematics( systLargeR , m_jetUncertaintiesToolLargeR , m_systMap_LargeR , largeR , true);
    }else{
      ATH_MSG_WARNING("TA Mass & Calo Mass are not supported for large-R jet uncertainties at the moment. Large-R jet systemtatics skipped!");
    }
  }

  

  ///-- Large R jet substructure --///
  if (m_config->jetSubstructureName() == "Trimmer")
    m_jetSubstructure.reset(new top::LargeJetTrimmer);

  if (m_config->jetSubstructureName() == "SubjetMaker")
    m_jetSubstructure.reset(new top::SubjetMaker);

  m_config->systematicsJets( specifiedSystematics() );
  m_config->systematicsLargeRJets( specifiedSystematicsLargeR() );
  m_config->systematicsTrackJets( m_specifiedSystematicsTrackJets );

  ///-- Large R jet truth labeling --///
  m_TaggerForJES = nullptr;
  if(m_config->isMC()){
    m_TaggerForJES = std::unique_ptr<SmoothedWZTagger>( new SmoothedWZTagger( "TaggerTruthLabelling" ) );
    top::check(m_TaggerForJES->setProperty("CalibArea",  "SmoothedWZTaggers/Rel21"), "Failed to set CalibArea for m_TaggerForJES");
    top::check(m_TaggerForJES->setProperty("ConfigFile",  "SmoothedContainedWTagger_AntiKt10LCTopoTrimmed_FixedSignalEfficiency50_MC16d_20190410.dat"),
               "Failed to set ConfigFile for m_TaggerForJES");
    top::check(m_TaggerForJES->setProperty("DSID", m_config->getDSID()), "Failed to set DSID for m_TaggerForJet");
    top::check(m_TaggerForJES->initialize(), "Failed to initialize m_TaggerForJES");
  }

  ///-- DL1 Decoration --///
  m_btagSelToolsDL1Decor["DL1"]    = "BTaggingSelectionTool_forEventSaver_DL1_"+m_config->sgKeyJets();
  m_btagSelToolsDL1Decor["DL1r"]  = "BTaggingSelectionTool_forEventSaver_DL1r_"+m_config->sgKeyJets();
  m_btagSelToolsDL1Decor["DL1rmu"] = "BTaggingSelectionTool_forEventSaver_DL1rmu_"+m_config->sgKeyJets();
  top::check(m_btagSelToolsDL1Decor["DL1"].retrieve(), "Failed to retrieve eventsaver btagging selector");
  top::check(m_btagSelToolsDL1Decor["DL1r"].retrieve(), "Failed to retrieve eventsaver btagging selector");
  top::check(m_btagSelToolsDL1Decor["DL1rmu"].retrieve(), "Failed to retrieve eventsaver btagging selector");
  // Store a lightweight flag to limit error messages if the DL1 weights are not present                                                                                           
  m_DL1Possible = true;
  m_DL1rPossible = true;
  m_DL1rmuPossible = true;

  return StatusCode::SUCCESS;
}

StatusCode JetObjectCollectionMaker::executeJets(bool executeNominal) {
  bool isLargeR(false);
  return execute( isLargeR, executeNominal );
}

StatusCode JetObjectCollectionMaker::executeLargeRJets(bool executeNominal) {
  bool isLargeR(true);
  return execute( isLargeR, executeNominal );
}

StatusCode JetObjectCollectionMaker::execute( const bool isLargeR, bool executeNominal ) {
    
  ///-- Run nominal first, if executing nominal
  if(executeNominal){
    // decorating the HS jets with truth info on which are HS jets
    if (!isLargeR & m_isMC) {
      top::check( decorateHSJets() , "Failed to decorate jets with truth info of which are HS - this is needed for JVT scale-factors!");
    }
  
    // Decorate the DL1 variable
    top::check( decorateDL1() , "Failed to decorate jets with DL1 b-tagging discriminant");

    ///-- First calibrate the nominal jets, everything else comes from this, so let's only do it once not 3000 times --///
    top::check( calibrate( isLargeR ) , "Failed to calibrate jets");
    
    ///-- Return after calibrating the nominal --///
    return StatusCode::SUCCESS;
  }
    
  ///-- Systematics from here --///
        
  ///-- JES, JER regular atk4 for now --///
  if (!isLargeR) {
    ///-- JES --///
    if (m_isMC || m_doFull_JER) {
      if (!m_doMultipleJES) {
        top::check( applySystematic( m_jetUncertaintiesTool , m_systMap_AllNP ) , "Failed to apply JES");
	if ( m_config->jetCalibSequence() == "JMS" ){
	  top::check( applySystematic( m_jetUncertaintiesToolFrozenJMS , m_systMap_AllNP_FrozenJMS ) , "Failed to apply JES Frozen JMS");
	}
      }
      if (m_doMultipleJES) {
        top::check( applySystematic( m_jetUncertaintiesToolReducedNPScenario1 , m_systMap_ReducedNPScenario1 ) , "Failed to apply JES");
        top::check( applySystematic( m_jetUncertaintiesToolReducedNPScenario2 , m_systMap_ReducedNPScenario2 ) , "Failed to apply JES");
        top::check( applySystematic( m_jetUncertaintiesToolReducedNPScenario3 , m_systMap_ReducedNPScenario3 ) , "Failed to apply JES");
        top::check( applySystematic( m_jetUncertaintiesToolReducedNPScenario4 , m_systMap_ReducedNPScenario4 ) , "Failed to apply JES");
      }
    }
  } 
  else {
    if(m_config->isMC()){
      top::check( applySystematic( m_jetUncertaintiesToolLargeR , m_systMap_LargeR, true ) , "Failed to apply large-R syst.");
    }
  }

  return StatusCode::SUCCESS;
}

StatusCode JetObjectCollectionMaker::calibrate(const bool isLargeR) {
  ///-- Get base jets from xAOD --///
  std::string sgKey = isLargeR ? m_config->sgKeyLargeRJets() : m_config->sgKeyJets();

  const xAOD::JetContainer* xaod(nullptr);
  top::check( evtStore()->retrieve( xaod , sgKey ) , "Failed to retrieve Jets" );

  ///-- Shallow copy of the xAOD --///
  std::pair< xAOD::JetContainer*, xAOD::ShallowAuxContainer* > shallow_xaod_copy = xAOD::shallowCopyContainer( *xaod );

  ///-- Loop over the xAOD Container --///
  for( auto jet : *(shallow_xaod_copy.first) ){

    if (!isLargeR) { ///-- small-R jets used in most analyses --///
      ///-- Calibrate jet --///
      
      top::check(m_jetCalibrationTool->applyCalibration(*jet), "Failed to applyCalibration");
      
      // only multiply by JSF and bJSF if one of them != 1.0 (used by top mass analysis)
      float JSF  = m_config -> JSF();
      float bJSF = m_config -> bJSF();

      if(JSF != 1.0 || bJSF != 1.0){

        int truthflav = -1;
        if(jet->isAvailable<int>("PartonTruthLabelID")){
          jet->getAttribute("PartonTruthLabelID", truthflav);
        }
  
	xAOD::JetFourMom_t jet_p4 = jet->jetP4()*JSF;
        if(truthflav == 5) jet_p4 = jet_p4*bJSF;

        jet -> setJetP4(jet_p4);
  
      }
      // end application JSF/bJSF


      top::check(decorateBJets(*jet), "Failed to decorate if b-jet");
    }

    if (isLargeR && m_jetSubstructure.get() != nullptr) {
      m_jetSubstructure->correctJet( *jet );
    }

    if (isLargeR) {
      ///-- Calibrate jet --///

      float tau3 = jet->getAttribute<float>("Tau3_wta");
      float tau2 = jet->getAttribute<float>("Tau2_wta");
      float tau1 = jet->getAttribute<float>("Tau1_wta");

      jet -> auxdecor<float>("Tau32_wta") =  fabs(tau2) > 1.e-6 ? (tau3/tau2) : -999;  // 999 to match JetSubStructureMomentTools/NSubjettinessRatiosTool
      jet -> auxdecor<float>("Tau21_wta") =  fabs(tau1) > 1.e-6 ? (tau2/tau1) : -999;  // 999 to match JetSubStructureMomentTools/NSubjettinessRatiosTool

      top::check( m_jetCalibrationToolLargeR->applyCalibration( *jet ) , "Failed to applyCalibration" );

      ///-- for TA mass or calo mass, the calibrated mass or pt needs special treatment --///
      const std::string calibChoice = m_config->largeRJESJMSConfig();
      if(m_config->isMC()){
        ///-- Truth labeling required by the large-R jet uncertainties --///
        top::check( m_TaggerForJES->decorateTruthLabel(*jet) , "Failed to do truth labeling for large-R jet" );
        if(calibChoice == "TAMass"){
          xAOD::JetFourMom_t jet_calib_p4;
          jet->getAttribute<xAOD::JetFourMom_t>("JetJMSScaleMomentumTA", jet_calib_p4);
          jet->setJetP4(jet_calib_p4);
        }
      }else{ //For data, there's only one config file so special method is required for TA mass and Calos mass
        if(calibChoice == "CaloMass"){
          xAOD::JetFourMom_t jetInsituP4_calo;
          jet->getAttribute<xAOD::JetFourMom_t>("JetInsituScaleMomentumCalo",jetInsituP4_calo);
          jet->setJetP4(jetInsituP4_calo);
        }else if(calibChoice == "TAMass"){
          xAOD::JetFourMom_t jetInsituP4_ta;
          jet->getAttribute<xAOD::JetFourMom_t>("JetInsituScaleMomentumTA",jetInsituP4_ta);
          jet->setJetP4(jetInsituP4_ta);
        }
      }
    }

    ///-- Update JVT --///
    if (!isLargeR) {
      jet->auxdecor<float>("AnalysisTop_JVT") = m_jetUpdateJvtTool->updateJvt( *jet );
    }
   
  }
  
  // Check if the derivation we are running on contains 
  // MET_Track (once), if so apply the fJVT decoration
  // if not then don't
  static bool checked_track_MET = false;
  if (!checked_track_MET) {
    if (evtStore()->contains<xAOD::MissingETContainer>("MET_Track")) {
      m_do_fjvt = true;
    }
    else {
      if (m_config->fwdJetAndMET() == "fJVT")
        ATH_MSG_ERROR(" Cannot retrieve MET_Track while fJVT is to be applied on forward jets" );
      else
        ATH_MSG_WARNING(" Cannot retrieve MET_Track, so fJVT information will probably be wrong in output" );
    }
    checked_track_MET = true;
  }

  if (m_do_fjvt) {
    top::check(!m_fjvtTool->modify(*shallow_xaod_copy.first),
	       "Failed to apply fJVT decoration");
  }

  if (!isLargeR) {
    ///-- Save calibrated jet to TStore --///
    ///-- set links to original objects- needed for MET calculation --///
    // NOTE, if we use one of the b-tagging re-trained collections, we need to load
    // the original uncalibrated jet container to which the b-tagging shallow-copy is pointing to
    const xAOD::JetContainer *xaod_original(nullptr);
    // for small-R jet collections, set links to uncalibrated *original* jets (not BTagging shallow-copy)
    if (m_config->sgKeyJets() != m_config->sgKeyJetsType()) {
      top::check( evtStore()->retrieve(xaod_original, m_config->sgKeyJetsType()), "Failed to retrieve uncalibrated Jets for METMaker!");
    } else {
      xaod_original = xaod;
    }
    bool setLinks = xAOD::setOriginalObjectLink( *xaod_original, *shallow_xaod_copy.first );
    if (!setLinks)
      ATH_MSG_ERROR(" Cannot set original object links for jets, MET recalculation may struggle" );
  }

  ///-- Save corrected xAOD Container to StoreGate / TStore --///
  std::string outputSGKey;
  if (!isLargeR) {
    outputSGKey = m_config->sgKeyJetsStandAlone( m_nominalSystematicSet.hash() );
  }
  if (isLargeR) {
    outputSGKey = m_config->sgKeyLargeRJets( m_nominalSystematicSet.hash() );
  }

  std::string outputSGKeyAux = outputSGKey + "Aux.";

  xAOD::TReturnCode save = evtStore()->tds()->record( shallow_xaod_copy.first , outputSGKey );
  xAOD::TReturnCode saveAux = evtStore()->tds()->record( shallow_xaod_copy.second , outputSGKeyAux );
  if( !save || !saveAux ){
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}


StatusCode JetObjectCollectionMaker::applySystematic(ToolHandle<ICPJetUncertaintiesTool>& tool,
                                                     const std::unordered_map<CP::SystematicSet,CP::SystematicSet>& map,
                                                     bool isLargeR ) {
  ///-- Get calibrated jets --///
  const xAOD::JetContainer* xaod(nullptr);
  if (!isLargeR) {
    top::check( evtStore()->retrieve( xaod , m_config->sgKeyJetsStandAlone( m_nominalSystematicSet.hash() ) ) , "Failed to retrieve Jets" );
  } else {
    top::check( evtStore()->retrieve( xaod , m_config->sgKeyLargeRJets( m_nominalSystematicSet.hash() ) ) , "Failed to retrieve Jets" );
  }

  ///-- Loop over the systematics --///

  for (Itr syst=map.begin();syst!=map.end();++syst) {
    ///-- Don't do the nominal, we've already done that --///
    if ((*syst).second.hash() != m_nominalSystematicSet.hash()) {
           
      ///-- Tell the tool which systematic to use --///
      ///-- Here we use the second, original CP::SystematicSet --///
      top::check( tool->applySystematicVariation((*syst).second) ,
                  "Failed to applySystematicVariation" );
      
      ///-- Shallow copy of the xAOD --///
      std::pair< xAOD::JetContainer*, xAOD::ShallowAuxContainer* >
        shallow_xaod_copy = xAOD::shallowCopyContainer( *xaod );
        
      ///-- Loop over the xAOD Container --///
      for( auto jet : *(shallow_xaod_copy.first) ){
          
        if (isLargeR && m_config->isMC()) { //JES for large-R jets only exist for MC

          ///-- Only large R jets with the following properties can be calibrated.--///
          bool calibratable_jet = (std::fabs(jet->eta()) <= 2.0
                                   && jet->pt() > 200e3);
          if(!calibratable_jet)
            continue;
        }
        ///-- Apply Corrrection --///
        if(!(isLargeR && !m_config->isMC())){ //Large-R jet uncertainties don't apply to Data
          top::check( tool->applyCorrection( *jet ) , "Failed to applyCorrection" );
        }
        ///-- Update JVT --///
        if (!isLargeR) jet->auxdecor<float>("AnalysisTop_JVT") = m_jetUpdateJvtTool->updateJvt( *jet );
      }
      
      ///-- set links to original objects- needed for MET calculation --///
      bool setLinks = xAOD::setOriginalObjectLink(*xaod,
                                                  *shallow_xaod_copy.first );
      if (!setLinks)
        ATH_MSG_ERROR(" Cannot set original object links"
                      " for jets, MET recalculation may struggle" );

      ///-- Save corrected xAOD Container to StoreGate / TStore --///
      ///-- Here we use the first, AnalysisTop modified CP::SystematicSer --///
      ///-- This allows us to run multiple JES scenarios, which all have the same hash values --///
      std::string outputSGKey;
      if (isLargeR) {
        outputSGKey = m_config->sgKeyLargeRJets( (*syst).first.hash() );
      } else {
        outputSGKey = m_config->sgKeyJetsStandAlone( (*syst).first.hash() );
      }
      std::string outputSGKeyAux = outputSGKey + "Aux.";

      xAOD::TReturnCode save = evtStore()->tds()->record( shallow_xaod_copy.first , outputSGKey );
      xAOD::TReturnCode saveAux = evtStore()->tds()->record( shallow_xaod_copy.second , outputSGKeyAux );
      if( !save || !saveAux ){
        return StatusCode::FAILURE;
      }
    }
  }

  return StatusCode::SUCCESS;
}


StatusCode JetObjectCollectionMaker::executeTrackJets(bool executeNominal) {
  ///-- No calibrations or systematics yet --///
  ///-- Only run this on the nominal execution --///
  if(!executeNominal) return StatusCode::SUCCESS;

  top::check( decorateDL1(true) , "Failed to decorate track jets with DL1 b-tagging discriminant");
    
  ///-- Just make a shallow copy to keep these in line with everything else --///

  const xAOD::JetContainer* xaod(nullptr);
  top::check( evtStore()->retrieve( xaod , m_config->sgKeyTrackJets() ) , "Failed to retrieve Jets" );

  ///-- Shallow copy of the xAOD --///
  std::pair< xAOD::JetContainer*, xAOD::ShallowAuxContainer* > shallow_xaod_copy = xAOD::shallowCopyContainer( *xaod );

  ///-- set links to original objects --///
  bool setLinks = xAOD::setOriginalObjectLink( *xaod, *shallow_xaod_copy.first );
  if (!setLinks)
    ATH_MSG_ERROR(" Cannot set original object links for track jets" );

  ///-- Save corrected xAOD Container to StoreGate / TStore --///
  std::string outputSGKey = m_config->sgKeyTrackJets( m_config->nominalHashValue() );
  std::string outputSGKeyAux = outputSGKey + "Aux.";

  xAOD::TReturnCode save = evtStore()->tds()->record( shallow_xaod_copy.first , outputSGKey );
  xAOD::TReturnCode saveAux = evtStore()->tds()->record( shallow_xaod_copy.second , outputSGKeyAux );
  if( !save || !saveAux ){
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

StatusCode JetObjectCollectionMaker::printoutJets() {
  bool isLargeR(false);
  return this->printout( isLargeR );
}

StatusCode JetObjectCollectionMaker::printoutLargeRJets() {
  bool isLargeR(true);
  return this->printout( isLargeR );
}

StatusCode JetObjectCollectionMaker::printout( const bool isLargeR ) {
  ///-- Loop over all systematics --///
  for( auto s : m_specifiedSystematics ){
    std::string sgKey = isLargeR ? m_config->sgKeyLargeRJets() : m_config->sgKeyJets();

    const xAOD::JetContainer* xaod(nullptr);
    top::check( evtStore()->retrieve( xaod , sgKey ) , "Failed to retrieve Jets" );

    ATH_MSG_INFO(" Jets with sgKey = "<<sgKey );
    for( auto x : *xaod ){
      ATH_MSG_INFO("   Jet pT , eta  = "<<x->pt()<<" , "<<x->eta() );
    }
  }

  return StatusCode::SUCCESS;
}


void JetObjectCollectionMaker::specifiedSystematics(const std::set<std::string>& specifiedSystematics,
                                                    const ToolHandle<ICPJetUncertaintiesTool>& tool,
                                                    std::unordered_map<CP::SystematicSet,CP::SystematicSet>& map,
                                                    const std::string& modName, bool isLargeR , bool onlyJER) {

  
  ///-- Get the recommended systematics from the tool, in std::vector format --///
  const std::vector<CP::SystematicSet> systList = CP::make_systematics_vector( tool->recommendedSystematics() );

  for (const CP::SystematicSet& s : systList) {
    if (s.size() == 1) {
      CP::SystematicSet::const_iterator ss = s.begin();

      if( onlyJER && ss->name().find("JER")==std::string::npos ) continue;
      
      CP::SystematicSet modSet( modName + ss->name() );
      
      m_recommendedSystematics.push_back(modSet);
      if (!m_config->isSystNominal( m_config->systematics() )) {
	if (specifiedSystematics.size() == 0) {
	  if (!isLargeR)
	    m_specifiedSystematics.push_back(modSet);
	  else
	    m_specifiedSystematicsLargeR.push_back(modSet);
	  map.insert( std::make_pair(modSet,s) );
	} else if (specifiedSystematics.size()  > 0) {
	  for (std::string i : specifiedSystematics) {
	    if ( i == modSet.name() ) {
	      if (!isLargeR)
		m_specifiedSystematics.push_back(modSet);
	      else
		m_specifiedSystematicsLargeR.push_back(modSet);
	      map.insert( std::make_pair(modSet,s) );
	    }
	  }
	} // User has specified a systematic
	
      } // Don't do anything if the user requests nominal only
    } // size() == 1 -- this means that there is a CP::SystematicVariation
  } // Loop over systList

  m_recommendedSystematics.sort();
  m_recommendedSystematics.unique();
  
  m_specifiedSystematics.sort();
  m_specifiedSystematics.unique();
  
  m_specifiedSystematicsLargeR.sort();
  m_specifiedSystematicsLargeR.unique();
}


StatusCode JetObjectCollectionMaker::decorateBJets(xAOD::Jet& jet) {
  // initialise decorator and accessor
  static SG::AuxElement::Decorator<char> isbjet("IsBjet");
  static const std::string labelB = "PartonTruthLabelID";
  static SG::AuxElement::Accessor<int> truth_label(labelB);

  // Is b-jet if truth label == 5 and pT > 15 GeV
  isbjet(jet) = (jet.pt() > 15000. && truth_label(jet) == 5);

  return StatusCode::SUCCESS;
}

StatusCode JetObjectCollectionMaker::decorateHSJets() {
  // initialise decorator
  static SG::AuxElement::Decorator<char> isHS("AnalysisTop_isHS");
  
  // retrieve small-R jets collection
  const xAOD::JetContainer* jets(nullptr);
  top::check( evtStore()->retrieve( jets , m_config->sgKeyJets() ) , "Failed to retrieve small-R jet collection"+m_config->sgKeyJets() );

  // retrieve truth jet collection
  const xAOD::JetContainer *truthJets = nullptr;
  top::check(  evtStore()->retrieve(truthJets, m_truthJetCollForHS) , "Unable to retrieve truth jet container "+m_truthJetCollForHS+" - this is needed to define HS jets for application of JVT" );

  for(const auto& jet : *jets) {
    bool ishs = false;
    for(const auto& tjet : *truthJets) {
      if (tjet->p4().DeltaR(jet->p4())<0.3 && tjet->pt()>10e3) ishs = true;
    }
    isHS(*jet)=ishs;
  }

  return StatusCode::SUCCESS;
}

StatusCode JetObjectCollectionMaker::decorateDL1(bool trackJets) {
  // initialise decorators
  static const SG::AuxElement::Decorator<float> DL1("AnalysisTop_DL1");
  static const SG::AuxElement::Decorator<float> DL1r("AnalysisTop_DL1r");
  static const SG::AuxElement::Decorator<float> DL1rmu("AnalysisTop_DL1rmu");

  // retrieve small-R jets collection -- either calo or track jets
  const xAOD::JetContainer* jets(nullptr);
  if (trackJets) {
    top::check( evtStore()->retrieve( jets , m_config->sgKeyTrackJets() ) , "Failed to retrieve track jet collection"+m_config->sgKeyTrackJets() );
  } else {
    top::check( evtStore()->retrieve( jets , m_config->sgKeyJets() ) , "Failed to retrieve small-R jet collection"+m_config->sgKeyJets() );
  }

  for(const auto& jet : *jets) {
    // Default value
    double DL1_weight, DL1r_weight, DL1rmu_weight = -999;

    // Suppress warnings if the DL1 weights do not exist and avoid repeated failed computation
    if(m_DL1Possible) { 
      if(! m_btagSelToolsDL1Decor["DL1"]->getTaggerWeight(*jet, DL1_weight) ){
	DL1_weight = -999;
	m_DL1Possible = false;
      }
    }
    if(m_DL1rPossible) { 
      if(! m_btagSelToolsDL1Decor["DL1r"]->getTaggerWeight(*jet, DL1r_weight) ){
	DL1r_weight = -999;
	m_DL1rPossible = false;
      }
    }
    if(m_DL1rmuPossible) {
      if(! m_btagSelToolsDL1Decor["DL1rmu"]->getTaggerWeight(*jet, DL1rmu_weight) ){
	DL1rmu_weight = -999;
	m_DL1rmuPossible = false;
      }
    }
    DL1(*jet)    = DL1_weight;
    DL1r(*jet)  = DL1r_weight;    
    DL1rmu(*jet) = DL1rmu_weight;
  }

  return StatusCode::SUCCESS;
}

void JetObjectCollectionMaker::addCorrelation(const std::string& name,
                                              systMap& map_one,
                                              const std::string& syst_one_name,
                                              systMap& map_two,
                                              const std::string& syst_two_name) {

  std::vector<std::string> directions = {"__1up", "__1down"};
  for (const std::string& d : directions) {
    std::string tree_name = name+d;
    map_one.insert(std::make_pair(CP::SystematicSet(tree_name),
                                  CP::SystematicSet(syst_one_name+d)));
    map_two.insert(std::make_pair(CP::SystematicSet(tree_name),
                                  CP::SystematicSet(syst_two_name+d)));

    m_specifiedSystematics.push_back(tree_name);
    m_specifiedSystematicsLargeR.push_back(tree_name);
    
  }  // loop through up and down

  m_specifiedSystematics.sort();
  m_specifiedSystematics.unique();
  
  m_specifiedSystematicsLargeR.sort();
  m_specifiedSystematicsLargeR.unique();
   
}

void JetObjectCollectionMaker::addCorrelation(const std::string& name,
                                              systMap& map_one,
                                              const std::string& syst_one_name,
                                              std::list<CP::SystematicSet>& jet_specified) {
  
  map_one.insert(std::make_pair(CP::SystematicSet(name),
                                CP::SystematicSet(syst_one_name)));

  jet_specified.push_back(name);
    
  jet_specified.sort();
  jet_specified.unique();
     
}
  
}  // namespace top
