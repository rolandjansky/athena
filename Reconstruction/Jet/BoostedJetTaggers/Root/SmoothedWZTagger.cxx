/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "BoostedJetTaggers/SmoothedWZTagger.h"

#include <stdexcept>

SmoothedWZTagger::SmoothedWZTagger( const std::string& name ) :
  JSSTaggerBase( name )
{

  /// Functional forms for cuts
  declareProperty( "MassCutLowFunc",  m_strMassCutLow = "",  "Lower mass cut");
  declareProperty( "MassCutHighFunc", m_strMassCutHigh = "", "Higher mass cut");
  declareProperty( "D2CutFunc",       m_strD2Cut = "",       "Upper cut on D2");
  declareProperty( "NtrkCutFunc",     m_strNtrkCut = "",     "Upper cut on Ntrk");
  declareProperty( "CNNCutFunc",      m_strCNNCut = "",      "Lower cut on CNN");
  declareProperty( "DisCoDNNCutFunc", m_strDisCoCut = "",    "Lower cut on DisCoDNN");
  declareProperty ("DisCoJetHelper",  m_DisCoJetHelper,      "Helper tool to manage ONNX inference of CNN/DNN models");
}

StatusCode SmoothedWZTagger::initialize() {

  ATH_MSG_INFO( "Initializing SmoothedWZTagger tool" );

  /// Pt values are defined in GeV
  m_ptGeV = true;

  /// Use mass cut
  m_useMassCut = true;

  if ( ! m_configFile.empty() ) {

    /// Get configReader
    ATH_CHECK( getConfigReader() );

    // flag to use or not the CNNTagger
    m_LoadCNNTagger = !((std::string)m_configReader.GetValue("CNNTaggerFileName", "")).empty();
    m_UseCNNTagger = !((std::string)m_configReader.GetValue("CNNCut", "")).empty();
    m_UseDisCoTagger = !((std::string)m_configReader.GetValue("DisCoDNNCut", "")).empty();

    if ( m_wkpt.empty() ) {
      m_strMassCutLow = m_configReader.GetValue("MassCutLow", "");
      m_strMassCutHigh = m_configReader.GetValue("MassCutHigh", "");
      m_strD2Cut = m_configReader.GetValue("D2Cut", "");
      m_strNtrkCut = m_configReader.GetValue("NtrkCut", "");
      if(m_UseCNNTagger)
      	m_strCNNCut = m_configReader.GetValue("CNNCut", "");
      else if(m_UseDisCoTagger)
        m_strDisCoCut = m_configReader.GetValue("DisCoDNNCut", "");
    }
    else {
      m_strMassCutLow = m_configReader.GetValue((m_wkpt+".MassCutLow").c_str(), "");
      m_strMassCutHigh = m_configReader.GetValue((m_wkpt+".MassCutHigh").c_str(), "");
      m_strD2Cut = m_configReader.GetValue((m_wkpt+".D2Cut").c_str(), "");
      m_strNtrkCut = m_configReader.GetValue((m_wkpt+".NtrkCut").c_str(), "");
      if(m_UseCNNTagger)
	      m_strCNNCut = m_configReader.GetValue((m_wkpt+".CNNCut").c_str(), "");
      else if(m_UseDisCoTagger)
	      m_strDisCoCut = m_configReader.GetValue((m_wkpt+".DisCoDNNCut").c_str(), "");
    }

    /// Get min and max jet pt
    m_jetPtMin = m_configReader.GetValue("pTCutLow", 200.0);
    m_jetPtMax = m_configReader.GetValue("pTCutHigh", 4000.0);

    /// Get the decoration name
    m_decorationName = m_configReader.GetValue("DecorationName", "");

    /// Get the scale factor configuration
    m_calcSF = m_configReader.GetValue("CalcSF", false);
    if ( m_calcSF ) {
      m_weightdecorationName = m_configReader.GetValue("WeightDecorationName", "");
      m_weightFileName = m_configReader.GetValue("WeightFile", "");
      m_weightHistogramName = m_configReader.GetValue("WeightHistogramName", "");
      m_efficiencyHistogramName = m_configReader.GetValue("EfficiencyHistogramName", "");
      m_weightFlavors = m_configReader.GetValue("WeightFlavors", "");
    
      /// Get truth label name information
      // un-groomed based truth labelling
      // https://cds.cern.ch/record/2777009/files/ATL-PHYS-PUB-2021-029.pdf
      m_truthLabelName = m_configReader.GetValue("TruthLabelName", "R10TruthLabel_R21Precision_2022v1");

      if ( m_calibArea.compare("Local") == 0 ) {
        m_weightConfigPath = PathResolverFindCalibFile(("$WorkDir_DIR/data/BoostedJetTaggers/SmoothedWZTaggers/"+m_weightFileName).c_str());      
      }
      else if ( m_calibArea.find("eos") != std::string::npos ) {
        m_weightConfigPath = PathResolverFindCalibFile((m_calibArea+"/"+m_weightFileName).c_str());
      }
      else {
        m_weightConfigPath = PathResolverFindCalibFile(("BoostedJetTaggers/"+m_calibArea+"/"+m_weightFileName).c_str());
      }
    }
    
    if((m_LoadCNNTagger || m_UseDisCoTagger) && m_DisCoJetHelper.empty()){
      asg::AsgToolConfig config ("SmoothedWZDisCoJetTagger/DisCoJetHelper");
      ANA_CHECK( config.setProperty("CalibArea", m_calibArea) );
      ANA_CHECK( config.setProperty("ConfigFile", m_configFile) );
      ANA_CHECK( config.makePrivateTool(m_DisCoJetHelper) );
      ANA_CHECK( m_DisCoJetHelper.retrieve() );
    }

  }
  else { /// No config file
    /// Assume the cut functions have been set through properties.
    /// Check they are non empty
    if( m_strD2Cut.empty() || m_strMassCutLow.empty() || m_strMassCutHigh.empty() ||
        ((m_weightdecorationName.empty() ||
          m_weightHistogramName.empty() ||
	  m_weightFlavors.empty()) && m_calcSF) ) {
      ATH_MSG_ERROR( "No config file provided AND no parameters specified." ) ;
      return StatusCode::FAILURE;
    }
  }

  /// Set flag to indicate if Ntrk cut is used
  m_useNtrk = !m_strNtrkCut.empty();

  /// Transform these strings into functions
  if ( m_UseCNNTagger )  m_funcCNNCut = std::make_unique<TF1>("strCNNCut", m_strCNNCut.c_str(), 0, 14000);
  else if ( m_UseDisCoTagger )  m_funcDisCoCut = std::make_unique<TF1>("strDisCoDNNCut", m_strDisCoCut.c_str(), 0, 14000);
  else             m_funcD2Cut = std::make_unique<TF1>("strD2Cut", m_strD2Cut.c_str(), 0, 14000);
  if ( m_useNtrk ) m_funcNtrkCut = std::make_unique<TF1>("strNtrkCut", m_strNtrkCut.c_str(), 0, 14000);

  ATH_MSG_INFO( "Smoothed WZ Tagger tool initialized" );
  ATH_MSG_INFO( "  Mass cut low      : " << m_strMassCutLow );
  ATH_MSG_INFO( "  Mass cut High     : " << m_strMassCutHigh );
  if( m_UseCNNTagger ) 
    ATH_MSG_INFO( "  CNN cut low       : " << m_strCNNCut );
  else if( m_UseDisCoTagger ) 
    ATH_MSG_INFO( "  DisCoDNN cut low       : " << m_strDisCoCut );
  else
    ATH_MSG_INFO( "  D2 cut low        : " << m_strD2Cut );
  if ( m_useNtrk )
    ATH_MSG_INFO( "  Ntrk cut low      : " << m_strNtrkCut );
  ATH_MSG_INFO( "  Decorate          : " << m_decorate );
  ATH_MSG_INFO( "  DecorationName    : " << m_decorationName );
  if ( m_calcSF ) {
    ATH_MSG_INFO( "weightdecorationName    : " << m_weightdecorationName );
    ATH_MSG_INFO( "weightFile              : " << m_weightFileName );
    ATH_MSG_INFO( "weightHistogramName     : " << m_weightHistogramName );
    ATH_MSG_INFO( "efficiencyHistogramName : " << m_efficiencyHistogramName );
    ATH_MSG_INFO( "weightFlavors           : " << m_weightFlavors );
    ATH_MSG_INFO( "TruthLabelName          : " << m_truthLabelName );
  }
  ATH_MSG_INFO( "  Pt cut low        : " << m_jetPtMin );
  ATH_MSG_INFO( "  Pt cut high       : " << m_jetPtMax );

  /// Set the possible states that the tagger can be left in after the JSSTaggerBase::tag() function is called
  m_accept.addCut( "PassMassLow", "mJet > mCutLow" );
  m_accept.addCut( "PassMassHigh", "mJet < mCutHigh" );
  if(m_UseCNNTagger)
    m_accept.addCut( "PassCNN", "CNNJet > CNNCut" );
  else if(m_UseDisCoTagger)
    m_accept.addCut( "PassDisCoDNN", "DisCoDNNJet > DisCoDNNCut" );
  else 
    m_accept.addCut( "PassD2", "D2Jet < D2Cut" );
  if ( m_useNtrk ) {
    m_accept.addCut( "PassNtrk", "NtrkJet < NtrkCut" );
  }

  /// Loop over and print out the cuts that have been configured
  printCuts();

  /// Call base class initialize
  ATH_CHECK( JSSTaggerBase::initialize() );

  /// Initialize additional decorators
  ATH_MSG_INFO( "Additional decorators that will be attached to jet :" );

  if(m_UseCNNTagger){
    ATH_MSG_INFO( "  " << m_decorationName << "_Cut_CNN : CNN cut" );
    m_dec_cnncut = std::make_unique< SG::AuxElement::Decorator<float> >( m_decorationName + "_Cut_CNN" );
    m_dec_cnn = std::make_unique< SG::AuxElement::Decorator<float> >( m_decorationName + "_ConstituentsCNN" );
  }
  else if(m_UseDisCoTagger){
    ATH_MSG_INFO( "  " << m_decorationName << "_Cut_DisCoDNN : DisCoDNN cut" );
    m_dec_cnn = std::make_unique< SG::AuxElement::Decorator<float> >( m_decorationName + "_ConstituentsCNN" );
    m_dec_discocut = std::make_unique< SG::AuxElement::Decorator<float> >( m_decorationName + "_Cut_DisCoDNN" );
    m_dec_disco = std::make_unique< SG::AuxElement::Decorator<float> >( m_decorationName + "_DisCoJet" );
  }
  else{
    ATH_MSG_INFO( "  " << m_decorationName << "_Cut_D2 : D2 cut" );
    m_dec_d2cut = std::make_unique< SG::AuxElement::Decorator<float> >( m_decorationName + "_Cut_D2" );
  }
  if ( m_useNtrk ) {
    ATH_MSG_INFO( "  " << m_decorationName << "_Cut_Ntrk : Ntrk cut" );
    m_dec_ntrkcut = std::make_unique< SG::AuxElement::Decorator<float> >( m_decorationName + "_Cut_Ntrk" );
  }
  
  return StatusCode::SUCCESS;

}

