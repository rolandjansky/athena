/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "BoostedJetTaggers/SmoothedWZDisCoJetTagger.h"

#include "CxxUtils/fpcompare.h"
bool DescendingPtSorterConstituents(const xAOD::JetConstituent p1, const xAOD::JetConstituent p2)
{
  return CxxUtils::fpcompare::greater(p1.pt(), p2.pt());
}

SmoothedWZDisCoJetTagger::SmoothedWZDisCoJetTagger( const std::string& name ) :
  JSSTaggerBase( name )
{
  declareProperty ("MLBosonTagger", m_MLBosonTagger, "Tool to manage the data pre-processing and inference of the CNN model");
  declareProperty ("MLBosonTaggerDisCo", m_MLBosonTagger_DisCo, "Tool to manage the data pre-processing and inference of the DNN-DisCo model");
}

StatusCode SmoothedWZDisCoJetTagger::initialize(){

  ATH_MSG_INFO( "Initializing SmoothedWZDisCoJetTagger tool" );

  if ( ! m_configFile.empty() ) {

    /// Get configReader
    ATH_CHECK( getConfigReader() );

    /// Get the decoration name
    m_decorationName = m_configReader.GetValue("DecorationName", "");

    m_LoadCNNTagger = !((std::string)m_configReader.GetValue("CNNTaggerFileName", "")).empty();
    m_UseCNNTagger = !((std::string)m_configReader.GetValue("CNNCut", "")).empty();
    m_UseDisCoTagger = !((std::string)m_configReader.GetValue("DisCoDNNCut", "")).empty();

    std::string CNNTaggerFileName = m_configReader.GetValue("CNNTaggerFileName", "aaa");
    std::string DisCoTaggerFileName = m_configReader.GetValue("DisCoTaggerFileName", "aaa");

    if(m_LoadCNNTagger && m_MLBosonTagger.empty()){
      // init tool
      std::string m_ModelPath = ("/data/BoostedJetTaggers/SmoothedWZTaggers/"+CNNTaggerFileName);
      ATH_MSG_INFO("SmoothedWZTagger::MLBosonTagger()" << "   + ModelPath " << m_ModelPath );

      asg::AsgToolConfig config ("AthONNX::JSSMLTool/MLBosonTagger");
      ATH_CHECK( config.setProperty("ModelPath", m_ModelPath));
    
      // get model paramters from the config file
      ATH_MSG_INFO("SmoothedWZTagger::MLBosonTagger() read value from config" );

      // set parameters
      ATH_CHECK( config.setProperty("nPixelsEta", (int)m_configReader.GetValue("nPixelsEta", -99)) );
      ATH_CHECK( config.setProperty("MinEtaRange", m_configReader.GetValue("aEta", -99.)) );
      ATH_CHECK( config.setProperty("MaxEtaRange", m_configReader.GetValue("bEta", -99.)) );
      ATH_CHECK( config.setProperty("nPixelsPhi", (int)m_configReader.GetValue("nPixelsPhi", -99)) );
      ATH_CHECK( config.setProperty("MinPhiRange", m_configReader.GetValue("aPhi", -99.)) );
      ATH_CHECK( config.setProperty("MaxPhiRange", m_configReader.GetValue("bPhi", -99.)) );
      ATH_CHECK( config.setProperty("nColors", m_configReader.GetValue("nColors", -99)) );
      ATH_CHECK( config.setProperty("DoRScaling", (bool)m_configReader.GetValue("DoRScaling", -99)) );
      ATH_CHECK( config.setProperty("RScaling_p0", m_configReader.GetValue("RScaling_p0", -99.)) );
      ATH_CHECK( config.setProperty("RScaling_p1", m_configReader.GetValue("RScaling_p1", -99.)) );

      ATH_CHECK( config.makePrivateTool(m_MLBosonTagger) );
      ATH_CHECK( m_MLBosonTagger.retrieve() );

      m_MLBosonTagger -> PrintInfo();

    }
    if(m_UseDisCoTagger){
      // init tool
      std::string m_ModelPath = ("/data/BoostedJetTaggers/SmoothedWZTaggers/"+DisCoTaggerFileName);
      ATH_MSG_INFO("SmoothedWZTagger::MLBosonTagger()" << "   + ModelPath " << m_ModelPath );

      asg::AsgToolConfig config ("AthONNX::JSSMLTool/MLBosonTagger_DisCo");
      ATH_CHECK( config.setProperty("ModelPath", m_ModelPath));
      
      ATH_CHECK( config.makePrivateTool(m_MLBosonTagger_DisCo) );
      ATH_CHECK( m_MLBosonTagger_DisCo.retrieve() );
    
      // get model paramters from the config file
      ATH_MSG_INFO("SmoothedWZTDisCoJetagger::MLBosonTagger() read value from config" );
      ATH_CHECK( ReadScaler() );

    }

  }

  return StatusCode::SUCCESS;

}

