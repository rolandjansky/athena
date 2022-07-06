#include "xAODBTaggingEfficiency/BTaggingTruthTaggingTool.h"
#include "xAODBTaggingEfficiency/BTaggingEfficiencyTool.h"
#include "xAODBTagging/BTagging.h"
#include "CalibrationDataInterface/CalibrationDataInterfaceROOT.h"
#include "CalibrationDataInterface/CalibrationDataContainer.h"
#include "PATInterfaces/SystematicRegistry.h"
#include "PathResolver/PathResolver.h"

#include "TFile.h"
#include "TMatrixD.h"

#include <algorithm>
#include <numeric>

using std::string;

using CP::CorrectionCode;
using CP::SystematicSet;
using CP::SystematicVariation;
using CP::SystematicCode;
using CP::SystematicRegistry;

using Analysis::Uncertainty;
using Analysis::CalibrationDataVariables;
using Analysis::CalibrationDataContainer;
using Analysis::CalibResult;
using Analysis::CalibrationStatus;
using Analysis::Total;
using Analysis::SFEigen;
using Analysis::SFNamed;
using Analysis::None;

using xAOD::IParticle;
using namespace asg::msgUserCode;

BTaggingTruthTaggingTool::BTaggingTruthTaggingTool( const std::string & name)
  : asg::AsgTool( name ),
    m_effTool ("BTaggingEfficiencyTool/effTool", this),
    m_selTool ("BTaggingSelectionTool/selTool", this)
  {

  m_initialised = false;

  // properties of BTaggingTruthTaggingTool
  declareProperty( "IgnoreScaleFactors", m_ignoreSF=true, "ignore scale factors in computation of TRF weight");
  declareProperty( "UsePermutations", m_usePerm=true, "if the chosen permutation is used, a reweighting is applied to the TRF weight for systematics");
  declareProperty( "UseQuantile", m_useQuntile=true, "if the chosen quantile is used, a reweighting is applied to the TRF weight for systematics");
  declareProperty( "UseSystematics", m_useSys=false, "will the results contain all systematic variations, or just the nominal");
  declareProperty( "MaxNtagged", m_nbtag=2, "what is the maximal possible number of tagged jets");

  // properties of BtaggingSelectionTool
  declareProperty( "MaxEta", m_maxEta = 2.5 );
  declareProperty( "MinPt", m_minPt = 20000 /*MeV*/);
  declareProperty( "MaxRangePt", m_maxRangePt = 1000000 /*MeV*/);

  // properties of BTaggingEfficiencyTool
  declareProperty("TaggerName",                      m_taggerName="MV2c10",            "tagging algorithm name as specified in CDI file");
  declareProperty("OperatingPoint",                  m_OP="FixedCutBEff_77",                    "operating point as specified in CDI file");
  declareProperty("JetAuthor",                       m_jetAuthor="AntiKt4EMTopoJets",             "jet collection & JVF/JVT specification in CDI file");
  declareProperty("ScaleFactorFileName",             m_SFFile = "xAODBTaggingEfficiency/13TeV/2016-20_7-13TeV-MC15-CDI-July12_v1.root",              "name of the official scale factor calibration CDI file (uses PathResolver)");
  declareProperty("UseDevelopmentFile",              m_useDevFile = false,       "specify whether or not to use the (PathResolver) area for temporary scale factor calibration CDI files");
  declareProperty("EfficiencyFileName",              m_EffFile = "",             "name of optional user-provided MC efficiency CDI file");
  declareProperty("ScaleFactorBCalibration",         m_SFBName = "default",      "name of b-jet scale factor calibration object");
  declareProperty("ScaleFactorCCalibration",         m_SFCName = "default",      "name of c-jet scale factor calibration object");
  declareProperty("ScaleFactorTCalibration",         m_SFTName = "default",      "name of tau-jet scale factor calibration object");
  declareProperty("ScaleFactorLightCalibration",     m_SFLightName = "default",  "name of light-flavour jet scale factor calibration object");
  declareProperty("EigenvectorReductionB",           m_EVReductionB = "Loose",   "b-jet scale factor Eigenvector reduction strategy; choose between 'Loose', 'Medium', 'Tight'");
  declareProperty("EigenvectorReductionC",           m_EVReductionC = "Loose",   "c-jet scale factor Eigenvector reduction strategy; choose between 'Loose', 'Medium', 'Tight'");
  declareProperty("EigenvectorReductionLight",       m_EVReductionLight = "Loose","light-flavour jet scale factor Eigenvector reduction strategy; choose between 'Loose', 'Medium', 'Tight'");
  declareProperty("EfficiencyBCalibrations",         m_EffBName = "default",     "(semicolon-separated) name(s) of b-jet efficiency object(s)");
  declareProperty("EfficiencyCCalibrations",         m_EffCName = "default",     "(semicolon-separated) name(s) of c-jet efficiency object(s)");
  declareProperty("EfficiencyTCalibrations",         m_EffTName = "default",     "(semicolon-separated) name(s) of tau-jet efficiency object(s)");
  declareProperty("EfficiencyLightCalibrations",     m_EffLightName = "default", "(semicolon-separated) name(s) of light-flavour-jet efficiency object(s)");
  declareProperty("ExcludeFromEigenVectorTreatment", m_excludeFromEV = "",       "(semicolon-separated) names of uncertainties to be excluded from eigenvector decomposition (if used)");
  declareProperty("SystematicsStrategy",             m_systStrategy = "SFEigen", "name of systematics model; presently choose between 'SFEigen' and 'Envelope'");
  declareProperty("ConeFlavourLabel",                m_coneFlavourLabel = true, "specify whether or not to use the cone-based flavour labelling instead of the default ghost association based labelling");
  declareProperty("OldConeFlavourLabel",             m_oldConeFlavourLabel = false, "when using cone-based flavour labelling, specify whether or not to use the (deprecated) Run-1 legacy labelling");
  declareProperty("CutBenchmark",                    m_cutBenchmark = "1,2", "if you want to run in continuous you need to fix a benchmark - it does something only if running in Continuous OP");
  declareProperty("ExcludeSpecificEigens",           m_excludeEV = "" ,    "(semicolon-separated) names of Eigens you want to exclude. in case of continuous some eigenvectors can be ignored to make the computation faster");
  declareProperty("StoreOnlyUpVariations",           m_doOnlyUpVariations = false ,    "If set to true it processes only the __1up b-tagging variations. It speeds up the computation in case of symmetric variations.");
  declareProperty("doDirectTagging",                 m_doDirectTag = false ,    "If set to true it also computes and stores the direct tagging choice and the related SFs for each jet");
  // if it is empty, the onnx tool won't be initialised
  declareProperty( "pathToONNX",                     m_pathToONNX = "",          "path to the onnx file that will be used for inference");
  declareProperty( "TaggingStrategy",                m_taggingStrategy = "AllJets",     "tagging strategy in the Analysis (eg. 'leading2SignalJets' in boosted VHbb). Required to do TT with GNN");
}

StatusCode BTaggingTruthTaggingTool::setEffMapIndex(const std::string& flavour, unsigned int index){
    ANA_CHECK(m_effTool->setMapIndex(flavour, index));
    return  StatusCode::SUCCESS;
}

void BTaggingTruthTaggingTool::setUseSystematics(bool useSystematics){
  ATH_MSG_DEBUG("setting the use of systematics to: " <<useSystematics);
  m_useSys = useSystematics;
  ATH_MSG_DEBUG(" m_useSys = " << m_useSys);
}

SystematicSet BTaggingTruthTaggingTool::affectingSystematics() const {
  return m_effTool->affectingSystematics();
}

SystematicCode BTaggingTruthTaggingTool::applySystematicVariation( const CP::SystematicSet & systConfig )
{
    for (auto syst : systConfig) {
        CP::SystematicSet myset;
        ATH_MSG_WARNING("applySystematicVariation was called for " << syst.name() << " but BTaggingTruthTaggingTool does not apply Systematic Variations");
        //the truth tagging tool provides results for all possible systematic variations in its results objects, the user does not need to call each one seperatly.
    }
   return SystematicCode::Ok;
}

