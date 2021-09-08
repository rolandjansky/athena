/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "BoostedJetTaggers/JSSWTopTaggerBDT.h"

JSSWTopTaggerBDT::JSSWTopTaggerBDT( const std::string& name ) :
  JSSTaggerBase( name ),
  m_inputVariableSet("NvarM"),
  m_BDTmethod("BDTG method")
{

  declareProperty( "JetPtMin",  m_jetPtMin = 200.0e3);
  declareProperty( "JetPtMax",  m_jetPtMax = 3000.0e3);

}

StatusCode JSSWTopTaggerBDT::initialize() {
  
  ATH_MSG_INFO( "Initializing JSSWTopTaggerBDT tool" );

  /// Use mass cut
  m_useMassCut = true;

  /// Use discriminant score cut
  m_useScoreCut = true;

  if( ! m_configFile.empty() ) {

    /// Get configReader
    ATH_CHECK( getConfigReader() );

    /// Get tagger type
    m_tagType = m_configReader.GetValue("TaggerType" ,"");

    /// Get the CVMFS calib area where stuff is stored
    m_tmvaCalibArea = m_configReader.GetValue("CalibAreaTMVA" ,"");

    /// Get the name/path of the JSON config
    m_tmvaConfigFileName = m_configReader.GetValue("TMVAConfigFile" ,"");

    /// Input parameter setting type
    m_inputVariableSet = m_configReader.GetValue("InputVariableSet" ,"");

    /// Get the configured cut values
    m_strMassCutLow  = m_configReader.GetValue("MassCutLow" ,"");
    m_strMassCutHigh = m_configReader.GetValue("MassCutHigh" ,"");
    m_strScoreCut    = m_configReader.GetValue("ScoreCut" ,"");

    /// Get the decoration name
    m_decorationName = m_configReader.GetValue("DecorationName" ,"");

    ATH_MSG_INFO( "Configurations Loaded  :");
    ATH_MSG_INFO( "tagType                : "<<m_tagType );
    ATH_MSG_INFO( "calibarea_tmva         : "<<m_tmvaCalibArea );
    ATH_MSG_INFO( "tmvaConfigFileName     : "<<m_tmvaConfigFileName );
    ATH_MSG_INFO( "strMassCutLow          : "<<m_strMassCutLow  );
    ATH_MSG_INFO( "strMassCutHigh         : "<<m_strMassCutHigh );
    ATH_MSG_INFO( "strScoreCut            : "<<m_strScoreCut );
    ATH_MSG_INFO( "decorationName         : "<<m_decorationName );

  }
  else { /// No config file
    /// Assume the cut functions have been set through properties.
    /// Check they are non empty
    if ( m_tmvaConfigFileName.empty() ||
        m_strScoreCut.empty() ||
        m_strMassCutLow.empty() ||
        m_strMassCutHigh.empty() ||
        m_decorationName.empty()) {
      ATH_MSG_ERROR( "No config file provided AND you haven't manually specified all needed parameters" ) ;
      ATH_MSG_ERROR( "Please read the TWiki for this tool" );
      return StatusCode::FAILURE;
    }

  }

  ATH_MSG_INFO( "Mass cut low   : "<< m_strMassCutLow );
  ATH_MSG_INFO( "Mass cut High  : "<< m_strMassCutHigh );
  ATH_MSG_INFO( "Score cut low  : "<< m_strScoreCut );

  /// If the calibarea is specified to be "Local" then it looks in the same place as the top level configs
  if ( m_tmvaCalibArea.empty() ) {
    ATH_MSG_ERROR( "You need to specify where the calibarea is as either being Local or on CVMFS" );
    return StatusCode::FAILURE;
  }
  else if ( !m_tmvaCalibArea.compare("Local") ) {
    std::string localCalibArea = "BoostedJetTaggers/JSSWTopTaggerBDT/";
    ATH_MSG_INFO( "Using Local calibarea " << localCalibArea );
    /// Convert the JSON config file name to the full path
    m_tmvaConfigFilePath = PathResolverFindCalibFile(localCalibArea+m_tmvaConfigFileName);
  }
  else {
    ATH_MSG_INFO( "Using CVMFS calibarea" );
    /// Get the config file from CVMFS
    /// Necessary because xml files are too large to house on the data space
    m_tmvaConfigFilePath = PathResolverFindCalibFile( (m_tmvaCalibArea+m_tmvaConfigFileName).c_str() );
  }

  /// Read json file for DNN weights
  ATH_MSG_INFO( "BDT Tagger configured with: " << m_tmvaConfigFilePath );

  /// Initialize TMVA for BDTs
  TMVA::Tools::Instance();
  m_bdtTagger = std::make_unique<TMVA::Reader>( "!Color:!Silent" );

  /// Set internal tagger type
  if ( !m_tagType.compare("TopQuarkContained") ) {
    ATH_MSG_DEBUG( "This is a top quark tagger" );
    m_tagClass = TAGCLASS::TopQuark;
  }
  else if ( !m_tagType.compare("WBoson") ) {
    ATH_MSG_DEBUG( "This is a W boson tagger" );
    m_tagClass = TAGCLASS::WBoson;
  }
  else if ( !m_tagType.compare("ZBoson") ) {
    ATH_MSG_DEBUG( "This is a Z boson tagger" );
    m_tagClass = TAGCLASS::ZBoson;
  }
  else {
    ATH_MSG_ERROR( "I can't tell what kind of tagger your configuration is for." );
    return StatusCode::FAILURE;
  }

  /// Add variables based on tagger type
  if ( m_tagClass == TAGCLASS::WBoson ) {
    ATH_MSG_DEBUG( "W Boson BDT Tagger Selected" );

    m_bdtTagger->AddVariable( "fjet_CaloTACombinedMassUncorrelated", &m_mass );
    m_bdtTagger->AddVariable( "fjet_JetpTCorrByCombinedMass", &m_pt );
    m_bdtTagger->AddVariable( "fjet_KtDR", &m_KtDR );
    m_bdtTagger->AddVariable( "fjet_Tau1_wta", &m_Tau1_wta );
    m_bdtTagger->AddVariable( "fjet_Tau21_wta", &m_Tau21_wta );
    m_bdtTagger->AddVariable( "fjet_C2", &m_C2 );
    m_bdtTagger->AddVariable( "fjet_D2", &m_D2 );
    m_bdtTagger->AddVariable( "fjet_Angularity", &m_Angularity );
    m_bdtTagger->AddVariable( "fjet_Aplanarity", &m_Aplanarity );
    m_bdtTagger->AddVariable( "fjet_PlanarFlow", &m_PlanarFlow );
    m_bdtTagger->AddVariable( "fjet_FoxWolfram20", &m_FoxWolfram20 );

  }
  else if ( m_tagClass == TAGCLASS::TopQuark ) {
    ATH_MSG_DEBUG( "Top Quark (Fully Contained) BDT Tagger Selected" );

    m_bdtTagger->AddVariable( "fjet_CaloTACombinedMassUncorrelated", &m_mass );
    m_bdtTagger->AddVariable( "fjet_JetpTCorrByCombinedMass", &m_pt );
    m_bdtTagger->AddVariable( "fjet_Tau2_wta", &m_Tau2_wta );
    m_bdtTagger->AddVariable( "fjet_Tau21_wta", &m_Tau21_wta );
    m_bdtTagger->AddVariable( "fjet_Tau32_wta", &m_Tau32_wta );
    m_bdtTagger->AddVariable( "fjet_D2", &m_D2 );
    m_bdtTagger->AddVariable( "fjet_e3", &m_e3 );
    m_bdtTagger->AddVariable( "fjet_Split12", &m_Split12 );
    m_bdtTagger->AddVariable( "fjet_Split23", &m_Split23 );
    m_bdtTagger->AddVariable( "fjet_Qw", &m_Qw );

  }
  else {
    ATH_MSG_DEBUG( "Using custom configuration - if you are doing this, you may need to do some hardcoding because the tool is not flexible enough to change inputs ..." );
  }

  /// Configure the BDT
  m_bdtTagger->BookMVA( m_BDTmethod.c_str(), m_tmvaConfigFilePath.c_str() );

  /// Set the possible states that the tagger can be left in after the JSSTaggerBase::tag() function is called
  m_accept.addCut( "PassMassLow" , "mJet > mCutLow" );
  m_accept.addCut( "PassScore"   , "ScoreJet > ScoreCut" );
  if ( m_tagClass == TAGCLASS::WBoson || m_tagClass == TAGCLASS::ZBoson ) {
    m_accept.addCut( "PassMassHigh", "mJet < mCutHigh" );
  }
  
  /// Call base class initialize
  ATH_CHECK( JSSTaggerBase::initialize() );

  /// Loop over and print out the cuts that have been configured
  printCuts();
  
  ATH_MSG_INFO( "BDT Tagger tool initialized" );

  return StatusCode::SUCCESS;

}

