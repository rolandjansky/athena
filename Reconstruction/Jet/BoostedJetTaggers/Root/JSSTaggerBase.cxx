/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "BoostedJetTaggers/JSSTaggerBase.h"

#include "ParticleJetTools/LargeRJetLabelEnum.h"

#include <TSystem.h>

JSSTaggerBase::JSSTaggerBase(const std::string &name) :
  asg::AsgTool(name),
  m_calibArea(""),
  m_jetPtMin(200000.),
  m_jetPtMax(3000000.),
  m_jetEtaMax(2.0),
  m_strMassCutLow(""),
  m_strMassCutHigh(""),
  m_strScoreCut("")
{

  /// Tagger configuration properties
  declareProperty( "ContainerName", m_containerName = "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets",     "Name of jet container" );
  declareProperty( "ConfigFile",    m_configFile = "",        "Name of config file" );
  declareProperty( "CalibArea",     m_calibArea = "",         "Path to config file" );
  declareProperty( "CalcSF",        m_calcSF = false,         "Flag to calculate the efficiency SF"  );
  declareProperty( "WorkingPoint",  m_wkpt = "",              "If specified, name of working point is added to the variable names" );
  declareProperty( "TaggerType",    m_tagType = "XXX",        "Tagger type (e.g. SmoothedWZTagger, JSSWTopTaggerDNN, etc.)" );
  declareProperty( "Decoration",    m_decorationName = "XX",  "Prefix for the variables decorated to xAOD::Jet" );
  declareProperty( "IsMC",          m_isMC = true,            "Flag to identify data or MC" );

  /// Jet kinematics properties
  declareProperty( "JetEtaMax", m_jetEtaMax = 2.0, "Eta cut to define fiducial phase space for the tagger");

  /// Truth labeling properties
  declareProperty( "UseTRUTH3",                     m_truthLabelUseTRUTH3 = true,
		   "Flag to use TRUTH3 containers. If false, TRUTH1 format is used." );
  declareProperty( "TruthParticleContainerName",    m_truthParticleContainerName = "TruthParticles",
		   "Name of truth-particle container (with UseTRUTH3=false). TruthParticles by default" );
  declareProperty( "TruthBosonContainerName",       m_truthBosonContainerName = "TruthBosonsWithDecayParticles",
		   "Name of truth-boson container (with UseTRUTH3=true). TruthBosonWithDecayParticles by default" );
  declareProperty( "TruthTopQuarkContainerName",    m_truthTopQuarkContainerName = "TruthTopQuarkWithDecayParticles",
		   "Name of truth-top container (with UseTRUTH3=true). TruthTopQuarkWithDecayParticles by default" );

  /// Keras properties
  declareProperty( "CalibAreaKeras",  m_kerasCalibArea = "BoostedJetTaggers/TopoclusterTopTagger/Boost2017/",
		   "Path to json file to configure ML-taggers (Keras)" );
  declareProperty( "KerasConfigFile", m_kerasConfigFileName = "XXX",
		   "Name of json file to configure ML-taggers (Keras)" );
  declareProperty( "KerasOutput",     m_kerasConfigOutputName = "XXX",
		   "Name of output variable by the ML-tagger (Keras)" );

  /// TMVA properties
  declareProperty( "CalibAreaTMVA",  m_tmvaCalibArea = "BoostedJetTaggers/JSSWTopTaggerBDT/Boost2017/",
		   "Path to xml file to configure ML-taggers (TMVA)" );
  declareProperty( "TMVAConfigFile", m_tmvaConfigFileName = "XXX",
		   "Name of xml file to configure ML-taggers (TMVA)" );

  /// Tagging scale factors
  declareProperty( "WeightDecorationName",      m_weightDecorationName = "SF",
		   "Name of SF variable decorated to xAOD::Jet" );
  declareProperty( "WeightFile",                m_weightFileName = "",
		   "Name of config ROOT file for SF calculation" );
  declareProperty( "WeightHistogramName",       m_weightHistogramName = "",
		   "Name of SF histograms in the ROOT file" );
  declareProperty( "EfficiencyHistogramName",   m_efficiencyHistogramName = "",
		   "Name of efficiency histograms in the ROOT file" );
  declareProperty( "WeightFlavors",             m_weightFlavors = "",
		   "List of jet flavours for which the SF is available. Divided by comma" );
}