bool BTaggingTruthTaggingTool::isAffectedBySystematic( const CP::SystematicVariation & systematic ) const
{
  SystematicSet sys = affectingSystematics();
  return sys.find( systematic) != sys.end();
}

SystematicSet BTaggingTruthTaggingTool::recommendedSystematics() const {
  return affectingSystematics();
}

StatusCode BTaggingTruthTaggingTool::initialize() {

  ANA_CHECK_SET_TYPE (StatusCode);
  m_initialised = true;

  ANA_CHECK(m_selTool.setProperty("MaxEta",  m_maxEta));
  ANA_CHECK(m_selTool.setProperty("MaxRangePt",  m_maxRangePt));
  ANA_CHECK(m_selTool.setProperty("MinPt",  m_minPt));
  ANA_CHECK(m_selTool.setProperty("TaggerName",  m_taggerName));
  ANA_CHECK(m_selTool.setProperty("OperatingPoint", m_OP));
  ANA_CHECK(m_selTool.setProperty("JetAuthor", m_jetAuthor));
  ANA_CHECK(m_selTool.setProperty("FlvTagCutDefinitionsFileName", m_SFFile));
  ANA_CHECK(m_selTool.initialize());

  ANA_CHECK(m_effTool.setProperty("MinPt",                           m_minPt));
  ANA_CHECK(m_effTool.setProperty("TaggerName",                      m_taggerName));
  ANA_CHECK(m_effTool.setProperty("OperatingPoint",                  m_OP));
  ANA_CHECK(m_effTool.setProperty("JetAuthor",                       m_jetAuthor));
  ANA_CHECK(m_effTool.setProperty("ScaleFactorFileName",             m_SFFile ));
  ANA_CHECK(m_effTool.setProperty("UseDevelopmentFile",              m_useDevFile ));
  ANA_CHECK(m_effTool.setProperty("EfficiencyFileName",              m_EffFile ));
  ANA_CHECK(m_effTool.setProperty("ScaleFactorBCalibration",         m_SFBName ));
  ANA_CHECK(m_effTool.setProperty("ScaleFactorCCalibration",         m_SFCName ));
  ANA_CHECK(m_effTool.setProperty("ScaleFactorTCalibration",         m_SFTName ));
  ANA_CHECK(m_effTool.setProperty("ScaleFactorLightCalibration",     m_SFLightName ));
  ANA_CHECK(m_effTool.setProperty("EigenvectorReductionB",           m_EVReductionB ));
  ANA_CHECK(m_effTool.setProperty("EigenvectorReductionC",           m_EVReductionC ));
  ANA_CHECK(m_effTool.setProperty("EigenvectorReductionLight",       m_EVReductionLight ));
  ANA_CHECK(m_effTool.setProperty("EfficiencyBCalibrations",         m_EffBName ));
  ANA_CHECK(m_effTool.setProperty("EfficiencyCCalibrations",         m_EffCName ));
  ANA_CHECK(m_effTool.setProperty("EfficiencyTCalibrations",         m_EffTName ));
  ANA_CHECK(m_effTool.setProperty("EfficiencyLightCalibrations",     m_EffLightName ));
  ANA_CHECK(m_effTool.setProperty("ExcludeFromEigenVectorTreatment", m_excludeFromEV ));
  ANA_CHECK(m_effTool.setProperty("SystematicsStrategy",             m_systStrategy ));
  ANA_CHECK(m_effTool.setProperty("ConeFlavourLabel",                m_coneFlavourLabel ));
  ANA_CHECK(m_effTool.setProperty("OldConeFlavourLabel",             m_oldConeFlavourLabel ));
  ANA_CHECK(m_effTool.setProperty("pathToONNX",                      m_pathToONNX ));

  ANA_CHECK(m_effTool.initialize());

  m_continuous = false;
  m_continuous2D = false;
  if(m_OP.find("Continuous") != std::string::npos){
    ATH_MSG_INFO("You are running in Continuous and you chose " << m_cutBenchmark <<" as benchmarks" );
    m_continuous = true;
    if(m_OP.find("Continuous2D") != std::string::npos)
      m_continuous2D = true;
    
    //tokenize the quantiles that will be used as tagged bin indices.
    std::vector<std::string> tag_benchmarks = split(m_cutBenchmark, ',');
    for (const auto & tag_bin : tag_benchmarks ){
      if(m_continuous2D)
	m_OperatingBins.push_back(std::atoi(tag_bin.c_str()));
      else //continuous
	m_OperatingBins.push_back(std::atoi(tag_bin.c_str())-1); //different convention
    }
  }
  else{ //FixedCut working points
    m_OperatingBins.clear();
    m_OperatingBins.push_back(1); //first bin: untagged, second bin: tagged.
}
  
  if(m_useQuntile && !m_continuous){
    ATH_MSG_ERROR("UseQuantile is available only in Continuous WP. Please consider switching the Operating Point.");
    return StatusCode::FAILURE;
  }
  
  if (m_pathToONNX != ""){
    // 60% = 4, 70% = 3, 77% = 2, 85% = 1, 100% = 0
    m_OP_index_for_GNN = find(m_availableOP_fixCut.begin(), m_availableOP_fixCut.end(), m_OP) - m_availableOP_fixCut.begin() + 1; // GNN predicts 5 bins    
  }
  
  m_eff_syst.clear();
  m_sys_name.clear();
  CP::SystematicSet def_set;
  m_eff_syst.push_back(def_set);
  m_sys_name.push_back("Nominal");
  
  if(m_useSys){
    std::vector<std::string> m_excludeEV_vector;
    if(m_excludeEV != "")
      m_excludeEV_vector = split(m_excludeEV, ';');
    CP::SystematicSet systs = m_effTool->affectingSystematics();
    for (const auto & syst : systs) {
      CP::SystematicSet myset;
      string s = syst.name();
      if(m_doOnlyUpVariations && s.find("__1down") !=string::npos) continue;
      if(std::any_of(m_excludeEV_vector.begin(), m_excludeEV_vector.end(), [&s](const std::string& str) { return str.find(s) != std::string::npos; })) continue;
      //      if(m_excludeEV_vector.size() != 0 && find(m_excludeEV_vector.begin(), m_excludeEV_vector.end(), syst.name()) != m_excludeEV_vector.end()) continue; //if the systematics is in the list of excluded EV we just ignore it.
      myset.insert(syst);
      if(m_excludeEV != "")
        ATH_MSG_INFO("Adding systematic " << syst.name() << "to the list ");
      else
        ATH_MSG_DEBUG("Adding systematic " << syst.name() << "to the list ");
      m_eff_syst.push_back(myset);
      m_sys_name.push_back(syst.name());
    }
  }
  
  ATH_MSG_INFO("end of systematics ");

  //set the cuts
  TString pathtofile =  PathResolverFindCalibFile(m_SFFile);
  m_inf = TFile::Open(pathtofile, "read");

  float min_cut_low = (m_taggerName == "MV2c10") ? -1 : -20;
  float max_cut_hig = (m_taggerName == "MV2c10") ? +1 : +20;
  
  if(m_OP == "Continuous"){
    std::vector<float> bin_edges;
    bin_edges.push_back(min_cut_low);
    for (auto bin_name : m_availableOP_fixCut){
      TString cutname = m_taggerName+"/"+m_jetAuthor+"/"+bin_name+"/cutvalue";
      float cutval = ((TVector*) m_inf->Get(cutname))[0](0);
      bin_edges.push_back(cutval);
    }
    bin_edges.push_back(max_cut_hig); 
    for (unsigned int be = 0 ; be < bin_edges.size()-1 ; be++){
      //check if the bin is one of the bins used for btagging
      //NB. i+1 is to follow the convention 1-5!
      bool is_tagbin = std::count(m_OperatingBins.begin(), m_OperatingBins.end(), be);
      TagBin one_cut = TagBin(is_tagbin, bin_edges.at(be), bin_edges.at(be+1));
      m_cuts.push_back(one_cut);
    }
  } //== continuous
  else if (m_OP.find("Continuous2D")  != std::string::npos){
    ATH_MSG_INFO("TruthTagging initialization: Working with Continuous2D WP.");
    TString cutname = m_taggerName+"/"+m_jetAuthor+"/Continuous2D/cutvalue";
    TMatrixD* m_cuts2D = (TMatrixD*) m_inf->Get(cutname);
    ATH_MSG_DEBUG("Counting "  <<m_cuts2D->GetNrows() <<" bins ");

    for (int bin = 0; bin < m_cuts2D->GetNrows(); bin++){
      ATH_MSG_DEBUG("INITIALIZATION c-cuts : " <<(*m_cuts2D)(bin,0) <<" "
		    <<(*m_cuts2D)(bin,1) <<" b-cuts : "
		    <<(*m_cuts2D)(bin,2) <<" "
		    <<(*m_cuts2D)(bin,3));
      
      //check if the bin is one of the bins used for btagging
      //NB. i follows the convention 0-4 of the continuous2D tagging
      bool is_tagbin = std::count(m_OperatingBins.begin(), m_OperatingBins.end(), bin);

      //set protection limits, so the final generated tagweight values have a meaning
      float c_tagcut_low = ((*m_cuts2D)(bin,0) < min_cut_low) ? min_cut_low : (*m_cuts2D)(bin,0);
      float c_tagcut_hig = ((*m_cuts2D)(bin,1) > max_cut_hig) ? max_cut_hig : (*m_cuts2D)(bin,1);
      float b_tagcut_low = ((*m_cuts2D)(bin,2) < min_cut_low) ? min_cut_low : (*m_cuts2D)(bin,2);
      float b_tagcut_hig = ((*m_cuts2D)(bin,3) > max_cut_hig) ? max_cut_hig : (*m_cuts2D)(bin,3);

      TagBin one_cut = TagBin(is_tagbin, b_tagcut_low, b_tagcut_hig, c_tagcut_low, c_tagcut_hig);
      m_cuts.push_back(one_cut);
    }
  }//continuous2D
  else{ //fixed-cut
    TString cutname = m_taggerName+"/"+m_jetAuthor+"/"+m_OP+"/cutvalue";
    float cutval = ((TVector*) m_inf->Get(cutname))[0](0);

    TagBin untag_bin = TagBin(false, min_cut_low, cutval); //bin 0
    TagBin tag_bin   = TagBin(true, cutval, max_cut_hig); //bin 1

    m_cuts.push_back(untag_bin);
    m_cuts.push_back(tag_bin);
  } //FixedCut

  if(m_continuous)
    for (auto c :  m_cuts){
      ATH_MSG_INFO("b-tag low: " <<c.bcut_low <<" b-tag hig: " <<c.bcut_hig  <<" c-tag low: " <<c.ccut_low <<" c-tag hig: " <<c.ccut_hig <<" Added to tag bins: " <<c.is_tagbin);
    }
  
  //total number of bins. helpful in loops
  m_nbins = m_cuts.size(); //FixedCut
  ATH_MSG_INFO("Total number of bins: " <<m_nbins);


  // "AllJets" is the default strategy
  if (m_taggingStrategy == "AllJets"){
    m_njetsTagStrategy = AllJets;
  } else if (m_taggingStrategy == "Leading2SignalJets"){
    m_njetsTagStrategy = Leading2SignalJets;
  } else if (m_taggingStrategy == "Leading3SignalJets"){
    m_njetsTagStrategy = Leading3SignalJets;
  } else {
    ATH_MSG_ERROR("BTaggingTruthTaggingTool::tagging strategy " << m_taggingStrategy << " is not implemented in the TruthTagging Tool");
    return StatusCode::FAILURE;    
  }

  return StatusCode::SUCCESS;
  }


