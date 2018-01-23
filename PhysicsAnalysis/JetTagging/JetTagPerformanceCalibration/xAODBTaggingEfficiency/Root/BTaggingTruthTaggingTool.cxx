#include "xAODBTaggingEfficiency/BTaggingTruthTaggingTool.h"
#include "xAODBTagging/BTagging.h"
#include "CalibrationDataInterface/CalibrationDataInterfaceROOT.h"
#include "CalibrationDataInterface/CalibrationDataContainer.h"
#include "PATInterfaces/SystematicRegistry.h"
#include "PathResolver/PathResolver.h"

#include "TFile.h"

#include <algorithm>

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
  : asg::AsgTool( name ){

  m_initialised = false;

  //  m_taggerName, m_OP, m_jetAuthor, m_SFfile

  // properties of BTaggingTruthTaggingTool
  declareProperty( "IgnoreScaleFactors", m_ignoreSF=true, "ignore scale factors in computation of TRF weight");
  declareProperty( "UsePermutations", m_usePerm=true, "if the chosen permutation is used, a reweighting is applied to the TRF weight for systematics");
  declareProperty( "UseQuantile", m_useQuntile=true, "if the chosen quantile is used, a reweighting is applied to the TRF weight for systematics");


  // properties of BtaggingSelectionTool
  declareProperty( "MaxEta", m_maxEta = 2.5 );
  declareProperty( "MinPt", m_minPt = 20000 /*MeV*/);
  declareProperty( "MaxRangePt", m_maxRangePt = 1000000 /*MeV*/);

  // I put it = to CDI file for SF
  //  declareProperty( "FlvTagCutDefinitionsFileName", m_CutFileName = "", "name of the files containing official cut definitions (uses PathResolver)");

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
  // declareProperty("ExcludeJESFromEVTreatment",       m_excludeJESFromEV = true,  "specify whether or not to exclude JES uncertainties from eigenvector decomposition (if used)");
  declareProperty("SystematicsStrategy",             m_systStrategy = "SFEigen", "name of systematics model; presently choose between 'SFEigen' and 'Envelope'");
  declareProperty("ConeFlavourLabel",                m_coneFlavourLabel = true, "specify whether or not to use the cone-based flavour labelling instead of the default ghost association based labelling");
  declareProperty("OldConeFlavourLabel",          m_oldConeFlavourLabel = false, "when using cone-based flavour labelling, specify whether or not to use the (deprecated) Run-1 legacy labelling");

}

StatusCode BTaggingTruthTaggingTool::setEffMapIndex(const std::string& flavour, unsigned int index){
    ANA_CHECK(m_effTool->setMapIndex(flavour, index));
    if(m_useQuntile){
      for(unsigned int iop=0; iop<m_availableOP.size(); iop++){
	ANA_CHECK(m_effTool_allOP[m_availableOP.at(iop)]->setMapIndex(flavour, index));
      }
    }
    return  StatusCode::SUCCESS;
}


