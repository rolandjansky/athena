/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: JetObjectCollectionMaker.cxx 802997 2017-04-17 01:24:36Z tpelzer $
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

  m_specifiedSystematics(),
  m_specifiedSystematicsLargeR(),
  m_specifiedSystematicsTrackJets(),
  m_recommendedSystematics(),
  m_nominalSystematicSet(),

  m_jetCalibrationTool("JetCalibrationTool"),
  m_jetCalibrationToolLargeR("JetCalibrationToolLargeR"),

  m_jetUncertaintiesTool("JetUncertaintiesTool"),
  m_jetUncertaintiesToolReducedNPScenario1("JetUncertaintiesToolReducedNPScenario1"),
  m_jetUncertaintiesToolReducedNPScenario2("JetUncertaintiesToolReducedNPScenario2"),
  m_jetUncertaintiesToolReducedNPScenario3("JetUncertaintiesToolReducedNPScenario3"),
  m_jetUncertaintiesToolReducedNPScenario4("JetUncertaintiesToolReducedNPScenario4"),

  m_jetUncertaintiesToolLargeR_strong("JetUncertaintiesToolLargeR_Strong"),
  m_jetUncertaintiesToolLargeR_medium("JetUncertaintiesToolLargeR_Medium"),
  m_jetUncertaintiesToolLargeR_weak("JetUncertaintiesToolLargeR_Weak"),

  m_jetJERSmearingTool("JetJERSmearingTool"),
  m_jetUpdateJvtTool("JetUpdateJvtTool"),
  m_fjvtTool("fJVTTool"),

  m_jetSubstructure(nullptr),

  m_systMap_AllNP(),
  m_systMap_ReducedNPScenario1(),
  m_systMap_ReducedNPScenario2(),
  m_systMap_ReducedNPScenario3(),
  m_systMap_ReducedNPScenario4(),
  m_systMap_LargeR_strong(),
  m_systMap_JER()
{
  declareProperty( "config" , m_config );

  declareProperty( "JetCalibrationTool" , m_jetCalibrationTool );
  declareProperty( "JetCalibrationToolLargeR" , m_jetCalibrationToolLargeR );

  declareProperty( "JetUncertaintiesToolLargeR" , m_jetUncertaintiesToolLargeR_strong);
  declareProperty( "JetUncertaintiesTool" , m_jetUncertaintiesTool);
  declareProperty( "JetUncertaintiesToolReducedNPScenario1" , m_jetUncertaintiesToolReducedNPScenario1 );
  declareProperty( "JetUncertaintiesToolReducedNPScenario2" , m_jetUncertaintiesToolReducedNPScenario2 );
  declareProperty( "JetUncertaintiesToolReducedNPScenario3" , m_jetUncertaintiesToolReducedNPScenario3 );
  declareProperty( "JetUncertaintiesToolReducedNPScenario4" , m_jetUncertaintiesToolReducedNPScenario4 );

  declareProperty( "JetJERSmearingTool" , m_jetJERSmearingTool );
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
    top::check( m_jetUncertaintiesToolLargeR_strong.retrieve(),
                "Failed to retrieve JetUncertaintiesToolLargeR_Strong" );
    top::check( m_jetUncertaintiesToolLargeR_medium.retrieve(),
                "Failed to retrieve JetUncertaintiesToolLargeR_Medium" );
    top::check( m_jetUncertaintiesToolLargeR_weak.retrieve(),
                "Failed to retrieve JetUncertaintiesToolLargeR_Weak" );
  }

  // If we are using particle flow jets then we
  // don't have any uncertainties so skip the retrieval.
  if (!m_config->useParticleFlowJets()) {
    ///-- JER uncertainties model --///
    if (m_config->jetJERSmearingModel() == "Full")
      m_doFull_JER = true;
    if (m_config->jetJERSmearingModel() == "Simple")
      m_doFull_JER = false;

    ///-- Are we doing JER? Let's not repeat this logic over and over --///
    if (m_config->isMC())
      m_doJER = true;
    if (!m_config->isMC() && m_doFull_JER)
      m_doJER = true;


    if (m_config->isMC()) {
      if (!m_config->doMultipleJES()) {
        top::check( m_jetUncertaintiesTool.retrieve() , "Failed to retrieve JetUncertaintiesToolAllNP" );
      }
      if (m_config->doMultipleJES()) {
        top::check( m_jetUncertaintiesToolReducedNPScenario1.retrieve() , "Failed to retrieve JetUncertaintiesToolReducedNPScenario1" );
        top::check( m_jetUncertaintiesToolReducedNPScenario2.retrieve() , "Failed to retrieve JetUncertaintiesToolReducedNPScenario2" );
        top::check( m_jetUncertaintiesToolReducedNPScenario3.retrieve() , "Failed to retrieve JetUncertaintiesToolReducedNPScenario3" );
        top::check( m_jetUncertaintiesToolReducedNPScenario4.retrieve() , "Failed to retrieve JetUncertaintiesToolReducedNPScenario4" );
      }
    }

    top::check( m_jetJERSmearingTool.retrieve() , "Failed to retrieve JetJERSmearingTool" );
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
  }

  // If we are using particle flow jets then we
  // don't have any uncertainties so skip
  // trying to use the tools
  if (!m_config->useParticleFlowJets()) {
    ///-- JES systematics --///
    if (m_config->isMC()) {
      std::string allNP("JET_"+m_config->jetUncertainties_NPModel()+"_"),np1("JET_NPScenario1_"),np2("JET_NPScenario2_"),np3("JET_NPScenario3_"),np4("JET_NPScenario4_");
      std::string largeR_strong("LARGERJET_Strong_"),
        largeR_medium("LARGERJET_Medium_"),
        largeR_weak("LARGERJET_Weak_");

      if (!m_config->doMultipleJES()) {
        specifiedSystematics( syst , m_jetUncertaintiesTool , m_systMap_AllNP , allNP );
      }
      if (m_config->doMultipleJES()) {
        specifiedSystematics( syst , m_jetUncertaintiesToolReducedNPScenario1 , m_systMap_ReducedNPScenario1 , np1 );
        specifiedSystematics( syst , m_jetUncertaintiesToolReducedNPScenario2 , m_systMap_ReducedNPScenario2 , np2 );
        specifiedSystematics( syst , m_jetUncertaintiesToolReducedNPScenario3 , m_systMap_ReducedNPScenario3 , np3 );
        specifiedSystematics( syst , m_jetUncertaintiesToolReducedNPScenario4 , m_systMap_ReducedNPScenario4 , np4 );
      }
      if (m_config->useLargeRJets()) {
        specifiedSystematics( syst , m_jetUncertaintiesToolLargeR_strong , m_systMap_LargeR_strong , largeR_strong , true);
        specifiedSystematics( syst , m_jetUncertaintiesToolLargeR_medium , m_systMap_LargeR_medium , largeR_medium , true);
        specifiedSystematics( syst , m_jetUncertaintiesToolLargeR_weak , m_systMap_LargeR_weak , largeR_weak , true);
      }
    }

    ///-- JER systematics --///
    if (m_doJER)
      specifiedSystematics( syst , m_jetJERSmearingTool , m_systMap_JER );
  }

  // See http://cern.ch/go/nHF6 for more information
  if (m_config->doLargeRSmallRCorrelations()) {
    systMap* smallR_systs = &m_systMap_AllNP;
    if (m_config->doMultipleJES()) {
      smallR_systs = &m_systMap_ReducedNPScenario1;
    }
    addCorrelation("CORR_LargeRSmallR_A", *smallR_systs,
                   "JET_GroupedNP_2",
                   m_systMap_LargeR_strong, "JET_Top_Run1_Tau32");
    
    addCorrelation("CORR_LargeRSmallR_B", *smallR_systs,
                   "JET_GroupedNP_2",
                   m_systMap_LargeR_weak, "JET_Rtrk_Baseline_pT");
    
    addCorrelation("CORR_LargeRSmallR_C__1up", m_systMap_LargeR_weak,
                   "JET_Rtrk_Baseline_mass__1up-JET_Top_Run1_Tau32__1up",
                   m_specifiedSystematicsLargeR);
    
    addCorrelation("CORR_LargeRSmallR_C__1down", m_systMap_LargeR_weak,
                   "JET_Rtrk_Baseline_mass__1down-JET_Top_Run1_Tau32__1down",
                   m_specifiedSystematicsLargeR);
  }

  ///-- Large R jet substructure --///
  if (m_config->jetSubstructureName() == "Trimmer")
    m_jetSubstructure.reset(new top::LargeJetTrimmer);

  if (m_config->jetSubstructureName() == "SubjetMaker")
    m_jetSubstructure.reset(new top::SubjetMaker);

  m_config->systematicsJets( specifiedSystematics() );
  m_config->systematicsLargeRJets( specifiedSystematicsLargeR() );
  m_config->systematicsTrackJets( m_specifiedSystematicsTrackJets );

  return StatusCode::SUCCESS;
}

