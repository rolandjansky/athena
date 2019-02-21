/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "BoostedJetTaggers/TopoclusterTopTagger.h"

#include "PathResolver/PathResolver.h"

#include "TEnv.h"
#include "TF1.h"
#include "TSystem.h"

#define APP_NAME "TopoclusterTopTagger"

#define N_CONSTITUENTS 10
#define EMPTY_CONSTITUENT 0.0

TopoclusterTopTagger::TopoclusterTopTagger( const std::string& name ) :
  JSSTaggerBase( name ),
  m_name(name),
  m_APP_NAME(APP_NAME),
  m_lwnn(nullptr),
  m_jetPtMin(200000.),
  m_jetPtMax(300000.),
  m_jetEtaMax(2.0),
  m_dec_mcutL("mcutL"),
  m_dec_mcutH("mcutH"),
  m_dec_scoreCut("scoreCut"),
  m_dec_scoreValue("scoreValue")
  {

    declareProperty( "ConfigFile",   m_configFile="");
    declareProperty( "Decoration",   m_decorationName="XX");
    declareProperty( "DecorateJet",           m_decorate = true);

    declareProperty( "JetPtMin",              m_jetPtMin = 200000.0);
    declareProperty( "JetPtMax",              m_jetPtMax = 3000000.0);
    declareProperty( "JetEtaMax",             m_jetEtaMax = 2.0);

    declareProperty( "TaggerType",    m_tagType="XXX");

    declareProperty( "CalibArea",       m_calibarea = "");
    declareProperty( "CalibAreaKeras",  m_calibarea_keras = "BoostedJetTaggers/TopoclusterTopTagger/Boost2017/");
    declareProperty( "KerasConfigFile", m_kerasConfigFileName="XXX");
    declareProperty( "KerasOutput",     m_kerasConfigOutputName="XXX");

}

TopoclusterTopTagger::~TopoclusterTopTagger() {}

