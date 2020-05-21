/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAURECTOOLSDEV_MVATESEVALUATOR_H
#define TAURECTOOLSDEV_MVATESEVALUATOR_H

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
  virtual StatusCode finalize() override { return StatusCode::SUCCESS; }
  
 private:
  // MVA input variables (provide all variables in float)
  struct MvaInputVariables
  {
    float mu{0.0}; //!
    float nVtxPU{0.0}; //!

    float center_lambda{0.0}; //!
    float first_eng_dens{0.0}; //!
    float second_lambda{0.0}; //!
    float presampler_frac{0.0}; //!
    float eprobability{0.0}; //!

    float ptCombined{0.0}; //!
    float ptLC_D_ptCombined{0.0}; //!
    float ptConstituent_D_ptCombined{0.0};//!
    float etaConstituent{0.0}; //!

    float PanTauBDT_1p0n_vs_1p1n{0.0}; //!
    float PanTauBDT_1p1n_vs_1pXn{0.0}; //!
    float PanTauBDT_3p0n_vs_3pXn{0.0}; //!
    float nTracks{0.0}; //!
    float PFOEngRelDiff{0.0}; //!

    // Spectators
    float truthPtVis{0.0}; //!
    float pt{0.0}; //!
    float ptPanTauCellBased{0.0}; //!
    float ptDetectorAxis{0.0}; //!
    float truthDecayMode{0.0}; //!
    float PanTau_DecayMode{0.0}; //!

    // for online calibration
    float etaDetectorAxis{0.0}; //!
    float upsilon_cluster{0.0}; //!
    float lead_cluster_frac{0.0}; //!
  };

  std::unique_ptr<tauRecTools::BDTHelper> m_bdtHelper;

  // Configurable properties
  std::string m_sWeightFileName;
};

#endif // TAURECTOOLSDEV_MVATESEVALUATOR_H
