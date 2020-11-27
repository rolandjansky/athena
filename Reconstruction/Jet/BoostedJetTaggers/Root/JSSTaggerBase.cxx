/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "BoostedJetTaggers/JSSTaggerBase.h"

#include <TSystem.h>

JSSTaggerBase::JSSTaggerBase(const std::string &name) :
  asg::AsgTool(name),
  m_calibArea(""),
  m_jetPtMin(200000.),
  m_jetPtMax(3000000.),
  m_jetEtaMax(2.0),
  m_strMassCutLow(""),
  m_strMassCutHigh(""),
  m_strScoreCut(""),
  m_decorate(true)
{

  /// Tagger configuration properties
  declareProperty( "ConfigFile",    m_configFile = "",        "Name of config file" );
  declareProperty( "CalibArea",     m_calibArea = "",         "Path to config file" );
  declareProperty( "CalcSF",        m_calcSF = false,         "Flag to calculate the efficiency SF"  );
  declareProperty( "WorkingPoint",  m_wkpt = "",              "If specified, name of working point is added to the variable names" );
  declareProperty( "TaggerType",    m_tagType = "XXX",        "Tagger type (e.g. SmoothedWZTagger, JSSWTopTaggerDNN, etc.)" );
  declareProperty( "DecorateJet",   m_decorate = true,        "Flag to decorate tagging results to xAOD::Jet" );
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
  declareProperty( "WeightDecorationName",      m_weightdecorationName = "SF",
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

  /// Initialize warning counters
  m_nWarnKin = 0;
  m_nWarnVar = 0;

  /// Define common tagger states
  m_accept.addCut( "ValidPtRangeHigh"  , "True if the jet is not too high pT" );
  m_accept.addCut( "ValidPtRangeLow"   , "True if the jet is not too low pT" );
  m_accept.addCut( "ValidEtaRange"     , "True if the jet is not too forward" );
  
  m_accept.addCut( "ValidJetContent"   , "True if the jet is alright technically (e.g. all attributes necessary for tag)" );
  m_accept.addCut( "ValidEventContent" , "True if the event is alright technically (e.g. primary vertices)" );

  /// Create cut functions from strings
  if ( !m_strMassCutLow.empty() ) m_funcMassCutLow = std::make_unique<TF1>("strMassCutLow", m_strMassCutLow.c_str(), 0, 14000);
  if ( !m_strMassCutHigh.empty() ) m_funcMassCutHigh = std::make_unique<TF1>("strMassCutHigh", m_strMassCutHigh.c_str(), 0, 14000);
  if ( !m_strScoreCut.empty() ) m_funcScoreCut = std::make_unique<TF1>("strScoreCut", m_strScoreCut.c_str(), 0, 14000);

  /// Initialize decorators
  ATH_MSG_INFO( "Decorators that will be attached to jet :" );

  if ( m_useMassCut ) {
    ATH_MSG_INFO( "  " << m_decorationName << "_Cut_mlow : lower mass cut" );
    ATH_MSG_INFO( "  " << m_decorationName << "_Cut_mhigh : upper mass cut" );

    m_dec_mcutL = std::make_unique< SG::AuxElement::Decorator<float> >( m_decorationName + "_Cut_mlow" );
    m_dec_mcutH = std::make_unique< SG::AuxElement::Decorator<float> >( m_decorationName + "_Cut_mhigh" );
  }

  if ( m_useScoreCut ) {
    ATH_MSG_INFO( "  " << m_decorationName << "_Cut_score : MVA score cut" );
    ATH_MSG_INFO( "  " << m_decorationName << "_Score : evaluated MVA score" );

    m_dec_scoreCut = std::make_unique< SG::AuxElement::Decorator<float> >( m_decorationName + "_Cut_score" );
    m_dec_scoreValue = std::make_unique< SG::AuxElement::Decorator<float> >( m_decorationName + "_Score" );
  }

  if ( m_calcSF ) {
    ATH_MSG_INFO( "  " << m_decorationName << "_" << m_weightdecorationName << " : tagging SF" );
    m_dec_weight     = std::make_unique< SG::AuxElement::Decorator<float> >( m_decorationName + "_" + m_weightdecorationName );
    m_dec_effSF      = std::make_unique< SG::AuxElement::Decorator<float> >( m_decorationName + "_effSF" );
    m_dec_efficiency = std::make_unique< SG::AuxElement::Decorator<float> >( m_decorationName + "_efficiency" );
    m_acc_truthLabel = std::make_unique< SG::AuxElement::ConstAccessor<int> >( m_truthLabelName );
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

    /// Set up jet truth labeling tool for SF calculation
    ASG_SET_ANA_TOOL_TYPE( m_JetTruthLabelingTool, JetTruthLabelingTool);
    m_JetTruthLabelingTool.setName("JetTruthLabelingTool");
    ATH_CHECK( m_JetTruthLabelingTool.setProperty("TruthLabelName", m_truthLabelName) );
    ATH_CHECK( m_JetTruthLabelingTool.setProperty("UseTRUTH3", m_truthLabelUseTRUTH3) );
    ATH_CHECK( m_JetTruthLabelingTool.setProperty("TruthParticleContainerName", m_truthParticleContainerName) );
    ATH_CHECK( m_JetTruthLabelingTool.setProperty("TruthBosonContainerName", m_truthBosonContainerName) );
    ATH_CHECK( m_JetTruthLabelingTool.setProperty("TruthTopQuarkContainerName", m_truthTopQuarkContainerName) );
    ATH_CHECK( m_JetTruthLabelingTool.retrieve() );
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

// Reset cuts
void JSSTaggerBase::resetCuts() const {

  /// Reset the TAccept cut results to false
  m_accept.clear();

  /// Initialize common cuts to true by default
  m_accept.setCutResult( "ValidJetContent", true );
  m_accept.setCutResult( "ValidEventContent", true );
  
  m_accept.setCutResult( "ValidPtRangeHigh", true );
  m_accept.setCutResult( "ValidPtRangeLow" , true );
  m_accept.setCutResult( "ValidEtaRange"   , true );

  return;

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
void JSSTaggerBase::checkKinRange( const xAOD::Jet &jet ) const {

  float scale = 1.0;
  if ( m_ptGeV ) scale = 1.e3;

  /// Check each kinematic constraint
  /// Print warnings using counters
  if ( std::abs(jet.eta()) > m_jetEtaMax ) {
    if ( m_nWarnKin++ < m_nWarnMax ) ATH_MSG_WARNING( "Jet does not pass basic kinematic selection (|eta| < " << m_jetEtaMax << "). Jet eta = " << jet.eta() );
    else ATH_MSG_DEBUG( "Jet does not pass basic kinematic selection (|eta| < " << m_jetEtaMax << "). Jet eta = " << jet.eta() );
    m_accept.setCutResult( "ValidEtaRange", false );
  }

  if ( jet.pt() < m_jetPtMin * scale ) {
    if ( m_nWarnKin++ < m_nWarnMax ) ATH_MSG_WARNING("Jet does not pass basic kinematic selection (pT > " << m_jetPtMin * scale / 1.e3 << "). Jet pT = " << jet.pt() / 1.e3 << " GeV" );
    else ATH_MSG_DEBUG( "Jet does not pass basic kinematic selection (pT > " << m_jetPtMin * scale / 1.e3 << "). Jet pT = " << jet.pt() / 1.e3 << " GeV" );
    m_accept.setCutResult( "ValidPtRangeLow", false );
  }

  if ( jet.pt() > m_jetPtMax * scale ) {
    if( m_nWarnKin++ < m_nWarnMax ) ATH_MSG_WARNING( "Jet does not pass basic kinematic selection (pT < " << m_jetPtMax * scale / 1.e3 << "). Jet pT = " << jet.pt() / 1.e3 << " GeV" );
    else ATH_MSG_DEBUG( "Jet does not pass basic kinematic selection (pT < " << m_jetPtMax * scale / 1.e3 << "). Jet pT = " << jet.pt() / 1.e3 << " GeV" );
    m_accept.setCutResult( "ValidPtRangeHigh", false );
  }

  return;

}

/// Calculate JSS moment ratios in case they are not already saved
/// These are calculated by hand here because JetSubStructureMomentTools
/// does not operate on const jets. This should be changed in the future
int JSSTaggerBase::calculateJSSRatios( const xAOD::Jet &jet ) const {

  int result = 0;

  /// WTA N-subjettiness ratios
  float tau21_wta = -999.0;
  float tau32_wta = -999.0;

  float tau1_wta = acc_Tau1_wta(jet);
  float tau2_wta = acc_Tau2_wta(jet);
  float tau3_wta = acc_Tau3_wta(jet);

  if ( tau1_wta > 1e-8 ) {
    tau21_wta = tau2_wta / tau1_wta;
  }
  else result = 1;

  if ( tau2_wta > 1e-8 ) {
    tau32_wta = tau3_wta / tau2_wta;
  }
  else result = 1;

  dec_Tau21_wta(jet) = tau21_wta;
  dec_Tau32_wta(jet) = tau32_wta;

  /// ECF ratios
  float C2 = -999.0;
  float D2 = -999.0;
  float e3 = -999.0;
  
  float ECF1 = acc_ECF1(jet);
  float ECF2 = acc_ECF2(jet);
  float ECF3 = acc_ECF3(jet);

  if ( ECF2 > 1e-8 ) {
    C2 = ECF3 * ECF1 / std::pow( ECF2, 2.0 );
    D2 = ECF3 * std::pow( ECF1, 3.0 ) / std::pow( ECF2, 3.0 );
  }
  else result = 1;

  e3 = ECF3 / std::pow( ECF1, 3.0 );
  
  dec_C2(jet) = C2;
  dec_D2(jet) = D2;
  dec_e3(jet) = e3;

  // TODO: Add L-series for UFO taggers
  // TODO: Add ECFG for ANN tagger whenever it is defined

  return result;

}

/// Get SF weight
void JSSTaggerBase::getWeight( const xAOD::Jet& jet, bool passSel ) const {

  if ( !m_calcSF ) return;

  float weight = 1.0;
  float effSF = 1.0;
  float efficiency = 1.0;

  if ( m_isMC ) {

    std::tie(effSF, efficiency) = getSF(jet);

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

  if ( m_decorate ) {
    (*m_dec_weight)(jet)     = weight;
    (*m_dec_efficiency)(jet) = efficiency;
    (*m_dec_effSF)(jet)      = effSF;
  }

  ATH_MSG_VERBOSE( "Jet SF = " << weight );
  
  return;

}

/// Get scale factor and efficiency
std::pair<double, double> JSSTaggerBase::getSF( const xAOD::Jet& jet ) const {

  if ( !passKinRange(jet) ) return std::make_pair( 1.0, 1.0 );

  /// Truth label string
  std::string truthLabelStr;

  /// Truth label value
  LargeRJetTruthLabel::TypeEnum jetContainment = LargeRJetTruthLabel::intToEnum((*m_acc_truthLabel)(jet));

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
    if ( m_accept.getCutResult("PassMassLow") && m_accept.getCutResult("PassMassHigh") && m_accept.getCutResult("PassD2") ) {
      truthLabelStr += "passMpassD2";
    }
    /// Fail mass, pass D2
    else if ( !(m_accept.getCutResult("PassMassLow") && m_accept.getCutResult("PassMassHigh")) && m_accept.getCutResult("PassD2") ) {
      truthLabelStr += "failMpassD2";
    }
    /// Pass mass, fail D2
    else if ( m_accept.getCutResult("PassMassLow") && m_accept.getCutResult("PassMassHigh") && !m_accept.getCutResult("PassD2") ) {
      truthLabelStr += "passMfailD2";
    }
    /// Fail mass and D2
    else{
      truthLabelStr += "failMfailD2";
    }

  }
  
  /// W/Z or inclusive top tagger
  else {

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

  double logmOverPt = std::log(jet.m()/jet.pt());
  if ( m_decorationName.find("SmoothZ") != std::string::npos ) {
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
    ATH_MSG_DEBUG( "SF for truth label for " << truthLabelStr << " is not available. Returning 1.0" );
    return std::make_pair( 1.0, 1.0 );
  }

  if ( SF < 1e-3 ) {
    ATH_MSG_DEBUG( "(pt, m/pt) (" << jet.pt()/1.e3 << ", " << jet.m()/jet.pt() << ") is out of range for SF calculation. Returning 1.0" );
    return std::make_pair( 1.0, 1.0 );
  }
  else {
    return std::make_pair( SF, eff );
  }

}

/// Print configured cuts
void JSSTaggerBase::printCuts() const {
  ATH_MSG_INFO( "After tagging, you will have access to the following cuts as a Root::TAccept : (<NCut>) <cut> : <description>)" );
  int nCuts = m_accept.getNCuts();
  for(int iCut=0; iCut<nCuts; iCut++){
    std::string cut_string = "";
    cut_string += "  (";
    cut_string += std::to_string(iCut);
    cut_string += ")  ";
    cut_string += m_accept.getCutName(iCut).Data();
    cut_string += " : ";
    cut_string += m_accept.getCutDescription(iCut).Data();
    ATH_MSG_INFO( cut_string );
  }
}