StatusCode TopoclusterTopTagger::initialize(){

  /* Initialize the DNN tagger tool */
  ATH_MSG_INFO( (m_APP_NAME+": Initializing TopoclusterTopTagger tool").c_str() );
  ATH_MSG_INFO( (m_APP_NAME+": Using config file :"+m_configFile).c_str() );

  if( ! m_configFile.empty() ) {
    ATH_MSG_INFO( "Using config file : "<< m_configFile );
    // check for the existence of the configuration file
    std::string configPath;

    configPath = PathResolverFindCalibFile(("BoostedJetTaggers/"+m_calibarea+"/"+m_configFile).c_str());

    /* https://root.cern.ch/root/roottalk/roottalk02/5332.html */
    FileStat_t fStats;
    int fSuccess = gSystem->GetPathInfo(configPath.c_str(), fStats);
    if(fSuccess != 0){
      ATH_MSG_ERROR("Recommendations file could not be found : ");
      return StatusCode::FAILURE;
    }
    else {
      ATH_MSG_DEBUG("Recommendations file was found : "<<configPath);
    }


    TEnv configReader;
    if(configReader.ReadFile( configPath.c_str(), EEnvLevel(0) ) != 0 ) {
      ATH_MSG_ERROR( "Error while reading config file : "<< configPath );
      return StatusCode::FAILURE;
    }

    // get tagger type
    m_tagType = configReader.GetValue("TaggerType" ,"");

    // get the CVMFS calib area where stuff is stored
    // if this is set to "Local" then it will look for the config file in the share space
    m_calibarea_keras = configReader.GetValue("CalibAreaKeras" ,"");

    // get the name/path of the JSON config
    m_kerasConfigFileName = configReader.GetValue("KerasConfigFile" ,"");

    // get the name of the Keras output node
    m_kerasConfigOutputName = configReader.GetValue("KerasOutput" ,"");

    // get the configured cut values
    m_strMassCutLow  = configReader.GetValue("MassCutLow" ,"");
    m_strMassCutHigh = configReader.GetValue("MassCutHigh" ,"");
    m_strScoreCut    = configReader.GetValue("ScoreCut" ,"");

    // get the decoration name
    m_decorationName = configReader.GetValue("DecorationName" ,"");

    // print out the configuration parameters for viewing
    ATH_MSG_INFO( "Configurations Loaded  :");
    ATH_MSG_INFO( "tagType                : "<<m_tagType );
    ATH_MSG_INFO( "calibarea_keras        : "<<m_calibarea_keras );
    ATH_MSG_INFO( "kerasConfigFileName    : "<<m_kerasConfigFileName );
    ATH_MSG_INFO( "kerasConfigOutputName  : "<<m_kerasConfigOutputName );
    ATH_MSG_INFO( "strMassCutLow          : "<<m_strMassCutLow  );
    ATH_MSG_INFO( "strMassCutHigh         : "<<m_strMassCutHigh );
    ATH_MSG_INFO( "strScoreCut            : "<<m_strScoreCut );
    ATH_MSG_INFO( "decorationName         : "<<m_decorationName );

  }
  else { // no config file
    // Assume the cut functions have been set through properties.
    // check they are non empty
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

  // decorators to be used throughout
  ATH_MSG_INFO( "Decorators that will be attached to jet :" );
  std::string dec_name;

  dec_name = m_decorationName+"_Cut_mlow";
  ATH_MSG_INFO( "  "<<dec_name<<" : lower mass cut for tagger choice" );
  m_dec_mcutL = SG::AuxElement::Decorator<float>((m_decorationName+"_Cut_mlow").c_str());
  dec_name = m_decorationName+"_Cut_mhigh";
  ATH_MSG_INFO( "  "<<dec_name<<" : upper mass cut for tagger choice" );
  m_dec_mcutH = SG::AuxElement::Decorator<float> ((m_decorationName+"_Cut_mhigh").c_str());
  dec_name = m_decorationName+"_Cut_score";
  ATH_MSG_INFO( "  "<<dec_name<<" : cut on the MVA score for the given working point" );
  m_dec_scoreCut = SG::AuxElement::Decorator<float>((m_decorationName+"_Cut_score").c_str());
  dec_name = m_decorationName+"_Score";
  ATH_MSG_INFO( "  "<<dec_name<<" : name of the evaluated MVA score" );
  m_dec_scoreValue = SG::AuxElement::Decorator<float>((m_decorationName+"_Score").c_str());

  // transform these strings into functions
  m_funcMassCutLow   = new TF1("strMassCutLow",  m_strMassCutLow.c_str(),  0, 14000);
  m_funcMassCutHigh  = new TF1("strMassCutHigh", m_strMassCutHigh.c_str(), 0, 14000);
  m_funcScoreCut     = new TF1("strDNNCut",      m_strScoreCut.c_str(),      0, 14000);

  ATH_MSG_INFO( "TopoclusterTopTagger tool initialized" );
  ATH_MSG_INFO( "  Mass cut low   : "<< m_strMassCutLow );
  ATH_MSG_INFO( "  Mass cut High  : "<< m_strMassCutHigh );
  ATH_MSG_INFO( "  Score cut low    : "<< m_strScoreCut );

  // if the calibarea is specified to be "Local" then it looks in the same place as the top level configs
  if( m_calibarea_keras.empty() ){
    ATH_MSG_INFO( (m_APP_NAME+": You need to specify where the calibarea is as either being Local or on CVMFS") );
    return StatusCode::FAILURE;
  }
  else if(m_calibarea_keras.compare("Local")==0){
    std::string localCalibArea = "BoostedJetTaggers/TopoclusterTopTagger/";
    ATH_MSG_INFO( (m_APP_NAME+": Using Local calibarea "+localCalibArea ));
    // convert the JSON config file name to the full path
    m_kerasConfigFilePath = PathResolverFindCalibFile(localCalibArea+m_kerasConfigFileName);
  }
  else{
    ATH_MSG_INFO( (m_APP_NAME+": Using CVMFS calibarea") );
    // get the config file from CVMFS
    // necessary because xml files are too large to house on the data space
    m_kerasConfigFilePath = PathResolverFindCalibFile( (m_calibarea_keras+m_kerasConfigFileName).c_str() );
  }

  // read json file for DNN weights
  ATH_MSG_INFO( (m_APP_NAME+": DNN Tagger configured with: "+m_kerasConfigFilePath.c_str() ));

  std::ifstream input_cfg( m_kerasConfigFilePath.c_str() );

  if(input_cfg.is_open()==false){
    ATH_MSG_INFO( (m_APP_NAME+": Error openning config file : "+m_kerasConfigFilePath.c_str() ));
    ATH_MSG_INFO( (m_APP_NAME+": Are you sure that the file exists at this path?" ));
    return StatusCode::FAILURE;
  }

  lwt::JSONConfig cfg = lwt::parse_json( input_cfg );

  ATH_MSG_INFO( (m_APP_NAME+": Keras Network NLayers : "+std::to_string(cfg.layers.size()).c_str() ));

  m_lwnn = std::unique_ptr<lwt::LightweightNeuralNetwork>
              (new lwt::LightweightNeuralNetwork(cfg.inputs, cfg.layers, cfg.outputs) );

  //setting the possible states that the tagger can be left in after the JSSTaggerBase::tag() function is called
  m_accept.addCut( "ValidPtRangeHigh"    , "True if the jet is not too high pT"  );
  m_accept.addCut( "ValidPtRangeLow"     , "True if the jet is not too low pT"   );
  m_accept.addCut( "ValidEtaRange"       , "True if the jet is not too forward"     );
  m_accept.addCut( "ValidJetContent"     , "True if the jet is alright technically (e.g. all attributes necessary for tag)"        );

  m_accept.addCut( "PassMassLow"         , "mJet > mCut"       );
  m_accept.addCut( "PassScore"           , "ScoreJet > ScoreCut"         );

  //loop over and print out the cuts that have been configured
  ATH_MSG_INFO( "After tagging, you will have access to the following cuts as a Root::TAccept : (<NCut>) <cut> : <description>)" );
  showCuts();


    ATH_MSG_INFO( (m_APP_NAME+": DNN Tagger tool initialized").c_str() );

    return StatusCode::SUCCESS;
}

Root::TAccept TopoclusterTopTagger::tag(const xAOD::Jet& jet) const{

  ATH_MSG_DEBUG( ": Obtaining DNN result" );

  //clear all accept values
  m_accept.clear();

  // set the jet validity bits to 1 by default
  m_accept.setCutResult( "ValidPtRangeHigh", true);
  m_accept.setCutResult( "ValidPtRangeLow" , true);
  m_accept.setCutResult( "ValidEtaRange"   , true);
  m_accept.setCutResult( "ValidJetContent" , true);

  // check basic kinematic selection
  if (std::fabs(jet.eta()) > m_jetEtaMax) {
    ATH_MSG_DEBUG("Jet does not pass basic kinematic selection (|eta| < " << m_jetEtaMax << "). Jet eta = " << jet.eta());
    m_accept.setCutResult("ValidEtaRange", false);
    if(m_decorate)
      decorateJet(jet, -1., -1., -1., -666.);
  }
  if (jet.pt() < m_jetPtMin) {
    ATH_MSG_DEBUG("Jet does not pass basic kinematic selection (pT > " << m_jetPtMin << "). Jet pT = " << jet.pt()/1.e3);
    m_accept.setCutResult("ValidPtRangeLow", false);
    if(m_decorate)
      decorateJet(jet, -1., -1., -1., -666.);
  }
  if (jet.pt() > m_jetPtMax) {
    ATH_MSG_DEBUG("Jet does not pass basic kinematic selection (pT < " << m_jetPtMax << "). Jet pT = " << jet.pt()/1.e3);
    m_accept.setCutResult("ValidPtRangeHigh", false);
    if(m_decorate)
      decorateJet(jet, -1., -1., -1., -666.);
  }
  float jet_score = -666.;
  if (!jet.getConstituents().isValid()) {
    ATH_MSG_DEBUG("Jet constituents not available for tagging");
    m_accept.setCutResult("ValidJetContent", false);
    if(m_decorate)
      decorateJet(jet, -1., -1., -1., -666.);
  } else {
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

  // decorate the cut value if needed;
  if(m_decorate){
    ATH_MSG_DEBUG("Decorating with score");
    decorateJet(jet, cut_mass_high, cut_mass_low, cut_score, jet_score);
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

double TopoclusterTopTagger::getScore(const xAOD::Jet& jet) const{

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
void TopoclusterTopTagger::preprocess(std::map<std::string,double> &clusters, const xAOD::Jet jet) const {
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
void TopoclusterTopTagger::decorateJet(const xAOD::Jet& jet, float mcutH, float mcutL, float scoreCut, float scoreValue) const{
    /* decorate jet with attributes */

    m_dec_mcutH(jet)      = mcutH;
    m_dec_mcutL(jet)      = mcutL;
    m_dec_scoreCut(jet)   = scoreCut;
    m_dec_scoreValue(jet) = scoreValue;

}

// RN
std::map<std::string,double> TopoclusterTopTagger::getJetConstituents(const xAOD::Jet& jet) const{
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

std::map<std::string,double> TopoclusterTopTagger::getJetProperties(const xAOD::Jet& jet) const{
    // Update the jet substructure variables for this jet
    std::map<std::string,double> DNN_inputValues;

    // Splitting Scales
    DNN_inputValues["Split12"] = jet.getAttribute<float>("Split12");
    DNN_inputValues["Split23"] = jet.getAttribute<float>("Split23");

    // Energy Correlation Functions
    float ecf1(0.0);
    float ecf2(0.0);
    float ecf3(0.0);
    jet.getAttribute("ECF1", ecf1);
    jet.getAttribute("ECF2", ecf2);
    jet.getAttribute("ECF3", ecf3);
    DNN_inputValues["ECF1"] = ecf1;
    DNN_inputValues["ECF2"] = ecf2;
    DNN_inputValues["ECF3"] = ecf3;
    if (!jet.isAvailable<float>("C2"))
        DNN_inputValues["C2"] = ecf3 * ecf1 / pow(ecf2, 2.0);
    else
        DNN_inputValues["C2"] = jet.getAttribute<float>("C2");
    if (!jet.isAvailable<float>("D2"))
        DNN_inputValues["D2"] = ecf3 * pow(ecf1, 3.0) / pow(ecf2, 3.0);
    else
        DNN_inputValues["D2"] = jet.getAttribute<float>("D2");

    // N-subjettiness
    float tau1wta = jet.getAttribute<float>("Tau1_wta");
    float tau2wta = jet.getAttribute<float>("Tau2_wta");
    float tau3wta = jet.getAttribute<float>("Tau3_wta");
    DNN_inputValues["Tau1_wta"] = tau1wta;
    DNN_inputValues["Tau2_wta"] = tau2wta;
    DNN_inputValues["Tau3_wta"] = tau3wta;

    m_undefInput = false;

    if (!jet.isAvailable<float>("Tau21_wta")){
        double tau21wta = tau2wta / tau1wta;
	if(tau21wta!=tau21wta){
	  m_undefInput = true;
	}
        DNN_inputValues["Tau21_wta"] = tau21wta;
    }else{
        DNN_inputValues["Tau21_wta"] = jet.getAttribute<float>("Tau21_wta");
    }

    if (!jet.isAvailable<float>("Tau32_wta")){
        double tau32wta = tau3wta / tau2wta;
	if(tau32wta!=tau32wta){
	  m_undefInput = true;
	}
        DNN_inputValues["Tau32_wta"] = tau32wta;
    }else{
        DNN_inputValues["Tau32_wta"] = jet.getAttribute<float>("Tau32_wta");
    }

    // Qw observable for top tagging
    DNN_inputValues["Qw"] = jet.getAttribute<float>("Qw");

    return DNN_inputValues;
}

StatusCode TopoclusterTopTagger::finalize(){
    // Delete or clear anything
    return StatusCode::SUCCESS;
}