Root::TAccept& SmoothedWZDisCoJetTagger::tag( const xAOD::Jet& jet ) const {

  ATH_MSG_DEBUG( "Obtaining Smooth WZ DisCoJet result   " << jet.pt() << "   " << jet.m() );

  return m_accept;

}

float SmoothedWZDisCoJetTagger::GetCNNScore(const xAOD::Jet& jet) const{

  // init value
  float CNNScore (-99.);

  // preliminary actions for constituents
  // add a dedicated function for this?

  // get constituents
  std::vector<xAOD::JetConstituent> constituents = jet.getConstituents().asSTLVector();
  std::sort( constituents.begin(), constituents.end(), DescendingPtSorterConstituents) ;

  int MaxConstituents (100);
  std::vector<xAOD::JetConstituent> constituentsForCNN;

  if( constituents.size() > 100 )
    constituentsForCNN = std::vector<xAOD::JetConstituent> (constituents.begin(), constituents.begin() + MaxConstituents);
  else 
    constituentsForCNN = constituents;

  // constituents - charged
  std::vector<xAOD::JetConstituent> csts_charged = constituentsForCNN;
  csts_charged.erase( std::remove_if( csts_charged.begin(), csts_charged.end(),
				              [] (xAOD::JetConstituent constituent) -> bool {
					              const xAOD::TrackCaloCluster* tcc = dynamic_cast<const xAOD::TrackCaloCluster*>(constituent.rawConstituent());
					              return tcc -> taste()!=0;
				              }), csts_charged.end()) ;

  // constituents - neutral
  std::vector<xAOD::JetConstituent> csts_neutral = constituentsForCNN;
  csts_neutral.erase( std::remove_if( csts_neutral.begin(), csts_neutral.end(),
				              [] (xAOD::JetConstituent constituent) -> bool {
					              const xAOD::TrackCaloCluster* tcc = dynamic_cast<const xAOD::TrackCaloCluster*>(constituent.rawConstituent());
					              return tcc -> taste()!=1;
				              }), csts_neutral.end()) ;

  // constituents - combined
  std::vector<xAOD::JetConstituent> csts_combined = constituentsForCNN;
  csts_combined.erase( std::remove_if( csts_combined.begin(), csts_combined.end(),
				              [] (xAOD::JetConstituent constituent){
					              const xAOD::TrackCaloCluster* tcc = dynamic_cast<const xAOD::TrackCaloCluster*>(constituent.rawConstituent());
					              return tcc -> taste()!=2;
				              }), csts_combined.end()) ;

  // use ML tool on constituents
  m_MLBosonTagger -> MakeJetImage("Charged" , &jet, csts_charged );
  m_MLBosonTagger -> MakeJetImage("Neutral" , &jet, csts_neutral );
  m_MLBosonTagger -> MakeJetImage("Combined", &jet, csts_combined);

  // evaluate the model
  ATH_CHECK( m_MLBosonTagger -> retriveConstituentsScore() );

  // save ML score
  CNNScore = m_MLBosonTagger -> GetCNNScore();

  // reset images and go to next jet
  m_MLBosonTagger -> ResetImages();

  return CNNScore;

}

float SmoothedWZDisCoJetTagger::GetDisCoDNNScore(const xAOD::Jet& jet) const {

  // init value
  float DisCoDNNScore (-99.);
  
  // get input variables
  std::map<std::string, double> JSSVars = GetJSSVars(jet);

  // set input variables for ml model
  m_MLBosonTagger_DisCo -> SetJSSInputs(JSSVars);

  // evaluate the model
  ATH_CHECK( m_MLBosonTagger_DisCo -> retrieveHighLevelScore() );
  
  // save ML score
  DisCoDNNScore = m_MLBosonTagger_DisCo -> GetDNNScore();

  return DisCoDNNScore;

}

