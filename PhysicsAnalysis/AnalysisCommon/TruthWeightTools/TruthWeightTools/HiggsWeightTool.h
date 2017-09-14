// $Id$
#ifndef TRUTHWEIGHTTOOLS_HIGGSWEIGHTTOOL_H
#define TRUTHWEIGHTTOOLS_HIGGSWEIGHTTOOL_H

// Local include(s):
#include "TruthWeightTools/TruthWeightTool.h"
#include "xAODTruth/TruthEvent.h"

namespace xAOD
{

  /// Struct-like class for Higgs weights
  //// @author Dag Gillberg <dag.gillberg@cern.ch>
  class HiggsWeights
  {
  public:
    /// Nominal event weight
    double nominal, weight0;

    /// 30 PDF4LHC uncertainty variations + alphaS up/down
    std::vector<double> pdf4lhc_unc, nnpdf30_unc;
    double alphaS_up, alphaS_dn;

    /// Weights to reweigh central to a different PDF set
    /// If the PDF set is not present in file, this weight will be zero
    double nnpdf30_nlo, nnpdf30_nnlo, mmht2014nlo, pdf4lhc_nlo, pdf4lhc_nnlo;
    double ct10nlo, ct10nlo_0118, ct14nlo, ct14nlo_0118;

    /// QCD scale variations (muR,muF)
    std::vector<double> qcd;


    /// Special weights for Powheg ggF NNLOPS
    /// 1. QCD scale variations 3x(NNLO), 9xPowheg(muR,muF) - 26 variations
    std::vector<double> qcd_nnlops;

    /// 2. quark mass variations
    double mt_inf, mb_minlo;

    /// ggF specific uncertainty weights
    /// various different schemes

    /// WG1 proposed QCD uncertainty scheme
    double qcd_wg1_mu, qcd_wg1_res, qcd_wg1_mig01, qcd_wg1_mig12;
    double qcd_wg1_pTH, qcd_wg1_qm_b, qcd_wg1_qm_t, qcd_wg1_vbf2j, qcd_wg1_vbf3j;

    /// WG1 proposed uncertainty scheme
    std::vector<double> ggF_qcd_wg1()
    {
      return {qcd_wg1_mu, qcd_wg1_res, qcd_wg1_mig01, qcd_wg1_mig12,
              qcd_wg1_vbf2j, qcd_wg1_vbf3j, qcd_wg1_pTH, qcd_wg1_qm_t};
    }

    /// Tackmann proposed QCD uncertainty scheme
    std::vector<double> ggF_qcd_stxs;

    /// Merging of STXS and WG1 schemes
    std::vector<double> ggF_qcd_2017;

    /// Jet veto efficiency method for cross check
    std::vector<double> ggF_qcd_jve;

    /// Powheg NNLOPS possible scheme
    double qcd_nnlops_nnlo, qcd_nnlops_pow;

    /// information of the current event kinematiocs
    double pTH;
    int Njets30, STXS;

    /// methods to print weights to the screen
    char *uncStr(double var, double nom) { return var == 0 ? Form("  N/A") : Form("%s%.1f%%", var >= nom ? "+" : "", (var - nom) / nom * 100); }
    void print()
    {
      double n = nominal;
      printf("\n------\n  Higgs MC weights of current event, pTH = %.1f GeV, Njets = %i\n",
             pTH, Njets30);
      printf("    Nominal weight: %.3f\n", nominal);
      printf("    Weight 0:       %.3f\n", weight0);

      printf("\n   There are %lu PDF4LHC NLO uncertainty variations\n", pdf4lhc_unc.size());

      if (pdf4lhc_unc.size() == 30) {
        printf("    PDF unc  1-10:");

        for (size_t i = 0; i < 10; ++i) { printf(" %s", uncStr(pdf4lhc_unc[i], n)); }

        printf("\n    PDF unc 11-20:");

        for (size_t i = 10; i < 20; ++i) { printf(" %s", uncStr(pdf4lhc_unc[i], n)); }

        printf("\n    PDF unc 21-30:");

        for (size_t i = 20; i < 30; ++i) { printf(" %s", uncStr(pdf4lhc_unc[i], n)); }

        printf("\n    alphaS up: %s, down: %s\n",
               uncStr(alphaS_up, n), uncStr(alphaS_dn, n));
      }

      printf("\n   There are %lu NNPDF 3.0 NLO uncertainty variations\n", nnpdf30_unc.size());

      // NNLOPS Specific stuff
      if (qcd_nnlops.size()) {
        printf("\n    ggF Powheg NNLOPS with %lu QCD uncertainty variations\n",
               qcd_nnlops.size());
        // for (auto q:qcd_nnlops) printf(" %s",uncStr(q,n)); printf("\n");

        printf("\n    Quark mass variations  (m_top=inf): %s  (m_b minlo): %s\n",
               uncStr(mt_inf, n), uncStr(mb_minlo, n));
        printf("\n    WG1 proposed QCD uncertainty scheme\n");
        printf("      mu: %s,   res: %s,   mig01: %s,   mig12: %s\n",
               uncStr(qcd_wg1_mu, n), uncStr(qcd_wg1_res, n), uncStr(qcd_wg1_mig01, n), uncStr(qcd_wg1_mig12, n));
        printf("      pTH: %s,   quark-mass, b: %s, t: %s\n",
               uncStr(qcd_wg1_pTH, n), uncStr(qcd_wg1_qm_b, n), uncStr(qcd_wg1_qm_b, n));

      }

      if (qcd.size()) {
        printf("\n    %lu Powheg QCD scale variations\n   ", qcd_nnlops.size());

        for (auto q : qcd) { printf(" %s", uncStr(q, n)); }

        printf("\n");
      }

      printf("\n   PDF central values\n");
      printf("     PDF4LHC_nlo:  %s, PDF4LHC_nnlo: %s\n", uncStr(pdf4lhc_nlo, n), uncStr(pdf4lhc_nnlo, n));
      printf("     NNPDF30_nlo:  %s, NNPDF30_nnlo: %s\n", uncStr(nnpdf30_nlo, n), uncStr(nnpdf30_nnlo, n));
      printf("     CT10nlo:      %s, CT14nlo:      %s\n", uncStr(ct10nlo, n), uncStr(ct14nlo, n));
      printf("     CT10nlo_0118: %s, CT14nlo_0118: %s\n", uncStr(ct10nlo_0118, n), uncStr(ct14nlo_0118, n));
      printf("     MMHT2014nlo:  %s\n", uncStr(mmht2014nlo, n));
      printf("\n------\n");
    } // print()

  };

