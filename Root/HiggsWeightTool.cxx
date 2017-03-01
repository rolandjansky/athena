// $Id$

// Local include(s):
#include "TruthWeightTools/HiggsWeightTool.h"
#include "xAODEventInfo/EventInfo.h"

#include "TString.h"

namespace xAOD {

  HiggsWeightTool::HiggsWeightTool( const std::string& name )
    : asg::AsgTool(name), m_init(false), m_Nweights(0), m_mcID(999),
      //m_weightTool("xAOD::TruthWeightTool/TruthWeightTool"),
      m_weightTool(nullptr) {
    
    declareProperty("ForceNNLOPS",m_forceNNLOPS=false);
  }

  StatusCode HiggsWeightTool::initialize() {
    m_mode=m_forceNNLOPS?FORCE_GGF_NNLOPS:AUTO;
    if (m_mode==AUTO) {
      m_weightTool = new xAOD::TruthWeightTool("TruthWeightTool");
      //if ( m_weightTool.retrieve().isFailure() )
      //std::runtime_error("Failed accessing truth weight tool");
      ::Info("HiggsWeightTool","AUTO MODE");
    } else {
      ::Info("HiggsWeightTool","FORCE_GGF_NNLOPS MODE");
    }
    m_init=true;
    return StatusCode::SUCCESS;
  }

  void HiggsWeightTool::setupWeights(size_t Nweights) {
    static TString name = "HiggsWeightTool::setupWeights";
    const std::vector<std::string> &wNames = getWeightNames();
    m_Nweights=wNames.size();
    if (m_Nweights!=Nweights) 
      std::runtime_error(Form("Current event has %lu weights, while we expect %lu weights from the metadata",
			      Nweights,m_Nweights));

    ::Info(name,"%lu weights availalbe",Nweights);

    bool isNNLOPS = hasWeight(" nnlops-nominal-pdflhc ");
    
    m_nom = isNNLOPS ? getWeightIndex(" nnlops-nominal-pdflhc ") : getWeightIndex(" nominal ");
    if (!isNNLOPS) std::runtime_error("NOT NNLOPS?"); // all my test samples should be NNLOPS!
    ::Info(name,"Nominal weight at index %lu",m_nom);

    // Check if we have the Higgs PDF uncertainty variations are there
    m_pdfUnc.clear();
    if ( hasWeight(" PDF set = 90400 ") && hasWeight(" PDF set = 90430 ") ) {
      for (int id=90400;id<=90430;++id) m_pdfUnc.push_back(getWeightIndex(Form(" PDF set = %i ",id)));
      ::Info(name,"PDF4LHC uncertainty variations at positions %lu-%lu",
	     getWeightIndex(" PDF set = 90400 "),getWeightIndex(" PDF set = 90430 "));
    }
    m_aS_up=m_aS_dn=0;
    if ( hasWeight(" PDF set = 90431 ") && hasWeight(" PDF set = 90432 ") ) {
      m_aS_up = getWeightIndex(" PDF set = 90431 ");
      m_aS_dn = getWeightIndex(" PDF set = 90432 ");
      ::Info(name,"PDF4LHC alphaS varations identified");
    }

    m_qcd.clear();
    if ( hasWeight(" muR = 0.5, muF = 0.5 ") ) {
      for (TString mur:{"0.5","1.0","2.0"})
	for (TString muf:{"0.5","1.0","2.0"})
	  if (!(mur=="1.0"&&muf=="1.0"))
	    m_qcd.push_back(getWeightIndex((" muR = "+mur+", muF = "+muf+" ").Data()));
      ::Info(name,"Read in 8 standard QCD variations");
    }

    m_tinf=m_bminlo=m_nnlopsNom=0;
    m_qcd_nnlops.clear();
    if (isNNLOPS) {
      // NNLOPS quark mass variations (with NNLO QCD correction)
      m_tinf       = getWeightIndex(" nnlops-mtinf ");
      m_bminlo     = getWeightIndex(" nnlops-bminlo ");
      if (hasWeight(" nnlops-nominal "))
	m_nnlopsNom  = getWeightIndex(" nnlops-nominal ");

      // NNLOPS QCD scale variations
      for (TString nn:{"Dn","Nom","Up"})
	for (TString mur:{"Dn","Nom","Up"})
	  for (TString muf:{"Dn","Nom","Up"})
	    if (!(mur=="Nom"&&muf=="Nom"&&nn=="Nom"))
	      m_qcd_nnlops.push_back(getWeightIndex((" nnlops-nnlo"+nn+"-pwg"+mur+muf+" ").Data()));
      ::Info(name,"Read in 3 NNLOPS quark mass weights and 26 QCD weights");
    }

    /*
      TO DO
      HiggsWeightTool::setup... INFO     PDF set = 91400 
      HiggsWeightTool::setup... INFO     PDF set = 260000
      ...
      HiggsWeightTool::setup... INFO     PDF set = 260100 
      HiggsWeightTool::setup... INFO     PDF set = 11000 
      HiggsWeightTool::setup... INFO     PDF set = 25200 
      HiggsWeightTool::setup... INFO     PDF set = 13100 
      HiggsWeightTool::setup... INFO     mtmb 
      HiggsWeightTool::setup... INFO     mtinf 
      nnlops-nnloNom-pwgDnDn 
     */

    /*
    for (std::string wn:wNames)
      ::Info(name,"\'%s\'",wn.c_str());
    */
  }