Root::TAccept& SmoothedWZTagger::tag( const xAOD::Jet& jet ) const {

  ATH_MSG_DEBUG( "Obtaining Smooth WZ result" );

  /// Reset the TAccept cut results
  resetCuts();

  /// Check basic kinematic selection
  checkKinRange(jet);

  /// Get the relevant attributes of the jet
  /// Mass and pt - note that this will depend on the configuration of the calibration used
  float jet_pt   = jet.pt()/1000.0;
  float jet_mass = jet.m()/1000.0;

  /// Initialize d2 to 0.  This probably gets used when the jet has one constituent, so it will fail the mass cut anyways
  float jet_d2 = 0;

  /// Calculate NSubjettiness and ECF ratios
  calculateJSSRatios(jet);

  /// Get D2 value
  jet_d2 = acc_D2(jet);

  // Retrieve the CNN score
  float jet_cnn (-99.);
  float jet_disco (-99.);

  if(m_LoadCNNTagger){
    jet_cnn = m_DisCoJetHelper -> GetCNNScore(jet);
    (*m_dec_cnn)(jet) = jet_cnn;
  }
  if(m_UseDisCoTagger){
    jet_disco = m_DisCoJetHelper -> GetDisCoDNNScore(jet);
    (*m_dec_disco)(jet) = jet_disco;
  }

  /// Evaluate the values of the upper and lower mass bounds and the d2 cut
  float cut_mass_low  = m_funcMassCutLow ->Eval(jet_pt);
  float cut_mass_high = m_funcMassCutHigh->Eval(jet_pt);
  float cut_d2 (-99.), cut_cnn (-99.), cut_disco (-99.);
  if(m_UseCNNTagger)        cut_cnn       = m_funcCNNCut     ->Eval(jet_pt);
  else if(m_UseDisCoTagger) cut_disco     = m_funcDisCoCut   ->Eval(jet_pt);
  else                      cut_d2        = m_funcD2Cut      ->Eval(jet_pt);


  /// Decorate the cut value if needed;
  if ( m_decorate ) {
    (*m_dec_mcutH)(jet) = cut_mass_high;
    (*m_dec_mcutL)(jet) = cut_mass_low;
    if(m_UseCNNTagger)        (*m_dec_cnncut)(jet)   = cut_cnn;
    else if(m_UseDisCoTagger) (*m_dec_discocut)(jet) = cut_disco;
    else                      (*m_dec_d2cut)(jet)    = cut_d2;
  }

  /// Evaluate the cut criteria on mass and d2
  if(m_UseCNNTagger){
    ATH_MSG_VERBOSE( "Cut Values : MassWindow = [" << cut_mass_low << "," << cut_mass_high << "], CNNCut = " << cut_cnn );
    ATH_MSG_VERBOSE( "Cut Values : JetMass = " << jet_mass << ", CNN = " << jet_cnn );
    if ( jet_cnn > cut_cnn ) m_accept.setCutResult( "PassCNN", true );
  }
  else if(m_UseDisCoTagger){
    ATH_MSG_VERBOSE( "Cut Values : MassWindow = [" << cut_mass_low << "," << cut_mass_high << "], DisCoDNNCut = " << cut_disco );
    ATH_MSG_VERBOSE( "Cut Values : JetMass = " << jet_mass << ", CNN = " << jet_disco );
    if ( jet_disco > cut_disco ) m_accept.setCutResult( "PassDisCoDNN", true );
  }
  else{
    ATH_MSG_VERBOSE( "Cut Values : MassWindow = [" << cut_mass_low << "," << cut_mass_high << "], D2Cut = " << cut_d2 );
    ATH_MSG_VERBOSE( "Cut Values : JetMass = " << jet_mass << ", D2 = " << jet_d2 );
    if ( jet_d2 < cut_d2 ) m_accept.setCutResult( "PassD2", true );
  }

  if ( jet_mass >= cut_mass_low ) m_accept.setCutResult( "PassMassLow", true );

  if ( jet_mass <= cut_mass_high ) m_accept.setCutResult( "PassMassHigh", true );

  /// Check if it's a smooth three-variable tagger (ntrk)
  if ( m_useNtrk ) {

    float cut_ntrk = m_funcNtrkCut->Eval(jet_pt);

    if ( m_decorate ) (*m_dec_ntrkcut)(jet) = cut_ntrk;

    // decorate the jet with tracks multiplicity from the un-grommed jet
    ANA_CHECK_THROW( GetUnGroomTracks(jet) );
  }

  /// Get enum to decorate m_accept state if only using 2-var tagger
  TagResult::TypeEnum myCutResultForSF = TagResult::UNKNOWN;
  if ( !m_useNtrk ) {
    /// Pass mass cut
    if ( m_accept.getCutResult("PassMassLow") && m_accept.getCutResult("PassMassHigh") ) {
      if ( m_accept.getCutResult("PassD2") ) {
        myCutResultForSF = TagResult::passMpassD2_2Var;
      }
      else {
        myCutResultForSF = TagResult::passMfailD2_2Var;
      }
    }
    /// Fail mass cut
    else {
      if ( m_accept.getCutResult("PassD2") ) {
        myCutResultForSF = TagResult::failMpassD2_2Var;
      }
      else {
        myCutResultForSF = TagResult::failMfailD2_2Var;
      }
    }
  }
  else{
    if ( m_accept ) myCutResultForSF = TagResult::passAll;
    else myCutResultForSF = TagResult::fail;
  }

  /// Decorate truth label for SF provider
  if ( m_calcSF && (!m_acc_truthLabel->isAvailable(jet) || LargeRJetTruthLabel::intToEnum((*m_acc_truthLabel)(jet))==LargeRJetTruthLabel::UNKNOWN) ) {
    if ( m_isMC ) {
      if ( m_JetTruthLabelingTool->modifyJet(jet) == StatusCode::FAILURE ) {
        ATH_MSG_FATAL( "Failed to decorate jet truth label. Please check truth container names" );
      }
    }
  }

  /// Get SF weight
  getWeight( jet, m_accept );

  if ( m_decorate ) {
    if ( m_calcSF ) { 
      (*m_dec_accept)(jet)     = myCutResultForSF;
    }
  }

  /// Return the TAccept to be queried later
  return m_accept;

}