StatusCode BTaggingTruthTaggingTool::setJets(TRFinfo &trfinf,std::vector<float>& pt, std::vector<float>& eta, std::vector<int>& flav, std::vector<float>& tagw){
  ANA_CHECK_SET_TYPE (StatusCode);
  if(pt.size()!=eta.size() || pt.size()!=flav.size() || pt.size()!=tagw.size()){
    ATH_MSG_ERROR( "Vectors of pt, eta, flav and tagw should have same size" );
    return StatusCode::FAILURE;
  }

  auto vars = std::make_unique<std::vector<Analysis::CalibrationDataVariables>>(0);
  for(unsigned int i =0; i<pt.size(); i++){
    Analysis::CalibrationDataVariables vars_appo;
    if (!fillVariables(pt.at(i), eta.at(i), tagw.at(i), vars_appo)){
      ATH_MSG_ERROR("unable to fill variables");
      return StatusCode::FAILURE;
    }
    vars->push_back(vars_appo);
  }

  ANA_CHECK(setJets(trfinf, flav, *vars));
  return StatusCode::SUCCESS;
}

StatusCode BTaggingTruthTaggingTool::setJets(TRFinfo &trfinf,const xAOD::JetContainer& jets){
  ANA_CHECK_SET_TYPE (StatusCode);
  std::vector<int> flav;
  auto vars = std::make_unique<std::vector<Analysis::CalibrationDataVariables>>(0);
  for(const auto jet : jets) {
    Analysis::CalibrationDataVariables vars_appo;
    if (!fillVariables(*jet, vars_appo)){
      ATH_MSG_ERROR("unable to fill variables");
      return StatusCode::FAILURE;
    }

    vars->push_back(vars_appo);
    flav.push_back(jetFlavourLabel(*jet));
  }

  ANA_CHECK(setJets(trfinf, flav, *vars));
  flav.clear();
  return StatusCode::SUCCESS;
}

StatusCode BTaggingTruthTaggingTool::setJets(TRFinfo &trfinf, std::vector<int>& flav, const std::vector<Analysis::CalibrationDataVariables>& vars){
  if(flav.size()!=vars.size()){
    ATH_MSG_ERROR( "Vector of CalibrationDataVariables and flavour should have same size" );
    return StatusCode::FAILURE;
  }
  trfinf.jets.clear();
  for(unsigned int i =0; i<vars.size(); i++){
    jetVariable jetVar_appo;
    jetVar_appo.flav=flav.at(i);
    jetVar_appo.vars=vars.at(i);
    trfinf.jets.push_back(jetVar_appo);

  }
  trfinf.njets=trfinf.jets.size();
  return StatusCode::SUCCESS;
}

// sets node_feat which will be used by the onnx tool
StatusCode BTaggingTruthTaggingTool::setJets(TRFinfo &trfinf, const std::vector<std::vector<float>>& node_feat, std::vector<float>& tagw){
  ANA_CHECK_SET_TYPE (StatusCode);
  if(node_feat.size()!=tagw.size()){
    ATH_MSG_ERROR( "Vectors of node_feat (outer axis) and tagw should have same size" );
    return StatusCode::FAILURE;
  }

  std::vector<int> flav;
  auto vars = std::make_unique<std::vector<Analysis::CalibrationDataVariables>>(0);
  for(unsigned int i =0; i<tagw.size(); i++){
    Analysis::CalibrationDataVariables vars_appo;
      
    // node_feat[i] is always {flav, pt, eta, ...}
    float pt  = node_feat.at(i)[1];
    float eta = node_feat.at(i)[2];

    if (!fillVariables(pt, eta, tagw.at(i), vars_appo)){
      ATH_MSG_ERROR("unable to fill variables");
      return StatusCode::FAILURE;
    }
    vars->push_back(vars_appo);
    
    flav.push_back(node_feat.at(i)[0]);
  }

  ANA_CHECK(setJets(trfinf, flav, *vars, node_feat));
  return StatusCode::SUCCESS;
}

