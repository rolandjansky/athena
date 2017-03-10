// $Id$

// Local include(s):
#include "TruthWeightTools/HiggsWeightTool.h"
#include "xAODEventInfo/EventInfo.h"

#include "TString.h"

namespace xAOD {

  HiggsWeightTool::HiggsWeightTool( const std::string& name )
    : asg::AsgTool(name), m_init(false), m_Nweights(0), m_mcID(999),
      //m_weightTool("xAOD::TruthWeightTool/TruthWeightTool"),
      m_weightTool(nullptr), m_cutOff(false) {

    // wether to put constraints on the weights
    declareProperty("RequireFinite", m_requireFinite=false);
    declareProperty("WeightCutOff", m_weightCutOff=-1.0);
    if (m_weightCutOff>0) m_cutOff=true;
    
    // Force modes
    declareProperty("ForceNNLOPS", m_forceNNLOPS=false); // Run2-default Powheg NNLOPS ggF
    declareProperty("ForceVBF",    m_forceVBF=false);    // Run2-default Powheg VBF
    declareProperty("ForceVH",     m_forceVH=false);     // Run2-default Powheg VH (WpH, WmH, qq->ZH)
  }

  StatusCode HiggsWeightTool::initialize() {

    int sum=m_forceNNLOPS+m_forceVBF+m_forceVH;
    if (sum>1) throw std::runtime_error("Must not call more than one of ForceNNLOPS, ForceVBF or ForceVH");

    if (sum==0) {
      m_mode = AUTO;
      m_weightTool = new xAOD::TruthWeightTool("TruthWeightTool");
      ::Info("HiggsWeightTool","AUTO MODE");
    } else if (m_forceNNLOPS) {
      m_mode=FORCE_GGF_NNLOPS;
      ::Info("HiggsWeightTool","FORCE_GGF_NNLOPS MODE");
    } else if (m_forceVBF) {
      m_mode=FORCE_POWPY8_VBF;
      ::Info("HiggsWeightTool","FORCE_POWPY8_VBF MODE");
    } else if (m_forceVH) {
      m_mode=FORCE_POWPY8_VH;
      ::Info("HiggsWeightTool","FORCE_POWPY8_VH MODE");
    }
    m_init=true;
    return StatusCode::SUCCESS;
  }

