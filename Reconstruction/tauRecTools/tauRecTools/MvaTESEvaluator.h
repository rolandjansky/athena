/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAURECTOOLS_MVATESEVALUATOR_H
#define TAURECTOOLS_MVATESEVALUATOR_H

// tauRecTools include(s)
#include "tauRecTools/TauRecToolBase.h"
#include "tauRecTools/BDTHelper.h"

#include <map>

class MvaTESEvaluator
: public TauRecToolBase
{
 public:
  ASG_TOOL_CLASS2(MvaTESEvaluator, TauRecToolBase, ITauToolBase)
    
  MvaTESEvaluator(const std::string& name="MvaTESEvaluator");
  virtual ~MvaTESEvaluator();
    
  virtual StatusCode initialize() override;
  virtual StatusCode execute(xAOD::TauJet& xTau) const override;
  
 private:
  // MVA input variables (provide all variables in float)
  struct MvaInputVariables
  {
    float mu{0.0};
    float nVtxPU{0.0};
    float rho{0.0};

    float center_lambda{0.0};
    float first_eng_dens{0.0};
    float second_lambda{0.0};
    float presampler_frac{0.0};
    float eprobability{0.0};

    float ptEM_D_ptLC{0.0};
    float ptCombined{0.0};
    float ptLC_D_ptCombined{0.0};
    float ptConstituent_D_ptCombined{0.0};
    float etaConstituent{0.0};

    float PanTauBDT_1p0n_vs_1p1n{0.0};
    float PanTauBDT_1p1n_vs_1pXn{0.0};
    float PanTauBDT_3p0n_vs_3pXn{0.0};
    float nTracks{0.0};
    float PFOEngRelDiff{0.0};

    // for 0p
    float centFrac{0.0};
    float logPtCombined{0.0};

    // for online calibration
    float ptDetectorAxis{0.0};
    float etaDetectorAxis{0.0};
    float upsilon_cluster{0.0};
    float lead_cluster_frac{0.0};
  };

  std::unique_ptr<tauRecTools::BDTHelper> m_bdtHelper;
  std::unique_ptr<tauRecTools::BDTHelper> m_bdtHelper0p;

  // Configurable properties
  std::string m_sWeightFileName;
  std::string m_sWeightFileName0p;
};

#endif // TAURECTOOLS_MVATESEVALUATOR_H