  const std::vector<std::string> &HiggsWeightTool::getWeightNames() {
    if (!m_init) initialize();
    if (m_mode==AUTO) return m_weightTool->getWeightNames();
    static std::vector<std::string> wNames = getDefaultNNLOPSweightNames();
    return wNames;
  }

  bool HiggsWeightTool::hasWeight(std::string wName) {
    if (m_mode==AUTO) return m_weightTool->hasWeight(wName);
    const std::vector<std::string> &wnames = getWeightNames();
    return std::find(wnames.begin(),wnames.end(),wName)!=wnames.end();
  }

  size_t HiggsWeightTool::getWeightIndex(std::string wName) {
    if (m_mode==AUTO) return m_weightTool->getWeightIndex(wName);
    if (!hasWeight(wName)) std::runtime_error("Weight "+wName+" doesn't exist.");
    const std::vector<std::string> &wnames = getWeightNames();
    return static_cast<size_t>(std::find(wnames.begin(),wnames.end(),wName)-wnames.begin());
  }

  float HiggsWeightTool::getWeight(std::string wName) {
    if (m_mode==AUTO) std::runtime_error("getWeight(string) only supported in AUTO mode so far");
    return m_weightTool->getWeight(wName);
  }

  const std::vector<float> &HiggsWeightTool::getMCweights() const {
    // 1. Check if truth events contain a weight vector. If so, take it.
    if (evtStore()->contains<xAOD::TruthEventContainer>("TruthEvents")) {
      const xAOD::TruthEventContainer *truthEvents = nullptr;
      if ( evtStore()->retrieve( truthEvents, "TruthEvents" ).isFailure() )
	std::runtime_error("Cannot access TruthEvents ??");
      if (truthEvents->at(0)) return truthEvents->at(0)->weights();
    }
    // 2. Otherwise use the mcWeights in EventInfo
    const xAOD::EventInfo *evtInfo = nullptr;
    if ( evtStore()->retrieve( evtInfo, "EventInfo" ).isFailure() )
      std::runtime_error("Cannot access EventInfo (nor TruthEvents) in input file");
    return evtInfo->mcEventWeights();
  }

  HiggsWeights HiggsWeightTool::getHiggsWeights(double pTH, int Njets30, int STXS_Stage1) {
    const std::vector<float> &weights = getMCweights();

    const xAOD::EventInfo *evtInfo = nullptr;
    if ( evtStore()->retrieve( evtInfo, "EventInfo" ).isFailure() )
      std::runtime_error("Cannot access EventInfo (nor TruthEvents) in input file");

    // if needed, setup weight structure
    if (m_Nweights!=weights.size()||evtInfo->mcChannelNumber()!=m_mcID)
      setupWeights(weights.size());
    // update current MC channel number
    m_mcID=evtInfo->mcChannelNumber();
    
    HiggsWeights hw;
    // set kinematics
    hw.pTH=pTH; hw.Njets30=Njets30;

    // 1. Nominal weight
    hw.nominal = weights[m_nom];

    // 2. PDF weights
    if (m_pdfUnc.size()==31) {
      double pdf0=weights[m_pdfUnc[0]];
      // PDF uncertainty. Scale to nominal weight (since PDF0 is relative to Powheg NLO)
      //   w_PDF[i] = PDF[i] / PDF[0] * w_nominal
      for (size_t i=1;i<=30;++i)
	hw.pdf4lhc.push_back(weights[m_pdfUnc[i]]/pdf0*hw.nominal);
    }
    // 2b. alphaS weights
    if (m_aS_up&&m_aS_dn) {
      hw.alphaS_up = weights[m_aS_up];
      hw.alphaS_dn = weights[m_aS_dn];
    }

    // 3. Quark mass variations
    m_tinf=m_bminlo=m_nnlopsNom=0;


    return hw;
  }
  
} // namespace xAOD