  void HiggsWeightTool::setupWeights(size_t Nweights) {
    static TString name = "HiggsWeightTool::setupWeights";
    const std::vector<std::string> &wNames = getWeightNames();
    m_Nweights=wNames.size();
    ::Info(name,"Setting up weights in %s. %lu weights available in input file, %lu expected.",
	   m_mode==AUTO?"AUTO mode":Form("FORCE mode %i",m_mode),Nweights,m_Nweights);
    if (m_Nweights!=Nweights) 
      throw std::runtime_error(Form("Current event has %lu weights, while we expect %lu weights from the metadata",
				    Nweights,m_Nweights));

    bool isNNLOPS = hasWeight(" nnlops-nominal-pdflhc ");
    
    // Always use PDF4LHC as central value. For NNLOPS, use PDF4LHC @NNLO (91400), while VBF and VH use PDF4LHC @NLO (90400)
    m_nom = isNNLOPS ? getWeightIndex(" nnlops-nominal-pdflhc ") : getWeightIndex(" PDF set = 90400 ");
    ::Info(name,"Nominal weight at index %lu",m_nom);

    // Check if we have the Higgs PDF uncertainty variations are there
    m_pdfUnc.clear();
    if ( hasWeight(" PDF set = 90400 ") && hasWeight(" PDF set = 90430 ") ) {
      for (int id=90400;id<=90430;++id) m_pdfUnc.push_back(getWeightIndex(Form(" PDF set = %i ",id)));
      ::Info(name,"PDF4LHC uncertainty variations at positions %lu-%lu",
	     getWeightIndex(" PDF set = 90400 "),getWeightIndex(" PDF set = 90430 "));
    }
    m_pdfNNPDF30.clear();
    if ( hasWeight(" PDF set = 260001 ") ) {
      m_pdfNNPDF30.push_back(isNNLOPS?getWeightIndex(" PDF set = 260000 "):0);
      for (int id=260001;id<=260100;++id) m_pdfNNPDF30.push_back(getWeightIndex(Form(" PDF set = %i ",id)));
      ::Info(name,"NNPDF30 NLO uncertainty variations at positions %lu-%lu",
             getWeightIndex(" PDF set = 260001 "),getWeightIndex(" PDF set = 260100 "));
    }

    m_aS_up=m_aS_dn=0;
    if ( hasWeight(" PDF set = 90431 ") && hasWeight(" PDF set = 90432 ") ) {
      m_aS_dn = getWeightIndex(" PDF set = 90431 ");
      m_aS_up = getWeightIndex(" PDF set = 90432 ");
      ::Info(name,"PDF4LHC alphaS variations identified");
    }

    m_qcd.clear();
    if ( hasWeight(" muR = 0.5, muF = 0.5 ") ) {
      for (TString mur:{"0.5","1.0","2.0"})
	for (TString muf:{"0.5","1.0","2.0"})
	  if (!(mur=="1.0"&&muf=="1.0")) {
	    std::string wn=(" muR = "+mur+", muF = "+muf+" ").Data();
	    if (hasWeight(wn)) // for some strange reason, VBF is missing some weights
	      m_qcd.push_back(getWeightIndex(wn));
	  }
      ::Info(name,"Read in %lu standard QCD variations",m_qcd.size());
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
      ::Info(name,"Read in 3 NNLOPS quark mass weights and %lu QCD weights",m_qcd_nnlops.size());
    }

    // special PDFs

    m_pdf4lhc_nnlo = getIndex(" PDF set = 91400 ");
    m_pdf4lhc_nlo  = getIndex(" PDF set = 90400 ");
    m_nnpdf30_nnlo = getIndex(" PDF set = 261000 "); // NNLOPS nominal
    m_nnpdf30_nlo  = getIndex(" PDF set = 260000 "); // VBF+VH nominal
    m_ct10nlo      = getIndex(" PDF set = 11000 ");
    m_ct10nlo_0118 = getIndex(" PDF set = 11068 ");
    m_ct14nlo      = getIndex(" PDF set = 13100 ");
    m_ct14nlo_0118 = getIndex(" PDF set = 13165 ");
    m_mmht2014nlo  = getIndex(" PDF set = 25200 ");

    /*
      TO DO
      90400 PDF4LHC15_nlo_30_pdfas
      HiggsWeightTool::setup... INFO     PDF set = 91400  PDF4LHC15_nnlo_30_pdfas
      HiggsWeightTool::setup... INFO     PDF set = 260000 NNPDF30_nlo_as_0118
      ...
      HiggsWeightTool::setup... INFO     PDF set = 11000  CT10nlo
      HiggsWeightTool::setup... INFO     PDF set = 25200  MMHT2014nlo68clas118
      HiggsWeightTool::setup... INFO     PDF set = 13100  CT14nlo
      HiggsWeightTool::setup... INFO     mtmb 
      HiggsWeightTool::setup... INFO     mtinf 
      nnlops-nnloNom-pwgDnDn 

      " PDF set = 11068 "  CT10nlo_as_0118
      " PDF set = 13165 "  CT14nlo_as_0118
     */

    //for (std::string wn:wNames)
    // ::Info(name,"\'%s\'",wn.c_str());
  }

  const std::vector<std::string> &HiggsWeightTool::getWeightNames() {
    if (!m_init) initialize();
    if (m_mode==AUTO) return m_weightTool->getWeightNames();
    static std::vector<std::string> wNames = 
      m_mode==FORCE_GGF_NNLOPS ? getDefaultNNLOPSweightNames() :
      m_mode==FORCE_POWPY8_VBF ? getDefaultVBFweightNames() : getDefaultVHweightNames();
    return wNames;
  }

  bool HiggsWeightTool::hasWeight(std::string wName) {
    if (m_mode==AUTO) return m_weightTool->hasWeight(wName);
    const std::vector<std::string> &wnames = getWeightNames();
    return std::find(wnames.begin(),wnames.end(),wName)!=wnames.end();
  }