StatusCode JSSTaggerBase::initialize() {

  /// Make sure jet container is set
  if ( m_containerName.empty() ) {
    ATH_MSG_ERROR( "ContainerName has not been set. Exiting" );
    return StatusCode::FAILURE;
  }

  /// Initialize warning counters
  m_nWarnVar = 0;

  /// Define common tagger states
  m_acceptInfo.addCut( "ValidPtRangeHigh"  , "True if the jet is not too high pT" );
  m_acceptInfo.addCut( "ValidPtRangeLow"   , "True if the jet is not too low pT" );
  m_acceptInfo.addCut( "ValidEtaRange"     , "True if the jet is not too forward" );
  
  m_acceptInfo.addCut( "ValidJetContent"   , "True if the jet is alright technically (e.g. all attributes necessary for tag)" );
  m_acceptInfo.addCut( "ValidEventContent" , "True if the event is alright technically (e.g. primary vertices)" );

  /// Initialize decorators
  ATH_MSG_INFO( "Decorators that will be attached to jet :" );

  m_decTaggedKey = m_containerName + "." + m_decorationName + "_" + m_decTaggedKey.key();
  m_decValidPtRangeHighKey = m_containerName + "." + m_decorationName + "_" + m_decValidPtRangeHighKey.key();
  m_decValidPtRangeLowKey = m_containerName + "." + m_decorationName + "_" + m_decValidPtRangeLowKey.key();
  m_decValidEtaRangeKey = m_containerName + "." + m_decorationName + "_" + m_decValidEtaRangeKey.key();
  m_decValidKinRangeKey = m_containerName + "." + m_decorationName + "_" + m_decValidKinRangeKey.key();
  m_decValidJetContentKey = m_containerName + "." + m_decorationName + "_" + m_decValidJetContentKey.key();
  m_decValidEventContentKey = m_containerName + "." + m_decorationName + "_" + m_decValidEventContentKey.key();

  m_decTau21WTAKey = m_containerName + "." + m_decTau21WTAKey.key();
  m_decTau32WTAKey = m_containerName + "." + m_decTau32WTAKey.key();
  m_decTau42WTAKey = m_containerName + "." + m_decTau42WTAKey.key();
  m_decC2Key = m_containerName + "." + m_decC2Key.key();
  m_decD2Key = m_containerName + "." + m_decD2Key.key();
  m_decE3Key = m_containerName + "." + m_decE3Key.key();
  m_decL2Key = m_containerName + "." + m_decL2Key.key();
  m_decL3Key = m_containerName + "." + m_decL3Key.key();

  m_readTau1WTAKey = m_containerName + "." + m_readTau1WTAKey.key();
  m_readTau2WTAKey = m_containerName + "." + m_readTau2WTAKey.key();
  m_readTau3WTAKey = m_containerName + "." + m_readTau3WTAKey.key();
  m_readTau4WTAKey = m_containerName + "." + m_readTau4WTAKey.key();
  m_readTau21WTAKey = m_containerName + "." + m_readTau21WTAKey.key();
  m_readTau32WTAKey = m_containerName + "." + m_readTau32WTAKey.key();
  m_readTau42WTAKey = m_containerName + "." + m_readTau42WTAKey.key();
  m_readECF1Key = m_containerName + "." + m_readECF1Key.key();
  m_readECF2Key = m_containerName + "." + m_readECF2Key.key();
  m_readECF3Key = m_containerName + "." + m_readECF3Key.key();
  m_readC2Key = m_containerName + "." + m_readC2Key.key();
  m_readD2Key = m_containerName + "." + m_readD2Key.key();
  m_readE3Key = m_containerName + "." + m_readE3Key.key();
  m_readSplit12Key = m_containerName + "." + m_readSplit12Key.key();
  m_readSplit23Key = m_containerName + "." + m_readSplit23Key.key();
  m_readQwKey = m_containerName + "." + m_readQwKey.key();
  m_readThrustMajKey = m_containerName + "." + m_readThrustMajKey.key();
  m_readL2Key = m_containerName + "." + m_readL2Key.key();
  m_readL3Key = m_containerName + "." + m_readL3Key.key();
  m_readECFG331Key = m_containerName + "." + m_readECFG331Key.key();
  m_readECFG311Key = m_containerName + "." + m_readECFG311Key.key();
  m_readECFG212Key = m_containerName + "." + m_readECFG212Key.key();

  ATH_CHECK( m_decTau21WTAKey.initialize() );
  ATH_CHECK( m_decTau32WTAKey.initialize() );
  ATH_CHECK( m_decTau42WTAKey.initialize() );
  ATH_CHECK( m_decC2Key.initialize() );
  ATH_CHECK( m_decD2Key.initialize() );
  ATH_CHECK( m_decE3Key.initialize() );
  ATH_CHECK( m_decL2Key.initialize() );
  ATH_CHECK( m_decL3Key.initialize() );

  ATH_CHECK( m_readTau1WTAKey.initialize() );
  ATH_CHECK( m_readTau2WTAKey.initialize() );
  ATH_CHECK( m_readTau3WTAKey.initialize() );
  ATH_CHECK( m_readTau4WTAKey.initialize() );
  ATH_CHECK( m_readTau21WTAKey.initialize() );
  ATH_CHECK( m_readTau32WTAKey.initialize() );
  ATH_CHECK( m_readTau42WTAKey.initialize() );
  ATH_CHECK( m_readECF1Key.initialize() );
  ATH_CHECK( m_readECF2Key.initialize() );
  ATH_CHECK( m_readECF3Key.initialize() );
  ATH_CHECK( m_readC2Key.initialize() );
  ATH_CHECK( m_readD2Key.initialize() );
  ATH_CHECK( m_readE3Key.initialize() );
  ATH_CHECK( m_readSplit12Key.initialize() );
  ATH_CHECK( m_readSplit23Key.initialize() );
  ATH_CHECK( m_readQwKey.initialize() );
  ATH_CHECK( m_readThrustMajKey.initialize() );
  ATH_CHECK( m_readL2Key.initialize() );
  ATH_CHECK( m_readL3Key.initialize() );
  ATH_CHECK( m_readECFG331Key.initialize() );
  ATH_CHECK( m_readECFG311Key.initialize() );
  ATH_CHECK( m_readECFG212Key.initialize() );
  
  ATH_CHECK( m_decTaggedKey.initialize() );
  ATH_CHECK( m_decValidPtRangeHighKey.initialize() );
  ATH_CHECK( m_decValidPtRangeLowKey.initialize() );
  ATH_CHECK( m_decValidEtaRangeKey.initialize() );
  ATH_CHECK( m_decValidKinRangeKey.initialize() );
  ATH_CHECK( m_decValidJetContentKey.initialize() );
  ATH_CHECK( m_decValidEventContentKey.initialize() );

  ATH_MSG_INFO( "  " << m_decTaggedKey.key() << " : pass tagging criteria" );
  ATH_MSG_INFO( "  " << m_decValidPtRangeHighKey.key() << " : pass upper pt range" );
  ATH_MSG_INFO( "  " << m_decValidPtRangeLowKey.key() << " : pass lower pt range" );
  ATH_MSG_INFO( "  " << m_decValidEtaRangeKey.key() << " : pass eta range" );
  ATH_MSG_INFO( "  " << m_decValidKinRangeKey.key() << " : pass kinematic range" );
  ATH_MSG_INFO( "  " << m_decValidJetContentKey.key() << " : has valid jet content" );
  ATH_MSG_INFO( "  " << m_decValidEventContentKey.key() << " : has valid event content" );

  if ( m_useMassCut ) {

    m_decPassMassKey = m_containerName + "." + m_decorationName + "_" + m_decPassMassKey.key();
    m_decCutMLowKey = m_containerName + "." + m_decorationName + "_" + m_decCutMLowKey.key();
    m_decCutMHighKey = m_containerName + "." + m_decorationName + "_" + m_decCutMHighKey.key();

    ATH_CHECK( m_decPassMassKey.initialize() );
    ATH_CHECK( m_decCutMLowKey.initialize() );
    ATH_CHECK( m_decCutMHighKey.initialize() );

    ATH_MSG_INFO( "  " << m_decPassMassKey.key() << " : pass mass cut" );
    ATH_MSG_INFO( "  " << m_decCutMLowKey.key() << " : lower mass cut" );
    ATH_MSG_INFO( "  " << m_decCutMHighKey.key() << " : upper mass cut" );

  }

  if ( m_useScoreCut ) {
  
    m_decPassScoreKey = m_containerName + "." + m_decorationName + "_" + m_decPassScoreKey.key();
    m_decScoreCutKey = m_containerName + "." + m_decorationName + "_" + m_decScoreCutKey.key();
    m_decScoreValueKey = m_containerName + "." + m_decorationName + "_" + m_decScoreValueKey.key();

    ATH_CHECK( m_decPassScoreKey.initialize() );
    ATH_CHECK( m_decScoreCutKey.initialize() );
    ATH_CHECK( m_decScoreValueKey.initialize() );

    ATH_MSG_INFO( "  " << m_decPassScoreKey.key() << " : pass MVA score cut" );
    ATH_MSG_INFO( "  " << m_decScoreCutKey.key() << " : MVA score cut" );
    ATH_MSG_INFO( "  " << m_decScoreValueKey.key() << " : evaluated MVA score" );

  }

  if ( m_calcSF ) {
  
    m_decWeightKey = m_containerName + "." + m_decorationName + "_" + m_weightDecorationName;
    m_decEfficiencyKey = m_containerName + "." + m_decorationName + "_" + m_decEfficiencyKey.key();
    m_decEffSFKey = m_containerName + "." +m_decorationName + "_" + m_decEffSFKey.key();
    m_decSigeffSFKey = m_containerName + "." +m_decorationName + "_" + m_decSigeffSFKey.key();
    
    ATH_CHECK( m_decWeightKey.initialize() );
    ATH_CHECK( m_decEfficiencyKey.initialize() );
    ATH_CHECK( m_decEffSFKey.initialize() );
    ATH_CHECK( m_decSigeffSFKey.initialize() );
    
    ATH_MSG_INFO( "  " << m_decWeightKey.key() << " : tagging SF" );
   
    m_readTruthLabelKey = m_containerName + "." + m_truthLabelName;
    ATH_CHECK( m_readTruthLabelKey.initialize() );
  
  }

  /// Initialize SFs if they are needed
  if ( m_calcSF ) {

    /// Get weight config file
    m_weightConfig = std::make_unique<TFile>( m_weightConfigPath.c_str() );
    if( !m_weightConfig ) {
      ATH_MSG_ERROR( "SmoothedWZTagger: Error openning config file : " << m_weightConfigPath );
      return StatusCode::FAILURE;
    }

    /// Install histograms for tagging SFs
    std::stringstream ss{m_weightFlavors};
    std::string flavor;
    while ( std::getline(ss, flavor, ',') ) {
      m_weightHistograms.insert( std::make_pair( flavor, (TH2D*)m_weightConfig->Get((m_weightHistogramName+"_"+flavor).c_str()) ) );
      if ( !m_efficiencyHistogramName.empty() ) {
        m_efficiencyHistograms.insert( std::make_pair( flavor, (TH2D*)m_weightConfig->Get((m_efficiencyHistogramName+"_"+flavor).c_str()) ) );
      }
      ATH_MSG_INFO( "Tagging SF histogram for " << flavor << " is installed." );
    }

  }

  /// Initialize the TF1 functions
  if ( !m_strMassCutLow.empty() ) m_funcMassCutLow = std::make_unique<TF1>("strMassCutLow", m_strMassCutLow.c_str(), 0, 14000);
  if ( !m_strMassCutHigh.empty() ) m_funcMassCutHigh = std::make_unique<TF1>("strMassCutHigh", m_strMassCutHigh.c_str(), 0, 14000);
  if ( !m_strScoreCut.empty() ) m_funcScoreCut = std::make_unique<TF1>("strScoreCut", m_strScoreCut.c_str(), 0, 14000);

  return StatusCode::SUCCESS;

}