// sets node_feat which will be used by the onnx tool
StatusCode BTaggingTruthTaggingTool::setJets(TRFinfo &trfinf, const xAOD::JetContainer& jets, const std::vector<std::vector<float>>& node_feat){
  ANA_CHECK_SET_TYPE (StatusCode);
  std::vector<int> flav;
  auto vars = std::make_unique<std::vector<Analysis::CalibrationDataVariables>>(0);
  for(const auto jet : jets) {
    Analysis::CalibrationDataVariables vars_appo;
    if (!fillVariables(*jet, vars_appo)){
      ATH_MSG_ERROR("unable to fill variables");
      return StatusCode::FAILURE;
    }

    vars->push_back(vars_appo);
    flav.push_back(jetFlavourLabel(*jet));
  }

  ANA_CHECK(setJets(trfinf, flav, *vars, node_feat));
  flav.clear();
  return StatusCode::SUCCESS;
}

// sets node_feat which will be used by the onnx tool
// node_feat contains feats for all the jets as it will be used by the GNN.
// trfinf.jets contains only the jets we care about for TT. eg. the leading2 jets if taggingStrategy == Leading2SignalJets
StatusCode BTaggingTruthTaggingTool::setJets(TRFinfo &trfinf,std::vector<int>& flav, const std::vector<Analysis::CalibrationDataVariables>& vars, const std::vector<std::vector<float>>& node_feat){
  if(flav.size()!=vars.size()){
    ATH_MSG_ERROR( "Vector of CalibrationDataVariables and flavour should have same size" );
    return StatusCode::FAILURE;
  }
  trfinf.jets.clear();

  if (m_njetsTagStrategy == -1){
    trfinf.njets = vars.size();
  } else {
    trfinf.njets = std::min(static_cast<unsigned int>(m_njetsTagStrategy), static_cast<unsigned int>(vars.size()));
  }

  for(unsigned int i =0; i<trfinf.njets; i++){
    jetVariable jetVar_appo;
    jetVar_appo.flav=flav.at(i);
    jetVar_appo.vars=vars.at(i);
    trfinf.jets.push_back(jetVar_appo);
  }
  trfinf.node_feat = node_feat;
  return StatusCode::SUCCESS;
}


bool BTaggingTruthTaggingTool::fillVariables( const xAOD::Jet& jet, CalibrationDataVariables& x){
  x.jetPt = jet.pt();
  x.jetEta = jet.eta();
  x.jetAuthor = m_jetAuthor;

  const xAOD::BTagging* tagInfo = jet.btagging();
  if (!tagInfo) return false;

  ANA_CHECK( m_selTool->getTaggerWeight(jet, x.jetTagWeight) );
  return true;
}

bool BTaggingTruthTaggingTool::fillVariables( const float jetPt, const float jetEta, const float jetTagWeight, CalibrationDataVariables& x){
  x.jetPt = jetPt;
  x.jetEta = jetEta;
  x.jetTagWeight = jetTagWeight;
  x.jetAuthor = m_jetAuthor;
  return true;
}

StatusCode BTaggingTruthTaggingTool::GetTruthTagWeights(TRFinfo &trfinf, std::vector<float> &trf_weight_ex, std::vector<float> &trf_weight_in){

  ANA_CHECK_SET_TYPE (StatusCode);
  //get MC efficiencies
  ANA_CHECK(getAllEffMC(trfinf));
  //compute truth tag weights
  ANA_CHECK(getTRFweight(trfinf,m_nbtag, true));
  // choice of the selected permutation
  ANA_CHECK(chooseAllTagPermutation(trfinf,m_nbtag));

  // choice of the tagged bins
  ANA_CHECK(chooseAllTagBins(trfinf));

  trf_weight_ex = trfinf.trfw_ex;
  trf_weight_in = trfinf.trfw_in;
  return StatusCode::SUCCESS;
}


StatusCode BTaggingTruthTaggingTool::CalculateResults(TRFinfo &trfinf, Analysis::TruthTagResults& results, int rand_seed){

  ANA_CHECK_SET_TYPE (StatusCode);
  results.clear();

  if(rand_seed!=-1){
    trfinf.rand.SetSeed(rand_seed);
  }

  std::vector<float> trf_weight_ex,  trf_weight_in;

  trf_weight_ex.clear();
  trf_weight_in.clear();

  ANA_CHECK(GetTruthTagWeights(trfinf, trf_weight_ex, trf_weight_in));

  ANA_CHECK(getTagPermutation(trfinf,results.trf_chosen_perm_ex,results.trf_chosen_perm_in));

  //modify quantiles according to the different conventions
  ANA_CHECK(getQuantiles(trfinf,results.trf_bin_ex, results.trf_bin_in));
    
  //new tagger scores
  ANA_CHECK(generateRandomTaggerScores(results.trf_bin_ex, results.trf_bin_score_ex, results.trf_ctag_bin_score_ex));
  ANA_CHECK(generateRandomTaggerScores(results.trf_bin_in, results.trf_bin_score_in, results.trf_ctag_bin_score_in));

  //direct tag
  if(m_doDirectTag)
    ANA_CHECK( getDirectTaggedJets(trfinf, results.is_tagged) );

  //handle systematics
  unsigned int n_systs = (m_useSys) ? m_eff_syst.size() : 1;
  results.syst_names.clear();

  for(unsigned int i = 0; i< n_systs; i++){
    
    results.syst_names.push_back(m_sys_name.at(i));
    
    results.map_trf_weight_ex[m_sys_name.at(i)].resize(trfinf.trfw_ex.size());
    results.map_trf_weight_in[m_sys_name.at(i)].resize(trfinf.trfw_in.size());

    results.map_SF_ex[m_sys_name.at(i)].resize(trfinf.trfw_ex.size());
    results.map_SF_in[m_sys_name.at(i)].resize(trfinf.trfw_in.size());

    //direct tagging
    if(m_doDirectTag){
      std::vector<int> is_tagged;
      for(auto t : results.is_tagged){
	is_tagged.push_back(static_cast<int>(t));
      }
      results.map_SF[m_sys_name.at(i)]=getEvtSF(trfinf,is_tagged, i);
    }

    //go over the ntag combinations 
    for(unsigned int j=0; j< trfinf.trfw_ex.size(); j++){
      if(j > trfinf.njets) {
	results.map_SF_ex[m_sys_name.at(i)].at(j) = 1.;
	results.map_SF_in[m_sys_name.at(i)].at(j) = 1.;
	ATH_MSG_DEBUG("number of jets: " <<trfinf.njets <<" less than max btag: " <<m_nbtag <<". Return BTag SF = 1. Consider applying an event pre-selection if this happens too often.");
      }
      else{
	results.map_SF_ex[m_sys_name.at(i)].at(j) = getEvtSF(trfinf,results.trf_bin_ex.at(j), i);
	results.map_SF_in[m_sys_name.at(i)].at(j) = getEvtSF(trfinf,results.trf_bin_in.at(j), i);
      }
      
      results.map_trf_weight_ex[m_sys_name.at(i)].at(j) = trfinf.trfw_ex.at(j) * results.map_SF_ex[m_sys_name.at(i)].at(j);
      results.map_trf_weight_in[m_sys_name.at(i)].at(j) = trfinf.trfw_in.at(j) * results.map_SF_in[m_sys_name.at(i)].at(j);
    }
  }

  return StatusCode::SUCCESS;

}

StatusCode BTaggingTruthTaggingTool::CalculateResults(std::vector<float>& pt, std::vector<float>& eta, std::vector<int>& flav, std::vector<float>& tagw, Analysis::TruthTagResults& results,int rand_seed){

  ANA_CHECK_SET_TYPE (StatusCode);

  TRFinfo trfinf;

  ANA_CHECK(setJets(trfinf,pt, eta, flav, tagw));

  return CalculateResults(trfinf,results,rand_seed);
}

