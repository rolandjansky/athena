#include "xAODBTaggingEfficiency/BTaggingTruthTaggingTool.h"
#include "xAODBTaggingEfficiency/BTaggingEfficiencyTool.h"
#include "xAODBTagging/BTagging.h"
#include "CalibrationDataInterface/CalibrationDataInterfaceROOT.h"
#include "CalibrationDataInterface/CalibrationDataContainer.h"
#include "PATInterfaces/SystematicRegistry.h"
#include "PathResolver/PathResolver.h"

#include "TFile.h"

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
  declareProperty("CutBenchmark",                    m_cutBenchmark = "FixedCutBEff_70", "if you want to run in continuous you need to fix a benchmark - it does something only if running in Continuous OP");
  declareProperty("ExcludeSpecificEigens",            m_excludeEV = "" ,    "(semicolon-separated) names of Eigens you want to exclude. in case of continuous some eigenvectors can be ignored to make the computation faster");
  declareProperty("StoreOnlyUpVariations",            m_doOnlyUpVariations = false ,    "If set to true it processes only the __1up b-tagging variations. It speeds up the computation in case of symmetric variations.");
  declareProperty("doDirectTagging",                  m_doDirectTag = false ,    "If set to true it also computes and stores the direct tagging choice and the related SFs for each jet");
      
  // if it is empty, the onnx tool won't be initialised
  declareProperty( "pathToONNX",                      m_pathToONNX = "",          "path to the onnx file that will be used for inference");
}