  size_t HiggsWeightTool::getWeightIndex(std::string wName) {
    if (m_mode==AUTO) return m_weightTool->getWeightIndex(wName);
    if (!hasWeight(wName)) throw std::runtime_error("Weight "+wName+" doesn't exist.");
    const std::vector<std::string> &wnames = getWeightNames();
    return static_cast<size_t>(std::find(wnames.begin(),wnames.end(),wName)-wnames.begin());
  }

  float HiggsWeightTool::getWeight(std::string wName) {
    if (m_mode!=AUTO) throw std::runtime_error("getWeight(string) only supported in AUTO mode so far");
    return m_weightTool->getWeight(wName);
  }

  const std::vector<float> &HiggsWeightTool::getMCweights() const {
    // 1. Check if truth events contain a weight vector. If so, take it.
    if (evtStore()->contains<xAOD::TruthEventContainer>("TruthEvents")) {
      const xAOD::TruthEventContainer *truthEvents = nullptr;
      if ( evtStore()->retrieve( truthEvents, "TruthEvents" ).isFailure() )
	throw std::runtime_error("Cannot access TruthEvents ??");
      if (truthEvents->at(0)) return truthEvents->at(0)->weights();
    }
    // 2. Otherwise use the mcWeights in EventInfo
    const xAOD::EventInfo *evtInfo = nullptr;
    if ( evtStore()->retrieve( evtInfo, "EventInfo" ).isFailure() )
      throw std::runtime_error("Cannot access EventInfo (nor TruthEvents) in input file");
    return evtInfo->mcEventWeights();
  }

  /// Access MC weight for uncertainty propagation
  /// Note: input kinematics should be HTXS_Higgs_pt, HTXS_Njets_pTjet30, and HTXS_Stage1_Category_pTjet30
  HiggsWeights HiggsWeightTool::getHiggsWeights(int STXS_Njets30, double STXS_pTH, int STXS_Stage1) {
    HiggsWeights hw = getHiggsWeightsInternal(STXS_Njets30,STXS_pTH,STXS_Stage1);
    if (m_requireFinite) updateWeights(hw);
    return hw;
  }

  void HiggsWeightTool::updateWeights(HiggsWeights &hw) {
    updateWeight(hw.nominal,hw.nominal);
    updateWeights(hw.nominal,hw.weight0,hw.alphaS_up,hw.alphaS_dn);
    updateWeights(hw.nominal,hw.pdf4lhc_unc); updateWeights(hw.nominal,hw.nnpdf30_unc);    
    updateWeights(hw.nominal,hw.qcd); updateWeights(hw.nominal,qcd_nnlops);    
    updateWeights(hw.nominal,hw.mt_inf,hw.mb_minlo);
    updateWeights(hw.nominal,hw.nnpdf30_nlo,hw.nnpdf30_nnlo,hw.mmht2014nlo);
    updateWeights(hw.nominal,hw.pdf4lhc_nlo,hw.pdf4lhc_nnlo);
    updateWeights(hw.nominal,hw.ct10nlo,hw.ct10nlo_0118);
    updateWeights(hw.nominal,hw.ct14nlo,hw.ct14nlo_0118);
    updateWeights(hw.nominal,hw.qcd_wg1_mu,hw.qcd_wg1_res);
    updateWeights(hw.nominal,hw.qcd_wg1_mig01,hw.qcd_wg1_mig12);
    updateWeights(hw.nominal,hw.qcd_wg1_pTH,hw.qcd_wg1_qm);
    updateWeights(hw.nominal,hw.qcd_nnlops_nnlo,hw.qcd_nnlops_pow);
  }

