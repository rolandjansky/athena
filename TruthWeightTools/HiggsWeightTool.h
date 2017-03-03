// $Id$
#ifndef TRUTHWEIGHTTOOLS_HIGGSWEIGHTTOOL_H
#define TRUTHWEIGHTTOOLS_HIGGSWEIGHTTOOL_H

// Local include(s):
#include "TruthWeightTools/TruthWeightTool.h"
#include "xAODTruth/TruthEvent.h"

namespace xAOD {

  /// Struct-like class for Higgs weights
  //// @author Dag Gillberg <dag.gillberg@cern.ch> 
  class HiggsWeights {
  public:
    /// Nominal event weight
    double nominal;
    
    /// 30 PDF4LHC variations + alphaS up/down
    std::vector<double> pdf4lhc;
    double alphaS_up, alphaS_dn;

    /// ggF QCD scale variations (muR,muF), 8 variations
    std::vector<double> qcd;

    /// NNLOPS
    /// 1. QCD scale variations 3x(NNLO), 9xPowheg(muR,muF) - 26 variations
    std::vector<double> qcd_nnlops;

    /// 2. quark mass variations
    double mt_inf, mb_minlo;

    /// Special PDF weights
    double nnpdf30_nlo, nnpdf30_nnlo, mmht2014nlo, pdf4lhc_nlo, pdf4lhc_nnlo;
    double ct10nlo, ct10nlo_0118, ct14nlo, ct14nlo_0118;
    
    /// WG1 proposed QCD uncertainty scheme
    double qcd_wg1_mu, qcd_wg1_res, qcd_wg1_mig01, qcd_wg1_mig12;
    double qcd_wg1_pTH, qcd_wg1_qm;

    // Tackmann proposed QCD uncertainty scheme, TODO

    /// Powheg NNLOPS possible scheme
    double qcd_nnlops_nnlo, qcd_nnlops_pow;

    /// information of the current event kinematiocs
    double pTH;
    int Njets30, STXScat;

    /// methods to print weights to the screen
    char *uncStr(double var, double nom) { return Form("%s%.1f%%",var>nom?"+":"",(var-nom)/nom*100); }
    void print() {
      double n=nominal;
      printf("\n------\n  Higgs MC weights of current event, pTH = %.1f GeV, Njets = %i\n",
	     pTH,Njets30);
      printf("    Nominal weight: %.3f\n",nominal);
      if (pdf4lhc.size()==30) {
	printf("\n    PDF unc  1-10:");
	for (size_t i=0;i<10;++i) printf(" %s",uncStr(pdf4lhc[i],n));
	printf("\n    PDF unc 11-20:");
	for (size_t i=10;i<20;++i) printf(" %s",uncStr(pdf4lhc[i],n));
	printf("\n    PDF unc 21-30:");
	for (size_t i=20;i<30;++i) printf(" %s",uncStr(pdf4lhc[i],n));
	printf("\n    alphaS up: %s, down: %s\n",
	       uncStr(alphaS_up,n),uncStr(alphaS_dn,n));
	printf("\n    Quark mass varations  (m_top=inf): %s  (m_b minlo): %s\n",
	       uncStr(mt_inf,n),uncStr(mb_minlo,n));
	printf("\n    WG1 proposed QCD uncertainty scheme\n");
	printf("      mu: %s,   res: %s,   mig01: %s,   mig12: %s\n",
	       uncStr(qcd_wg1_mu,n),uncStr(qcd_wg1_res,n),uncStr(qcd_wg1_mig01,n),uncStr(qcd_wg1_mig12,n));
	printf("      pTH: %s,   quark-mass: %s\n",
	       uncStr(qcd_wg1_pTH,n),uncStr(qcd_wg1_qm,n));
      }
    }

  };

  /// Tool for accessing of MC weights and other weigthts for QCD uncertainty propagation for Higgs analyses
  ///
  /// @author Dag Gillberg <dag.gillberg@cern.ch>
  ///
  /// $Revision$
  /// $Date$
  ///
  class HiggsWeightTool : public asg::AsgTool {
  public:
    
    /// Create a constructor for standalone usage
    HiggsWeightTool( const std::string& name = "xAOD::TruthWeightTool");
    virtual ~HiggsWeightTool(){}

    virtual StatusCode initialize();

    /// @name Function(s) accessed via the truth weight tool
    /// @{
    
    /// Get the MC weight vector
    const std::vector<float> &getMCweights() const;

    /// Value of MC event weight 
    float getWeight(std::string weightName);

    /// Value of MC event weight 
    bool hasWeight(std::string weightName);
    
    /// Index of MC event weight
    size_t getWeightIndex(std::string weightName);

    HiggsWeights getHiggsWeights(int HTXS_Njets30=-1, double HTXS_pTH=-99.0, int HTXS_cat=-1);

    // returns hardcoded list of weight names matching expecation
    // of ATLAS-default ggF NNLOPS 
    const std::vector<std::string> getDefaultNNLOPSweightNames();
    const std::vector<std::string> getDefaultVBFweightNames();
    const std::vector<std::string> getDefaultVHweightNames();

    /// Weight names in metadata
    const std::vector<std::string> &getWeightNames();

    /// @}
    
  private:

    /// Setup weights
    void setupWeights(size_t Nweights);

    double getWeight(std::vector<float> &ws, size_t idx) {
      if (idx==0) return 0.0;
      return ws.at(idx);
    }

    size_t getIndex(std::string wn) {
      if (hasWeight(wn)) return getWeightIndex(wn);
      return 0;
    }
    
    /// Flags 
    bool m_init;

    enum mode { AUTO=0, FORCE_GGF_NNLOPS = 1, FORCE_POWPY8_VBF = 2, FORCE_POWPY8_VH = 3 };
    mode m_mode;
    bool m_forceNNLOPS, m_forceVBF, m_forceVH;

    /// number of expected weights
    size_t m_Nweights;

    /// Current MC channel number
    uint32_t m_mcID;
    
    /// The truth weight tool
    xAOD::TruthWeightTool *m_weightTool;
    
    /// getWeight 
    double getWeight(size_t idx);
    
    /// index of weights
    size_t m_nom;
    
    /// weight indices for PDF+alphaS uncertainites
    std::vector<size_t> m_pdfUnc;
    size_t m_aS_up, m_aS_dn; 

    /// Special PDF sets
    int m_nnpdf30_nlo, m_nnpdf30_nnlo, m_mmht2014nlo, m_pdf4lhc_nlo, m_pdf4lhc_nnlo;
    int m_ct10nlo, m_ct10nlo_0118, m_ct14nlo, m_ct14nlo_0118;
    
    /// Special weight indices for Powheg NNLOPS
    size_t m_tinf, m_bminlo, m_nnlopsNom;
    std::vector<size_t> m_qcd, m_qcd_nnlops;
    
  protected:
    /// EventInfo
    //const xAOD::EventInfo *m_evtInfo;
    //const xAOD::TruthEvent *m_truthEvt;
    //const std::vector<float> *m_weights;
  };
  
} // namespace xAOD

#endif