  /// Tool for accessing of MC weights and other weigthts for QCD uncertainty propagation for Higgs analyses
  ///
  /// @author Dag Gillberg <dag.gillberg@cern.ch>
  ///
  /// $Revision$
  /// $Date$
  ///
  class HiggsWeightTool : public asg::AsgTool
  {
  public:

    /// Create a constructor for standalone usage
    HiggsWeightTool(const std::string &name = "xAOD::TruthWeightTool");
    virtual ~HiggsWeightTool() {}

    virtual StatusCode initialize();

    virtual StatusCode finalize();

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

    HiggsWeights getHiggsWeights(int HTXS_Njets30 = -1, double HTXS_pTH = -99.0, int HTXS_cat = -1);

    // returns hardcoded list of weight names matching expecation
    // of ATLAS-default ggF NNLOPS
    const std::vector<std::string> getDefaultNNLOPSweightNames();
    const std::vector<std::string> getDefaultVBFweightNames();
    const std::vector<std::string> getDefaultVHweightNames();

    /// Weight names in metadata
    const std::vector<std::string> &getWeightNames();

    /// @}

  private:

    /// linear interpolation
    double linInter(double x, double x1, double y1, double x2, double y2)
    {
      if (x < x1) { return y1; }

      if (x > x2) { return y2; }

      return y1 + (y2 - y1) * (x - x1) / (x2 - x1);
    }

    /// Access the HiggsWeights
    HiggsWeights getHiggsWeightsInternal(int HTXS_Njets30 = -1, double HTXS_pTH = -99.0, int HTXS_cat = -1);

    /// Protect against non-finite or outside-reqired-range weights
    void updateWeights(HiggsWeights &hw);
    void updateWeight(const double &w_nom, double &w);
    void updateWeights(const double &w_nom, std::vector<double> &ws) { for (auto &w : ws) updateWeight(w_nom, w); }
    void updateWeights(const double &w_nom, double &w1, double &w2) { updateWeight(w_nom, w1); updateWeight(w_nom, w2); }
    void updateWeights(const double &w_nom, double &w1, double &w2, double &w3) { updateWeights(w_nom, w1, w2); updateWeight(w_nom, w3); }


    /// Setup weights
    void setupWeights(size_t Nweights);

    double getWeight(const std::vector<float> &ws, size_t idx)
    {
      if (idx == 0) { return 0.0; }

      return ws.at(idx);
    }

    size_t getIndex(std::string wn)
    {
      if (hasWeight(wn)) { return getWeightIndex(wn); }

      return 0;
    }

    /// Flags
    bool m_init;

    enum mode { AUTO = 0, FORCE_GGF_NNLOPS = 1, FORCE_POWPY8_VBF = 2, FORCE_POWPY8_VH = 3 };
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

    /// options
    bool m_requireFinite;
    bool m_cutOff;
    double m_weightCutOff;

    /// For statistics
    int m_Nnom, m_Nws;
    double m_sumw_nom, m_sumw2_nom, m_sumw, m_sumw2;
    double m_sumw_nomC, m_sumw2_nomC, m_sumwC, m_sumw2C;

    /// index of weights
    size_t m_nom;

    /// weight indices for PDF+alphaS uncertainites
    std::vector<size_t> m_pdfUnc, m_pdfNNPDF30;
    size_t m_aS_up, m_aS_dn;

    /// Special PDF sets
    size_t m_nnpdf30_nlo, m_nnpdf30_nnlo, m_mmht2014nlo, m_pdf4lhc_nlo, m_pdf4lhc_nnlo;
    size_t m_ct10nlo, m_ct10nlo_0118, m_ct14nlo, m_ct14nlo_0118;

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