StatusCode BTaggingTruthTaggingTool::CalculateResults(const xAOD::JetContainer& jets, Analysis::TruthTagResults& results,int rand_seed){

  ANA_CHECK_SET_TYPE (StatusCode);

  TRFinfo trfinf;

  ANA_CHECK(setJets(trfinf,jets));

  return CalculateResults(trfinf,results,rand_seed);
}

// setting inputs that the onnx tool will use
StatusCode BTaggingTruthTaggingTool::CalculateResultsONNX(const std::vector<std::vector<float>>& node_feat, std::vector<float>& tagw, Analysis::TruthTagResults& results, int rand_seed){

  ANA_CHECK_SET_TYPE (StatusCode);

  TRFinfo trfinf;
    
  ANA_CHECK(setJets(trfinf, node_feat, tagw));

  return CalculateResults(trfinf, results, rand_seed);
}

// setting inputs that the onnx tool will use
StatusCode BTaggingTruthTaggingTool::CalculateResultsONNX(const xAOD::JetContainer& jets, const std::vector<std::vector<float>>& node_feat, Analysis::TruthTagResults& results, int rand_seed){

  ANA_CHECK_SET_TYPE (StatusCode);

  TRFinfo trfinf;

  ANA_CHECK(setJets(trfinf, jets, node_feat));

  return CalculateResults(trfinf, results, rand_seed);
}

StatusCode BTaggingTruthTaggingTool::getAllEffMC(TRFinfo &trfinf){
  if (trfinf.node_feat.size() == 0){
    return getAllEffMCCDI(trfinf);
  } else {
    return getAllEffMCGNN(trfinf);
  }
}

// uses onnx tool
StatusCode BTaggingTruthTaggingTool::getAllEffMCGNN(TRFinfo &trfinf){

  //  trfinf.effMC.clear();
  for(int bin = 0; bin < m_nbins; bin++)
    trfinf.effMC_allBins[bin].clear();
    
  //extract efficiency
  std::vector<std::vector<float>> tmp_effMC_allBins; // shape:{num_jets, num_wp}
  CorrectionCode code = m_effTool->getMCEfficiencyONNX(trfinf.node_feat, tmp_effMC_allBins);
  
  if(!(code==CorrectionCode::Ok || code==CorrectionCode::OutOfValidityRange)){
    ATH_MSG_ERROR("BTaggingEfficiencyTool::getMCEfficiencyONNX returned CorrectionCode::Error");
    return StatusCode::FAILURE;
  }

  if(m_continuous) {
    for(int bin = 0; bin < m_nbins; bin++){
      // need to transpose
      std::vector<float> tmp_effMC_oneBin; // shape: {num_jet} = trfinf.njets
      for (unsigned int jet_index=0; jet_index<trfinf.njets; jet_index++){
        tmp_effMC_oneBin.push_back(tmp_effMC_allBins[jet_index][bin]);
      }
      trfinf.effMC_allBins[bin] = tmp_effMC_oneBin;
    }
  } // m_continuous
  else{
    for (unsigned int jet_index=0; jet_index<trfinf.njets; jet_index++){
      float tmp_effMC = std::accumulate(tmp_effMC_allBins[jet_index].begin()+m_OP_index_for_GNN, tmp_effMC_allBins[jet_index].end(), 0.0);
     trfinf.effMC_allBins[0].push_back(1-tmp_effMC);
     trfinf.effMC_allBins[1].push_back(tmp_effMC);
    }
 } // !m_continuous
         
  return StatusCode::SUCCESS;
}

StatusCode BTaggingTruthTaggingTool::getAllEffMCCDI(TRFinfo &trfinf){

  float eff_all =1.;
  for(int bin = 0; bin < m_nbins; bin++)
    trfinf.effMC_allBins[bin].clear();
  
  for(size_t i=0; i < trfinf.jets.size(); i++){
      for(int bin = 0; bin < m_nbins; bin++){   // loop on bins	
        eff_all=1.;

        //set a dumb value of the truth tag weight to get the different efficiency maps for each bin. to be improved..
	if (m_continuous2D)
	  trfinf.jets.at(i).vars.jetTagWeight = bin+0.5;
	else
	  trfinf.jets.at(i).vars.jetTagWeight = (m_cuts[bin].bcut_low + m_cuts[bin].bcut_hig)/2.;

	ATH_MSG_DEBUG("effMC_allBins: tag bin: " <<bin <<" tagw: "  << trfinf.jets.at(i).vars.jetTagWeight <<" "  <<m_cuts[bin].bcut_low <<" "  << m_cuts[bin].bcut_hig <<" " <<trfinf.jets.at(i).vars.jetPt <<" " <<trfinf.jets.at(i).vars.jetEta <<" " <<trfinf.jets.at(i).flav);
	//get the efficiency	
        CorrectionCode code = m_effTool->getMCEfficiency(trfinf.jets.at(i).flav, trfinf.jets.at(i).vars, eff_all);
	if(!(code==CorrectionCode::Ok || code==CorrectionCode::OutOfValidityRange)){
	  ATH_MSG_ERROR("BTaggingEfficiencyTool::getMCEfficiency returned CorrectionCode::Error");
	  return StatusCode::FAILURE;
	}

        trfinf.effMC_allBins[bin].push_back(eff_all);
	ATH_MSG_DEBUG("effMC_allBins: tag bin: " <<bin <<" tagw: "  << trfinf.jets.at(i).vars.jetTagWeight  <<" eff: " <<eff_all);
      } // end loop on bins
  } // end loop on jets

  return StatusCode::SUCCESS;
}

std::vector<std::vector<bool> > BTaggingTruthTaggingTool::generatePermutations(int njets, int tags, int start){
  std::vector<std::vector<bool> > perm;
  std::vector<std::vector<bool> > temp_perm;

  if(tags==0){
    std::vector<bool> tags(njets,false);
    perm.push_back(tags);
  }
  else if(tags == njets) {
    std::vector<bool> tags(njets,true);
    perm.push_back(tags);
  }
  else {
    for(int i=start; i<njets;i++){
      temp_perm = generatePermutations(njets,tags-1,i+1);
      for(unsigned int j=0; j<temp_perm.size(); j++){
        temp_perm.at(j).at(i)=true;
        perm.push_back(temp_perm.at(j));
      }
    }
  }

  return perm;
}


float BTaggingTruthTaggingTool::trfWeight(TRFinfo &trfinf,const std::vector<bool> &tags){

  float weight = 1;
  for (unsigned int t=0; t<tags.size();t++) {
    float trf = 0.;
    
    //sum of the efficiencies in the tagged bins
    for (int bin : m_OperatingBins){

      trf += trfinf.effMC_allBins[bin][t];
      
    }
    if(trf>1.) {
      ATH_MSG_WARNING("Truth Tagging weight > 1. --> setting it to 1. check maps!");
      trf = 1.;
    }
    if(tags.at(t)) weight *= trf;
    else weight *= (1.-trf);
    ATH_MSG_DEBUG("weight " <<weight);
  } // end loop over jets
  return weight;
}