  void HiggsWeightTool::updateWeight(const double &w_nom, double &w) {
    if (m_requireFinite&&!std::isfinite(w)) { w=w_nom;
    if (m_cutOff&&w>m_weightCutOff) w=m_weighCutOff;
    if (m_cutOff&&w<-m_weightCutOff) w=-m_weighCutOff;
  }
  
  /// Access MC weight for uncertainty propagation
  /// Note: input kinematics should be HTXS_Higgs_pt, HTXS_Njets_pTjet30, and HTXS_Stage1_Category_pTjet30
  HiggsWeights HiggsWeightTool::getHiggsWeightsInternal(int STXS_Njets30, double STXS_pTH, int STXS_Stage1) {
    // convert to GeV
    double pTH = STXS_pTH/1000; int Njets=STXS_Njets30;
    const std::vector<float> &weights = getMCweights();

    const xAOD::EventInfo *evtInfo = nullptr;
    if ( evtStore()->retrieve( evtInfo, "EventInfo" ).isFailure() )
      throw std::runtime_error("Cannot access EventInfo (nor TruthEvents) in input file");

    // if needed, setup weight structure
    if (m_Nweights!=weights.size()||evtInfo->mcChannelNumber()!=m_mcID)
      setupWeights(weights.size());
    // update current MC channel number
    m_mcID=evtInfo->mcChannelNumber();
    
    HiggsWeights hw;
    // set kinematics
    hw.pTH=pTH; hw.Njets30=Njets; hw.STXS = STXS_Stage1;

    // 1. Nominal weight
    double w_nom = weights[m_nom];
    hw.nominal = w_nom;
    hw.weight0 = weights[0];

    // 2. PDF weights
    hw.alphaS_up=hw.alphaS_dn=0;
    if (m_pdfUnc.size()==31) {
      double pdf0=weights[m_pdfUnc[0]];
      // PDF uncertainty. Scale to nominal weight (since PDF0 is relative to Powheg NLO)
      //   w_PDF[i] = PDF[i] / PDF[0] * w_nominal
      for (size_t i=1;i<=30;++i)
	hw.pdf4lhc_unc.push_back(weights[m_pdfUnc[i]]/pdf0*w_nom);
      // 2b. alphaS weights
      if (m_aS_up&&m_aS_dn) {
	hw.alphaS_up = weights[m_aS_up]/pdf0*w_nom;
	hw.alphaS_dn = weights[m_aS_dn]/pdf0*w_nom;
      }
    }
    if (m_pdfNNPDF30.size()==101) {
      double pdf0=weights[m_pdfNNPDF30[0]];
      for (size_t i=1;i<=100;++i)
        hw.nnpdf30_unc.push_back(weights[m_pdfNNPDF30[i]]/pdf0*w_nom);
    }

    // Standard QCD uncertainties (which are relative to weight 0)
    for (auto idx:m_qcd) hw.qcd.push_back(getWeight(weights,idx)*w_nom/hw.weight0);

    // NNLOPS QCD uncertainties
    double nnlo=getWeight(weights,m_nnlopsNom);
    if (!m_nnlopsNom) nnlo=w_nom; // fix for rare NNLOPS samples

    for (auto idx:m_qcd_nnlops) hw.qcd_nnlops.push_back(getWeight(weights,idx)*w_nom/nnlo);

    // Powheg uncertainty
    hw.qcd_nnlops_nnlo = hw.qcd_nnlops_pow = w_nom;
    if (m_qcd_nnlops.size()==26) {
      hw.qcd_nnlops_nnlo = hw.qcd_nnlops[4]; // nnlops-nnloDn-PowNomNom
      hw.qcd_nnlops_pow  = hw.qcd_nnlops[9]; // nnlops-nnloNom-PowDnDn
    } 

    // Central values of different PDF sets
    // for NNLOPS, these are relative to the NLO cross section - need to adjust with k-factor
    double k = m_nnlopsNom ? nnlo/hw.weight0 : 1.0;
    
    //hw.nnpdf30_nnlo = m_nnlopsNom ? weights[m_nnlopsNom] : 0; <<<<<
    hw.nnpdf30_nlo  = getWeight(weights,m_nnpdf30_nlo)*k;
    hw.nnpdf30_nnlo = getWeight(weights,m_nnpdf30_nnlo)*k;
    hw.mmht2014nlo  = getWeight(weights,m_mmht2014nlo)*k;
    hw.pdf4lhc_nlo  = getWeight(weights,m_pdf4lhc_nlo)*k;
    hw.pdf4lhc_nnlo = getWeight(weights,m_pdf4lhc_nnlo)*k;
    hw.ct10nlo      = getWeight(weights,m_ct10nlo)*k;
    hw.ct10nlo_0118 = getWeight(weights,m_ct10nlo_0118)*k;
    hw.ct14nlo      = getWeight(weights,m_ct14nlo)*k;
    hw.ct14nlo_0118 = getWeight(weights,m_ct14nlo_0118)*k;
    
    // special catch
    if (m_bminlo||m_qcd_nnlops.size()) {// NNLOPS!!
      hw.nnpdf30_nnlo = hw.weight0*k;
      //      hw.pdf4lhc_nnlo = w_nom; // usually close, but can be 4% different...
      // printf("  %.4f = %.4f\n",hw.nnpdf30_nnlo,nnlo); // should be the same! // works!
      // printf("PDF4LHC NLO * k_NNLO / NNLO: %.4f\n",getWeight(weights,m_pdf4lhc_nnlo)*k/w_nom);
    }
    else if (m_pdfUnc.size()==31)  // should be VBF or VH
      hw.nnpdf30_nlo = hw.weight0; // sf=1 here
    

    // 3. Quark mass variations
    //m_tinf=m_bminlo=m_nnlopsNom=0;
    hw.mt_inf   = getWeight(weights,m_tinf)*w_nom/nnlo;
    hw.mb_minlo = getWeight(weights,m_bminlo)*w_nom/nnlo;

    //TODO - check
    //       " nnlops-nominal ",
    // AND NLO quark mass weights: mtmb, mtinf, mtmb-bminlo
    
    

    /*********
     *  WG1 proposed ggF QCD uncertainty
     */

    // Cross sections in the =0, =1, and >=2 jets of Powheg ggH after reweighing scaled to  sigma(N3LO)
    static std::vector<double> sig({30.26,13.12,5.14});

    // BLPTW absolute uncertainties in pb
    static std::vector<double> yieldUnc({ 1.12, 0.66, 0.42});
    static std::vector<double> resUnc  ({ 0.03, 0.57, 0.42});
    static std::vector<double> cut01Unc({-1.22, 1.00, 0.21});
    static std::vector<double> cut12Unc({    0,-0.86, 0.86});
    
    // account for missing EW+quark mass effects by scaling BLPTW total cross section to sigma(N3LO)
    double sf = 48.52/47.4;

    int jetBin = (Njets > 1 ? 2 : Njets);
    hw.qcd_wg1_mu    = (1.0 + yieldUnc[jetBin]/sig[jetBin]*sf)*w_nom;
    hw.qcd_wg1_res   = (1.0 + resUnc[jetBin]/sig[jetBin]*sf)*w_nom;
    hw.qcd_wg1_mig01 = (1.0 + cut01Unc[jetBin]/sig[jetBin]*sf)*w_nom;
    hw.qcd_wg1_mig12 = (1.0 + cut12Unc[jetBin]/sig[jetBin]*sf)*w_nom;
    
    // High pT uncertainty 
    static double y1_1 = 0.88, y2_1 = 1.16, x2_1 = 150;
    static double y1_ge2 = 0.88, y2_ge2 = 1.16, x2_ge2 = 225;
    double pTH_unc = 1.0;
    if      (Njets>=2) pTH_unc = pTH>x2_ge2?y2_ge2:y1_ge2+(y2_ge2-y1_ge2)*pTH/x2_ge2;
    else if (Njets==1) pTH_unc = pTH>x2_1?y2_1:y1_1+(y2_1-y1_1)*pTH/x2_1;
    hw.qcd_wg1_pTH = pTH_unc*w_nom;

    // Quark-mass uncdertainty - TODO
    hw.qcd_wg1_qm = w_nom;

    /*********
     *  Tackmann STXS uncertainty scheme
     */

    return hw;
  }
  
} // namespace xAOD