StatusCode BTaggingTruthTaggingTool::setEffMapIndex(const std::string& flavour, unsigned int index){
    ANA_CHECK(m_effTool->setMapIndex(flavour, index));
    if(m_useQuntile && !m_continuous){
      for(unsigned int iop=0; iop<m_availableOP.size(); iop++){
          ANA_CHECK(m_effTool_allOP[m_availableOP.at(iop)]->setMapIndex(flavour, index));
      }
    }
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

  if(m_OP.find("FlatBEff") != std::string::npos){
    m_availableOP.resize(m_availableOP_fixEff.size());
    m_availableOP=m_availableOP_fixEff;
  }
  else if(m_OP.find("FixedCutBEff") != std::string::npos){
    m_availableOP.resize(m_availableOP_fixCut.size());
    m_availableOP=m_availableOP_fixCut;
  }
  else if(m_OP.find("Continuous") != std::string::npos){
    ATH_MSG_INFO("You are running in Continuous and you chose " << m_cutBenchmark <<" as benchmark" );
    m_continuous = true;
    m_availableOP.resize(m_availableOP_fixCut.size());
    m_availableOP=m_availableOP_fixCut;
  }
  else if(m_useQuntile){
    ATH_MSG_ERROR(m_OP << " not in the list of available OPs");
    return StatusCode::FAILURE;
  }

  if(m_continuous){
    m_OperatingPoint_index = find(m_availableOP.begin(), m_availableOP.end(), m_cutBenchmark) - m_availableOP.begin();
  }
  else{
    if(m_useQuntile){
      m_OperatingPoint_index = find(m_availableOP.begin(), m_availableOP.end(), m_OP) - m_availableOP.begin();
      if(m_OperatingPoint_index >= m_availableOP.size()) {
        ATH_MSG_ERROR(m_OP << " not in the list of available OPs");
        return StatusCode::FAILURE;
      }
    }
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
  if(m_useQuntile == true || m_continuous == true){
    ATH_MSG_INFO("m_useQuntile true");

    //open the CDI file to get the cutvalues for all working points.
    TString pathtofile =  PathResolverFindCalibFile(m_SFFile);
    m_inf = TFile::Open(pathtofile, "read");
    m_binEdges.clear();

    for(unsigned int iop=0; iop<m_availableOP.size(); iop++){

      if(!m_continuous){
        std::string toolname = name()+"_eff_"+m_availableOP.at(iop);
        
        m_effTool_allOP[m_availableOP.at(iop)] = asg::AnaToolHandle<IBTaggingEfficiencyTool>("BTaggingEfficiencyTool/"+toolname,this);
        
        ANA_CHECK(m_effTool_allOP[m_availableOP.at(iop)].setProperty("OperatingPoint", m_availableOP.at(iop)));
        ANA_CHECK(m_effTool_allOP[m_availableOP.at(iop)].setProperty("TaggerName",                      m_taggerName));
        ANA_CHECK(m_effTool_allOP[m_availableOP.at(iop)].setProperty("JetAuthor",                       m_jetAuthor));
        ANA_CHECK(m_effTool_allOP[m_availableOP.at(iop)].setProperty("ScaleFactorFileName",             m_SFFile ));
        ANA_CHECK(m_effTool_allOP[m_availableOP.at(iop)].setProperty("UseDevelopmentFile",              m_useDevFile ));
        ANA_CHECK(m_effTool_allOP[m_availableOP.at(iop)].setProperty("EfficiencyFileName",              m_EffFile ));
        ANA_CHECK(m_effTool_allOP[m_availableOP.at(iop)].setProperty("ScaleFactorBCalibration",         m_SFBName ));
        ANA_CHECK(m_effTool_allOP[m_availableOP.at(iop)].setProperty("ScaleFactorCCalibration",         m_SFCName ));
        ANA_CHECK(m_effTool_allOP[m_availableOP.at(iop)].setProperty("ScaleFactorTCalibration",         m_SFTName ));
        ANA_CHECK(m_effTool_allOP[m_availableOP.at(iop)].setProperty("ScaleFactorLightCalibration",     m_SFLightName ));
        ANA_CHECK(m_effTool_allOP[m_availableOP.at(iop)].setProperty("EigenvectorReductionB",           m_EVReductionB ));
        ANA_CHECK(m_effTool_allOP[m_availableOP.at(iop)].setProperty("EigenvectorReductionC",           m_EVReductionC ));
        ANA_CHECK(m_effTool_allOP[m_availableOP.at(iop)].setProperty("EigenvectorReductionLight",       m_EVReductionLight ));
        ANA_CHECK(m_effTool_allOP[m_availableOP.at(iop)].setProperty("EfficiencyBCalibrations",         m_EffBName ));
        ANA_CHECK(m_effTool_allOP[m_availableOP.at(iop)].setProperty("EfficiencyCCalibrations",         m_EffCName ));
        ANA_CHECK(m_effTool_allOP[m_availableOP.at(iop)].setProperty("EfficiencyTCalibrations",         m_EffTName ));
        ANA_CHECK(m_effTool_allOP[m_availableOP.at(iop)].setProperty("EfficiencyLightCalibrations",     m_EffLightName ));
        ANA_CHECK(m_effTool_allOP[m_availableOP.at(iop)].setProperty("ExcludeFromEigenVectorTreatment", m_excludeFromEV ));
        ANA_CHECK(m_effTool_allOP[m_availableOP.at(iop)].setProperty("SystematicsStrategy",             m_systStrategy ));
        ANA_CHECK(m_effTool_allOP[m_availableOP.at(iop)].setProperty("ConeFlavourLabel",                m_coneFlavourLabel ));
        ANA_CHECK(m_effTool_allOP[m_availableOP.at(iop)].setProperty("OldConeFlavourLabel",             m_oldConeFlavourLabel ));
    
        //Add minPt requirements for Jets https://twiki.cern.ch/twiki/bin/view/AtlasProtected/BTagCalibrationRecommendationsRelease21
        if(m_jetAuthor.find("EMPFlow")!= std::string::npos){
          ANA_CHECK(m_effTool_allOP[m_availableOP.at(iop)].setProperty("MinPt", 20e3));
        }
        if(m_jetAuthor.find("EMTopo")!= std::string::npos){
          ANA_CHECK(m_effTool_allOP[m_availableOP.at(iop)].setProperty("MinPt", 20e3));
        }
        if(m_jetAuthor.find("VRTrack")  != std::string::npos ||
            // for AntiKtVR30Rmax4Rmin02TrackJets
            (m_jetAuthor.find("AntiKtVR")  != std::string::npos &&
             m_jetAuthor.find("TrackJets") != std::string::npos)) {
          ANA_CHECK(m_effTool_allOP[m_availableOP.at(iop)].setProperty("MinPt", 10e3));
        }

        ANA_CHECK(m_effTool_allOP[m_availableOP.at(iop)].initialize());
        
        if(m_OP.find("FixedCutBEff")!= std::string::npos){
          TString cutname = m_taggerName+"/"+m_jetAuthor+"/"+m_availableOP.at(iop)+"/cutvalue";
          float cutval = ((TVector*) m_inf->Get(cutname))[0](0);
          m_binEdges.push_back(cutval);
        } //FCBEff
      } //!Continuous

      else{
        if(m_cutBenchmark.find("FixedCutBEff")!= std::string::npos){
              TString cutname = m_taggerName+"/"+m_jetAuthor+"/"+m_availableOP.at(iop)+"/cutvalue";
              float cutval = ((TVector*) m_inf->Get(cutname))[0](0);
              m_binEdges.push_back(cutval);
        } //FCBEff
      } //Continuous
    } //loop
  } //quantile

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
StatusCode BTaggingTruthTaggingTool::setJets(TRFinfo &trfinf,std::vector<int>& flav, const std::vector<Analysis::CalibrationDataVariables>& vars, const std::vector<std::vector<float>>& node_feat){
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
  trfinf.node_feat = node_feat;
  return StatusCode::SUCCESS;
}


bool BTaggingTruthTaggingTool::fillVariables( const xAOD::Jet& jet, CalibrationDataVariables& x){
  x.jetPt = jet.pt();
  x.jetEta = jet.eta();
  x.jetTagWeight = 0.;
  x.jetAuthor = m_jetAuthor;

  const xAOD::BTagging* tagInfo = jet.btagging();
  if (!tagInfo) return false;

  return tagInfo->MVx_discriminant(m_taggerName, x.jetTagWeight);
  delete tagInfo;

}

bool BTaggingTruthTaggingTool::fillVariables( const float jetPt, const float jetEta, const float jetTagWeight, CalibrationDataVariables& x){
  x.jetPt = jetPt;
  x.jetEta = jetEta;
  x.jetTagWeight = jetTagWeight;
  x.jetAuthor = m_jetAuthor;
  return true;
}

float BTaggingTruthTaggingTool::getPermutationRW(TRFinfo &trfinf,bool isIncl, unsigned int nbtag, int sys){
  float w = 1.;
  if(nbtag < trfinf.njets && ((!isIncl && trfinf.permprob_ex.at(nbtag) == 0.) || (isIncl && trfinf.permprob_in.at(nbtag) == 0.)) )  {
      ATH_MSG_ERROR("Permutations need to be chosen before computing reweighting");
  }
  if(nbtag > trfinf.njets) return 1.;
  if(isIncl) {
    w = trfWeight(trfinf,trfinf.perm_in.at(nbtag));
    return w/trfinf.trfwsys_in[sys].at(nbtag)/trfinf.permprob_in.at(nbtag);
  }
  else {
    w = trfWeight(trfinf,trfinf.perm_ex.at(nbtag));
    return w/trfinf.trfwsys_ex[sys].at(nbtag)/trfinf.permprob_ex.at(nbtag);
  }
}



StatusCode BTaggingTruthTaggingTool::GetTruthTagWeights(TRFinfo &trfinf, std::vector<float> &trf_weight_ex, std::vector<float> &trf_weight_in, int sys){

  ANA_CHECK_SET_TYPE (StatusCode);

  if(sys==0) ANA_CHECK(getAllEffMC(trfinf));
  ANA_CHECK(check_syst_range(sys));
  if(trfinf.trfwsys_ex.size()==0)  trfinf.trfwsys_ex.resize(m_eff_syst.size());
  if(trfinf.trfwsys_in.size()==0) trfinf.trfwsys_in.resize(m_eff_syst.size());

  ANA_CHECK(getAllEffSF(trfinf, sys));
  ANA_CHECK(getTRFweight(trfinf,m_nbtag, true, sys));

  if(sys==0){ // don't need permutation or tag bin reweighting
    if(m_usePerm){
      // choice of the selected permutation
      ANA_CHECK(chooseAllTagPermutation(trfinf,m_nbtag));
    }
    if(m_useQuntile) {
      // choice of the tagged bins
      ANA_CHECK(chooseAllTagBins(trfinf));
    }
  }
  else {
    if(m_usePerm) {
      for(unsigned int ib = 0; ib < trfinf.trfwsys_ex[sys].size(); ib++) {
        trfinf.trfwsys_ex[sys].at(ib) *= getPermutationRW(trfinf,false, ib, sys);
        trfinf.trfwsys_in[sys].at(ib) *= getPermutationRW(trfinf,true, ib, sys);
        if(m_useQuntile) {
          trfinf.trfwsys_ex[sys].at(ib) *= getTagBinsRW(trfinf,false, ib);
          trfinf.trfwsys_in[sys].at(ib) *= getTagBinsRW(trfinf,true, ib);
        }
      }
    }
  }
  trf_weight_ex = trfinf.trfwsys_ex[sys];
  trf_weight_in = trfinf.trfwsys_in[sys];
  return StatusCode::SUCCESS;
}


StatusCode BTaggingTruthTaggingTool::CalculateResults(TRFinfo &trfinf, Analysis::TruthTagResults& results, int rand_seed){
  ANA_CHECK_SET_TYPE (StatusCode);
  results.clear();

  results.tagweight_bin_edges = m_binEdges;

  if(rand_seed!=-1){
    trfinf.rand.SetSeed(rand_seed);
  }

  unsigned int n_systs = m_eff_syst.size();
  if(m_useSys){
    trfinf.trfwsys_ex.resize(m_eff_syst.size());
    trfinf.trfwsys_in.resize(m_eff_syst.size());
  }
  else{
    n_systs = 1; //compute only the nominal
    trfinf.trfwsys_ex.resize(1);
    trfinf.trfwsys_in.resize(1);
  }
  std::vector<float> trf_weight_ex,  trf_weight_in;

  for(unsigned int i =0; i< n_systs; i++){
    trf_weight_ex.clear();
    trf_weight_in.clear();

    ANA_CHECK(GetTruthTagWeights(trfinf, trf_weight_ex, trf_weight_in, i));

  }

  results.syst_names.clear();

  for(unsigned int i=0; i<trfinf.trfwsys_ex.size(); i++){
  //  for(unsigned int i=0; i<Nsysts; i++){

    results.map_trf_weight_ex[m_sys_name.at(i)].resize(trfinf.trfwsys_ex.at(i).size());
    results.map_trf_weight_in[m_sys_name.at(i)].resize(trfinf.trfwsys_in.at(i).size());

    //direct tagged SF
    if(m_doDirectTag)
      results.map_SF[m_sys_name.at(i)]=getEvtSF(trfinf,i);
    
    results.syst_names.push_back(m_sys_name.at(i));
    
    for(unsigned int j=0; j< trfinf.trfwsys_ex.at(i).size(); j++){
      results.map_trf_weight_ex[m_sys_name.at(i)].at(j) = trfinf.trfwsys_ex.at(i).at(j);
      results.map_trf_weight_in[m_sys_name.at(i)].at(j) = trfinf.trfwsys_in.at(i).at(j);

    }
  }

  if(m_usePerm){
    ANA_CHECK(getTagPermutation(trfinf,results.trf_chosen_perm_ex,results.trf_chosen_perm_in));
  }
  if(m_useQuntile){
    ANA_CHECK(getQuantiles(trfinf,results.trf_bin_ex, results.trf_bin_in));
    ANA_CHECK(generateRandomTaggerScores(results.trf_bin_ex, results.trf_bin_score_ex));
    ANA_CHECK(generateRandomTaggerScores(results.trf_bin_in, results.trf_bin_score_in));
  }
  if(m_doDirectTag)
    ANA_CHECK(getDirectTaggedJets(trfinf,results.is_tagged));
  
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
StatusCode BTaggingTruthTaggingTool::CalculateResultsONNX(const std::vector<std::vector<float>>& node_feat, std::vector<float>& tagw, Analysis::TruthTagResults& results,int rand_seed){

  ANA_CHECK_SET_TYPE (StatusCode);

  TRFinfo trfinf;
    
  ANA_CHECK(setJets(trfinf, node_feat, tagw));

  return CalculateResults(trfinf,results,rand_seed);
}

// setting inputs that the onnx tool will use
StatusCode BTaggingTruthTaggingTool::CalculateResultsONNX(const xAOD::JetContainer& jets, const std::vector<std::vector<float>>& node_feat, Analysis::TruthTagResults& results,int rand_seed){

  ANA_CHECK_SET_TYPE (StatusCode);

  TRFinfo trfinf;

  ANA_CHECK(setJets(trfinf, jets, node_feat));

  return CalculateResults(trfinf,results,rand_seed);
}

StatusCode BTaggingTruthTaggingTool::getAllEffMC(TRFinfo &trfinf){
  if ( m_pathToONNX == "" ){
    return getAllEffMCCDI(trfinf);
  } else {

    return getAllEffMCGNN(trfinf);
  }
}

// uses onnx tool (no support for m_useQuantile now)
StatusCode BTaggingTruthTaggingTool::getAllEffMCGNN(TRFinfo &trfinf){

  trfinf.effMC.clear();
  if(m_useQuntile == true || m_continuous == true){
    for(auto op_appo: m_availableOP)
      trfinf.effMC_allOP[op_appo].clear();
  }
    
  if (m_continuous){
    std::vector<std::vector<float>> tmp_effMC_allOP; // shape:{num_jets, num_wp}
    CorrectionCode code = m_effTool->getMCEfficiencyONNX(trfinf.node_feat, tmp_effMC_allOP);
    if(!(code==CorrectionCode::Ok || code==CorrectionCode::OutOfValidityRange)){
      ATH_MSG_ERROR("BTaggingEfficiencyTool::getMCEfficiencyONNX returned CorrectionCode::Error");
      return StatusCode::FAILURE;
    }
    
    int OP_index=0;
    for(const auto & op_appo: m_availableOP){
      
      // need to transpose
      std::vector<float> tmp_effMC_oneOP; // shape:{num_jet}
      for (int jet_index=0; jet_index<static_cast<int>(tmp_effMC_allOP.size()); jet_index++){
        tmp_effMC_oneOP.push_back(tmp_effMC_allOP[jet_index][OP_index]);
      }
      trfinf.effMC_allOP[op_appo] = tmp_effMC_oneOP;
      OP_index++;
    }
  } else {
    CorrectionCode code = m_effTool->getMCEfficiencyONNX(trfinf.node_feat, trfinf.effMC);
    if(!(code==CorrectionCode::Ok || code==CorrectionCode::OutOfValidityRange)){
      ATH_MSG_ERROR("BTaggingEfficiencyTool::getMCEfficiencyONNX returned CorrectionCode::Error");
      return StatusCode::FAILURE;
    }
  }
        
  return StatusCode::SUCCESS;
}

StatusCode BTaggingTruthTaggingTool::getAllEffMCCDI(TRFinfo &trfinf){

  float eff =1.;
  float eff_all =1.;
  trfinf.effMC.clear();
  if(m_useQuntile == true || m_continuous == true){
    for(auto op_appo: m_availableOP)
      trfinf.effMC_allOP[op_appo].clear();
  }
  for(size_t i=0; i < trfinf.jets.size(); i++){
    if(m_continuous){
      eff=1.;
      // loop on OP
      int OP_index = 0;
      for(const auto & op_appo: m_availableOP){
        eff_all=1.;
        //set a dumb value of the truth tag weight to get the different efficiency maps for each bin. to be improved..
        if(OP_index+1 < static_cast<int>(m_availableOP.size())){
          trfinf.jets.at(i).vars.jetTagWeight = (m_binEdges.at(OP_index)+m_binEdges.at(OP_index+1))/2.; //to-do: make it fancy? random distribution inside the bin probably?
        }
        else{
          trfinf.jets.at(i).vars.jetTagWeight = (m_binEdges.at(OP_index)+1)/2.; //only for 60% WP
        }

        CorrectionCode code = m_effTool->getMCEfficiency(trfinf.jets.at(i).flav, trfinf.jets.at(i).vars, eff_all) ;

        if(!(code==CorrectionCode::Ok || code==CorrectionCode::OutOfValidityRange)){
          ATH_MSG_ERROR("BTaggingEfficiencyTool::getMCEfficiency returned CorrectionCode::Error");
          return StatusCode::FAILURE;
        }
        trfinf.effMC_allOP[op_appo].push_back(eff_all);
        OP_index++;

      } // end loop on OP
    } //inside Continuous
    else{
      eff=1.;
      CorrectionCode code = m_effTool->getMCEfficiency(trfinf.jets.at(i).flav, trfinf.jets.at(i).vars, eff);
      if(!(code==CorrectionCode::Ok || code==CorrectionCode::OutOfValidityRange)){
        ATH_MSG_ERROR("BTaggingEfficiencyTool::getMCEfficiency returned CorrectionCode::Error");
        return StatusCode::FAILURE;}
      
      trfinf.effMC.push_back(eff);
      
      if(m_useQuntile){
        // loop on OP
        for(const auto & op_appo: m_availableOP){
      
          if ( op_appo==m_OP) {
            trfinf.effMC_allOP[op_appo].push_back(eff);
            continue;
          }
      
          eff_all=1.;
          CorrectionCode code = m_effTool_allOP[op_appo]->getMCEfficiency(trfinf.jets.at(i).flav, trfinf.jets.at(i).vars, eff_all) ;
      
          if(!(code==CorrectionCode::Ok || code==CorrectionCode::OutOfValidityRange)){
            ATH_MSG_ERROR("BTaggingEfficiencyTool::getMCEfficiency returned CorrectionCode::Error");
            return StatusCode::FAILURE;
          }
      
          trfinf.effMC_allOP[op_appo].push_back(eff_all);
        } // end loop on OP
      } // if useQuantile
    } //!useContinuous
  } // end loop on jets
  return StatusCode::SUCCESS;
}

StatusCode BTaggingTruthTaggingTool::getAllEffSF(TRFinfo &trfinf,int sys){
  ANA_CHECK_SET_TYPE ( StatusCode );

  trfinf.eff.clear();
  if(!m_continuous){
    trfinf.eff.resize(trfinf.effMC.size());
    if(m_useQuntile){
      for(const auto & op_appo: m_availableOP){
    trfinf.eff_allOP[op_appo].clear();
    trfinf.eff_allOP[op_appo].resize(trfinf.effMC.size());
      }
    }
  }
  else{
    trfinf.eff.resize(trfinf.effMC_allOP[m_cutBenchmark].size());
    for(const auto & op_appo: m_availableOP){
      trfinf.eff_allOP[op_appo].clear();
      trfinf.eff_allOP[op_appo].resize(trfinf.effMC_allOP[op_appo].size());
    }
  }
  
  if(m_ignoreSF && sys==0){
    if(m_continuous){
      for(int iop = static_cast<int>(m_availableOP.size())-1; iop >= 0; iop--) { // loop on the tighter OPs
        string op_appo = m_availableOP.at(iop);
        if(!m_useQuntile &&  iop < static_cast<int>(m_OperatingPoint_index)) continue;
        for(size_t ieff =0; ieff< trfinf.eff_allOP[op_appo].size(); ieff++){ //jets
          trfinf.eff_allOP[op_appo].at(ieff)= 0;
          for(unsigned int p = iop; p<m_availableOP.size(); p++){ //add all the eff above the WP
            trfinf.eff_allOP[op_appo].at(ieff)+=trfinf.effMC_allOP[m_availableOP.at(p)].at(ieff);
          }
          if( op_appo == m_cutBenchmark)
            trfinf.eff.at(ieff) = trfinf.eff_allOP[m_cutBenchmark].at(ieff);
        } //jets
      } //OP
    } //continuous
    else{
      for(size_t i =0; i< trfinf.effMC.size(); i++){
        trfinf.eff.at(i)=trfinf.effMC.at(i);
        if(m_useQuntile){
          for(const auto & op_appo: m_availableOP){
            trfinf.eff_allOP[op_appo].at(i)=trfinf.effMC_allOP[op_appo].at(i);
          }
        }
      }
    }
    return StatusCode::SUCCESS;
  }

  if(m_ignoreSF && sys==0){
    ATH_MSG_ERROR("Tryig to access SF systematics but ignoring SF");
    return StatusCode::FAILURE;
  }

  float SF =1.;
  float SF_all =1.;

  if(sys!=0 && m_useSys){
    ATH_MSG_DEBUG("applying the syst: " <<m_sys_name[sys]);
    ANA_CHECK( m_effTool->applySystematicVariation(m_eff_syst[sys]) );
    if(m_useQuntile && !m_continuous){
      for(const auto & op_appo: m_availableOP){
        if (op_appo==m_OP) continue;
        ANA_CHECK( m_effTool_allOP[op_appo]->applySystematicVariation(m_eff_syst[sys]) );
      }
    }
  }

  if(m_continuous){
    for(int iop = static_cast<int>(m_availableOP.size())-1; iop >= 0; iop--) {
      std::string op_appo = m_availableOP.at(iop);
      if(!m_useQuntile &&  iop < static_cast<int>(m_OperatingPoint_index)) continue;
      for(size_t i=0; i<trfinf.jets.size(); i++){
      SF=1.;
      //set a dumb value of the truth tag weight to get the different efficiency maps for each bin. to be improved..
      if(iop+1 < static_cast<int>(m_availableOP.size())){
        trfinf.jets.at(i).vars.jetTagWeight = (m_binEdges.at(iop)+m_binEdges.at(iop+1))/2.; //to-do: make it fancy? random distribution for the tagger score
      }
      else{
        trfinf.jets.at(i).vars.jetTagWeight = (m_binEdges.at(iop)+1.)/2.;
      }
    
      CorrectionCode code = m_effTool->getScaleFactor(trfinf.jets.at(i).flav, trfinf.jets.at(i).vars, SF) ;
      if(!(code==CorrectionCode::Ok || code==CorrectionCode::OutOfValidityRange)){
        ATH_MSG_ERROR("BTaggingEfficiencyTool::getScaleFactor returned CorrectionCode::Error");
        return StatusCode::FAILURE;
      }

      trfinf.eff_allOP[op_appo].at(i)=trfinf.effMC_allOP[op_appo].at(i)*SF;

      //now sum all the corrected MC Eff together
      if(iop+1 < static_cast<int>(m_availableOP.size())){
          trfinf.eff_allOP[op_appo].at(i)+=trfinf.eff_allOP[m_availableOP.at(iop+1)].at(i); //they are already corrected for SF
      }
      if( op_appo == m_cutBenchmark)
        trfinf.eff.at(i) = trfinf.eff_allOP[m_cutBenchmark].at(i);
      } //jets
    } //OP
  } //continuous
  
  else{
    for(unsigned int i=0; i<trfinf.jets.size(); i++){
      SF=1.;
      CorrectionCode code = m_effTool->getScaleFactor(trfinf.jets.at(i).flav, trfinf.jets.at(i).vars, SF) ;
      if(!(code==CorrectionCode::Ok || code==CorrectionCode::OutOfValidityRange)){
        ATH_MSG_ERROR("BTaggingEfficiencyTool::getScaleFactor returned CorrectionCode::Error");
        return StatusCode::FAILURE;
      }
      trfinf.eff.at(i) = trfinf.effMC.at(i)*SF;
      if(m_useQuntile){
        // loop on OP
        for(const auto & op_appo: m_availableOP){
          if (op_appo==m_OP) {
            trfinf.eff_allOP[op_appo].at(i)=trfinf.effMC.at(i)*SF;
            continue;
          }
          SF_all=1.;
          CorrectionCode code = m_effTool_allOP[op_appo]->getScaleFactor(trfinf.jets.at(i).flav, trfinf.jets.at(i).vars, SF_all) ;
          if(!(code==CorrectionCode::Ok || code==CorrectionCode::OutOfValidityRange)){
            ATH_MSG_ERROR("BTaggingEfficiencyTool::getScaleFactor returned CorrectionCode::Error");
            return StatusCode::FAILURE;
          }
          
          trfinf.eff_allOP[op_appo].at(i) = trfinf.effMC_allOP[op_appo].at(i)*SF_all;
        } // end loop on OP
      } // if useQuantile
    } //!Continuous
  } //jets
  
  CP::SystematicSet defaultSet;
  ANA_CHECK( m_effTool->applySystematicVariation(defaultSet) );
  if(m_useQuntile && !m_continuous){
    for(const auto & op_appo: m_availableOP){
      ANA_CHECK( m_effTool_allOP[op_appo]->applySystematicVariation(defaultSet) );
    }
  }
  
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
  for (unsigned int j=0; j<tags.size();j++) {
    float trf = 0.;
    trf = trfinf.eff[j];
    if(trf>1.) {
      ATH_MSG_WARNING("Truth Tagging weight > 1. --> setting it to 1. check maps!");
      trf = 1.;
    }
    if(tags.at(j)) weight *= trf;
    else weight *= (1.-trf);
  } // end loop over jets
  return weight;
}


StatusCode BTaggingTruthTaggingTool::getTRFweight(TRFinfo &trfinf,unsigned int nbtag, bool isInclusive, int sys){

  unsigned int njets = trfinf.njets;
  // Consider permutations of njet jets with up to limit b-tags
  unsigned int limit = (njets > 7) ? 8 : njets+1;

  // Permutations: njets, ntags, permutations
  // From .h: std::map<int, std::vector<std::vector<std::vector<bool> > > > trfinf.perms;

  if (trfinf.perms.find(njets)==trfinf.perms.end()){   // if I don't have already saved the possible permutations for njet
    trfinf.perms[njets] = std::vector<std::vector<std::vector<bool> > >(limit);
    for(unsigned int i=0;i<limit;i++)
      trfinf.perms[njets].at(i) = generatePermutations(njets,i);
  }

  trfinf.permsWeight.clear(), trfinf.permsWeight.resize(limit); // trfinf.permsWeight.at(i).at(j): TRF weight of the j-th perm with i b-tags
  trfinf.permsSumWeight.clear(), trfinf.permsSumWeight.resize(limit); // trfinf.permsSumWeight.at(i).at(j): partial sum of  TRF weight of the permutations with i b-tags up to j (0,1,..,j-th) perm. Used in the choice of the selected permutation

  // compute TRF weight
  unsigned int max = nbtag+1; // from 0 to nbtag b-tags --> nbtag+1 positions
  trfinf.trfwsys_ex[sys].clear(), trfinf.trfwsys_in[sys].clear();
  trfinf.trfwsys_ex[sys].resize(max), trfinf.trfwsys_in[sys].resize(max);

  if(sys == 0) { // nominal case --> clear and resize elements of trfinf
    trfinf.perm_ex.clear(), trfinf.perm_in.clear(); // vector<vector<bool>> --> for each number of tags the chosen permutation
    trfinf.perm_ex.resize(max),  trfinf.perm_in.resize(max);
    trfinf.permprob_ex.clear(), trfinf.permprob_in.clear(); // probability of the perm in trfinf.perm_ex/in
    trfinf.permprob_ex.resize(max),  trfinf.permprob_in.resize(max);
  }
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
    //trfinf.permsWeight.at(i).push_back(w); //old way to do it without resizing, keep it for the moment
    //trfinf.permsSumWeight.at(i).push_back(sum);
    trfinf.permsWeight.at(i).at(p) = w;
        trfinf.permsSumWeight.at(i).at(p) = sum;

    ATH_MSG_DEBUG("nbtag = " << i << "  wei = " << w << "  sum = " << sum);
      }
      if(i<limit && i<max) {
      // note: I need to already have the exclusive weights filled to compite the inclusive
        trfinf.trfwsys_ex[sys].at(i) = sum; // sum of TRF weights for all perm with i b-tags
        if(i == 0) trfinf.trfwsys_in[sys].at(0) = 1.;
        else trfinf.trfwsys_in[sys].at(i) = trfinf.trfwsys_in[sys].at(i-1) - trfinf.trfwsys_ex[sys].at(i-1); // P(>=4) = P(>=3) - P(==3)
        ATH_MSG_DEBUG("i = " << i << "  sum = " << sum << "  TRF in " << trfinf.trfwsys_in[0].at(i) << "  ex = " << trfinf.trfwsys_ex[0].at(i));
      }
    }
    ATH_MSG_DEBUG("before return, nbtag = " << nbtag << "  size de trfinf.trfwsys_in[sys] = " << trfinf.trfwsys_in[sys].size());
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
      //      trfinf.permsWeight.at(nbtag).push_back(w); //old way, before resizing: keep it for the moment
      //      trfinf.permsSumWeight.at(nbtag).push_back(sum);
      trfinf.permsWeight.at(nbtag).at(p) = w;
      trfinf.permsSumWeight.at(nbtag).at(p) = sum;
    }
    trfinf.trfwsys_ex[sys].at(nbtag) = sum;
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
        trfinf.permprob_in.at(nbtag) = trfinf.permsWeight.at(trackPerm.at(ip).first).at(trackPerm.at(ip).second) / trfinf.trfwsys_in[0].at(nbtag);
      }
      else {
        trfinf.perm_ex.at(nbtag) = trfinf.perms[trfinf.njets].at(trackPerm.at(ip).first).at(trackPerm.at(ip).second);
        trfinf.permprob_ex.at(nbtag) = trfinf.permsWeight.at(trackPerm.at(ip).first).at(trackPerm.at(ip).second) / trfinf.trfwsys_ex[0].at(nbtag);
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
  // increasing the value by 1 to match conventions in selectionTool
  for(unsigned int i =0; i<trfinf.tbins_ex.size(); i++)
    for(unsigned int j=0; j<trfinf.tbins_ex.at(i).size(); j++)
      trf_bin_ex.at(i).at(j)=trfinf.tbins_ex.at(i).at(j) +1;

  for(unsigned int i =0; i<trfinf.tbins_in.size(); i++)
    for(unsigned int j=0; j<trfinf.tbins_in.at(i).size(); j++)
      trf_bin_in.at(i).at(j)=trfinf.tbins_in.at(i).at(j) +1;

  return StatusCode::SUCCESS;
}

