/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "BoostedJetTaggers/JSSWTopTaggerDNN.h"

#include <fstream>

JSSWTopTaggerDNN::JSSWTopTaggerDNN( const std::string& name ) :
  JSSTaggerBase( name ),
  m_lwnn(nullptr)
{

}

/// Initialize the tagger
StatusCode JSSWTopTaggerDNN::initialize() {

  ATH_MSG_INFO( "Initializing JSSWTopTaggerDNN tool" );
 
  /// Pt values are defined in GeV
  m_ptGeV = true;

  /// Use mass cut
  m_useMassCut = true;

  /// Use discriminant score cut
  m_useScoreCut = true;

  if ( ! m_configFile.empty() ) {

    /// Get configReader
    ATH_CHECK( getConfigReader() );

    /// Get tagger type
    m_tagType = m_configReader.GetValue("TaggerType" ,"");

    /// Get the CVMFS calib area where stuff is stored
    /// If this is set to "Local" then it will look for the config file in the share space
    m_kerasCalibArea = m_configReader.GetValue("CalibAreaKeras" ,"");

    /// Get the name/path of the JSON config
    m_kerasConfigFileName = m_configReader.GetValue("KerasConfigFile" ,"");

    /// Get the name of the Keras output node
    m_kerasConfigOutputName = m_configReader.GetValue("KerasOutput" ,"");

    /// Get the configured cut values
    m_strMassCutLow  = m_configReader.GetValue("MassCutLow" ,"");
    m_strMassCutHigh = m_configReader.GetValue("MassCutHigh" ,"");
    m_strScoreCut    = m_configReader.GetValue("ScoreCut" ,"");

    /// Get min and max jet pt
    m_jetPtMin = m_configReader.GetValue("pTCutLow", 350.0);
    m_jetPtMax = m_configReader.GetValue("pTCutHigh", 4000.0);

    /// Get the decoration name
    m_decorationName = m_configReader.GetValue("DecorationName" ,"");

    /// Get the scale factor configuration
    m_calcSF = m_configReader.GetValue("CalcSF", false);
    if ( m_calcSF ) {
      m_weightDecorationName = m_configReader.GetValue("WeightDecorationName", "");
      m_weightFileName = m_configReader.GetValue("WeightFile", "");
      m_weightHistogramName = m_configReader.GetValue("WeightHistogramName", "");
      m_efficiencyHistogramName = m_configReader.GetValue("EfficiencyHistogramName", "");
      m_weightFlavors = m_configReader.GetValue("WeightFlavors", "");

      /// Get truth label name information
      m_truthLabelName = m_configReader.GetValue("TruthLabelName" , "R10TruthLabel_R21Consolidated");
    }

    /// print out the configuration parameters for viewing
    ATH_MSG_INFO( "Configurations Loaded :");
    ATH_MSG_INFO( "tagType               : " << m_tagType );
    ATH_MSG_INFO( "calibarea_keras       : " << m_kerasCalibArea );
    ATH_MSG_INFO( "kerasConfigFileName   : " << m_kerasConfigFileName );
    ATH_MSG_INFO( "kerasConfigOutputName : " << m_kerasConfigOutputName );
    ATH_MSG_INFO( "strMassCutLow         : " << m_strMassCutLow );
    ATH_MSG_INFO( "strMassCutHigh        : " << m_strMassCutHigh );
    ATH_MSG_INFO( "pTCutLow              : " << m_jetPtMin );
    ATH_MSG_INFO( "pTCutHigh             : " << m_jetPtMax );
    ATH_MSG_INFO( "strScoreCut           : " << m_strScoreCut );
    ATH_MSG_INFO( "decorationName        : " << m_decorationName );
    if ( m_calcSF ) {
      ATH_MSG_INFO( "weightDecorationName  : " << m_weightDecorationName );
      ATH_MSG_INFO( "weightFile            : " << m_weightFileName );
      ATH_MSG_INFO( "weightHistogramName   : " << m_weightHistogramName );
      ATH_MSG_INFO( "efficiencyHistogramName : "<<m_efficiencyHistogramName );
      ATH_MSG_INFO( "weightFlavors         : " << m_weightFlavors );
      ATH_MSG_INFO( "TruthLabelName        : " << m_truthLabelName );
    }
  }
  else { /// No config file
    /// Assume the cut functions have been set through properties.
    /// Check they are non empty
    if ( (m_kerasConfigFileName.empty() ||
          m_kerasConfigOutputName.empty() ||
          m_strScoreCut.empty() ||
          m_strMassCutLow.empty() ||
          m_strMassCutHigh.empty() ||
          m_decorationName.empty() || 
          m_weightFileName.empty()) ||
        ((m_weightDecorationName.empty() ||
          m_weightHistogramName.empty() ||
          m_weightFlavors.empty()) && m_calcSF)
       )
    {
      ATH_MSG_ERROR( "No config file provided OR you haven't manually specified all needed parameters" ) ;
      ATH_MSG_ERROR( "Please read the TWiki for this tool" );
      return StatusCode::FAILURE;
    }

  }

  ATH_MSG_INFO( "Mass cut low   : " << m_strMassCutLow );
  ATH_MSG_INFO( "Mass cut High  : " << m_strMassCutHigh );
  ATH_MSG_INFO( "Score cut low  : " << m_strScoreCut );

  /// If the calibarea is specified to be "Local" then it looks in the same place as the top level configs
  if ( m_kerasCalibArea.empty() ) {
    ATH_MSG_ERROR( "You need to specify where the calibarea is as either being Local or on CVMFS" );
    return StatusCode::FAILURE;
  }
  else if ( !m_kerasCalibArea.compare("Local") ){
    std::string localCalibArea = "BoostedJetTaggers/JSSWTopTaggerDNN/";
    ATH_MSG_INFO( "Using Local calibarea " << localCalibArea );
    /// Convert the JSON config file name to the full path
    m_kerasConfigFilePath = PathResolverFindCalibFile(localCalibArea+m_kerasConfigFileName);
    if ( m_calcSF )
      m_weightConfigPath = PathResolverFindCalibFile(localCalibArea+m_weightFileName);
  }
  else {
    ATH_MSG_INFO( "Using CVMFS calibarea" );
    /// Get the config file from CVMFS
    /// Necessary because xml files are too large to house on the data space
    m_kerasConfigFilePath = PathResolverFindCalibFile( (m_kerasCalibArea+m_kerasConfigFileName).c_str() );
    if ( m_calcSF )
      m_weightConfigPath = PathResolverFindCalibFile( (m_kerasCalibArea+m_weightFileName).c_str());
  }

  /// Read json file for DNN weights
  ATH_MSG_INFO( "DNN Tagger configured with: " << m_kerasConfigFilePath );

  std::ifstream input_cfg( m_kerasConfigFilePath.c_str() );

  if ( !input_cfg.is_open() ) {
    ATH_MSG_ERROR( "Error openning config file: " << m_kerasConfigFilePath );
    ATH_MSG_ERROR( "Are you sure that the file exists at this path?" );
    return StatusCode::FAILURE;
  }

  lwt::JSONConfig cfg = lwt::parse_json( input_cfg );

  ATH_MSG_INFO( "Keras Network NLayers: " << cfg.layers.size() );

  m_lwnn = std::make_unique<lwt::LightweightNeuralNetwork>(cfg.inputs, cfg.layers, cfg.outputs);

  /// Set internal tagger type
  if ( !m_tagType.compare("TopQuark") ) {
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

  /// Set the possible states that the tagger can be left in after the JSSTaggerBase::tag() function is called
  m_acceptInfo.addCut( "PassMassLow" , "mJet > mCutLow" );
  m_acceptInfo.addCut( "PassScore"   , "ScoreJet > ScoreCut" );
  if ( m_tagClass == TAGCLASS::WBoson || m_tagClass == TAGCLASS::ZBoson ) {
    m_acceptInfo.addCut( "PassMassHigh", "mJet < mCutHigh" );
  }

  /// Loop over and print out the cuts that have been configured
  printCuts();

  /// Call base class initialize
  ATH_CHECK( JSSTaggerBase::initialize() );

  ATH_MSG_INFO( "DNN Tagger tool initialized" );

  return StatusCode::SUCCESS;

}

StatusCode JSSWTopTaggerDNN::tag( const xAOD::Jet& jet ) const {

  ATH_MSG_DEBUG( "Obtaining DNN result" );

  /// Create asg::AcceptData object
  asg::AcceptData acceptData( &m_acceptInfo );

  /// Reset the AcceptData cut results
  ATH_CHECK( resetCuts( acceptData ) );

  /// Check basic kinematic selection
  ATH_CHECK( checkKinRange( jet, acceptData ) );

  /// Get the relevant attributes of the jet
  /// Mass and pt - note that this will depend on the configuration of the calibration used
  float jet_pt   = jet.pt()/1000.0;
  float jet_mass = jet.m()/1000.0;

  /// Get DNN score for the jet
  float jet_score = getScore(jet);

  /// Evaluate the values of the upper and lower mass bounds and the d2 cut
  float cut_mass_low  = m_funcMassCutLow ->Eval(jet_pt);
  float cut_mass_high = m_funcMassCutHigh->Eval(jet_pt);
  float cut_score     = m_funcScoreCut   ->Eval(jet_pt);

  /// Print cut criteria and jet values
  ATH_MSG_VERBOSE( "Cut values : Mass window = [" << cut_mass_low << "," << cut_mass_high << "], score cut = " << cut_score );
  ATH_MSG_VERBOSE( "Jet values : Mass = " << jet_mass << ", score = " << jet_score );

  /// Get SF weight
  ATH_CHECK( getWeight( jet, jet_score > cut_score, acceptData ) );

  /// Decorate cut information if needed
  ATH_MSG_DEBUG( "Decorating with score" );

  /// Create WriteDecorHandles
  SG::WriteDecorHandle<xAOD::JetContainer, bool> decPassMass(m_decPassMassKey);
  SG::WriteDecorHandle<xAOD::JetContainer, bool> decPassScore(m_decPassScoreKey);
  SG::WriteDecorHandle<xAOD::JetContainer, bool> decTagged(m_decTaggedKey);
  SG::WriteDecorHandle<xAOD::JetContainer, float> decCutMLow(m_decCutMLowKey);
  SG::WriteDecorHandle<xAOD::JetContainer, float> decCutMHigh(m_decCutMHighKey);
  SG::WriteDecorHandle<xAOD::JetContainer, float> decScoreCut(m_decScoreCutKey);
  SG::WriteDecorHandle<xAOD::JetContainer, float> decScoreValue(m_decScoreValueKey);

  /// Decorate values
  decCutMLow(jet) = cut_mass_low;
  decCutMHigh(jet) = cut_mass_high;
  decScoreCut(jet) = cut_score;
  decScoreValue(jet) = jet_score;

  /// Cut summary
  bool passCuts = true;

  /// Set the AcceptData depending on whether it is a W/Z or a top tagger
  if ( m_tagClass == TAGCLASS::WBoson || m_tagClass == TAGCLASS::ZBoson ) {
    ATH_MSG_VERBOSE( "Determining WZ tag return" );
    if ( jet_mass > cut_mass_low ) acceptData.setCutResult( "PassMassLow", true );
    if ( jet_mass < cut_mass_high ) acceptData.setCutResult( "PassMassHigh", true );
    if ( jet_score > cut_score ) acceptData.setCutResult( "PassScore", true );
    decPassMass(jet) = acceptData.getCutResult( "PassMassLow" ) && acceptData.getCutResult( "PassMassHigh" );
    passCuts = passCuts && acceptData.getCutResult( "PassMassLow" ) && acceptData.getCutResult( "PassMassHigh" );
  }
  else if ( m_tagClass == TAGCLASS::TopQuark ) {
    ATH_MSG_VERBOSE( "Determining TopQuark tag return" );
    if ( jet_mass > cut_mass_low ) acceptData.setCutResult( "PassMassLow", true );
    if ( jet_score > cut_score ) acceptData.setCutResult( "PassScore", true );
    decPassMass(jet) = acceptData.getCutResult( "PassMassLow" );
    passCuts = passCuts && acceptData.getCutResult( "PassMassLow" );
  }

  decPassScore(jet) = acceptData.getCutResult( "PassScore" );

  passCuts = passCuts && acceptData.getCutResult( "PassScore" );

  decTagged(jet) = passCuts;

  return StatusCode::SUCCESS;

}

double JSSWTopTaggerDNN::getScore( const xAOD::Jet& jet ) const {

  /// Create input dictionary map<string,double> for argument to lwtnn
  std::map<std::string,double> DNN_inputValues = getJetProperties(jet);

  /// Evaluate the network response
  lwt::ValueMap discriminant = m_lwnn->compute(DNN_inputValues);

  /// Obtain the output associated with the single output node
  double DNNscore = -666.;

  /// Check that input variables are valid
  bool validVars = true;
  SG::ReadDecorHandle<xAOD::JetContainer, float> readTau21WTA(m_readTau21WTAKey);
  if ( readTau21WTA(jet) < 0.0 ) validVars = false;
  if ( m_tagClass == TAGCLASS::TopQuark ) {
    SG::ReadDecorHandle<xAOD::JetContainer, float> readTau32WTA(m_readTau32WTAKey);
    if ( readTau32WTA(jet) < 0.0 ) validVars = false;
  }

  if ( !validVars ) {

    if ( m_nWarnVar++ < m_nWarnMax ) ATH_MSG_WARNING( "One (or more) tagger input variable has an out-of-range value, setting score to -666" );
    else ATH_MSG_DEBUG( "One (or more) tagger input variable has an out-of-range value, setting score to -666" );

    return DNNscore;

  }

  DNNscore = discriminant[m_kerasConfigOutputName];

  return DNNscore;

}

std::map<std::string,double> JSSWTopTaggerDNN::getJetProperties( const xAOD::Jet& jet ) const {

  /// Map to store inputs
  std::map<std::string,double> DNN_inputValues;

  /// Calculate NSubjettiness and ECF ratios
  calculateJSSRatios(jet);

  ATH_MSG_DEBUG( "Loading variables for common DNN tagger" );

  /// Create common read decor handles
  SG::ReadDecorHandle<xAOD::JetContainer, float> readTau21WTA(m_readTau21WTAKey);
  SG::ReadDecorHandle<xAOD::JetContainer, float> readC2(m_readC2Key);
  SG::ReadDecorHandle<xAOD::JetContainer, float> readD2(m_readD2Key);
  SG::ReadDecorHandle<xAOD::JetContainer, float> readSplit12(m_readSplit12Key);

  /// Mass and pT
  /// It is assumed that these are the combined and calibrated mass and pT
  DNN_inputValues["CaloTACombinedMassUncorrelated"] = jet.m();
  DNN_inputValues["JetpTCorrByCombinedMass"] = jet.pt();

  /// Splitting scales
  DNN_inputValues["Split12"] = readSplit12(jet);

  /// Energy Correlation Functions
  DNN_inputValues["C2"] = readC2(jet);
  DNN_inputValues["D2"] = readD2(jet);

  /// Tau21 WTA
  DNN_inputValues["Tau21_wta"] = readTau21WTA(jet);

  if ( m_tagClass == TAGCLASS::WBoson ) {

    ATH_MSG_DEBUG( "Loading variables for W boson tagger" );

    /// Other moments
    DNN_inputValues["FoxWolfram20"] = jet.getAttribute<float>("FoxWolfram2") / jet.getAttribute<float>("FoxWolfram0");
    DNN_inputValues["PlanarFlow"] = jet.getAttribute<float>("PlanarFlow");
    DNN_inputValues["Angularity"] = jet.getAttribute<float>("Angularity");
    DNN_inputValues["Aplanarity"] = jet.getAttribute<float>("Aplanarity");
    DNN_inputValues["ZCut12"] = jet.getAttribute<float>("ZCut12");
    DNN_inputValues["KtDR"] = jet.getAttribute<float>("KtDR");
  
  }
  
  else if ( m_tagClass == TAGCLASS::TopQuark ) {
  
    ATH_MSG_DEBUG("Loading variables for top quark tagger");

    /// Create top quark read decor handles
    SG::ReadDecorHandle<xAOD::JetContainer, float> readTau1WTA(m_readTau1WTAKey);
    SG::ReadDecorHandle<xAOD::JetContainer, float> readTau2WTA(m_readTau2WTAKey);
    SG::ReadDecorHandle<xAOD::JetContainer, float> readTau3WTA(m_readTau3WTAKey);
    SG::ReadDecorHandle<xAOD::JetContainer, float> readTau32WTA(m_readTau32WTAKey);
    SG::ReadDecorHandle<xAOD::JetContainer, float> readSplit23(m_readSplit23Key);
    SG::ReadDecorHandle<xAOD::JetContainer, float> readQw(m_readQwKey);
    SG::ReadDecorHandle<xAOD::JetContainer, float> readE3(m_readE3Key);

    /// Mass and pT again
    DNN_inputValues["m"] = jet.m();
    DNN_inputValues["pt"] = jet.pt();

    /// Additional splitting scales
    DNN_inputValues["Split23"] = readSplit23(jet);

    /// e3 := normalized ECF3/ECF1**3
    DNN_inputValues["e3"] = readE3(jet);

    /// N-subjettiness
    DNN_inputValues["Tau1_wta"] = readTau1WTA(jet);
    DNN_inputValues["Tau2_wta"] = readTau2WTA(jet);
    DNN_inputValues["Tau3_wta"] = readTau3WTA(jet);

    DNN_inputValues["Tau32_wta"] = readTau32WTA(jet);

    /// Qw observable for top tagging
    DNN_inputValues["Qw"] = readQw(jet);
  
  }
  
  else {
    ATH_MSG_ERROR( "Loading variables failed because the tagger type is not supported" );
  }

  return DNN_inputValues;

}