StatusCode BTaggingTruthTaggingTool::getTRFweight(TRFinfo &trfinf,unsigned int nbtag, bool isInclusive){

  unsigned int njets = trfinf.njets;
  // Consider permutations of njet jets with up to limit b-tags
  unsigned int limit = (njets > 7) ? 8 : njets+1;

  // Permutations: njets, ntags, permutations
  if (trfinf.perms.find(njets)==trfinf.perms.end()){   // if I don't have already saved the possible permutations for njet
    trfinf.perms[njets] = std::vector<std::vector<std::vector<bool> > >(limit);
    for(unsigned int i=0;i<limit;i++)
      trfinf.perms[njets].at(i) = generatePermutations(njets,i);
  }
  trfinf.permsWeight.clear(), trfinf.permsWeight.resize(limit); // trfinf.permsWeight.at(i).at(j): TRF weight of the j-th perm with i b-tags
  trfinf.permsSumWeight.clear(), trfinf.permsSumWeight.resize(limit); // trfinf.permsSumWeight.at(i).at(j): partial sum of  TRF weight of the permutations with i b-tags up to j (0,1,..,j-th) perm. Used in the choice of the selected permutation

  // compute TRF weight
  unsigned int max = nbtag+1; // from 0 to nbtag b-tags --> nbtag+1 positions
  trfinf.trfw_ex.clear(), trfinf.trfw_in.clear();
  trfinf.trfw_ex.resize(max), trfinf.trfw_in.resize(max);

  //clear and resize elements of trfinf
    trfinf.perm_ex.clear(), trfinf.perm_in.clear(); // vector<vector<bool>> --> for each number of tags the chosen permutation
    trfinf.perm_ex.resize(max),  trfinf.perm_in.resize(max);
    trfinf.permprob_ex.clear(), trfinf.permprob_in.clear(); // probability of the perm in trfinf.perm_ex/in
    trfinf.permprob_ex.resize(max),  trfinf.permprob_in.resize(max);
    
  if(isInclusive) {
    for(unsigned int i=0; i<limit; i++) { // note: I consider maximum limit tags. It's an approximation
      std::vector<float> weights;
      float sum = 0., w = 0.;
      trfinf.permsWeight.at(i).clear();
      trfinf.permsSumWeight.at(i).clear();
      trfinf.permsWeight.at(i).resize(trfinf.perms[njets].at(i).size());
      trfinf.permsSumWeight.at(i).resize(trfinf.perms[njets].at(i).size());

      // loop on all the permutations with i tags
      for(unsigned int p=0; p<trfinf.perms[njets].at(i).size(); p++) {

	w = trfWeight(trfinf,trfinf.perms[njets].at(i).at(p));
	sum+=w;
	trfinf.permsWeight.at(i).at(p) = w;
	trfinf.permsSumWeight.at(i).at(p) = sum;
	
	ATH_MSG_DEBUG("nbtag = " << i << "  wei = " << w << "  sum = " << sum);
      }
      if(i<limit && i<max) {
      // note: I need to already have the exclusive weights filled to compite the inclusive
        trfinf.trfw_ex.at(i) = sum; // sum of TRF weights for all perm with i b-tags
        if(i == 0) trfinf.trfw_in.at(0) = 1.;
        else trfinf.trfw_in.at(i) = trfinf.trfw_in.at(i-1) - trfinf.trfw_ex.at(i-1); // P(>=4) = P(>=3) - P(==3)
        ATH_MSG_DEBUG("i = " << i << "  sum = " << sum << "  TRF in " << trfinf.trfw_in.at(i) << "  ex = " << trfinf.trfw_ex.at(i));
      }
    }
    ATH_MSG_DEBUG("before return, nbtag = " << nbtag << "  size de trfinf.trfw_in = " << trfinf.trfw_in.size());
    return StatusCode::SUCCESS;
  }
  else { // exclusive case, only one calculation needed
    std::vector<float> weights;
    float sum = 0., w = 0.;
    size_t size = trfinf.perms[njets].at(nbtag).size();
    trfinf.permsWeight.at(nbtag).clear();
    trfinf.permsSumWeight.at(nbtag).clear();
    trfinf.permsWeight.at(nbtag).resize(size);
    trfinf.permsSumWeight.at(nbtag).resize(size);
    // loop on all the permutations with i tags
    for(unsigned int p=0; p<trfinf.perms[njets].at(nbtag).size(); p++) {
      w = trfWeight(trfinf,trfinf.perms[njets].at(nbtag).at(p));
      sum+=w;
      trfinf.permsWeight.at(nbtag).at(p) = w;
      trfinf.permsSumWeight.at(nbtag).at(p) = sum;
    }
    trfinf.trfw_ex.at(nbtag) = sum;
    return StatusCode::SUCCESS;
  }
  return StatusCode::SUCCESS;
}


StatusCode BTaggingTruthTaggingTool::getTagPermutation(TRFinfo &trfinf, std::vector<std::vector<bool> > &trf_chosen_perm_ex, std::vector<std::vector<bool> > &trf_chosen_perm_in){
  trf_chosen_perm_ex.resize(m_nbtag+1);
  trf_chosen_perm_in.resize(m_nbtag+1);
  trf_chosen_perm_ex = trfinf.perm_ex;
  trf_chosen_perm_in = trfinf.perm_in;
  std::string print_perm = "Permutation: ";
  for(auto perm: trfinf.perm_ex){
    for(auto is: perm) {
      if(is) print_perm+=std::to_string(1);
      else print_perm+=std::to_string(0);
      print_perm+="  ";
    }
  }
  ATH_MSG_DEBUG(print_perm);
  return StatusCode::SUCCESS;
}

StatusCode BTaggingTruthTaggingTool::chooseAllTagPermutation(TRFinfo &trfinf,unsigned int nbtag){

  ANA_CHECK_SET_TYPE (StatusCode);

  if(!m_usePerm){
    return StatusCode::FAILURE;
  }

  unsigned int njets = trfinf.njets;
  unsigned int limit = (njets > 7) ? 8 : njets+1;
  unsigned int max = (njets < nbtag+1) ? limit : nbtag+1;

  trfinf.perm_ex.clear(), trfinf.perm_ex.resize(nbtag+1);
  trfinf.perm_in.clear(), trfinf.perm_in.resize(nbtag+1);

  trfinf.permprob_ex.clear(), trfinf.permprob_ex.resize(nbtag+1);
  trfinf.permprob_in.clear(), trfinf.permprob_in.resize(nbtag+1);

  for(unsigned int i=0; i<max; i++) { // need +1 as 0 is included
    ANA_CHECK(chooseTagPermutation(trfinf,i,false));
    ANA_CHECK(chooseTagPermutation(trfinf,i,true));
  }

  return StatusCode::SUCCESS;
}

StatusCode BTaggingTruthTaggingTool::chooseTagPermutation(TRFinfo &trfinf,unsigned int nbtag, bool isIncl){
  std::vector<float> incl;
  std::vector<std::pair<unsigned int, unsigned int> > trackPerm;
  float sum = 0;
  if(isIncl) {
    for(unsigned int itag=nbtag; itag < trfinf.permsWeight.size(); itag++) {
      for(unsigned int ip = 0; ip < trfinf.permsWeight.at(itag).size(); ip++) {
        sum += trfinf.permsWeight.at(itag).at(ip);
        incl.push_back(sum);
        trackPerm.push_back(std::make_pair(itag,ip));
      }
    }
  }
  else { // in exclusive case
    sum = trfinf.permsSumWeight.at(nbtag).back();
    incl = trfinf.permsSumWeight.at(nbtag);
    for(unsigned int ip = 0; ip < trfinf.permsSumWeight.at(nbtag).size(); ip++) trackPerm.push_back(std::make_pair(nbtag,ip));
  }
  float theX = trfinf.rand.Uniform(sum);
  for(unsigned int ip=0; ip < incl.size(); ip++) {
    ATH_MSG_DEBUG("incl.at(ip): " << incl.at(ip) << "  theX: " << theX);
    if(incl.at(ip) >= theX) {
      if(isIncl) {
        trfinf.perm_in.at(nbtag) = trfinf.perms[trfinf.njets].at(trackPerm.at(ip).first).at(trackPerm.at(ip).second);
        trfinf.permprob_in.at(nbtag) = trfinf.permsWeight.at(trackPerm.at(ip).first).at(trackPerm.at(ip).second) / trfinf.trfw_in.at(nbtag);
      }
      else {
        trfinf.perm_ex.at(nbtag) = trfinf.perms[trfinf.njets].at(trackPerm.at(ip).first).at(trackPerm.at(ip).second);
        trfinf.permprob_ex.at(nbtag) = trfinf.permsWeight.at(trackPerm.at(ip).first).at(trackPerm.at(ip).second) / trfinf.trfw_ex.at(nbtag);
      }
      return StatusCode::SUCCESS;
    }
  }
  return StatusCode::SUCCESS;
}