Root::TAccept& JSSWTopTaggerBDT::tag( const xAOD::Jet& jet ) const {

  ATH_MSG_DEBUG( "Obtaining BDT result" );

  /// Reset the TAccept cut results
  resetCuts();

  /// Check basic kinematic selection
  checkKinRange(jet);

  /// Get the relevant attributes of the jet
  /// Mass and pt - note that this will depend on the configuration of the calibration used
  float jet_pt   = jet.pt()/1000.0;
  float jet_mass = jet.m()/1000.0;

  /// Get BDT score
  float jet_score = getScore(jet);

  /// Evaluate the values of the upper and lower mass bounds and the d2 cut
  float cut_mass_low  = m_funcMassCutLow ->Eval(jet_pt);
  float cut_mass_high = m_funcMassCutHigh->Eval(jet_pt);
  float cut_score     = m_funcScoreCut   ->Eval(jet_pt);

  /// Decorate cut information if needed;
  if ( m_decorate ) {
    ATH_MSG_DEBUG( "Decorating with score" );
    (*m_dec_mcutH)(jet)      = cut_mass_high;
    (*m_dec_mcutL)(jet)      = cut_mass_low;
    (*m_dec_scoreCut)(jet)   = cut_score;
    (*m_dec_scoreValue)(jet) = jet_score;
  }

  /// Print cut criteria and jet values
  ATH_MSG_VERBOSE( "Cut values : Mass window = [" << cut_mass_low << "," << cut_mass_high << "], score cut = " << cut_score );
  ATH_MSG_VERBOSE( "Jet values : Mass = " << jet_mass << ", score = " << jet_score );

  /// Set the TAccept depending on whether it is a W/Z or a top tag
  if ( m_tagClass == TAGCLASS::WBoson || m_tagClass == TAGCLASS::ZBoson ) {
    ATH_MSG_VERBOSE( "Determining WZ tag return" );
    if( jet_mass > cut_mass_low )
      m_accept.setCutResult( "PassMassLow", true );
    if( jet_mass < cut_mass_high )
      m_accept.setCutResult( "PassMassHigh", true );
    if( jet_score > cut_score )
      m_accept.setCutResult( "PassScore", true );
  }
  else if ( m_tagClass == TAGCLASS::TopQuark ) {
    ATH_MSG_VERBOSE( "Determining TopQuarkContained tag return" );
    if( jet_mass > cut_mass_low )
      m_accept.setCutResult( "PassMassLow", true );
    if( jet_score > cut_score )
      m_accept.setCutResult( "PassScore", true );
  }

  /// Return the TAccept object that was created and filled
  return m_accept;

}