/// Loop over jet collection and decorate each jet
StatusCode JSSTaggerBase::decorate( const xAOD::JetContainer& jets ) const {

  for ( auto jet : jets ) {
    ATH_CHECK( tag(*jet) );
  }

  return StatusCode::SUCCESS;

}

/// Get configReader TEnv
StatusCode JSSTaggerBase::getConfigReader() {

  ATH_MSG_INFO( "Using config file : " << m_configFile );

  /// Check for the existence of the configuration file
  std::string configPath;

  if ( m_calibArea.compare("Local") == 0 ) {
    configPath = PathResolverFindCalibFile(("$WorkDir_DIR/data/BoostedJetTaggers/"+m_configFile).c_str());
  }
  else if ( m_calibArea.find("eos") != std::string::npos) {
    configPath = PathResolverFindCalibFile((m_calibArea+"/"+m_configFile).c_str());
  }
  else {
    configPath = PathResolverFindCalibFile(("BoostedJetTaggers/"+m_calibArea+"/"+m_configFile).c_str());
  }

  /// https://root.cern.ch/root/roottalk/roottalk02/5332.html
  FileStat_t fStats;
  int fSuccess = gSystem->GetPathInfo(configPath.c_str(), fStats);
  if ( fSuccess ) {
    ATH_MSG_ERROR( "Recommendations file " << m_configFile << " could not be found" );
    return StatusCode::FAILURE;
  }
  else {
    ATH_MSG_DEBUG( "Recommendations file was found : " << configPath );
  }

  if ( m_configReader.ReadFile( configPath.c_str(), EEnvLevel(0) ) ) {
    ATH_MSG_ERROR( "Error while reading config file : "<< configPath );
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;

}

/// Reset cuts
StatusCode JSSTaggerBase::resetCuts( asg::AcceptData &acceptData ) const {

  /// Reset the AcceptData cut results to false
  acceptData.clear();

  /// Initialize common cuts to true by default
  acceptData.setCutResult( "ValidJetContent", true );
  acceptData.setCutResult( "ValidEventContent", true );
  
  acceptData.setCutResult( "ValidPtRangeHigh", true );
  acceptData.setCutResult( "ValidPtRangeLow" , true );
  acceptData.setCutResult( "ValidEtaRange"   , true );

  return StatusCode::SUCCESS;

}

/// Check if jet passes kinematic constraints
bool JSSTaggerBase::passKinRange( const xAOD::Jet &jet ) const {

  float scale = 1.0;
  if ( m_ptGeV ) scale = 1.e3;

  if ( jet.pt() < m_jetPtMin * scale ) return false;
  if ( jet.pt() > m_jetPtMax * scale ) return false;
  if ( std::abs( jet.eta() ) > m_jetEtaMax ) return false;

  return true;

}

/// Check and record if jet passes kinematic constraints
StatusCode JSSTaggerBase::checkKinRange( const xAOD::Jet &jet, asg::AcceptData &acceptData ) const {

  float scale = 1.0;
  if ( m_ptGeV ) scale = 1.e3;

  bool passKinRange = true;

  /// Check each kinematic constraint
  /// Print warnings using counters
  if ( std::abs(jet.eta()) > m_jetEtaMax ) {
    ATH_MSG_VERBOSE( "Jet does not pass basic kinematic selection (|eta| < " << m_jetEtaMax << "). Jet eta = " << jet.eta() );
    acceptData.setCutResult( "ValidEtaRange", false );
    passKinRange = false;
  }

  if ( jet.pt() < m_jetPtMin * scale ) {
    ATH_MSG_VERBOSE( "Jet does not pass basic kinematic selection (pT > " << m_jetPtMin * scale / 1.e3 << "). Jet pT = " << jet.pt() / 1.e3 << " GeV" );
    acceptData.setCutResult( "ValidPtRangeLow", false );
    passKinRange = false;
  }

  if ( jet.pt() > m_jetPtMax * scale ) {
    ATH_MSG_VERBOSE( "Jet does not pass basic kinematic selection (pT < " << m_jetPtMax * scale / 1.e3 << "). Jet pT = " << jet.pt() / 1.e3 << " GeV" );
    acceptData.setCutResult( "ValidPtRangeHigh", false );
    passKinRange = false;
  }

  /// Create write decor handles
  SG::WriteDecorHandle<xAOD::JetContainer, bool> decValidPtRangeHigh(m_decValidPtRangeHighKey);
  SG::WriteDecorHandle<xAOD::JetContainer, bool> decValidPtRangeLow(m_decValidPtRangeLowKey);
  SG::WriteDecorHandle<xAOD::JetContainer, bool> decValidEtaRange(m_decValidEtaRangeKey);
  SG::WriteDecorHandle<xAOD::JetContainer, bool> decValidKinRange(m_decValidKinRangeKey);
 
  /// Decorate kinematic pass information
  decValidPtRangeHigh(jet) = acceptData.getCutResult( "ValidPtRangeHigh" );
  decValidPtRangeLow(jet) = acceptData.getCutResult( "ValidPtRangeLow" );
  decValidEtaRange(jet) = acceptData.getCutResult( "ValidEtaRange" );
  decValidKinRange(jet) = passKinRange;

  return StatusCode::SUCCESS;

}

/// Calculate JSS moment ratios in case they are not already saved
/// These are calculated by hand here because JetSubStructureMomentTools
/// does not operate on const jets. This should be changed in the future
int JSSTaggerBase::calculateJSSRatios( const xAOD::Jet &jet ) const {

  int result = 0;

  /// Create write decor handles
  SG::WriteDecorHandle<xAOD::JetContainer, float> decTau21WTA(m_decTau21WTAKey);
  SG::WriteDecorHandle<xAOD::JetContainer, float> decTau32WTA(m_decTau32WTAKey);
  SG::WriteDecorHandle<xAOD::JetContainer, float> decTau42WTA(m_decTau42WTAKey);
  SG::WriteDecorHandle<xAOD::JetContainer, float> decC2(m_decC2Key);
  SG::WriteDecorHandle<xAOD::JetContainer, float> decD2(m_decD2Key);
  SG::WriteDecorHandle<xAOD::JetContainer, float> decE3(m_decE3Key);
  SG::WriteDecorHandle<xAOD::JetContainer, float> decL2(m_decL2Key);
  SG::WriteDecorHandle<xAOD::JetContainer, float> decL3(m_decL3Key);

  /// Create read decor handles
  SG::ReadDecorHandle<xAOD::JetContainer, float> readTau1WTA(m_readTau1WTAKey);
  SG::ReadDecorHandle<xAOD::JetContainer, float> readTau2WTA(m_readTau2WTAKey);
  SG::ReadDecorHandle<xAOD::JetContainer, float> readTau3WTA(m_readTau3WTAKey);
  SG::ReadDecorHandle<xAOD::JetContainer, float> readTau4WTA(m_readTau4WTAKey);

  SG::ReadDecorHandle<xAOD::JetContainer, float> readECF1(m_readECF1Key);
  SG::ReadDecorHandle<xAOD::JetContainer, float> readECF2(m_readECF2Key);
  SG::ReadDecorHandle<xAOD::JetContainer, float> readECF3(m_readECF3Key);

  SG::ReadDecorHandle<xAOD::JetContainer, float> readECFG331(m_readECFG331Key);
  SG::ReadDecorHandle<xAOD::JetContainer, float> readECFG311(m_readECFG311Key);
  SG::ReadDecorHandle<xAOD::JetContainer, float> readECFG212(m_readECFG212Key);

  SG::ReadDecorHandle<xAOD::JetContainer, float> readL2(m_readL2Key);
  SG::ReadDecorHandle<xAOD::JetContainer, float> readL3(m_readL3Key);


  /// WTA N-subjettiness ratios
  float tau21_wta = -999.0;
  float tau32_wta = -999.0;
  float tau42_wta = -999.0;

  float tau1_wta = readTau1WTA(jet);
  float tau2_wta = readTau2WTA(jet);
  float tau3_wta = readTau3WTA(jet);
  float tau4_wta = -999.0;
  if(readTau4WTA.isAvailable()){
    tau4_wta = readTau4WTA(jet);
  }

  if ( tau1_wta > 1e-8 ) {
    tau21_wta = tau2_wta / tau1_wta;
  }
  else result = 1;

  if ( tau2_wta > 1e-8 ) {
    tau32_wta = tau3_wta / tau2_wta;
    if(readTau4WTA.isAvailable()){
      tau42_wta = tau4_wta / tau2_wta;
    }
  }
  else result = 1;

  decTau21WTA(jet) = tau21_wta;
  decTau32WTA(jet) = tau32_wta;
  decTau42WTA(jet) = tau42_wta;

  /// ECF ratios
  float C2 = -999.0;
  float D2 = -999.0;
  float e3 = -999.0;
  
  float ECF1 = readECF1(jet);
  float ECF2 = readECF2(jet);
  float ECF3 = readECF3(jet);

  if ( ECF2 > 1e-8 ) {
    C2 = ECF3 * ECF1 / std::pow( ECF2, 2.0 );
    D2 = ECF3 * std::pow( ECF1, 3.0 ) / std::pow( ECF2, 3.0 );
  }
  else result = 1;

  e3 = ECF3 / std::pow( ECF1, 3.0 );
  
  decC2(jet) = C2;
  decD2(jet) = D2;
  decE3(jet) = e3;

  // L-series for UFO top taggers
  float L2 = -999.0;
  float L3 = -999.0;

  if(!readL2.isAvailable()){
    if(readECFG331.isAvailable() && readECFG212.isAvailable()){
      if(readECFG212(jet) > 1e-8){
	L2 = readECFG331(jet) / pow(readECFG212(jet), (3.0/2.0));
      }
      else result = 1;
    }
  }

  if(!readL3.isAvailable()){
    if(readECFG331.isAvailable() && readECFG311.isAvailable()){
      if(readECFG331(jet) > 1e-8){
	L3 = readECFG311(jet) / pow(readECFG331(jet), (1.0/3.0));
      }
      else result = 1;
    }
  }

  decL2(jet) = L2;
  decL3(jet) = L3;

  // TODO: Add ECFG for ANN tagger whenever it is defined

  return result;

}

/// Get SF weight
StatusCode JSSTaggerBase::getWeight( const xAOD::Jet& jet, bool passSel, asg::AcceptData &acceptData ) const {
  if ( !m_calcSF ) return StatusCode::SUCCESS;

  float weight = 1.0;
  float effSF = 1.0;
  float sigeffSF = 1.0;
  float efficiency = 1.0;

  if ( m_isMC ) {

    std::string truthLabelStr = getTruthLabelStr( jet, acceptData );
    std::tie(effSF, efficiency) = getSF( jet, truthLabelStr );

    // calculate signal efficiency SF                                           
    if ( m_weightHistograms.count("t_qqb") ) {
      sigeffSF = getSF(jet, "t_qqb").first;
    } else if ( m_weightHistograms.count("V_qq") ) {
      sigeffSF = getSF(jet, "V_qq").first;
    } else if ( m_weightHistograms.count("t") ){
      sigeffSF = getSF(jet, "t").first;
    } else {
      sigeffSF = 1.0;
    }

    /// Inefficiency SF is directly used
    if ( m_weightFlavors.find("fail") != std::string::npos ) {
      weight = effSF;
    }

    else {

      /// Efficiency SF
      if ( passSel ) {
        weight = effSF;
      }

      /// Calculate inefficiency SF
      else {
        /// If inefficiency SF is not available, SF is always 1.0
        if ( m_efficiencyHistogramName.empty() ) {
          weight = 1.0;
        }
        else if ( efficiency < 1.0 ) {
          weight = ( 1. - effSF * efficiency ) / ( 1. - efficiency );
        }
        else {
          weight = 1.0;
        }
      }
    }

  }
  
  else {
    weight = 1.0;
  }

  /// Create write decor handles
  SG::WriteDecorHandle<xAOD::JetContainer, float> decWeight(m_decWeightKey);
  SG::WriteDecorHandle<xAOD::JetContainer, float> decEfficiency(m_decEfficiencyKey);
  SG::WriteDecorHandle<xAOD::JetContainer, float> decEffSF(m_decEffSFKey);
  SG::WriteDecorHandle<xAOD::JetContainer, float> decSigeffSF(m_decSigeffSFKey);

  /// Decorate values  
  decWeight(jet) = weight;
  decEfficiency(jet) = efficiency;
  decEffSF(jet) = effSF;
  decSigeffSF(jet) = sigeffSF;

  return StatusCode::SUCCESS;

}

/// Get scale factor and efficiency
std::pair<double, double> JSSTaggerBase::getSF( const xAOD::Jet& jet, std::string truthLabelStr ) const {

  if ( !passKinRange(jet) ) return std::make_pair( 1.0, 1.0 );


  double logmOverPt = std::log(jet.m()/jet.pt());
  if ( m_decorationName.find("SmoothZ") != std::string::npos ||
       m_decorationName.find("SmoothInclusiveZ") != std::string::npos ) {
    /// To apply W-tagging efficiency SF to Z-tagger, jet mass is shifted by 10.803 GeV
    const double WtoZmassShift = 10803;
    logmOverPt = std::log((jet.m()-WtoZmassShift)/jet.pt());
  }

  if ( logmOverPt > 0 ) logmOverPt = 0;

  double SF = 1.0;
  double eff = 1.0;

  if ( m_weightHistograms.count(truthLabelStr.c_str()) ) {

    int pt_mPt_bin = (m_weightHistograms.find(truthLabelStr.c_str())->second)->FindBin(jet.pt()*0.001, logmOverPt);
    SF = (m_weightHistograms.find(truthLabelStr.c_str())->second)->GetBinContent(pt_mPt_bin);

    if ( !m_efficiencyHistogramName.empty() ) {
      eff = (m_efficiencyHistograms.find(truthLabelStr.c_str())->second)->GetBinContent(pt_mPt_bin);
    }

  }
  else {
    // set the efficiency for "Other" category to be the signal efficiency
    std::string signal_truthLabel="";
    if ( m_weightHistograms.count("t_qqb") ) {
      signal_truthLabel="t_qqb";
    }else if ( m_weightHistograms.count("V_qq") ){
      signal_truthLabel="V_qq";
    }else if ( m_weightHistograms.count("t") ){
      signal_truthLabel="t";
    }
    if ( signal_truthLabel != "" && !m_efficiencyHistogramName.empty() ){
      int pt_mPt_bin = (m_weightHistograms.find(signal_truthLabel.c_str())->second)->FindBin(jet.pt()*0.001, logmOverPt);
      eff = (m_efficiencyHistograms.find(signal_truthLabel.c_str())->second)->GetBinContent(pt_mPt_bin);
    }

    return std::make_pair( 1.0, eff );
  }

  if ( SF < 1e-3 ) {
    ATH_MSG_DEBUG( "(pt, m/pt) (" << jet.pt()/1.e3 << ", " << jet.m()/jet.pt() << ") is out of range for SF calculation. Returning 1.0" );
    return std::make_pair( 1.0, 1.0 );
  }
  else {
    return std::make_pair( SF, eff );
  }

}



std::string JSSTaggerBase::getTruthLabelStr( const xAOD::Jet& jet, asg::AcceptData &acceptData ) const {
  /// Truth label string
  std::string truthLabelStr;

  /// Truth label value
  SG::ReadDecorHandle<xAOD::JetContainer, int> readTruthLabel(m_readTruthLabelKey);
  LargeRJetTruthLabel::TypeEnum jetContainment = LargeRJetTruthLabel::intToEnum(readTruthLabel(jet));

  /// Contained top tagger
  if ( m_weightHistograms.count("t_qqb") ) {

    /// Contained top
    if ( jetContainment==LargeRJetTruthLabel::tqqb ) {
      truthLabelStr = "t_qqb";
    }
    /// QCD
    else if ( jetContainment==LargeRJetTruthLabel::notruth || jetContainment==LargeRJetTruthLabel::qcd ) {
      truthLabelStr = "q";
    }

  }
  /// TCC W/Z 2-var tagger
  else if ( m_weightHistograms.count("V_qq_passMpassD2") ) {

    /// Top
    if ( jetContainment==LargeRJetTruthLabel::tqqb || jetContainment==LargeRJetTruthLabel::other_From_t ) {
      truthLabelStr = "t_";
    }
    /// W/Z
    else if ( jetContainment==LargeRJetTruthLabel::Wqq || jetContainment==LargeRJetTruthLabel::Zqq || jetContainment==LargeRJetTruthLabel::Wqq_From_t ) {
      truthLabelStr = "V_qq_";
    }
    /// QCD
    else if ( jetContainment==LargeRJetTruthLabel::notruth || jetContainment==LargeRJetTruthLabel::qcd ) {
      truthLabelStr = "q_";
    }

    /// Pass mass and D2
    if ( acceptData.getCutResult("PassMassLow") && acceptData.getCutResult("PassMassHigh") && acceptData.getCutResult("PassD2") ) {
      truthLabelStr += "passMpassD2";
    }
    /// Fail mass, pass D2
    else if ( !(acceptData.getCutResult("PassMassLow") && acceptData.getCutResult("PassMassHigh")) && acceptData.getCutResult("PassD2") ) {
      truthLabelStr += "failMpassD2";
    }
    /// Pass mass, fail D2
    else if ( acceptData.getCutResult("PassMassLow") && acceptData.getCutResult("PassMassHigh") && !acceptData.getCutResult("PassD2") ) {
      truthLabelStr += "passMfailD2";
    }
    /// Fail mass and D2
    else{
      truthLabelStr += "failMfailD2";
    }

  }

  /// W/Z tagger
  else if ( m_weightHistograms.count("V_qq") ) {

    /// Top
    if ( jetContainment==LargeRJetTruthLabel::tqqb || jetContainment==LargeRJetTruthLabel::other_From_t ) {
      truthLabelStr = "t";
    }
    /// W/Z
    else if ( jetContainment==LargeRJetTruthLabel::Wqq || jetContainment==LargeRJetTruthLabel::Zqq || jetContainment==LargeRJetTruthLabel::Wqq_From_t ) {
      truthLabelStr = "V_qq";
    }
    /// QCD
    else if ( jetContainment==LargeRJetTruthLabel::notruth || jetContainment==LargeRJetTruthLabel::qcd ) {
      truthLabelStr = "q";
    }
  }

  // inclusive top tagger
  else {
    /// Top
    if ( jetContainment==LargeRJetTruthLabel::tqqb || jetContainment==LargeRJetTruthLabel::other_From_t || jetContainment==LargeRJetTruthLabel::Wqq_From_t ) {
      truthLabelStr = "t";
    }
    /// QCD
    else if ( jetContainment==LargeRJetTruthLabel::notruth || jetContainment==LargeRJetTruthLabel::qcd ) {
      truthLabelStr = "q";
    }

  }

  return truthLabelStr;
}

void JSSTaggerBase::printCuts() const {
  ATH_MSG_INFO( "After tagging, you will have access to the following cuts as an asg::AcceptData : (<NCut>) <cut> : <description>)" );
  int nCuts = m_acceptInfo.getNCuts();
  for ( int iCut=0; iCut < nCuts; iCut++ ) {
    std::string cut_string = "";
    cut_string += "  (";
    cut_string += std::to_string(iCut);
    cut_string += ")  ";
    cut_string += m_acceptInfo.getCutName(iCut).data();
    cut_string += " : ";
    cut_string += m_acceptInfo.getCutDescription(iCut).data();
    ATH_MSG_INFO( cut_string );
  }
}