///// tag bins  //////////////////////
// Cheatsheet:
// returns 5 if between 60% and 0%
// returns 4 if between 70% and 60%
// returns 3 if between 77% and 70%
// returns 2 if between 85% and 77%
// returns 1 if between 100% and 85%
// return -1 if not in b-tagging acceptance
//////////////////////


StatusCode BTaggingTruthTaggingTool::getQuantiles(TRFinfo &trfinf,std::vector<std::vector<int> > &trf_bin_ex, std::vector<std::vector<int> > &trf_bin_in){
  trf_bin_ex.resize(trfinf.tbins_ex.size());

  for(unsigned int i =0; i<trfinf.tbins_ex.size(); i++)
    trf_bin_ex.at(i).resize(trfinf.tbins_ex.at(i).size());

  trf_bin_in.resize(trfinf.tbins_in.size());
  for(unsigned int i =0; i<trfinf.tbins_in.size(); i++)
    trf_bin_in.at(i).resize(trfinf.tbins_in.at(i).size());
  // increasing the value by 1 to match conventions in selectionTool (not continuous2D)
  for(unsigned int i =0; i<trfinf.tbins_ex.size(); i++)
    for(unsigned int j=0; j<trfinf.tbins_ex.at(i).size(); j++)
      trf_bin_ex.at(i).at(j)= (m_continuous2D || !m_continuous) ? trfinf.tbins_ex.at(i).at(j) : trfinf.tbins_ex.at(i).at(j) +1;

  for(unsigned int i =0; i<trfinf.tbins_in.size(); i++)
    for(unsigned int j=0; j<trfinf.tbins_in.at(i).size(); j++)
      trf_bin_in.at(i).at(j) = (m_continuous2D || !m_continuous) ? trfinf.tbins_in.at(i).at(j) : trfinf.tbins_in.at(i).at(j) +1;

  return StatusCode::SUCCESS;
}

StatusCode BTaggingTruthTaggingTool::chooseAllTagBins(TRFinfo &trfinf){
  ANA_CHECK_SET_TYPE (StatusCode);
  trfinf.tbins_ex.clear();
  trfinf.tbins_in.clear();
  trfinf.tbins_ex.resize( trfinf.trfw_ex.size());
  trfinf.tbins_in.resize( trfinf.trfw_in.size());

  trfinf.binsprob_ex.clear();
  trfinf.binsprob_in.clear();
  trfinf.binsprob_ex.resize( trfinf.trfw_ex.size());
  trfinf.binsprob_in.resize( trfinf.trfw_in.size());

  if(trfinf.perm_ex.size() != trfinf.perm_in.size()) ATH_MSG_WARNING("Different sizes in exclusive and inclusive permutation choices");

  for(unsigned int nb=0; nb<trfinf.perm_ex.size(); nb++) {
    ANA_CHECK(chooseTagBins_cum(trfinf,trfinf.perm_ex.at(nb), false, nb));
    ANA_CHECK(chooseTagBins_cum(trfinf,trfinf.perm_in.at(nb), true, nb));
  }
  return StatusCode::SUCCESS;
}


StatusCode BTaggingTruthTaggingTool::chooseTagBins_cum(TRFinfo &trfinf,std::vector<bool> &tagconf, bool isIncl, unsigned int nbtag){
  std::vector<int> btagops;

  ATH_MSG_DEBUG("chooseTagBins_cum ntag: " <<nbtag);

  std::vector<float> incl;
  std::vector<float> untagged_bins;
  float prob = 1.;

  //define the untagged bins
  for (int bin=0; bin<m_nbins; bin++) {
    if (std::count(m_OperatingBins.begin(), m_OperatingBins.end(), bin)) continue;
    untagged_bins.push_back(bin); 
  }

  //How to calculate Probabilities:
  //sum: [---- P(bin 0) -----][-- P(bin 1) ---][- P(bin 2) -]
  //random X up to sum of the bins. pick one prob based on the efficiency
  for(unsigned int j=0; j<tagconf.size(); j++) {

    ATH_MSG_DEBUG("tagconf.at(tc) " <<tagconf.at(j));

    if(tagconf.at(j)) { // tagged jet
      float sum=0.;
      incl.clear();
      for (int tagged_bin : m_OperatingBins){
	sum += trfinf.effMC_allBins[tagged_bin][j];
	incl.push_back(sum);
      }
      float theX = trfinf.rand.Uniform(sum);
      for(unsigned int bin = 0; bin < m_OperatingBins.size(); bin++){
	  float tag_index = (incl.at(bin) >= theX) ? m_OperatingBins[bin] : m_OperatingBins.back();
          btagops.push_back(tag_index);
          prob *= (trfinf.effMC_allBins[tag_index][j]);
	  ATH_MSG_DEBUG("bin " <<tag_index <<" theX " <<theX <<" incl.at(bin) "  <<incl.at(bin) <<" effMC " <<trfinf.effMC_allBins[tag_index][j] <<" prob* "  <<prob);
          break;
      }
    }
    else { // untagged jet
      float untag_sum  = 0. ;
      incl.clear();
      //sum over untagged bins
      for(auto bin : untagged_bins){
	untag_sum += trfinf.effMC_allBins[bin][j];
        incl.push_back(untag_sum);
      }
      
      float theX = trfinf.rand.Uniform(untag_sum);
      for(unsigned int bin=0; bin< untagged_bins.size(); bin++) {
	float untag_index = (incl.at(bin) >= theX) ? untagged_bins[bin] : untagged_bins.back();
	btagops.push_back(untag_index);
	prob *= 1 - trfinf.effMC_allBins[untag_index][j];
	ATH_MSG_DEBUG("i " <<bin  <<" unt_bin " <<untag_index <<" theX " <<theX <<" incl.at(bin) "  <<incl.at(bin) <<" 1-effMC " <<1-trfinf.effMC_allBins[untag_index][j] <<" prob* " <<prob);
	break;
      }
    }
  }

  if(btagops.size() != tagconf.size()) {
    ATH_MSG_ERROR("You should not be here -> wrong size of tag bins vector. btagops: " <<btagops.size() <<" tagconf: " <<tagconf.size());
    return StatusCode::FAILURE;
  }
  if(isIncl) {
    trfinf.tbins_in.at(nbtag) = btagops;
    trfinf.binsprob_in.at(nbtag) = prob;
    ATH_MSG_DEBUG("incl, nbtag " << nbtag << "   prob " << prob);
  }
  else {
    trfinf.tbins_ex.at(nbtag) = btagops;
    trfinf.binsprob_ex.at(nbtag) = prob;
    ATH_MSG_DEBUG("excl, nbtag " << nbtag << "   prob " << prob);
  }
  return StatusCode::SUCCESS;
}

float BTaggingTruthTaggingTool::getTagBinsConfProb(TRFinfo &trfinf,std::vector<int> &tagws){
  float prob = 1.;
  for(unsigned int j=0; j<tagws.size(); j++) {
    
    
    float sum_tag = 0.;
    //calculate sum of all tagged bins
    for (auto bin : m_OperatingBins)
      sum_tag += trfinf.effMC_allBins[bin][j]; 
      
    if(std::count(m_OperatingBins.begin(), m_OperatingBins.end(), tagws.at(j) ) ) { // tagged
      //      float prevBinW = 0.;
      prob *= (trfinf.effMC_allBins[tagws.at(j)][j]) / sum_tag;
      ATH_MSG_DEBUG("prob " << prob);
    }
    else { // untagged
      //float prevBinW = 0.;
      prob *= (1 - trfinf.effMC_allBins[tagws.at(j)][j]) / (1 - sum_tag);
      ATH_MSG_DEBUG("prob " << prob);
    }
  }
  return prob;
}

