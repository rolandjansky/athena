/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "BoostedJetTaggers/SmoothedWZTagger.h"

#include "PathResolver/PathResolver.h"

#include "xAODJet/JetContainer.h"

#include "TEnv.h"
#include "TF1.h"
#include "TSystem.h"

#include <stdexcept>

SmoothedWZTagger::SmoothedWZTagger( const std::string& name ) :
  JSSTaggerBase( name ),
  m_dec_mcutL("mcutL"),
  m_dec_mcutH("mcutH"),
  m_dec_d2cut("d2cut"),
  m_dec_ntrkcut("ntrkcut"),
  m_dec_accept("acceptdec")
{

  m_tagType=""; // Replace base class default value

  declareProperty( "JetEtaMax",             m_jetEtaMax = 2.0,      "Eta cut to define fiducial phase space for the tagger");

  declareProperty( "MassCutLowFunc",      m_strMassCutLow="" ,      "Lower mass cut");
  declareProperty( "MassCutHighFunc",     m_strMassCutHigh="" ,     "Higher mass cut");
  declareProperty( "D2CutFunc",           m_strD2Cut="" ,           "Upper cut on D2");

  declareProperty( "NtrkCutFunc",         m_strNtrkCut="",          "Upper cut on Ntrk");

}

SmoothedWZTagger::~SmoothedWZTagger() {}


