/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "BoostedJetTaggers/TopoclusterTopTagger.h"

#include <fstream>

#define N_CONSTITUENTS 10
#define EMPTY_CONSTITUENT 0.0

TopoclusterTopTagger::TopoclusterTopTagger( const std::string& name ) :
  JSSTaggerBase( name ),
  m_lwnn(nullptr)
{

  declareProperty( "JetPtMin", m_jetPtMin = 200.0e3);
  declareProperty( "JetPtMax", m_jetPtMax = 3000.0e3);

}

StatusCode TopoclusterTopTagger::initialize() {

  ATH_MSG_INFO( "Initializing TopoclusterTopTagger tool" );

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

    /// Get the decoration name
    m_decorationName = m_configReader.GetValue("DecorationName" ,"");

    /// Print out the configuration parameters for viewing
    ATH_MSG_INFO( "Configurations Loaded  :");
    ATH_MSG_INFO( "tagType                : "<<m_tagType );
    ATH_MSG_INFO( "kerasCalibArea        : "<<m_kerasCalibArea );
    ATH_MSG_INFO( "kerasConfigFileName    : "<<m_kerasConfigFileName );
    ATH_MSG_INFO( "kerasConfigOutputName  : "<<m_kerasConfigOutputName );
    ATH_MSG_INFO( "strMassCutLow          : "<<m_strMassCutLow  );
    ATH_MSG_INFO( "strMassCutHigh         : "<<m_strMassCutHigh );
    ATH_MSG_INFO( "strScoreCut            : "<<m_strScoreCut );
    ATH_MSG_INFO( "decorationName         : "<<m_decorationName );

  }
  else { /// No config file
    /// Assume the cut functions have been set through properties.
    /// Check they are non empty
    if( m_kerasConfigFileName.empty() ||
        m_kerasConfigOutputName.empty() ||
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
  ATH_MSG_INFO( "Score cut low    : "<< m_strScoreCut );

  // if the calibarea is specified to be "Local" then it looks in the same place as the top level configs
  if( m_kerasCalibArea.empty() ){
    ATH_MSG_ERROR( "You need to specify where the calibarea is as either being Local or on CVMFS" );
    return StatusCode::FAILURE;
  }
  else if(m_kerasCalibArea.compare("Local")==0){
    std::string localCalibArea = "BoostedJetTaggers/TopoclusterTopTagger/";
    ATH_MSG_INFO( "Using Local calibarea " << localCalibArea );
    // convert the JSON config file name to the full path
    m_kerasConfigFilePath = PathResolverFindCalibFile(localCalibArea+m_kerasConfigFileName);
  }
  else{
    ATH_MSG_INFO( "Using CVMFS calibarea" );
    // get the config file from CVMFS
    // necessary because xml files are too large to house on the data space
    m_kerasConfigFilePath = PathResolverFindCalibFile( (m_kerasCalibArea+m_kerasConfigFileName).c_str() );
  }

  // read json file for DNN weights
  ATH_MSG_INFO( "DNN Tagger configured with: " << m_kerasConfigFilePath );

  std::ifstream input_cfg( m_kerasConfigFilePath.c_str() );

  if(input_cfg.is_open()==false){
    ATH_MSG_ERROR( "Error openning config file: " << m_kerasConfigFilePath );
    ATH_MSG_ERROR( "Are you sure that the file exists at this path?" );
    return StatusCode::FAILURE;
  }

  lwt::JSONConfig cfg = lwt::parse_json( input_cfg );

  ATH_MSG_INFO( "Keras Network NLayers: " << cfg.layers.size() );

  m_lwnn = std::make_unique<lwt::LightweightNeuralNetwork>(cfg.inputs, cfg.layers, cfg.outputs);

  /// Setting the possible states that the tagger can be left in after the JSSTaggerBase::tag() function is called
  m_accept.addCut( "PassMassLow" , "mJet > mCut" );
  m_accept.addCut( "PassScore" , "ScoreJet > ScoreCut" );

  /// Call base class initialize
  ATH_CHECK( JSSTaggerBase::initialize() );

  /// Loop over and print out the cuts that have been configured
  printCuts();
  
  ATH_MSG_INFO( "TopoclusterTopTagger tool initialized" );

  return StatusCode::SUCCESS;

}

Root::TAccept& TopoclusterTopTagger::tag( const xAOD::Jet& jet ) const {

  ATH_MSG_DEBUG( "Obtaining topoDNN top result" );

  /// Reset the TAccept cut results
  resetCuts();

  /// Check basic kinematic selection
  checkKinRange(jet);

  bool validJet = true;

  if ( m_decorate && !passKinRange(jet) ) validJet = false;

  float jet_score = -666.;
  if (!jet.getConstituents().isValid()) {
    ATH_MSG_DEBUG("Jet constituents not available for tagging");
    m_accept.setCutResult("ValidJetContent", false);
    validJet = false;
  }
  else {
    jet_score = getScore(jet);
  }

  // get the relevant attributes of the jet
  // mass and pt - note that this will depend on the configuration of the calibration used
  float jet_pt   = jet.pt()/1000.0;
  float jet_mass = jet.m()/1000.0;

  // evaluate the values of the upper and lower mass bounds and the d2 cut
  float cut_mass_low  = m_funcMassCutLow ->Eval(jet_pt);
  float cut_mass_high = m_funcMassCutHigh->Eval(jet_pt);
  float cut_score     = m_funcScoreCut   ->Eval(jet_pt);

  if ( !validJet ) {
    cut_mass_low = -1.;
    cut_mass_high = -1.;
    cut_score = -1.;
    jet_score = -666.;
  }

  // decorate the cut value if needed;
  if(m_decorate){
    ATH_MSG_DEBUG("Decorating with score");
    (*m_dec_mcutH)(jet)      = cut_mass_high;
    (*m_dec_mcutL)(jet)      = cut_mass_low;
    (*m_dec_scoreCut)(jet)   = cut_score;
    (*m_dec_scoreValue)(jet) = jet_score;
  }

  // evaluate the cut criteria on mass and score
  ATH_MSG_VERBOSE(": CutsValues : MassWindow=["<<std::to_string(cut_mass_low)<<","<<std::to_string(cut_mass_high)<<"]  ,  scoreCut="<<std::to_string(cut_score) );
  ATH_MSG_VERBOSE(": JetValues  : JetMass="<<std::to_string(jet_mass)<<"  ,  score="<<std::to_string(jet_score) );

  //set the TAccept
  if( jet_mass>cut_mass_low )
    m_accept.setCutResult( "PassMassLow"  , true );
  if( jet_score > cut_score )
    m_accept.setCutResult( "PassScore"    , true );

  // return the TAccept you just set
  return m_accept;

}

double TopoclusterTopTagger::getScore( const xAOD::Jet& jet ) const {

    // create input dictionary map<string,double> for argument to lwtnn
    // Loading constituents from individual jet
    std::map<std::string,double> DNN_inputValues_clusters = getJetConstituents(jet); // RN

    // Preprocessing constituents
    preprocess(DNN_inputValues_clusters, jet);

    // evaluate the network
    lwt::ValueMap discriminant = m_lwnn->compute(DNN_inputValues_clusters);

    // obtain the output associated with the single output node
    double DNNscore(-666.);
    DNNscore = discriminant[m_kerasConfigOutputName];

    return DNNscore;
}

// RN
void TopoclusterTopTagger::preprocess( std::map<std::string,double> &clusters, const xAOD::Jet jet ) const {
    /* Adapted from Jannicke Pearkes */
    // We assume these constituents are sorted by pt

    // Extract jet properties
    double prim_eta = clusters["clust_0_eta"];
    double prim_phi = clusters["clust_0_phi"];

    // Instructions from Jannicke
    //- min max scaling (this is actually has a hard-coded min and max)
    for (int i = 0; i < N_CONSTITUENTS; ++i) {
      clusters["clust_"+std::to_string(i)+"_pt"] = TopoclusterTransform::pt_min_max_scale(clusters["clust_"+std::to_string(i)+"_pt"]);
    }
    // -  shift prim (translation about primary jet constituent)
    for (int i = 0; i < N_CONSTITUENTS; ++i) {
      clusters["clust_"+std::to_string(i)+"_eta"] = TopoclusterTransform::eta_shift(clusters["clust_"+std::to_string(i)+"_eta"], prim_eta);
      clusters["clust_"+std::to_string(i)+"_phi"] = TopoclusterTransform::phi_shift(clusters["clust_"+std::to_string(i)+"_phi"], prim_phi);
    }

    // - rotate
    // //// Code under "Calculating thetas for rotation” and “Rotating”,
    // //// the rotation part is just a python translation of TLorentzVector’s rotate method
    // //// https://root.cern.ch/doc/master/classTLorentzVector.html
    // Calculate axes of rotation
    double theta = 0.0;
    if (jet.getConstituents().size() >= 2){ // need at least 2 constituents
      theta = TopoclusterTransform::calculate_theta_for_rotations(clusters);
    }
    // Perform the rotation // TODO do we rotate if the theta == 0.0 ?
    TopoclusterTransform::rotate_jet(clusters, theta);

    // - flip
    // Code under  elif "flip" in eta_phi_prep_type:
    TopoclusterTransform::flip(clusters);

    return;
}

// RN
std::map<std::string,double> TopoclusterTopTagger::getJetConstituents( const xAOD::Jet& jet ) const {
    // Update the jet constituents for this jet
    std::map<std::string,double> DNN_inputValues;

    // Initialize map with empty values. Truncate at N_CONSTITUENTS
    for (int i = 0; i < N_CONSTITUENTS; ++i)
    {
      DNN_inputValues["clust_"+std::to_string(i)+"_pt"] = EMPTY_CONSTITUENT;
      DNN_inputValues["clust_"+std::to_string(i)+"_eta"] = EMPTY_CONSTITUENT;
      DNN_inputValues["clust_"+std::to_string(i)+"_phi"] = EMPTY_CONSTITUENT;
    }

    // Extract jet constituents from jet
    std::vector<xAOD::JetConstituent> clusters = jet.getConstituents().asSTLVector();
    // Sort them by pt (using lambda function for sorting)
    std::sort(clusters.begin(), clusters.end(),
      [](const xAOD::JetConstituent & a, const xAOD::JetConstituent & b) -> bool
      {
          return a.pt() > b.pt();
      });

    ATH_MSG_DEBUG("Clusters size: " << clusters.size());
    int count = std::min(N_CONSTITUENTS, int(clusters.size()));
    for (int i = 0; i < count; ++i)
    {
      DNN_inputValues["clust_"+std::to_string(i)+"_pt"] = clusters.at(i)->pt() / 1000; // convert to GeV
      DNN_inputValues["clust_"+std::to_string(i)+"_eta"] = clusters.at(i)->eta();
      DNN_inputValues["clust_"+std::to_string(i)+"_phi"] = clusters.at(i)->phi();
    }

    return DNN_inputValues;
}