StatusCode BTaggingTruthTaggingTool::getDirectTaggedJets(TRFinfo &trfinf,std::vector<bool> &is_tagged){
  is_tagged.clear();
  std::vector<int> appo;
  bool is_jettag = false;
  for(const auto jet : trfinf.jets) {
    ATH_MSG_DEBUG("pt " << jet.vars.jetPt << "   eta " << jet.vars.jetEta << "   wei " << jet.vars.jetTagWeight);
    is_jettag = m_selTool->accept(jet.vars.jetPt, jet.vars.jetEta, jet.vars.jetTagWeight);
  
  ATH_MSG_DEBUG("is tagged? " << is_jettag);
    if(is_jettag) is_tagged.push_back(1);
    else is_tagged.push_back(0);
  }

  return StatusCode::SUCCESS;
}


float BTaggingTruthTaggingTool::getEvtSF(TRFinfo &trfinf, std::vector<int>& quantiles, int sys){

  ANA_CHECK_SET_TYPE (StatusCode);

  float SF = 1.;

  if (m_ignoreSF) 
    return SF;

  if(sys!=0 && m_useSys) {
    ANA_CHECK( m_effTool->applySystematicVariation(m_eff_syst[sys]) );
  }

  for(unsigned int i =0; i< trfinf.njets; i++) {
    float jetSF = 1;

    if(!m_continuous){ //FixedCut

      bool is_btagged = quantiles.at(i);
      auto code = (is_btagged) ? m_effTool->getScaleFactor(trfinf.jets.at(i).flav, trfinf.jets.at(i).vars, jetSF) : m_effTool->getInefficiencyScaleFactor(trfinf.jets.at(i).flav, trfinf.jets.at(i).vars, jetSF);
     
      if(!(code==CorrectionCode::Ok || code==CorrectionCode::OutOfValidityRange)){
        ATH_MSG_ERROR("BTaggingEfficiencyTool::getScaleFactor returned CorrectionCode::Error");
        return StatusCode::FAILURE;
      }
    }
    else{ //continuous & continuous2D      
      int bin = quantiles.at(i);
      //set new tagweight bin according to the final decision
      //the quantile convention for continuous is: bin+1!  
      trfinf.jets.at(i).vars.jetTagWeight = (m_continuous2D) ? bin+0.5 : (m_cuts[bin-1].bcut_low + m_cuts[bin-1].bcut_hig)/2.;
      
      auto code = m_effTool->getScaleFactor(trfinf.jets.at(i).flav, trfinf.jets.at(i).vars, jetSF);
      if(!(code==CorrectionCode::Ok || code==CorrectionCode::OutOfValidityRange)){
        ATH_MSG_ERROR("BTaggingEfficiencyTool::getScaleFactor returned CorrectionCode::Error");
        return StatusCode::FAILURE;
      }
      ATH_MSG_DEBUG("BIN " <<bin <<"SF: pt " <<trfinf.jets.at(i).vars.jetPt <<" " <<trfinf.jets.at(i).vars.jetEta <<" " <<trfinf.jets.at(i).vars.jetTagWeight <<" jetSF " <<jetSF);
    }
    SF*=jetSF;
  } //loop over jets
  
  if(sys!=0 && m_useSys) {  // reset syst to nominal
    CP::SystematicSet defaultSet;
    
    ANA_CHECK( m_effTool->applySystematicVariation(defaultSet) );
  }
  ATH_MSG_DEBUG(" FINAL SF : " <<SF);
  return SF;
}


StatusCode BTaggingTruthTaggingTool::check_syst_range(unsigned int sys){
  if(sys < m_eff_syst.size()) return StatusCode::SUCCESS;
  ATH_MSG_ERROR(sys << " Out of range of available systematics");
  return StatusCode::FAILURE;
}



BTaggingTruthTaggingTool::~BTaggingTruthTaggingTool(){

  // delete trfinf.jets;

}


int BTaggingTruthTaggingTool::jetFlavourLabel (const xAOD::Jet& jet){
  if (m_coneFlavourLabel)
    return (m_oldConeFlavourLabel) ? ConeFinalPartonFlavourLabel(jet) : ExclusiveConeHadronFlavourLabel(jet);
  else
    return GAFinalHadronFlavourLabel(jet);
}

int BTaggingTruthTaggingTool::GAFinalHadronFlavourLabel (const xAOD::Jet& jet) {

  const std::string labelB = "GhostBHadronsFinal";
  const std::string labelC = "GhostCHadronsFinal";
  const std::string labelTau = "GhostTausFinal";

  std::vector<const IParticle*> ghostB;
  if (jet.getAssociatedObjects<IParticle>(labelB, ghostB) && ghostB.size() > 0) return 5;
  std::vector<const IParticle*> ghostC;
  if (jet.getAssociatedObjects<IParticle>(labelC, ghostC) && ghostC.size() > 0) return 4;
  std::vector<const IParticle*> ghostTau;
  if (jet.getAssociatedObjects<IParticle>(labelTau, ghostTau) && ghostTau.size() > 0) return 15;


  return 0;
}

int BTaggingTruthTaggingTool::ConeFinalPartonFlavourLabel (const xAOD::Jet& jet){
  // default label means "invalid"
  int label = -1;
  // First try the new naming scheme
  if (jet.getAttribute("ConeTruthLabelID",label)) return label;
  // If that fails, revert to the old scheme. In this case, further testing is not very useful
  jet.getAttribute("TruthLabelID", label);
  return label;
}

int BTaggingTruthTaggingTool::ExclusiveConeHadronFlavourLabel (const xAOD::Jet& jet){
  // default label means "invalid"
  int label = -1;
  // We don't check the return value, as we would not be able to handle it gracefully anyway
  jet.getAttribute("HadronConeExclTruthLabelID",label);
  return label;
}

// local utility function: trim leading and trailing whitespace in the property strings
std::string trim(const std::string& str,
         const std::string& whitespace = " \t") {
  const auto strBegin = str.find_first_not_of(whitespace);
  if (strBegin == std::string::npos)
    return ""; // no content

  const auto strEnd = str.find_last_not_of(whitespace);
  const auto strRange = strEnd - strBegin + 1;

  return str.substr(strBegin, strRange);
}


std::vector<std::string> BTaggingTruthTaggingTool::split(const std::string& str, char token) {
  std::vector<std::string> result;
  if (str.size() > 0) {
    std::string::size_type end;
    std::string tmp(str);
    do {
      end = tmp.find(token);
      std::string entry = trim(tmp.substr(0,end));
      if (entry.size() > 0) result.push_back(entry);
      if (end != std::string::npos) tmp = tmp.substr(end+1);
    } while (end != std::string::npos);
  }
  return result;
}

StatusCode BTaggingTruthTaggingTool::generateRandomTaggerScores(std::vector< std::vector<int> > &quantiles, std::vector< std::vector<float> > &btag_scores, std::vector< std::vector<float> > &ctag_scores){

  TRandom3 random;
  
  btag_scores.clear();
  btag_scores.resize(quantiles.size());
  
  if(m_continuous2D){
    ctag_scores.clear();
    ctag_scores.resize(quantiles.size());
  }
  
  for(unsigned int i=0; i <quantiles.size(); i++ ){

    btag_scores.at(i).resize(quantiles.at(i).size());
    if(m_continuous2D)
      ctag_scores.at(i).resize(quantiles.at(i).size());

    unsigned int njets = quantiles.at(i).size();
    for(unsigned int j=0; j < njets; j++ ){

      int temp_quantile = (m_continuous2D) ? quantiles.at(i).at(j) : quantiles.at(i).at(j) -1;

      float btag_low =  m_cuts[temp_quantile].bcut_low;
      float btag_hig =  m_cuts[temp_quantile].bcut_hig;
			      
      float ctag_low =  m_cuts[temp_quantile].ccut_low;
      float ctag_hig =  m_cuts[temp_quantile].ccut_hig;

      btag_scores.at(i).at(j) = btag_low + random.Uniform()*(btag_hig - btag_low );
      if(m_continuous2D)
	ctag_scores.at(i).at(j) = ctag_low + random.Uniform()*(ctag_hig - ctag_low );
    }
  }
  return StatusCode::SUCCESS;

}