StatusCode BTaggingTruthTaggingTool::chooseAllTagBins(TRFinfo &trfinf){
  ANA_CHECK_SET_TYPE (StatusCode);
  trfinf.tbins_ex.clear();
  trfinf.tbins_in.clear();
  trfinf.tbins_ex.resize( trfinf.trfwsys_ex[0].size());
  trfinf.tbins_in.resize( trfinf.trfwsys_in[0].size());

  trfinf.binsprob_ex.clear();
  trfinf.binsprob_in.clear();
  trfinf.binsprob_ex.resize( trfinf.trfwsys_ex[0].size());
  trfinf.binsprob_in.resize( trfinf.trfwsys_in[0].size());

  if(trfinf.perm_ex.size() != trfinf.perm_in.size()) ATH_MSG_WARNING("Different sizes in exclusive and inclusive permutation choices");

  for(unsigned int nb=0; nb<trfinf.perm_ex.size(); nb++) {
    ANA_CHECK(chooseTagBins_cum(trfinf,trfinf.perm_ex.at(nb), false, nb));
    ANA_CHECK(chooseTagBins_cum(trfinf,trfinf.perm_in.at(nb), true, nb));
  }
  return StatusCode::SUCCESS;
}


// chiara: non posso uniformare il codice con getTagBinsConfProb??
StatusCode BTaggingTruthTaggingTool::chooseTagBins_cum(TRFinfo &trfinf,std::vector<bool> &tagconf, bool isIncl, unsigned int nbtag){
  std::vector<int> btagops;

  std::vector<float> incl;
  float prob = 1.;

  for(unsigned int j=0; j<tagconf.size(); j++) {
    if(tagconf.at(j)) { // tagged jet
      float sum=0.;
      incl.clear();
      for(int iop = static_cast<int>(trfinf.eff_allOP.size())-1; iop >= static_cast<int>(m_OperatingPoint_index); iop--) { // loop on the tighter OPs
        sum = trfinf.eff_allOP[m_availableOP.at(iop)][j];
        incl.push_back(sum);
      }
      float theX = trfinf.rand.Uniform(sum);

      for(unsigned int k=0; k<incl.size(); k++) {
        float valm1 = 0.;
        if(k!=0) valm1 = incl.at(k-1);
        if(incl.at(k) >= theX) {
          btagops.push_back(m_availableOP.size()-k);
        prob *= (incl.at(k)-valm1) / sum;
          break;
        }
      }
    }
    else { // untagged jet
      float sum=0.;
      incl.clear();
      incl.push_back(sum); // fill the "0_0" OP as no real value affected and start the loop at 1
      for(unsigned int iop=0; iop<=m_OperatingPoint_index; iop++) { // need to include the chosen tag cut to have the last MV1 bin of untagged jets filled, start at 0 with cumulative as iop = 0 = first OP
        // sum = 1 - trf here
        sum = 1 - trfinf.eff_allOP[m_availableOP.at(iop)][j];
        incl.push_back(sum);
      }

      float theX = trfinf.rand.Uniform(sum);
      for(unsigned int k=1; k<incl.size(); k++) {
        if(incl.at(k) >= theX){
          btagops.push_back(k-1);
          prob *= (incl.at(k) - incl.at(k-1)) / sum;
          break;
        }
      }
    }
  }
  if(btagops.size() != tagconf.size()) {
    ATH_MSG_ERROR("You should not be here -> wrong size of tag bins vector");
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



float BTaggingTruthTaggingTool::getTagBinsRW(TRFinfo &trfinf,bool isIncl, unsigned int nbtag){

  if((!isIncl && trfinf.binsprob_ex.size() == 0) || (isIncl && trfinf.binsprob_in.size() == 0)) {
    ATH_MSG_ERROR("Need to choose quantiles before computing the reweighting");
    exit(-1);
  }

  float tbw = 0.;
  float prob_sys=1.;

  if(isIncl) {
    prob_sys=getTagBinsConfProb(trfinf,trfinf.tbins_in.at(nbtag));
    tbw = prob_sys/trfinf.binsprob_in.at(nbtag);
    return tbw;
  }
  else {
    prob_sys=getTagBinsConfProb(trfinf,trfinf.tbins_ex.at(nbtag));
    tbw = prob_sys/trfinf.binsprob_ex.at(nbtag);
    return tbw;
  }
}


float BTaggingTruthTaggingTool::getTagBinsConfProb(TRFinfo &trfinf,std::vector<int> &tagws){
  float prob = 1.;
  for(unsigned int j=0; j<tagws.size(); j++) {
    if((unsigned int)tagws.at(j) > m_OperatingPoint_index) { // tagged
      float prevBinW = 0.;
      int mOP = m_availableOP.size();
      if(tagws.at(j) != mOP) { // not the last tag-bin
        prevBinW =  trfinf.eff_allOP[m_availableOP.at(tagws.at(j))][j];
      }
      // prob *= (eff*SF-exactly-that-bin)/(ef*SF-all-tagged-bins)
      // (eff*SF-exactly-that-bin): eff(==60) = eff(70) - eff(60) --> eff(==5) = eff(4)-eff(5)
      prob *= (trfinf.eff_allOP[m_availableOP.at(tagws.at(j)-1)][j] - prevBinW) /  (trfinf.eff_allOP[m_availableOP.at(m_OperatingPoint_index)][j]);
      ATH_MSG_DEBUG("prob " << prob);
    }
    else { // untagged
      float prevBinW = 0.;
      if(tagws.at(j) != 0) {
        prevBinW = 1 - trfinf.eff_allOP[m_availableOP.at(tagws.at(j)-1)][j];
      }
      prob *= ((1 - trfinf.eff_allOP[m_availableOP.at(tagws.at(j))][j]) - prevBinW) / (1 - trfinf.eff_allOP[m_availableOP.at(m_OperatingPoint_index)][j]);
      ATH_MSG_DEBUG("prob " << prob);
    }
  }
  return prob;
}

StatusCode BTaggingTruthTaggingTool::getDirectTaggedJets(TRFinfo &trfinf,std::vector<bool> &is_tagged){
  is_tagged.clear();
  std::vector<int> appo;
  for(const auto jet : trfinf.jets) {
    ATH_MSG_DEBUG("pt " << jet.vars.jetPt << "   eta " << jet.vars.jetEta << "   wei " << jet.vars.jetTagWeight);
    bool is_btagged = false;
    if(!m_continuous)
      is_btagged = m_selTool->accept(jet.vars.jetPt, jet.vars.jetEta, jet.vars.jetTagWeight);
    else{
      int quantile = m_selTool->getQuantile(jet.vars.jetPt, jet.vars.jetEta, jet.vars.jetTagWeight);
      is_btagged = quantile > static_cast<int>(m_OperatingPoint_index) ? true : false;
      ATH_MSG_DEBUG("quantile " <<quantile <<" m_OperatingPoint_index " <<m_OperatingPoint_index <<" is_tagged? " <<is_btagged);

    }

    ATH_MSG_DEBUG("is tagged? " << is_btagged);
    if(is_btagged) is_tagged.push_back(1);
    else is_tagged.push_back(0);
  }

  return StatusCode::SUCCESS;
}


float BTaggingTruthTaggingTool::getEvtSF(TRFinfo &trfinf,int sys){
  ANA_CHECK_SET_TYPE (StatusCode);
  float SF = 1.;
  std::vector<bool> is_tagged;
  ANA_CHECK( getDirectTaggedJets(trfinf,is_tagged) );

  if(sys!=0 && m_useSys) {

    ANA_CHECK( m_effTool->applySystematicVariation(m_eff_syst[sys]) );
  }

  for(unsigned int i =0; i< trfinf.jets.size(); i++) {
    bool is_btagged = is_tagged.at(i);
    float ineffSF =1;
    float effSF=1;

    if(is_btagged){    // tagged --> look at sf
      CorrectionCode code = m_effTool->getScaleFactor(trfinf.jets.at(i).flav, trfinf.jets.at(i).vars, ineffSF);
      if(!(code==CorrectionCode::Ok || code==CorrectionCode::OutOfValidityRange)){
        ATH_MSG_ERROR("BTaggingEfficiencyTool::getMCEfficiency returned CorrectionCode::Error");
        return StatusCode::FAILURE;
      }


      SF*=ineffSF;
    }
    else{    // not tagged --> loop at ineff SF
      CorrectionCode code = m_effTool->getInefficiencyScaleFactor(trfinf.jets.at(i).flav, trfinf.jets.at(i).vars, effSF);
      if(!(code==CorrectionCode::Ok || code==CorrectionCode::OutOfValidityRange)){
        ATH_MSG_ERROR("BTaggingEfficiencyTool::getMCEfficiency returned CorrectionCode::Error");
        return StatusCode::FAILURE;
      }
      SF *= effSF;
    }
  }

  if(sys!=0 && m_useSys) {  // reset syst to nominal
    CP::SystematicSet defaultSet;

    ANA_CHECK( m_effTool->applySystematicVariation(defaultSet) );
  }
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

StatusCode BTaggingTruthTaggingTool::generateRandomTaggerScores(std::vector< std::vector<int> > &quantiles, std::vector< std::vector<float> > &scores){

  //quantiles:
  // returns 5 if between 60% and 0%
  // returns 4 if between 70% and 60%
  // returns 3 if between 77% and 70%
  // returns 2 if between 85% and 77%
  // returns 1 if between 100% and 85%
  //m_binEdges
  // 3 60%
  // 2 70%
  // 1 77%
  // 0 85%

  scores.clear();

  TRandom3 random;

  scores.resize(quantiles.size());
  for(unsigned int i=0; i <quantiles.size(); i++ ){

    scores.at(i).resize(quantiles.at(i).size());

    for(unsigned int j=0; j <quantiles.at(i).size(); j++ ){

        int quantile = quantiles.at(i).at(j);

        if(quantile == 1){
          float lowTaggerScore = -1.0;
          if(m_taggerName.find("MV2") != string::npos){ lowTaggerScore = -1.0; }
          if(m_taggerName.find("DL1") != string::npos){ lowTaggerScore = -20.0;}

          scores.at(i).at(j) = lowTaggerScore + random.Uniform()*( m_binEdges.at(0)-lowTaggerScore );
        }else if(quantile == 5){
          float highTaggerScore = +1.0;
          if(m_taggerName.find("MV2") != string::npos){ highTaggerScore = 1.0; }
          if(m_taggerName.find("DL1") != string::npos){ highTaggerScore = 20.0; }

          scores.at(i).at(j) = m_binEdges.at(3) + random.Uniform()*( highTaggerScore-m_binEdges.at(3) );
        }else{
          scores.at(i).at(j) = m_binEdges.at(quantile-2) + random.Uniform()*( m_binEdges.at(quantile-1)-m_binEdges.at(quantile-2) );
        }
    }
  }

  return StatusCode::SUCCESS;

}