float JSSWTopTaggerBDT::getScore( const xAOD::Jet& jet ) const {

  /// Load the new values of the variables for this jet
  getJetProperties(jet);

  /// Evaluate BDT
  float bdt_score = -666.;

  /// Check that input variables are valid
  bool validVars = true;
  if ( acc_Tau21_wta(jet) < 0.0 ) validVars = false;
  if ( acc_Tau32_wta(jet) < 0.0 ) validVars = false;

  if ( !validVars ) {

    if ( m_nWarnVar++ < m_nWarnMax ) ATH_MSG_WARNING( "One (or more) tagger input variable has an out-of-range value, setting score to -666" );
    else ATH_MSG_DEBUG( "One (or more) tagger input variable has an out-of-range value, setting score to -666" );

    return bdt_score;
  
  }

  bdt_score = m_bdtTagger->EvaluateMVA( m_BDTmethod.c_str() );

  return bdt_score;

}

void JSSWTopTaggerBDT::getJetProperties( const xAOD::Jet& jet ) const {

  /// Calculate NSubjettiness and ECF ratios
  calculateJSSRatios(jet);

  ATH_MSG_DEBUG( "Loading variables for common BDT tagger" );

  /// Mass and pT
  /// It is assumed that these are the combined mass and calibrated mass and pT
  m_pt   = jet.pt();
  m_mass = jet.m();

  /// N-subjettiness
  m_Tau1_wta = acc_Tau1_wta(jet);
  m_Tau2_wta = acc_Tau2_wta(jet);
  
  m_Tau21_wta = acc_Tau21_wta(jet);
  m_Tau32_wta = acc_Tau32_wta(jet);

  /// Energy Correlation Functions
  m_C2 = acc_C2(jet);
  m_D2 = acc_D2(jet);

  /// e3 := normalized ECF3/ECF1**3
  m_e3 = acc_e3(jet);

  if ( m_tagClass == TAGCLASS::WBoson ) {

    ATH_MSG_DEBUG("Loading variables for W boson tagger");

    /// Splitting scales
    m_KtDR = jet.getAttribute<float>("KtDR");

    /// Angularity
    m_Angularity = jet.getAttribute<float>("Angularity");

    /// Planar flow
    m_PlanarFlow   = jet.getAttribute<float>("PlanarFlow");

    /// Fox Wolfram ratio
    m_FoxWolfram20 = jet.getAttribute<float>("FoxWolfram2") / jet.getAttribute<float>("FoxWolfram0");

    /// Aplanarity
    m_Aplanarity = jet.getAttribute<float>("Aplanarity");

  }
  
  else if ( m_tagClass == TAGCLASS::TopQuark ) {

    ATH_MSG_DEBUG( "Loading variables for top quark tagger" );

    /// Splitting scales
    m_Split12 = acc_Split12(jet);
    m_Split23 = acc_Split23(jet);

    /// Qw
    m_Qw = jet.getAttribute<float>("Qw");
  
  }
 
  else {
    ATH_MSG_ERROR( "Loading variables failed because the tagger type is not supported" );
  }

  return;

}