StatusCode BTaggingTruthTaggingTool::initialize() {

  ANA_CHECK_SET_TYPE (StatusCode);

  m_jets = new std::vector<jetVariable>(0);

  m_initialised = true;

  m_selTool = new BTaggingSelectionTool(name()+"_sel");

  ANA_CHECK(m_selTool->setProperty("MaxEta",  m_maxEta));
  ANA_CHECK(m_selTool->setProperty("MaxRangePt",  m_maxRangePt));
  ANA_CHECK(m_selTool->setProperty("MinPt",  m_minPt));
  ANA_CHECK(m_selTool->setProperty("TaggerName",  m_taggerName));
  ANA_CHECK(m_selTool->setProperty("OperatingPoint", m_OP));
  ANA_CHECK(m_selTool->setProperty("JetAuthor", m_jetAuthor));
  ANA_CHECK(m_selTool->setProperty("FlvTagCutDefinitionsFileName", m_SFFile));
  ANA_CHECK(m_selTool->initialize());

  m_effTool = new BTaggingEfficiencyTool(name()+"_eff");
  ANA_CHECK(m_effTool->setProperty("TaggerName",                      m_taggerName));
  ANA_CHECK(m_effTool->setProperty("OperatingPoint",                  m_OP));
  ANA_CHECK(m_effTool->setProperty("JetAuthor",                       m_jetAuthor));
  ANA_CHECK(m_effTool->setProperty("ScaleFactorFileName",             m_SFFile ));
  ANA_CHECK(m_effTool->setProperty("UseDevelopmentFile",              m_useDevFile ));
  ANA_CHECK(m_effTool->setProperty("EfficiencyFileName",              m_EffFile ));
  ANA_CHECK(m_effTool->setProperty("ScaleFactorBCalibration",         m_SFBName ));
  ANA_CHECK(m_effTool->setProperty("ScaleFactorCCalibration",         m_SFCName ));
  ANA_CHECK(m_effTool->setProperty("ScaleFactorTCalibration",         m_SFTName ));
  ANA_CHECK(m_effTool->setProperty("ScaleFactorLightCalibration",     m_SFLightName ));
  ANA_CHECK(m_effTool->setProperty("EigenvectorReductionB",           m_EVReductionB ));
  ANA_CHECK(m_effTool->setProperty("EigenvectorReductionC",           m_EVReductionC ));
  ANA_CHECK(m_effTool->setProperty("EigenvectorReductionLight",       m_EVReductionLight ));
  ANA_CHECK(m_effTool->setProperty("EfficiencyBCalibrations",         m_EffBName ));
  ANA_CHECK(m_effTool->setProperty("EfficiencyCCalibrations",         m_EffCName ));
  ANA_CHECK(m_effTool->setProperty("EfficiencyTCalibrations",         m_EffTName ));
  ANA_CHECK(m_effTool->setProperty("EfficiencyLightCalibrations",     m_EffLightName ));
  ANA_CHECK(m_effTool->setProperty("ExcludeFromEigenVectorTreatment", m_excludeFromEV ));
  // ANA_CHECK(m_effTool->setProperty("ExcludeJESFromEVTreatment",       m_excludeJESFromEV ));
  ANA_CHECK(m_effTool->setProperty("SystematicsStrategy",             m_systStrategy ));
  ANA_CHECK(m_effTool->setProperty("ConeFlavourLabel",                m_coneFlavourLabel ));
  ANA_CHECK(m_effTool->setProperty("OldConeFlavourLabel",          m_oldConeFlavourLabel ));

  ANA_CHECK(m_effTool->initialize());

  if(m_OP.find("FlatBEff") != std::string::npos){
    m_availableOP.resize(m_availableOP_fixEff.size());
    m_availableOP=m_availableOP_fixEff;
  }
  else{
    m_availableOP.resize(m_availableOP_fixCut.size());
    m_availableOP=m_availableOP_fixCut;
  }

  m_OperatingPoint_index = find(m_availableOP.begin(), m_availableOP.end(), m_OP) - m_availableOP.begin();
  if(m_OperatingPoint_index >= m_availableOP.size()) {
    ATH_MSG_ERROR(m_OP << " not in the list of available OPs");
  }

  m_eff_syst.clear();
  m_sys_name.clear();
  CP::SystematicSet def_set;
  m_eff_syst.push_back(def_set);
  m_sys_name.push_back("Nominal");

  CP::SystematicSet systs = m_effTool->affectingSystematics();
  for (auto syst : systs) {
    CP::SystematicSet myset;
    myset.insert(syst);
    ATH_MSG_DEBUG("Adding systematic " << syst.name() << "to the list ");
    m_eff_syst.push_back(myset);
    m_sys_name.push_back(syst.name());
  }

  if(m_useQuntile){
    ATH_MSG_INFO("m_useQuantile true");
    for(unsigned int iop=0; iop<m_availableOP.size(); iop++){
      std::string toolname = name()+"_eff_"+m_availableOP.at(iop);
      if(iop == m_OperatingPoint_index){
	m_effTool_allOP[m_availableOP.at(iop)] = m_effTool;
	continue;
      }
      else {
	m_effTool_allOP[m_availableOP.at(iop)] = new BTaggingEfficiencyTool(toolname);
      }

      ANA_CHECK(m_effTool_allOP[m_availableOP.at(iop)]->setProperty("OperatingPoint", m_availableOP.at(iop)));
      ANA_CHECK(m_effTool_allOP[m_availableOP.at(iop)]->setProperty("TaggerName",                      m_taggerName));
      ANA_CHECK(m_effTool_allOP[m_availableOP.at(iop)]->setProperty("JetAuthor",                       m_jetAuthor));
      ANA_CHECK(m_effTool_allOP[m_availableOP.at(iop)]->setProperty("ScaleFactorFileName",             m_SFFile ));
      ANA_CHECK(m_effTool_allOP[m_availableOP.at(iop)]->setProperty("UseDevelopmentFile",              m_useDevFile ));
      ANA_CHECK(m_effTool_allOP[m_availableOP.at(iop)]->setProperty("EfficiencyFileName",              m_EffFile ));
      ANA_CHECK(m_effTool_allOP[m_availableOP.at(iop)]->setProperty("ScaleFactorBCalibration",         m_SFBName ));
      ANA_CHECK(m_effTool_allOP[m_availableOP.at(iop)]->setProperty("ScaleFactorCCalibration",         m_SFCName ));
      ANA_CHECK(m_effTool_allOP[m_availableOP.at(iop)]->setProperty("ScaleFactorTCalibration",         m_SFTName ));
      ANA_CHECK(m_effTool_allOP[m_availableOP.at(iop)]->setProperty("ScaleFactorLightCalibration",     m_SFLightName ));
      ANA_CHECK(m_effTool_allOP[m_availableOP.at(iop)]->setProperty("EigenvectorReductionB",           m_EVReductionB ));
      ANA_CHECK(m_effTool_allOP[m_availableOP.at(iop)]->setProperty("EigenvectorReductionC",           m_EVReductionC ));
      ANA_CHECK(m_effTool_allOP[m_availableOP.at(iop)]->setProperty("EigenvectorReductionLight",       m_EVReductionLight ));
      ANA_CHECK(m_effTool_allOP[m_availableOP.at(iop)]->setProperty("EfficiencyBCalibrations",         m_EffBName ));
      ANA_CHECK(m_effTool_allOP[m_availableOP.at(iop)]->setProperty("EfficiencyCCalibrations",         m_EffCName ));
      ANA_CHECK(m_effTool_allOP[m_availableOP.at(iop)]->setProperty("EfficiencyTCalibrations",         m_EffTName ));
      ANA_CHECK(m_effTool_allOP[m_availableOP.at(iop)]->setProperty("EfficiencyLightCalibrations",     m_EffLightName ));
      ANA_CHECK(m_effTool_allOP[m_availableOP.at(iop)]->setProperty("ExcludeFromEigenVectorTreatment", m_excludeFromEV ));
      // ANA_CHECK(m_effTool_allOP[m_availableOP.at(iop)]->setProperty("ExcludeJESFromEVTreatment",       m_excludeJESFromEV ));
      ANA_CHECK(m_effTool_allOP[m_availableOP.at(iop)]->setProperty("SystematicsStrategy",             m_systStrategy ));
      ANA_CHECK(m_effTool_allOP[m_availableOP.at(iop)]->setProperty("ConeFlavourLabel",                m_coneFlavourLabel ));
      ANA_CHECK(m_effTool_allOP[m_availableOP.at(iop)]->setProperty("OldConeFlavourLabel",          m_oldConeFlavourLabel ));

      ANA_CHECK(m_effTool_allOP[m_availableOP.at(iop)]->initialize());
    }
  }
  return StatusCode::SUCCESS;
}