StatusCode SmoothedWZTagger::initialize(){
  /* Initialize the BDT tagger tool */
  ATH_MSG_INFO( "Initializing SmoothedWZTagger tool" );

  if( ! m_configFile.empty() ) {
    ATH_MSG_INFO( "Using config file : "<< m_configFile );
    // check for the existence of the configuration file
    std::string configPath;

    if ( m_calibArea.compare("Local") == 0 ) {
      configPath = PathResolverFindCalibFile(("$WorkDir_DIR/data/BoostedJetTaggers/"+m_configFile).c_str());      
    } else if ( m_calibArea.find("eos") != std::string::npos) {
      configPath = PathResolverFindCalibFile((m_calibArea+"/"+m_configFile).c_str());
    } else {
      configPath = PathResolverFindCalibFile(("BoostedJetTaggers/"+m_calibArea+"/"+m_configFile).c_str());
    }
  
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

    if( m_wkpt.empty() ){
      m_strMassCutLow=configReader.GetValue("MassCutLow" ,"");
      m_strMassCutHigh=configReader.GetValue("MassCutHigh" ,"");
      m_strD2Cut=configReader.GetValue("D2Cut" ,"");
      m_strNtrkCut=configReader.GetValue("NtrkCut" ,"");
    } else {
      m_strMassCutLow=configReader.GetValue((m_wkpt+".MassCutLow").c_str() ,"");
      m_strMassCutHigh=configReader.GetValue((m_wkpt+".MassCutHigh").c_str() ,"");
      m_strD2Cut=configReader.GetValue((m_wkpt+".D2Cut").c_str() ,"");
      m_strNtrkCut=configReader.GetValue((m_wkpt+".NtrkCut").c_str() ,"");
    }

    // get min and max jet pt
    m_jetPtMin = configReader.GetValue("pTCutLow", 200.0);
    m_jetPtMax = configReader.GetValue("pTCutHigh", 4000.0);

    // get the decoration name
    m_decorationName = configReader.GetValue("DecorationName" , "");

    // get the scale factor configuration
    m_calcSF = configReader.GetValue("CalcSF", false);
    if(m_calcSF){
      m_weightdecorationName = configReader.GetValue("WeightDecorationName", "");
      m_weightFileName = configReader.GetValue("WeightFile", "");
      m_weightHistogramName = configReader.GetValue("WeightHistogramName", "");
      m_efficiencyHistogramName = configReader.GetValue("EfficiencyHistogramName", "");
      m_weightFlavors = configReader.GetValue("WeightFlavors", "");
    
      // get truth label name information
      m_truthLabelName = configReader.GetValue("TruthLabelName" , "R10TruthLabel_R21Consolidated");

      if ( m_calibArea.compare("Local") == 0 ){
	m_weightConfigPath = PathResolverFindCalibFile(("$WorkDir_DIR/data/BoostedJetTaggers/SmoothedWZTaggers/"+m_weightFileName).c_str());      
      } else if ( m_calibArea.find("eos") != std::string::npos) {
        m_weightConfigPath = PathResolverFindCalibFile((m_calibArea+"/"+m_weightFileName).c_str());
      } else {
        m_weightConfigPath = PathResolverFindCalibFile(("BoostedJetTaggers/"+m_calibArea+"/"+m_weightFileName).c_str());
      }
    }
    
  }
  else { // no config file
    // Assume the cut functions have been set through properties.
    // check they are non empty
    if( m_strD2Cut.empty() || m_strMassCutLow.empty() || m_strMassCutHigh.empty() ||
        ((m_weightdecorationName.empty() ||
          m_weightHistogramName.empty() ||
	  m_weightFlavors.empty()) && m_calcSF) ) {
      ATH_MSG_ERROR( "No config file provided AND no parameters specified." ) ;
      return StatusCode::FAILURE;
    }
  }

  // initialize decorators as decorationName+_decorator
  ATH_MSG_INFO( "Decorators that will be attached to jet :" );
  std::string dec_name;

  dec_name = m_decorationName+"_Cut_mlow";
  ATH_MSG_INFO( "  "<<dec_name<<" : lower mass cut for tagger choice" );
  m_dec_mcutL = SG::AuxElement::Decorator<float>((dec_name).c_str());
  dec_name = m_decorationName+"_Cut_mhigh";
  ATH_MSG_INFO( "  "<<dec_name<<" : upper mass cut for tagger choice" );
  m_dec_mcutH = SG::AuxElement::Decorator<float>((dec_name).c_str());
  dec_name = m_decorationName+"_Cut_D2";
  ATH_MSG_INFO( "  "<<dec_name<<" : D2 cut for tagger choice" );
  m_dec_d2cut = SG::AuxElement::Decorator<float>((dec_name).c_str());
  if(!m_strNtrkCut.empty()){
    dec_name = m_decorationName+"_Cut_Ntrk";
    ATH_MSG_INFO( "  "<<dec_name<<" : Ntrk cut for tagger choice" );
    m_dec_ntrkcut = SG::AuxElement::Decorator<float>((dec_name).c_str());
  }
  if(m_calcSF){
    dec_name = m_decorationName+"_"+m_weightdecorationName;
    ATH_MSG_INFO( "  "<<dec_name<<" : tagging SF" );
    m_dec_weight     = SG::AuxElement::Decorator<float>((dec_name).c_str());
    dec_name = m_decorationName+"_effSF";
    m_dec_effSF      = SG::AuxElement::Decorator<float>((dec_name).c_str());
    dec_name = m_decorationName+"_efficiency";
    m_dec_efficiency = SG::AuxElement::Decorator<float>((dec_name).c_str());
    dec_name = m_decorationName+"_accept";
    m_dec_accept     = SG::AuxElement::Decorator<int>((dec_name).c_str());
    m_acc_truthLabel = std::make_unique< SG::AuxElement::ConstAccessor<int> >((m_truthLabelName).c_str());
  }

  // transform these strings into functions
  m_funcMassCutLow   = new TF1("strMassCutLow",  m_strMassCutLow.c_str(),  0, 14000);
  m_funcMassCutHigh  = new TF1("strMassCutHigh", m_strMassCutHigh.c_str(), 0, 14000);
  m_funcD2Cut        = new TF1("strD2Cut",       m_strD2Cut.c_str(),       0, 14000);
  if(!m_strNtrkCut.empty())
    m_funcNtrkCut      = new TF1("strNtrkCut",     m_strNtrkCut.c_str(),     0, 14000);

  ATH_MSG_INFO( "Smoothed WZ Tagger tool initialized" );
  ATH_MSG_INFO( "  Mass cut low    : "<< m_strMassCutLow );
  ATH_MSG_INFO( "  Mass cut High   : "<< m_strMassCutHigh );
  ATH_MSG_INFO( "  D2 cut low      : "<< m_strD2Cut );
  if(!m_strNtrkCut.empty())
    ATH_MSG_INFO( "  Ntrk cut low    : "<< m_strNtrkCut );
  ATH_MSG_INFO( "  Decorate        : "<< m_decorate );
  ATH_MSG_INFO( "  DecorationName  : "<< m_decorationName );
  if(m_calcSF){
    ATH_MSG_INFO( "weightdecorationName  : "<<m_weightdecorationName );
    ATH_MSG_INFO( "weightFile            : "<<m_weightFileName );
    ATH_MSG_INFO( "weightHistogramName   : "<<m_weightHistogramName );
    ATH_MSG_INFO( "efficiencyHistogramName : "<<m_efficiencyHistogramName );
    ATH_MSG_INFO( "weightFlavors         : "<<m_weightFlavors );
    ATH_MSG_INFO( "TruthLabelName        : "<<m_truthLabelName );
  }
  ATH_MSG_INFO( "  Pt cut low      : "<< m_jetPtMin );
  ATH_MSG_INFO( "  Pt cut high     : "<< m_jetPtMax );

  //setting the possible states that the tagger can be left in after the JSSTaggerBase::tag() function is called
  m_accept.addCut( "ValidPtRangeHigh"    , "True if the jet is not too high pT"  );
  m_accept.addCut( "ValidPtRangeLow"     , "True if the jet is not too low pT"   );
  m_accept.addCut( "ValidEtaRange"       , "True if the jet is not too forward"     );
  m_accept.addCut( "ValidJetContent"     , "True if the jet is alright technicall (e.g. all attributes necessary for tag)"        );
  m_accept.addCut( "PassMassLow"         , "mJet > mCutLow"       );
  m_accept.addCut( "PassMassHigh"        , "mJet < mCutHigh"      );
  m_accept.addCut( "PassD2"              , "D2Jet < D2Cut"            );
  if(!m_strNtrkCut.empty()){
    m_accept.addCut( "ValidEventContent" , "True if primary vertex was found" );
    m_accept.addCut( "PassNtrk"          , "NtrkJet < NtrkCut"    );
  }

  //loop over and print out the cuts that have been configured
  ATH_MSG_INFO( "After tagging, you will have access to the following cuts as a Root::TAccept : (<NCut>) <cut> : <description>)" );
  showCuts();

  // setup scale factors
  if(m_calcSF){
    std::unique_ptr<TFile> weightConfig(TFile::Open( m_weightConfigPath.c_str()));
    m_weightConfig = std::move(weightConfig);
    if( !m_weightConfig ) {
      ATH_MSG_INFO( ("SmoothedWZTagger: Error openning config file : "+m_weightConfigPath ) );
      return StatusCode::FAILURE;
    }

    // install histograms for tagging SF
    std::stringstream ss{m_weightFlavors};
    std::string flavor;
    while(std::getline(ss, flavor, ',')){
      m_weightHistograms.insert( std::make_pair( flavor, (TH2D*)m_weightConfig->Get((m_weightHistogramName+"_"+flavor).c_str()) ) );
      if ( !m_efficiencyHistogramName.empty() ){
	m_efficiencyHistograms.insert( std::make_pair( flavor, (TH2D*)m_weightConfig->Get((m_efficiencyHistogramName+"_"+flavor).c_str()) ) );
      }
      ATH_MSG_INFO( ("Tagging SF histogram for "+flavor+" is installed."));
    }

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
} // end initialize()



Root::TAccept& SmoothedWZTagger::tag(const xAOD::Jet& jet) const {

  ATH_MSG_DEBUG( ": Obtaining Smooth WZ result" );

  //reset the TAccept cut results to false
  m_accept.clear();

  // set the jet validity bits to 1 by default
  m_accept.setCutResult( "ValidPtRangeHigh", true);
  m_accept.setCutResult( "ValidPtRangeLow" , true);
  m_accept.setCutResult( "ValidEtaRange"   , true);
  m_accept.setCutResult( "ValidJetContent" , true);

  // counter for pt range warnings
  const static int maxNWarn = 10;
  static int nWarn = 0;

  // check basic kinematic selection
  if (std::fabs(jet.eta()) > m_jetEtaMax) {
    ATH_MSG_DEBUG("Jet does not pass basic kinematic selection (|eta| < " << m_jetEtaMax << "). Jet eta = " << jet.eta());
    m_accept.setCutResult("ValidEtaRange", false);
  }
  if (jet.pt()/1.e3 < m_jetPtMin) {
    ATH_MSG_DEBUG("Jet does not pass basic kinematic selection (pT > " << m_jetPtMin << "). Jet pT = " << jet.pt()/1.e3);
    m_accept.setCutResult("ValidPtRangeLow", false);
  }
  if (jet.pt()/1.e3 > m_jetPtMax) {
    if(nWarn++ < maxNWarn) ATH_MSG_WARNING("Jet does not pass basic kinematic selection (pT < " << m_jetPtMax << "). Jet pT = " << jet.pt()/1.e3);
    else ATH_MSG_DEBUG("Jet does not pass basic kinematic selection (pT < " << m_jetPtMax << "). Jet pT = " << jet.pt()/1.e3);
    m_accept.setCutResult("ValidPtRangeHigh", false);
  }


  // get the relevant attributes of the jet
  // mass and pt - note that this will depend on the configuration of the calibration used
  float jet_pt   = jet.pt()/1000.0;
  float jet_mass = jet.m()/1000.0;

  // d2 - check if its there, otherwise, write it out
  float jet_d2 = 0;       // default d2 to 0.  This probably gets used when the jet has one constituent, so it will fail the mass cut anyways

  static SG::AuxElement::ConstAccessor<float>    acc_D2   ("D2");
  static SG::AuxElement::ConstAccessor<float>    acc_ECF1 ("ECF1");
  static SG::AuxElement::ConstAccessor<float>    acc_ECF2 ("ECF2");
  static SG::AuxElement::ConstAccessor<float>    acc_ECF3 ("ECF3");

  if(acc_D2.isAvailable(jet)){
    jet_d2 = acc_D2(jet);
    m_accept.setCutResult("ValidJetContent", true);
  }
  else {
    if((acc_ECF1.isAvailable(jet) && acc_ECF2.isAvailable(jet) && acc_ECF3.isAvailable(jet))){
      ATH_MSG_VERBOSE(": D2 is not available in the file your using, neither are the ECF# variables" );
      m_accept.setCutResult("ValidJetContent", true);
      if( acc_ECF2(jet)>0.0 ){
        jet_d2 = acc_ECF3(jet) * pow(acc_ECF1(jet), 3.0) / pow(acc_ECF2(jet), 3.0);
      }
    }
  }

  // evaluate the values of the upper and lower mass bounds and the d2 cut
  float cut_mass_low  = m_funcMassCutLow ->Eval(jet_pt);
  float cut_mass_high = m_funcMassCutHigh->Eval(jet_pt);
  float cut_d2        = m_funcD2Cut      ->Eval(jet_pt);

  // decorate the cut value if needed;
  if(m_decorate){
    m_dec_d2cut(jet) = cut_d2;
    m_dec_mcutH(jet) = cut_mass_high;
    m_dec_mcutL(jet) = cut_mass_low;
  }

  // evaluate the cut criteria on mass and d2
  ATH_MSG_VERBOSE(": CutsValues : MassWindow=["<<std::to_string(cut_mass_low)<<","<<std::to_string(cut_mass_high)<<"]  ,  D2Cut="<<std::to_string(cut_d2) );
  ATH_MSG_VERBOSE(": CutsValues : JetMass="<<std::to_string(jet_mass)<<"  ,  D2="<<std::to_string(jet_d2) );

  if(jet_mass>=cut_mass_low)
    m_accept.setCutResult("PassMassLow",true);

  if(jet_mass<=cut_mass_high)
    m_accept.setCutResult("PassMassHigh",true);

  if(jet_d2<cut_d2)
    m_accept.setCutResult("PassD2",true);

  // Check if it's a smooth three-variable tagger (ntrk) 
  if(!m_strNtrkCut.empty()){

    m_accept.setCutResult( "ValidEventContent" , true);

    float cut_ntrk        = m_funcNtrkCut->Eval(jet_pt);

    if(m_decorate)
      m_dec_ntrkcut(jet) = cut_ntrk;

    //Get the primary vertex
    bool isValid = false;
    const xAOD::Vertex* primaryVertex = 0;

    const xAOD::VertexContainer* vxCont = 0;
    if(evtStore()->retrieve( vxCont, "PrimaryVertices" ).isFailure()){
      ATH_MSG_WARNING("Unable to retrieve primary vertex container PrimaryVertices");
      isValid = false;
    }
    else{
      for(const auto& vx : *vxCont){
        if(vx->vertexType()==xAOD::VxType::PriVtx){
          primaryVertex = vx;
	  break;
        }
      }

      if(primaryVertex)
        isValid = true;
    }

    if(isValid){
      static SG::AuxElement::Accessor<ElementLink<xAOD::JetContainer> > ungroomedLink("Parent");
      const xAOD::Jet * ungroomedJet = 0;

      if(ungroomedLink.isAvailable(jet)){
        ElementLink<xAOD::JetContainer> linkToUngroomed = ungroomedLink(jet);
        if (  linkToUngroomed.isValid() ){
          ungroomedJet = *linkToUngroomed;

	  static SG::AuxElement::ConstAccessor< std::vector<int> >    acc_Ntrk   ("NumTrkPt500");

	  if(acc_Ntrk.isAvailable(*ungroomedJet)){

	    const std::vector<int> NTrkPt500 = acc_Ntrk(*ungroomedJet);
	    m_accept.setCutResult("ValidJetContent", true);
	    
	    int jet_ntrk = NTrkPt500.at(primaryVertex->index());
	    jet.auxdecor<int>("ParentJetNTrkPt500") = jet_ntrk;

	    if(jet_ntrk < cut_ntrk)
	      m_accept.setCutResult("PassNtrk",true);
	  }
	  else {
	    // Note: throwing an exception here because we can't return StatusCode::FAILURE or similar
            // This error message should only occur if analyses are not using smart slimming in their derivations
            throw std::runtime_error("ERROR: Unable to retrieve Ntrk of the ungroomed parent jet. Please make sure this variable is in your derivations!!!");
	  }
        }
        else{
	  // Note: throwing an exception here because we can't return StatusCode::FAILURE or similar
	  // This error message should only occur if analyses are not using smart slimming in their derivations
	  throw std::runtime_error("ERROR: Unable to retrieve the parent ungroomed jet. Please make sure this variable is in your derivations!!!");
        }
      }
      else{
	// Note: throwing an exception here because we can't return StatusCode::FAILURE or similar
	// This error message should only occur if analyses are not using smart slimming in their derivations
	throw std::runtime_error("ERROR: Unable to retrieve the link to the parent ungroomed jet. Please make sure this variable is in your derivations!!!");
      }
    }
    else{
      m_accept.setCutResult("ValidEventContent", false);
    }
  }

  // decorate truth label for SF provider
  TagResult::TypeEnum myCutResultForSF=TagResult::UNKNOWN;
  if(m_strNtrkCut.empty()){
    // 2Var tagger
    if(m_accept.getCutResult("PassMassLow") && m_accept.getCutResult("PassMassHigh") ){
      if(m_accept.getCutResult("PassD2")){
	myCutResultForSF=TagResult::passMpassD2_2Var;
      }else{
	myCutResultForSF=TagResult::passMfailD2_2Var;
      }
    }else{
      if(m_accept.getCutResult("PassD2")){
	myCutResultForSF=TagResult::failMpassD2_2Var;
      }else{
	myCutResultForSF=TagResult::failMfailD2_2Var;
      }
    }
  }

  if ( m_calcSF && (!m_acc_truthLabel->isAvailable(jet) || LargeRJetTruthLabel::intToEnum((*m_acc_truthLabel)(jet))==LargeRJetTruthLabel::UNKNOWN) ){
    if ( m_IsMC ){
      if (m_JetTruthLabelingTool->modifyJet(jet) == StatusCode::FAILURE){
        ATH_MSG_FATAL("Failed to decorate jet truth label. Please check truth container names");
      }
    }
  }

  if ( m_calcSF && m_decorate ){
    float effSF=1.0;
    float efficiency=1.0;
    if ( m_IsMC ){
      std::tie(effSF, efficiency) = getWeight(jet);
      m_dec_accept(jet) = myCutResultForSF;
      if ( m_weightFlavors.find("fail") != std::string::npos ) {
	// inefficiency SF is automatically decorated.
	m_dec_weight(jet) = effSF;
      } else {
	if ( m_accept ) {
	  // calculate efficiency SF
	  m_dec_weight(jet) = effSF;
	} else {
	  // calculate inefficiency SF
	  if ( m_efficiencyHistogramName.empty() ){
	    // If inefficiency SF is not available, SF is always 1.0
	    m_dec_weight(jet) = 1.0;
	  } else if ( efficiency < 1.0 ) {
	    m_dec_weight(jet) = (1. - effSF*efficiency)/(1. - efficiency);
	  } else {
	    m_dec_weight(jet) = 1.0;
	  }
	}
      }
    }else{
      m_dec_weight(jet) = 1.0;
      m_dec_accept(jet) = myCutResultForSF;
    }
  }


  // return the TAccept to be queried later
  return m_accept;

}

std::pair<double, double> SmoothedWZTagger::getWeight(const xAOD::Jet& jet) const {
    if ( jet.pt()*0.001 < m_jetPtMin ||
	 jet.pt()*0.001 > m_jetPtMax ||
	 fabs(jet.eta())>m_jetEtaMax ) return std::make_pair( 1.0, 1.0 );

    std::string truthLabelStr;
    LargeRJetTruthLabel::TypeEnum jetContainment=LargeRJetTruthLabel::intToEnum((*m_acc_truthLabel)(jet));
    if( m_weightHistograms.count("t_qqb") ) {
      // full-contained top tagger

      if( jetContainment==LargeRJetTruthLabel::tqqb ){
	truthLabelStr="t_qqb";
      }else if( jetContainment==LargeRJetTruthLabel::notruth || jetContainment==LargeRJetTruthLabel::qcd ) {
	truthLabelStr="q";
      }
    }else if( m_weightHistograms.count("V_qq_passMpassD2") ){
      // TCC W/Z 2var tagger

      if( jetContainment==LargeRJetTruthLabel::tqqb || jetContainment==LargeRJetTruthLabel::other_From_t ){
	truthLabelStr="t_";
      }else if( jetContainment==LargeRJetTruthLabel::Wqq || jetContainment==LargeRJetTruthLabel::Zqq || jetContainment==LargeRJetTruthLabel::Wqq_From_t ){
	truthLabelStr="V_qq_";
      }else if( jetContainment==LargeRJetTruthLabel::notruth || jetContainment==LargeRJetTruthLabel::qcd ) {
	truthLabelStr="q_";
      }

      if( m_accept.getCutResult("PassMassLow") && m_accept.getCutResult("PassMassHigh") && m_accept.getCutResult("PassD2") ){
	truthLabelStr+="passMpassD2";
      }else if( !(m_accept.getCutResult("PassMassLow") && m_accept.getCutResult("PassMassHigh")) && m_accept.getCutResult("PassD2") ){
	truthLabelStr+="failMpassD2";
      }else if( m_accept.getCutResult("PassMassLow") && m_accept.getCutResult("PassMassHigh") && !m_accept.getCutResult("PassD2") ){
	truthLabelStr+="passMfailD2";
      }else{
	truthLabelStr+="failMfailD2";
      }

    }else if( m_weightHistograms.count("V_qq") ){
      // W/Z tagger or inclusive top tagger

      if( jetContainment==LargeRJetTruthLabel::tqqb || jetContainment==LargeRJetTruthLabel::other_From_t ){
	truthLabelStr="t";
      }else if( jetContainment==LargeRJetTruthLabel::Wqq || jetContainment==LargeRJetTruthLabel::Zqq || jetContainment==LargeRJetTruthLabel::Wqq_From_t ){
	truthLabelStr="V_qq";
      }else if( jetContainment==LargeRJetTruthLabel::notruth || jetContainment==LargeRJetTruthLabel::qcd ) {
	truthLabelStr="q";
      }
    }

    double logmOverPt=log(jet.m()/jet.pt());
    if ( m_decorationName.find("SmoothZ") != std::string::npos ) {
      // to apply W-tagging efficiency SF to Z-tagger, jet mass is shifted by 10GeV
      const double WtoZmassShift = 10803;
      logmOverPt=log((jet.m()-WtoZmassShift)/jet.pt());
    }
    if ( logmOverPt > 0 ) logmOverPt=0;
    double SF=1.0;
    double eff=1.0;
    if( m_weightHistograms.count(truthLabelStr.c_str()) ){
      int pt_mPt_bin=(m_weightHistograms.find(truthLabelStr.c_str())->second)->FindBin(jet.pt()*0.001, logmOverPt);
      SF=(m_weightHistograms.find(truthLabelStr.c_str())->second)->GetBinContent(pt_mPt_bin);
      if ( !m_efficiencyHistogramName.empty() ){
	eff=(m_efficiencyHistograms.find(truthLabelStr.c_str())->second)->GetBinContent(pt_mPt_bin);
      }
    } else {
      ATH_MSG_DEBUG("SF for truth label for "+truthLabelStr+" is not available. Just return 1.0");
      return std::make_pair( 1.0, 1.0 );      
    }
    if ( SF < 1e-3 ) {
      ATH_MSG_DEBUG("(pt, m/pt) is out of range for SF calculation. Just return 1.0");
      return std::make_pair( 1.0, 1.0 );
    } else return std::make_pair( SF, eff );
}

StatusCode SmoothedWZTagger::finalize(){
  /* Delete or clear anything */
  return StatusCode::SUCCESS;
}


// the end
