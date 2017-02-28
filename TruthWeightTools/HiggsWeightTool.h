// $Id$
#ifndef TRUTHWEIGHTTOOLS_HIGGSWEIGHTTOOL_H
#define TRUTHWEIGHTTOOLS_HIGGSWEIGHTTOOL_H

// Local include(s):
#include "TruthWeightTools/TruthWeightTool.h"
#include "xAODTruth/TruthEvent.h"

namespace xAOD {

  /// Struct like class for Higgs weights
  //// @author Dag Gillberg <dag.gillberg@cern.ch> 
  class HiggsWeights {
  public:
    /// Nominal event weight
    double nominal;
    
    /// 30 PDF4LHC variations
    std::vector<double> pdf4lhc;

    /// alphaS up and down
    double alphaS_up, alphaS_dn;

    /// ggF QCD uncertainty scheme
    

    /// NNLOPS specific weights
    
    char *uncStr(double var, double nom) { return Form("%s%.3f",var>nom?"+":"",(var-nom)/nom); }
    void print() {
      double n=nominal;
      printf("\n  Higgs MC weights of current event\n");
      printf("    Nominal weight: %.3f\n",nominal);
      if (pdf4lhc.size()==30) {
	printf("\n    PDF unc  1-10:");
	for (size_t i=0;i<10;++i) printf(" %s%.3f",pdf4lhc[i]>n?"+":"",(pdf4lhc[i]-n)/n);
	printf("\n    PDF unc 11-20:");
	for (size_t i=10;i<20;++i) printf(" %s%.3f",pdf4lhc[i]>n?"+":"",(pdf4lhc[i]-n)/n);
	printf("\n    PDF unc 21-30:");
	for (size_t i=20;i<30;++i) printf(" %s%.3f",pdf4lhc[i]>n?"+":"",(pdf4lhc[i]-n)/n);
	printf("\n    alphaS up: %s, down: %s\n",uncStr(alphaS_up,n),uncStr(alphaS_dn,n));
	       
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

    HiggsWeights getHiggsWeights();

    // returns hardcoded list of weight names matching expecation
    // of ATLAS-default ggF NNLOPS 
    const std::vector<std::string> getDefaultNNLOPSweightNames();

    /// Weight names in metadata
    const std::vector<std::string> &getWeightNames();

    /// @}
    
  private:

    /// Setup weights
    void setupWeights(size_t Nweights);

    /// Flags 
    bool m_init;

    enum mode { AUTO=0, FORCE_GGF_NNLOPS = 1 };
    mode m_mode;
    bool m_forceNNLOPS;

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

    /// Special PDF sets TODO


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