StatusCode BTaggingTruthTaggingTool::setJets(std::vector<double>* pt, std::vector<double>* eta, std::vector<int>* flav, std::vector<double>* tagw){
  ANA_CHECK_SET_TYPE (StatusCode);
  if(pt->size()!=eta->size() || pt->size()!=flav->size() || pt->size()!=tagw->size()){
    ATH_MSG_ERROR( "Vectors of pt, eta, flav and tagw should have same size" );
    return StatusCode::FAILURE;
  }

  std::vector<Analysis::CalibrationDataVariables>* vars = new std::vector<Analysis::CalibrationDataVariables>(0);
  for(unsigned int i =0; i<pt->size(); i++){
    Analysis::CalibrationDataVariables vars_appo;
    if (!fillVariables(pt->at(i), eta->at(i), tagw->at(i), vars_appo)){
      ATH_MSG_ERROR("unable to fill variables");
      return StatusCode::FAILURE;
    }
    vars->push_back(vars_appo);
  }

  ANA_CHECK(setJets(flav, vars));
  delete vars;
  return StatusCode::SUCCESS;
}

StatusCode BTaggingTruthTaggingTool::setJets(const xAOD::JetContainer*& jets){
  ANA_CHECK_SET_TYPE (StatusCode);
  std::vector<int> * flav = new std::vector<int>(0);
  std::vector<Analysis::CalibrationDataVariables>* vars = new std::vector<Analysis::CalibrationDataVariables>(0);
  for(const auto jet : *jets) {
    Analysis::CalibrationDataVariables vars_appo;
    if (!fillVariables(*jet, vars_appo)){
      ATH_MSG_ERROR("unable to fill variables");
      return StatusCode::FAILURE;
    }
    vars->push_back(vars_appo);
    flav->push_back(jetFlavourLabel(*jet));
  }

  ANA_CHECK(setJets(flav, vars));
  delete vars;
  delete flav;
  return StatusCode::SUCCESS;
}

StatusCode BTaggingTruthTaggingTool::setJets(std::vector<int>* flav, std::vector<Analysis::CalibrationDataVariables>* vars){
  if(flav->size()!=vars->size()){
    ATH_MSG_ERROR( "Vector of CalibrationDataVariables and flavour should have same size" );
    return StatusCode::FAILURE;
  }
  m_jets->clear();
  for(unsigned int i =0; i<vars->size(); i++){
    jetVariable jetVar_appo;
    jetVar_appo.flav=flav->at(i);
    jetVar_appo.vars=vars->at(i);
    m_jets->push_back(jetVar_appo);
  }
  m_njets=m_jets->size();
  for(unsigned int i =0; i< m_jets->size(); i++){
    //    m_jets->at(i).print();
  }
  return StatusCode::SUCCESS;
}



bool BTaggingTruthTaggingTool::fillVariables( const xAOD::Jet & jet, CalibrationDataVariables& x){
  x.jetPt = jet.pt();
  x.jetEta = jet.eta();
  x.jetTagWeight = 0.;
  x.jetAuthor = m_jetAuthor;
  //bool weightOK = true;
  //  if (m_isContinuous) {
  const xAOD::BTagging* tagInfo = jet.btagging();
  if (!tagInfo) return false;
  // x.jetTagWeight = (tagInfo->*m_getTagWeight)();
  return tagInfo->MVx_discriminant(m_taggerName, x.jetTagWeight);
  delete tagInfo;
  //  }
  //  return true;
}

bool BTaggingTruthTaggingTool::fillVariables( const double jetPt, const double jetEta, const double jetTagWeight, CalibrationDataVariables& x){
  x.jetPt = jetPt;
  x.jetEta = jetEta;
  x.jetTagWeight = jetTagWeight;
  x.jetAuthor = m_jetAuthor;
  return true;
}

double BTaggingTruthTaggingTool::getPermutationRW(bool isIncl, unsigned int nbtag, int sys){
  double w = 1.;
  if(nbtag < m_njets && ((!isIncl && m_trfRes.permprob_ex.at(nbtag) == 0.) || (isIncl && m_trfRes.permprob_in.at(nbtag) == 0.)) )  {
    ATH_MSG_ERROR("Permutations need to be chosen before computing reweighting");
  }
  if(nbtag > m_njets) return 1.;
  if(isIncl) {
    w = trfWeight(m_trfRes.perm_in.at(nbtag));
    return w/m_trfRes.trfwsys_in[sys].at(nbtag)/m_trfRes.permprob_in.at(nbtag);
  }
  else {
    w = trfWeight(m_trfRes.perm_ex.at(nbtag));
    return w/m_trfRes.trfwsys_ex[sys].at(nbtag)/m_trfRes.permprob_ex.at(nbtag);
  }
}


StatusCode BTaggingTruthTaggingTool::getTruthTagWei(unsigned int nbtag, std::map<std::string,std::vector<double> > &map_trf_weight_ex, std::map<std::string,std::vector<double> > &map_trf_weight_in){
  ANA_CHECK_SET_TYPE (StatusCode);
  std::vector<std::vector<double> > trf_ex, trf_in;
  ANA_CHECK(getTruthTagWei(nbtag, trf_ex, trf_in));
  for(unsigned int i=0; i<trf_ex.size(); i++){
    map_trf_weight_ex[m_sys_name.at(i)].resize(trf_ex.at(i).size());
    map_trf_weight_ex[m_sys_name.at(i)]=trf_ex.at(i);
    map_trf_weight_in[m_sys_name.at(i)].resize(trf_in.at(i).size());
    map_trf_weight_in[m_sys_name.at(i)]=trf_in.at(i);
  }
  return StatusCode::SUCCESS;
}