StatusCode JetObjectCollectionMaker::executeJets() {
  bool isLargeR(false);
  return execute( isLargeR );
}

StatusCode JetObjectCollectionMaker::executeLargeRJets() {
  bool isLargeR(true);
  return execute( isLargeR );
}

StatusCode JetObjectCollectionMaker::execute( const bool isLargeR ) {
  // decorating the HS jets with truth info on which are HS jets
  if (!isLargeR & m_config->isMC()) {
    top::check( decorateHSJets() , "Failed to decorate jets with truth info of which are HS - this is needed for JVT scale-factors!");
  }
  
  ///-- First calibrate the nominal jets, everything else comes from this, so let's only do it once not 3000 times --///
  top::check( calibrate( isLargeR ) , "Failed to calibrate jets");

  // No uncertainties yet for pflow
  // - return SUCCESS after calibration
  if (m_config->useParticleFlowJets())
    return StatusCode::SUCCESS;

  ///-- JES, JER regular atk4 for now --///
  if (!isLargeR) {
    ///-- JES --///
    if (m_config->isMC()) {
      if (!m_config->doMultipleJES()) {
        top::check( applySystematic( m_jetUncertaintiesTool , m_systMap_AllNP ) , "Failed to apply JES");
      }
      if (m_config->doMultipleJES()) {
        top::check( applySystematic( m_jetUncertaintiesToolReducedNPScenario1 , m_systMap_ReducedNPScenario1 ) , "Failed to apply JES");
        top::check( applySystematic( m_jetUncertaintiesToolReducedNPScenario2 , m_systMap_ReducedNPScenario2 ) , "Failed to apply JES");
        top::check( applySystematic( m_jetUncertaintiesToolReducedNPScenario3 , m_systMap_ReducedNPScenario3 ) , "Failed to apply JES");
        top::check( applySystematic( m_jetUncertaintiesToolReducedNPScenario4 , m_systMap_ReducedNPScenario4 ) , "Failed to apply JES");
      }
    }

    ///-- JER --///
    if (m_doJER)
      top::check( applySystematic ( m_jetJERSmearingTool , m_systMap_JER ) , "Failed to apply JER" );
  } else {
    if (m_config->isMC()) {
      top::check( applySystematic( m_jetUncertaintiesToolLargeR_strong , m_systMap_LargeR_strong, true ) , "Failed to apply large-R syst.");
      top::check( applySystematic( m_jetUncertaintiesToolLargeR_medium , m_systMap_LargeR_medium, true ) , "Failed to apply large-R syst.");
      top::check( applySystematic( m_jetUncertaintiesToolLargeR_weak , m_systMap_LargeR_weak, true ) , "Failed to apply large-R syst.");
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
        if(jet->isAvailable<int>("HadronConeExclTruthLabelID")){
          jet->getAttribute("HadronConeExclTruthLabelID", truthflav);
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

  ///-- Save calibrated jet to TStore --///
  ///-- set links to original objects- needed for MET calculation --///
  bool setLinks = xAOD::setOriginalObjectLink( *xaod, *shallow_xaod_copy.first );
  if (!setLinks)
    ATH_MSG_ERROR(" Cannot set original object links for jets, MET recalculation may struggle" );

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
          
        if (isLargeR) {

          if ((*syst).first.name().find("LARGER") == std::string::npos)
            continue;
	  
          ///-- Only large R jets with the following properties can be calibrated.--///
          bool calibratable_jet = (jet->m()/jet->pt() <= 1
                                   && std::fabs(jet->eta()) <= 2.0
                                   && jet->pt() > 150e3
                                   && jet->pt() < 3000e3);
          if(!calibratable_jet)
            continue;
        }
        ///-- Apply Corrrection --///
        top::check( tool->applyCorrection( *jet ) , "Failed to applyCorrection" );
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



StatusCode JetObjectCollectionMaker::applySystematic(ToolHandle<IJERSmearingTool>& tool,
                                                     const std::unordered_map<CP::SystematicSet,CP::SystematicSet>& map ) {
  ///-- Get calibrated jets --///
  const xAOD::JetContainer* xaod(nullptr);
  top::check( evtStore()->retrieve( xaod , m_config->sgKeyJetsStandAlone( m_nominalSystematicSet.hash() ) ) , "Failed to retrieve Jets" );

  ///-- Loop over the systematics --///

  for (Itr syst=map.begin();syst!=map.end();++syst) {
    ///-- Don't do the nominal, we've already done that --///
    if ((*syst).second.hash() != m_nominalSystematicSet.hash()) {

      ///-- Tell the tool which systematic to use --///
      ///-- Here we use the second, original CP::SystematicSet --///
      top::check( tool->applySystematicVariation( (*syst).second ) , "Failed to apply systematic" );

      ///-- Shallow copy of the xAOD --///
      std::pair< xAOD::JetContainer*, xAOD::ShallowAuxContainer* > shallow_xaod_copy = xAOD::shallowCopyContainer( *xaod );

      ///-- Loop over the xAOD Container --///
      for( auto jet : *(shallow_xaod_copy.first) ){
        ///-- Apply Corrrection --///
        top::check( tool->applyCorrection( *jet ) , "Failed to applyCorrection" );
        ///-- Update JVT --///
        jet->auxdecor<float>("AnalysisTop_JVT") = m_jetUpdateJvtTool->updateJvt( *jet );
      }

      ///-- set links to original objects- needed for MET calculation --///
      bool setLinks = xAOD::setOriginalObjectLink( *xaod, *shallow_xaod_copy.first );
      if (!setLinks)
        ATH_MSG_ERROR(" Cannot set original object links for jets, MET recalculation may struggle" );

      ///-- Save corrected xAOD Container to StoreGate / TStore --///
      ///-- Here we use the first, AnalysisTop modified CP::SystematicSer --///
      ///-- This allows us to run multiple JES scenarios, which all have the same hash values --///
      std::string outputSGKey = m_config->sgKeyJetsStandAlone( (*syst).first.hash() );
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

StatusCode JetObjectCollectionMaker::executeTrackJets() {
  ///-- No calibrations or systematics yet --///
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
                                                    const std::string& modName, bool isLargeR ) {

  ///-- MC only --///
  if (m_config->isMC()) {
    ///-- Get the recommended systematics from the tool, in std::vector format --///
    const std::vector<CP::SystematicSet> systList = CP::make_systematics_vector( tool->recommendedSystematics() );

    for (auto s : systList) {
      if (s.size() == 1) {
        CP::SystematicSet::const_iterator ss = s.begin();
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
            for (auto i : specifiedSystematics) {
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
  } // MC only
  m_recommendedSystematics.sort();
  m_recommendedSystematics.unique();
  
  m_specifiedSystematics.sort();
  m_specifiedSystematics.unique();
  
  m_specifiedSystematicsLargeR.sort();
  m_specifiedSystematicsLargeR.unique();
}




///-- Don't really need to do this, but would rather stay consistent with JetObjectCollectionMaker --///
///-- This could be done like all the other xyzObjectCollectionMakers, but would need to change the way execute() works --///
void JetObjectCollectionMaker::specifiedSystematics( const std::set<std::string>& specifiedSystematics,
                                                     const ToolHandle<IJERSmearingTool>& tool,
                                                     std::unordered_map<CP::SystematicSet,CP::SystematicSet>& map ) {
  ///-- Get the recommended systematics from the tool, in std::vector format --///
  const std::vector<CP::SystematicSet> systList = CP::make_systematics_vector( tool->recommendedSystematics() );

  for (auto s : systList) {
    m_recommendedSystematics.push_back(s);

    ///-- Are we doing JER? Are we only doing Nominal? Did the user specify specific systematics to use? --///
    if (m_doJER) {
      if (!m_config->isSystNominal( m_config->systematics() )) {
        if (specifiedSystematics.size() == 0) {
          m_specifiedSystematics.push_back(s);
          map.insert( std::make_pair(s,s) );
        }
        if (specifiedSystematics.size()  > 0) {
          for (auto i : specifiedSystematics) {
            if ( i == s.name() ) {
              m_specifiedSystematics.push_back(s);
              map.insert( std::make_pair(s,s) );
            }
          }
        }
      }
    }
  }
  m_recommendedSystematics.sort();
  m_recommendedSystematics.unique();
  m_specifiedSystematics.sort();
  m_specifiedSystematics.unique();
}

StatusCode JetObjectCollectionMaker::decorateBJets(xAOD::Jet& jet) {
  // initialise decorator and accessor
  static SG::AuxElement::Decorator<char> isbjet("IsBjet");
  static const std::string labelB = "HadronConeExclTruthLabelID";
  static SG::AuxElement::Accessor<int> truth_label(labelB);

  // Is b-jet if truth label == 5 and pT > 20 GeV
  isbjet(jet) = (jet.pt() > 20000. && truth_label(jet) == 5);

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