std::map<std::string, double> SmoothedWZDisCoJetTagger::GetJSSVars(const xAOD::Jet& jet) const {

  std::map<std::string, double> JSSVars;

  // make available some missing JSS variables
  calculateJSSRatios(jet);
  ANA_CHECK_THROW( GetUnGroomTracks(jet) );

  // store input variables
  JSSVars["pT"] = jet.pt();
  JSSVars["nTracks"] = jet.isAvailable<int>("ParentJetNTrkPt500") ? jet.auxdata<int>("ParentJetNTrkPt500") : -99. ;

  // CNN
  JSSVars["CNN"] = jet.isAvailable<float>(m_decorationName + "_ConstituentsCNN") ? jet.auxdata<float>(m_decorationName + "_ConstituentsCNN") : -99. ;

  // split
  JSSVars["Split12"] = acc_Split12(jet);
  JSSVars["Split23"] = acc_Split23(jet);

  // Energy Correlation Functions
  JSSVars["D2"] = acc_D2(jet);

  // Tau123 WTA
  JSSVars["Tau1_wta"] = acc_Tau1_wta(jet);
  JSSVars["Tau2_wta"] = acc_Tau2_wta(jet);
  JSSVars["Tau3_wta"] = acc_Tau3_wta(jet);

  // ECF
  JSSVars["ECF1"] = acc_ECF1(jet);
  JSSVars["ECF2"] = acc_ECF2(jet);
  JSSVars["ECF3"] = acc_ECF3(jet);

  //
  JSSVars["Qw"] = acc_Qw(jet);

  // Other moments
  JSSVars["FoxWolfram0"] = jet.isAvailable<float>("FoxWolfram0") ? jet.getAttribute<float>("FoxWolfram0") : -99. ;
  JSSVars["FoxWolfram2"] = jet.isAvailable<float>("FoxWolfram2") ? jet.getAttribute<float>("FoxWolfram2") : -99. ;
  JSSVars["PlanarFlow"] = jet.isAvailable<float>("PlanarFlow") ? jet.getAttribute<float>("PlanarFlow") : -99. ;
  JSSVars["Angularity"] = jet.isAvailable<float>("Angularity") ? jet.getAttribute<float>("Angularity") : -99. ;
  JSSVars["Aplanarity"] = jet.isAvailable<float>("Aplanarity") ? jet.getAttribute<float>("Aplanarity") : -99. ;
  JSSVars["ZCut12"] = jet.isAvailable<float>("ZCut12") ? jet.getAttribute<float>("ZCut12") : -99. ;
  JSSVars["KtDR"] = jet.isAvailable<float>("KtDR") ? jet.getAttribute<float>("KtDR") : -99. ;
  
  return JSSVars;

}

StatusCode SmoothedWZDisCoJetTagger::ReadScaler(){

  // input list
  std::vector<std::string> vars_list = {
    "pT","CNN","D2","nTracks","ZCut12",
    "Tau1_wta","Tau2_wta","Tau3_wta",
    "KtDR","Split12","Split23",
    "ECF1","ECF2","ECF3",
    "Angularity","FoxWolfram0","FoxWolfram2",
    "Aplanarity","PlanarFlow","Qw",
  };

  // loop and read
  for(std::string var : vars_list){
    std::string s_mean = var + "_mean";
    std::string s_std  = var + "_std";
    double mean = m_configReader.GetValue(s_mean.c_str(), -99.);
    double std  = m_configReader.GetValue(s_std.c_str() , -99.);

    if(mean==-99. && std==-99.){
      ATH_MSG_ERROR("ERROR: one of the parameter for " << var << " is missing, please, double check the config!!!");
      return StatusCode::FAILURE;
    }
    else if(mean==-99. || std==-99.){
      ATH_MSG_ERROR("ERROR: parameters for " << var << " are missing, please, double check the config!!!");
      return StatusCode::FAILURE;
    }
    else{
      m_scaler[var] = {mean, std};
    }

    // pass the features scaling paramters to the ML tool
    // we need to apply the data pre-processing 
    // before to apply the model inference via ONNX
    ATH_CHECK( m_MLBosonTagger_DisCo -> SetScaler(m_scaler) );

  }

  return StatusCode::SUCCESS;

}