StatusCode BTaggingTruthTaggingTool::getTruthTagWei(unsigned int nbtag, std::vector<std::vector<double> > &map_trf_weight_ex, std::vector<std::vector<double> > &map_trf_weight_in){

  ANA_CHECK_SET_TYPE (StatusCode);

  m_trfRes.trfwsys_ex.resize(m_eff_syst.size());
  m_trfRes.trfwsys_in.resize(m_eff_syst.size());

  std::vector<double> trf_weight_ex,  trf_weight_in;
  //  trf_weight_ex.clear();
  //  trf_weight_in.clear();
  //  ANA_CHECK( getTruthTagWei(nbtag, trf_weight_ex, trf_weight_in, 0));

  //for (auto& x: m_eff_syst) {
  for(unsigned int i =0; i< m_eff_syst.size(); i++){
    trf_weight_ex.clear();
    trf_weight_in.clear();
    ANA_CHECK(getTruthTagWei(nbtag, trf_weight_ex, trf_weight_in, i));
  }

  map_trf_weight_ex.resize(m_trfRes.trfwsys_ex.size());
  map_trf_weight_in.resize(m_trfRes.trfwsys_in.size());
  for(unsigned int i=0; i<m_trfRes.trfwsys_ex.size(); i++){
    map_trf_weight_ex.at(i).resize(m_trfRes.trfwsys_ex.at(i).size());
    map_trf_weight_in.at(i).resize(m_trfRes.trfwsys_in.at(i).size());
    for(unsigned int j=0; j< map_trf_weight_in.at(i).size(); j++){
      map_trf_weight_ex.at(i).at(j) = m_trfRes.trfwsys_ex.at(i).at(j);
      map_trf_weight_in.at(i).at(j) = m_trfRes.trfwsys_in.at(i).at(j);
    }
  }

  /*  for (auto& x: m_trfRes.trfwsys_ex) {
    map_trf_weight_ex[x.first].resize(x.second.size());
    map_trf_weight_ex[x.first]=x.second;
  }
  for (auto& x: m_trfRes.trfwsys_in) {
    map_trf_weight_in[x.first].resize(x.second.size());
    map_trf_weight_in[x.first]=x.second;
    }*/
  return StatusCode::SUCCESS;
}


StatusCode BTaggingTruthTaggingTool::getTruthTagWei(unsigned int nbtag, std::vector<double> &trf_weight_ex, std::vector<double> &trf_weight_in, int sys){

  ANA_CHECK_SET_TYPE (StatusCode);
  if(sys==0) ANA_CHECK(getAllEffMC());
  ANA_CHECK(check_syst_range(sys));
  if(m_trfRes.trfwsys_ex.size()==0)  m_trfRes.trfwsys_ex.resize(m_eff_syst.size());
  if(m_trfRes.trfwsys_in.size()==0) m_trfRes.trfwsys_in.resize(m_eff_syst.size());

  ANA_CHECK(getAllEffSF(sys));
  ANA_CHECK(getTRFweight(nbtag, true, sys));

  if(sys==0){ // don't need permutation or tag bin reweighting
    if(m_usePerm){
      // choice of the selected permutation
      ANA_CHECK(chooseAllTagPermutation(nbtag));
    }
    if(m_useQuntile) {
      // choice of the tagged bins
      ANA_CHECK(chooseAllTagBins());
    }
  }
  else {
    if(m_usePerm) {
      for(unsigned int ib = 0; ib < m_trfRes.trfwsys_ex[sys].size(); ib++) {
        m_trfRes.trfwsys_ex[sys].at(ib) *= getPermutationRW(false, ib, sys);
        m_trfRes.trfwsys_in[sys].at(ib) *= getPermutationRW(true, ib, sys);
        if(m_useQuntile) {
	  m_trfRes.trfwsys_ex[sys].at(ib) *= getTagBinsRW(false, ib);
	  m_trfRes.trfwsys_in[sys].at(ib) *= getTagBinsRW(true, ib);
        }
      }
    }
  }
  trf_weight_ex = m_trfRes.trfwsys_ex[sys];
  trf_weight_in = m_trfRes.trfwsys_in[sys];
  return StatusCode::SUCCESS;
}


StatusCode BTaggingTruthTaggingTool::getAllEffMC(){
  //  ANA_CHECK_SET_TYPE (CP::CorrectionCode);
  float eff =1.;
  float eff_all =1.;
  m_trfRes.effMC.clear();
  if(m_useQuntile){
    for(auto op_appo: m_availableOP)
      m_trfRes.effMC_allOP[op_appo].clear();
  }
  for(unsigned int i=0; i<m_jets->size(); i++){
    eff=1.;
    m_effTool->getMCEfficiency(m_jets->at(i).flav, m_jets->at(i).vars, eff);
    m_trfRes.effMC.push_back(eff);
    if(m_useQuntile){
      // loop on OP
      for(auto op_appo: m_availableOP){
	if (op_appo==m_OP) {
	  m_trfRes.effMC_allOP[op_appo].push_back(eff);
	  continue;
	}
	eff_all=1.;
	m_effTool_allOP[op_appo]->getMCEfficiency(m_jets->at(i).flav, m_jets->at(i).vars, eff_all);
	m_trfRes.effMC_allOP[op_appo].push_back(eff_all);
      } // end loop on OP
    } // if useQuantile
  } // end loop on jets
  return StatusCode::SUCCESS;
}


StatusCode BTaggingTruthTaggingTool::getAllEffSF(int sys){
  //  ANA_CHECK_SET_TYPE (CP::CorrectionCode);

  m_trfRes.eff.clear();
  m_trfRes.eff.resize(m_trfRes.effMC.size());
  if(m_useQuntile){
    for(auto op_appo: m_availableOP){
      m_trfRes.eff_allOP[op_appo].clear();
      m_trfRes.eff_allOP[op_appo].resize(m_trfRes.effMC.size());
    }
  }

  if(m_ignoreSF && sys==0){
    for(unsigned int i =0; i< m_trfRes.effMC.size(); i++){
      m_trfRes.eff.at(i)=m_trfRes.effMC.at(i);
      if(m_useQuntile){
	for(auto op_appo: m_availableOP){
	  m_trfRes.eff_allOP[op_appo].at(i)=m_trfRes.effMC_allOP[op_appo].at(i);
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

  if(sys!=0){
    m_effTool->applySystematicVariation(m_eff_syst[sys]);
    if(m_useQuntile){
      for(auto op_appo: m_availableOP){
	if (op_appo==m_OP) continue;
	m_effTool_allOP[op_appo]->applySystematicVariation(m_eff_syst[sys]);
      }
    }
  }

  for(unsigned int i=0; i<m_jets->size(); i++){
    SF=1.;
    m_effTool->getScaleFactor(m_jets->at(i).flav, m_jets->at(i).vars, SF);
    m_trfRes.eff.at(i) = m_trfRes.effMC.at(i)*SF;
    if(m_useQuntile){
      // loop on OP
      for(auto op_appo: m_availableOP){
	if (op_appo==m_OP) {
	  m_trfRes.eff_allOP[op_appo].at(i)=m_trfRes.effMC.at(i)*SF;
	  continue;
	}
	SF_all=1.;
	m_effTool_allOP[op_appo]->getScaleFactor(m_jets->at(i).flav, m_jets->at(i).vars, SF_all);
	m_trfRes.eff_allOP[op_appo].at(i) = m_trfRes.effMC_allOP[op_appo].at(i)*SF_all;
      } // end loop on OP
    } // if useQuantile
  }

  CP::SystematicSet defaultSet;
  m_effTool->applySystematicVariation(defaultSet);
  if(m_useQuntile){
    for(auto op_appo: m_availableOP){
      m_effTool_allOP[op_appo]->applySystematicVariation(defaultSet);
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


double BTaggingTruthTaggingTool::trfWeight(const std::vector<bool> &tags){
  double weight = 1;
  for (unsigned int j=0; j<tags.size();j++) {
    double trf = 0.;
    trf = m_trfRes.eff[j];
    if(trf>1.) {
      ATH_MSG_WARNING("Truth Tagging weight > 1. --> setting it to 1. check maps!");
      trf = 1.;
    }
    if(tags.at(j)) weight *= trf;
    else weight *= (1.-trf);
  } // end loop over jets
  return weight;
}


StatusCode BTaggingTruthTaggingTool::getTRFweight(unsigned int nbtag, bool isInclusive, int sys){

  unsigned int njets = m_njets;
  // Consider permutations of njet jets with up to limit b-tags
  unsigned int limit = (njets > 7) ? 8 : njets+1;

  // Permutations: njets, ntags, permutations
  // From .h: std::map<int, std::vector<std::vector<std::vector<bool> > > > m_perms;

  if (m_perms.find(njets)==m_perms.end()){   // if I don't have already saved the possible permutations for njet
    m_perms[njets] = std::vector<std::vector<std::vector<bool> > >(limit);
    for(unsigned int i=0;i<limit;i++)
      m_perms[njets].at(i) = generatePermutations(njets,i);
  }

  m_permsWeight.clear(), m_permsWeight.resize(limit); // m_permsWeight.at(i).at(j): TRF weight of the j-th perm with i b-tags
  m_permsSumWeight.clear(), m_permsSumWeight.resize(limit); // m_permsSumWeight.at(i).at(j): partial sum of  TRF weight of the permutations with i b-tags up to j (0,1,..,j-th) perm. Used in the choice of the selected permutation

  // compute TRF weight
  unsigned int max = nbtag+1; // from 0 to nbtag b-tags --> nbtag+1 positions
  m_trfRes.trfwsys_ex[sys].clear(), m_trfRes.trfwsys_in[sys].clear();
  m_trfRes.trfwsys_ex[sys].resize(max), m_trfRes.trfwsys_in[sys].resize(max);

  if(sys == 0) { // nominal case --> clear and resize elements of m_trfRes
    m_trfRes.perm_ex.clear(), m_trfRes.perm_in.clear(); // vector<vector<bool>> --> for each number of tags the chosen permutation
    m_trfRes.perm_ex.resize(max),  m_trfRes.perm_in.resize(max);
    m_trfRes.permprob_ex.clear(), m_trfRes.permprob_in.clear(); // probability of the perm in m_trfRes.perm_ex/in
    m_trfRes.permprob_ex.resize(max),  m_trfRes.permprob_in.resize(max);
  }
  if(isInclusive) {
    for(unsigned int i=0; i<limit; i++) { // note: I consider maximum limit tags. It's an approximation
      std::vector<double> weights;
      double sum = 0., w = 0.;
      // loop on all the permutations with i tags
      for(unsigned int p=0; p<m_perms[njets].at(i).size(); p++) {
  	w = trfWeight(m_perms[njets].at(i).at(p));
  	sum+=w;
  	m_permsWeight.at(i).push_back(w);
  	m_permsSumWeight.at(i).push_back(sum);
	ATH_MSG_DEBUG("nbtag = " << i << "  wei = " << w << "  sum = " << sum);
      }
      if(i<limit && i<max) {
	// note: I need to already have the exclusive weights filled to compite the inclusive
  	m_trfRes.trfwsys_ex[sys].at(i) = sum; // sum of TRF weights for all perm with i b-tags
  	if(i == 0) m_trfRes.trfwsys_in[sys].at(0) = 1.;
  	else m_trfRes.trfwsys_in[sys].at(i) = m_trfRes.trfwsys_in[sys].at(i-1) - m_trfRes.trfwsys_ex[sys].at(i-1); // P(>=4) = P(>=3) - P(==3)
	ATH_MSG_DEBUG("i = " << i << "  sum = " << sum << "  TRF in " << m_trfRes.trfwsys_in[0].at(i) << "  ex = " << m_trfRes.trfwsys_ex[0].at(i));
      }
    }
    ATH_MSG_DEBUG("before return, nbtag = " << nbtag << "  size de m_trfRes.trfwsys_in[sys] = " << m_trfRes.trfwsys_in[sys].size());
    return StatusCode::SUCCESS;
  }
  else { // exclusive case, only one calculation needed
    std::vector<double> weights;
    double sum = 0., w = 0.;
    // loop on all the permutations with i tags
    for(unsigned int p=0; p<m_perms[njets].at(nbtag).size(); p++) {
      w = trfWeight(m_perms[njets].at(nbtag).at(p));
      sum+=w;
      m_permsWeight.at(nbtag).push_back(w);
      m_permsSumWeight.at(nbtag).push_back(sum);
    }
    m_trfRes.trfwsys_ex[sys].at(nbtag) = sum;
    return StatusCode::SUCCESS;
  }
  return StatusCode::SUCCESS;
}


StatusCode BTaggingTruthTaggingTool::getTagPermutation(unsigned int nbtag, std::vector<std::vector<bool> > &trf_chosen_perm_ex, std::vector<std::vector<bool> > &trf_chosen_perm_in){
  trf_chosen_perm_ex.resize(nbtag+1);
  trf_chosen_perm_in.resize(nbtag+1);
  trf_chosen_perm_ex = m_trfRes.perm_ex;
  trf_chosen_perm_in = m_trfRes.perm_in;
  std::string print_perm = "Permutation: ";
  for(auto perm: m_trfRes.perm_ex){
    for(auto is: perm) {
      if(is) print_perm+=std::to_string(1);
      else print_perm+=std::to_string(0);
      print_perm+="  ";
    }
  }
  ATH_MSG_DEBUG(print_perm);
  return StatusCode::SUCCESS;
}

StatusCode BTaggingTruthTaggingTool::chooseAllTagPermutation(unsigned int nbtag){

  ANA_CHECK_SET_TYPE (StatusCode);

  if(!m_usePerm){
    return StatusCode::FAILURE;
  }

  unsigned int njets = m_njets;
  unsigned int limit = (njets > 7) ? 8 : njets+1;
  unsigned int max = (njets < nbtag+1) ? limit : nbtag+1;

  m_trfRes.perm_ex.clear(), m_trfRes.perm_ex.resize(nbtag+1);
  m_trfRes.perm_in.clear(), m_trfRes.perm_in.resize(nbtag+1);

  m_trfRes.permprob_ex.clear(), m_trfRes.permprob_ex.resize(nbtag+1);
  m_trfRes.permprob_in.clear(), m_trfRes.permprob_in.resize(nbtag+1);

  for(unsigned int i=0; i<max; i++) { // need +1 as 0 is included
    ANA_CHECK(chooseTagPermutation(i,false));
    ANA_CHECK(chooseTagPermutation(i,true));
  }

  return StatusCode::SUCCESS;
}

StatusCode BTaggingTruthTaggingTool::chooseTagPermutation(unsigned int nbtag, bool isIncl) {
  std::vector<double> incl;
  std::vector<std::pair<unsigned int, unsigned int> > trackPerm;
  double sum = 0;
  if(isIncl) {
    for(unsigned int itag=nbtag; itag < m_permsWeight.size(); itag++) {
      for(unsigned int ip = 0; ip < m_permsWeight.at(itag).size(); ip++) {
  	sum += m_permsWeight.at(itag).at(ip);
  	incl.push_back(sum);
  	trackPerm.push_back(std::make_pair(itag,ip));
      }
    }
  }
  else { // in exclusive case
    sum = m_permsSumWeight.at(nbtag).back();
    incl = m_permsSumWeight.at(nbtag);
    for(unsigned int ip = 0; ip < m_permsSumWeight.at(nbtag).size(); ip++) trackPerm.push_back(std::make_pair(nbtag,ip));
  }
  double theX = m_rand.Uniform(sum);
  for(unsigned int ip=0; ip < incl.size(); ip++) {
    ATH_MSG_DEBUG("incl.at(ip): " << incl.at(ip) << "  theX: " << theX);
    if(incl.at(ip) >= theX) {
      if(isIncl) {
	m_trfRes.perm_in.at(nbtag) = m_perms[m_njets].at(trackPerm.at(ip).first).at(trackPerm.at(ip).second);
	m_trfRes.permprob_in.at(nbtag) = m_permsWeight.at(trackPerm.at(ip).first).at(trackPerm.at(ip).second) / m_trfRes.trfwsys_in[0].at(nbtag);
      }
      else {
	m_trfRes.perm_ex.at(nbtag) = m_perms[m_njets].at(trackPerm.at(ip).first).at(trackPerm.at(ip).second);
	m_trfRes.permprob_ex.at(nbtag) = m_permsWeight.at(trackPerm.at(ip).first).at(trackPerm.at(ip).second) / m_trfRes.trfwsys_ex[0].at(nbtag);
      }
      return StatusCode::SUCCESS;
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode BTaggingTruthTaggingTool::setSeed(unsigned int seed){
  m_rand.SetSeed(seed);
  return StatusCode::SUCCESS;
}




///// tag bins  //////////////////////
  // Cheatsheet:
  // returns 5 if between 60% and 0%
  // returns 4 if between 70% and 60%
  // returns 3 if between 77% and 70%
  // returns 2 if between 85% and 77%
  // returns 1 if between 100% and 85%
  // returns 0 if smaller than -1e4-> should never happen
  // return -1 if bigger than 1e4 or not in b-tagging acceptance
  //////////////////////


StatusCode BTaggingTruthTaggingTool::getQuantiles(std::vector<std::vector<int> > &trf_bin_ex, std::vector<std::vector<int> > &trf_bin_in){
  trf_bin_ex.resize(m_trfRes.tbins_ex.size());
  for(unsigned int i =0; i<m_trfRes.tbins_ex.size(); i++)
    trf_bin_ex.at(i).resize(m_trfRes.tbins_ex.at(i).size());

  trf_bin_in.resize(m_trfRes.tbins_in.size());
  for(unsigned int i =0; i<m_trfRes.tbins_in.size(); i++)
    trf_bin_in.at(i).resize(m_trfRes.tbins_in.at(i).size());
  // increasing the value by 1 to match conventions in selectionTool
  for(unsigned int i =0; i<m_trfRes.tbins_ex.size(); i++)
    for(unsigned int j=0; j<m_trfRes.tbins_ex.at(i).size(); j++)
      trf_bin_ex.at(i).at(j)=m_trfRes.tbins_ex.at(i).at(j) +1;

  for(unsigned int i =0; i<m_trfRes.tbins_in.size(); i++)
    for(unsigned int j=0; j<m_trfRes.tbins_in.at(i).size(); j++)
      trf_bin_in.at(i).at(j)=m_trfRes.tbins_in.at(i).at(j) +1;

  return StatusCode::SUCCESS;
}

StatusCode BTaggingTruthTaggingTool::chooseAllTagBins(){
  ANA_CHECK_SET_TYPE (StatusCode);
  m_trfRes.tbins_ex.clear();
  m_trfRes.tbins_in.clear();
  m_trfRes.tbins_ex.resize( m_trfRes.trfwsys_ex[0].size());
  m_trfRes.tbins_in.resize( m_trfRes.trfwsys_in[0].size());

  m_trfRes.binsprob_ex.clear();
  m_trfRes.binsprob_in.clear();
  m_trfRes.binsprob_ex.resize( m_trfRes.trfwsys_ex[0].size());
  m_trfRes.binsprob_in.resize( m_trfRes.trfwsys_in[0].size());

  if(m_trfRes.perm_ex.size() != m_trfRes.perm_in.size()) ATH_MSG_WARNING("Different sizes in exclusive and inclusive permutation choices");

  for(unsigned int nb=0; nb<m_trfRes.perm_ex.size(); nb++) {
    ANA_CHECK(chooseTagBins_cum(m_trfRes.perm_ex.at(nb), false, nb));
    ANA_CHECK(chooseTagBins_cum(m_trfRes.perm_in.at(nb), true, nb));
  }
  return StatusCode::SUCCESS;
}


// chiara: non posso uniformare il codice con getTagBinsConfProb??
StatusCode BTaggingTruthTaggingTool::chooseTagBins_cum(std::vector<bool> &tagconf, bool isIncl, unsigned int nbtag)
{
  std::vector<int> btagops;

  std::vector<double> incl;
  double prob = 1.;

  for(unsigned int j=0; j<tagconf.size(); j++) {
    if(tagconf.at(j)) { // tagged jet
      double sum=0.;
      incl.clear();
      for(int iop = (int)m_trfRes.eff_allOP.size()-1; iop >= (int)m_OperatingPoint_index; iop--) { // loop on the tighter OPs
	sum = m_trfRes.eff_allOP[m_availableOP.at(iop)][j];
  	incl.push_back(sum);
      }
      double theX = m_rand.Uniform(sum);

      for(unsigned int k=0; k<incl.size(); k++) {
	double valm1 = 0.;
	if(k!=0) valm1 = incl.at(k-1);
  	if(incl.at(k) >= theX) {
  	  btagops.push_back(m_availableOP.size()-k);
	  prob *= (incl.at(k)-valm1) / sum;
  	  break;
  	}
      }
    }
    else { // untagged jet
      double sum=0.;
      incl.clear();
      incl.push_back(sum); // fill the "0_0" OP as no real value affected and start the loop at 1
      for(unsigned int iop=0; iop<=m_OperatingPoint_index; iop++) { // need to include the chosen tag cut to have the last MV1 bin of untagged jets filled, start at 0 with cumulative as iop = 0 = first OP
  	// sum = 1 - trf here
  	sum = 1 - m_trfRes.eff_allOP[m_availableOP.at(iop)][j];
  	incl.push_back(sum);
      }

      double theX = m_rand.Uniform(sum);
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
    m_trfRes.tbins_in.at(nbtag) = btagops;
    m_trfRes.binsprob_in.at(nbtag) = prob;
    ATH_MSG_DEBUG("incl, nbtag " << nbtag << "   prob " << prob);
  }
  else {
    m_trfRes.tbins_ex.at(nbtag) = btagops;
    m_trfRes.binsprob_ex.at(nbtag) = prob;
    ATH_MSG_DEBUG("excl, nbtag " << nbtag << "   prob " << prob);
  }
  return StatusCode::SUCCESS;
}



double BTaggingTruthTaggingTool::getTagBinsRW(bool isIncl, unsigned int nbtag){

  if((!isIncl && m_trfRes.binsprob_ex.size() == 0) || (isIncl && m_trfRes.binsprob_in.size() == 0)) {
    ATH_MSG_ERROR("Need to choose quantiles before computing the reweighting");
    exit(-1);
  }

  double tbw = 0.;
  double prob_sys=1.;

  if(isIncl) {
    prob_sys=getTagBinsConfProb(m_trfRes.tbins_in.at(nbtag));
    tbw = prob_sys/m_trfRes.binsprob_in.at(nbtag);
    return tbw;
  }
  else {
    prob_sys=getTagBinsConfProb(m_trfRes.tbins_ex.at(nbtag));
    tbw = prob_sys/m_trfRes.binsprob_ex.at(nbtag);
    return tbw;
  }
}


double BTaggingTruthTaggingTool::getTagBinsConfProb(std::vector<int> &tagws){
  double prob = 1.;
  for(unsigned int j=0; j<tagws.size(); j++) {
    if((unsigned int)tagws.at(j) > m_OperatingPoint_index) { // tagged
      double prevBinW = 0.;
      int mOP = m_availableOP.size();
      if(tagws.at(j) != mOP) { // not the last tag-bin
	prevBinW =  m_trfRes.eff_allOP[m_availableOP.at(tagws.at(j))][j];
      }
      // prob *= (eff*SF-exactly-that-bin)/(ef*SF-all-tagged-bins)
      // (eff*SF-exactly-that-bin): eff(==60) = eff(70) - eff(60) --> eff(==5) = eff(4)-eff(5)
      prob *= (m_trfRes.eff_allOP[m_availableOP.at(tagws.at(j)-1)][j] - prevBinW) /  (m_trfRes.eff_allOP[m_availableOP.at(m_OperatingPoint_index)][j]);
      ATH_MSG_DEBUG("prob " << prob);
    }
    else { // untagged
      double prevBinW = 0.;
      if(tagws.at(j) != 0) {
	prevBinW = 1 - m_trfRes.eff_allOP[m_availableOP.at(tagws.at(j)-1)][j];
      }
      prob *= ((1 - m_trfRes.eff_allOP[m_availableOP.at(tagws.at(j))][j]) - prevBinW) / (1 - m_trfRes.eff_allOP[m_availableOP.at(m_OperatingPoint_index)][j]);
      ATH_MSG_DEBUG("prob " << prob);
    }
  }
  return prob;
}

/*
bool BTaggingTruthTaggingTool::checkRange(double pT, double eta) const
{
  // Do the |eta| cut:
  if( eta > m_maxEta ) {
    return false;
  }

  // Do the pT cut:
  ATH_MSG_VERBOSE( "Jet pT: " << pT );
  if( pT < m_minPt ) {
    return false;
  }

  return true;
}
*/

StatusCode BTaggingTruthTaggingTool::getDirectTaggedJets(std::vector<bool> &is_tagged){
  is_tagged.clear();
  std::vector<int> appo;
  for(const auto jet : *m_jets) {
    ATH_MSG_DEBUG("pt " << jet.vars.jetPt << "   eta " << jet.vars.jetEta << "   wei " << jet.vars.jetTagWeight);
    bool is_btagged = m_selTool->accept(jet.vars.jetPt, jet.vars.jetEta, jet.vars.jetTagWeight);
    ATH_MSG_DEBUG("is tagged? " << is_btagged);
    if(is_btagged) is_tagged.push_back(1);
    else is_tagged.push_back(0);
  }
  return StatusCode::SUCCESS;
}


double BTaggingTruthTaggingTool::getEvtSF(int sys){
  ANA_CHECK_SET_TYPE (StatusCode);
  double SF = 1.;
  std::vector<bool> is_tagged;
  ANA_CHECK(getDirectTaggedJets(is_tagged));

  if(sys!=0) {
    //    ANA_CHECK_SET_TYPE(CP::SystematicCode);
    m_effTool->applySystematicVariation(m_eff_syst[sys]);
  }

  //  for(const auto jet : *m_jets) {
  for(unsigned int i =0; i< m_jets->size(); i++) {
    bool is_btagged = is_tagged.at(i);
    float ineffSF =1;
    float effSF=1;
    //    ANA_CHECK_SET_TYPE(CP::CorrectionCode);
    if(is_btagged){    // tagged --> look at sf
      m_effTool->getScaleFactor(m_jets->at(i).flav, m_jets->at(i).vars, ineffSF);
      SF*=ineffSF;
    }
    else{    // not tagged --> loop at ineff SF
      ANA_CHECK(m_effTool->getInefficiencyScaleFactor(m_jets->at(i).flav, m_jets->at(i).vars, effSF));
      SF *= effSF;
    }
  }

  if(sys!=0) {  // reset syst to nominal
    CP::SystematicSet defaultSet;
    //    ANA_CHECK_SET_TYPE(CP::SystematicCode);
    m_effTool->applySystematicVariation(defaultSet);
  }
  return SF;
}


StatusCode BTaggingTruthTaggingTool::check_syst_range(unsigned int sys){
  if(sys < m_eff_syst.size()) return StatusCode::SUCCESS;
  ATH_MSG_ERROR(sys << " Out of range of available systematics");
  return StatusCode::FAILURE;
}



BTaggingTruthTaggingTool::~BTaggingTruthTaggingTool(){

  delete m_jets;

  delete m_selTool;
  delete m_effTool;

  if(m_useQuntile){
    for(unsigned int iop=0; iop<m_availableOP.size(); iop++){
      std::string toolname = name()+"_eff_"+m_availableOP.at(iop);
      if(iop == m_OperatingPoint_index){
      	continue;
      }
      delete m_effTool_allOP[m_availableOP.at(iop)];
    }
  }
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

  //  for(auto part: ghostB) delete part;
  //  for(auto part: ghostC) delete part;
  //  for(auto part: ghostTau) delete part;

  return 0;
}

int BTaggingTruthTaggingTool::ConeFinalPartonFlavourLabel (const xAOD::Jet& jet) {
  // default label means "invalid"
  int label = -1;
  // First try the new naming scheme
  if (jet.getAttribute("ConeTruthLabelID",label)) return label;
  // If that fails, revert to the old scheme. In this case, further testing is not very useful
  jet.getAttribute("TruthLabelID", label);
  return label;
}

int BTaggingTruthTaggingTool::ExclusiveConeHadronFlavourLabel (const xAOD::Jet& jet) {
  // default label means "invalid"
  int label = -1;
  // We don't check the return value, as we would not be able to handle it gracefully anyway
  jet.getAttribute("HadronConeExclTruthLabelID",label);
  return label;
}

