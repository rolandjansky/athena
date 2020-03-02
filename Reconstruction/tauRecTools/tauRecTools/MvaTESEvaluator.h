/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAURECTOOLSDEV_MVATESEVALUATOR_H
#define TAURECTOOLSDEV_MVATESEVALUATOR_H

// tauRecTools include(s)
#include "tauRecTools/TauRecToolBase.h"

#include "MVAUtils/BDT.h"

#include <map>

class MvaTESEvaluator
: public TauRecToolBase
{
 public:
  ASG_TOOL_CLASS2(MvaTESEvaluator, TauRecToolBase, ITauToolBase)
    
  MvaTESEvaluator(const std::string& name="MvaTESEvaluator");
  virtual ~MvaTESEvaluator();
    
  StatusCode initialize() override;
  StatusCode execute(xAOD::TauJet& xTau) override;
  StatusCode finalize() override { return StatusCode::SUCCESS; }
  
 private:
  // Configurable properties
  std::string m_sWeightFileName;

  MVAUtils::BDT *m_reader; //!

  std::map<TString, float*> m_availableVars; //!< addresses of the floats below
  
  // MVA input variables (provide all variables in float)
  float m_mu; //!
  float m_nVtxPU; //!
  
  float m_center_lambda; //!
  float m_first_eng_dens; //!
  float m_second_lambda; //!
  float m_presampler_frac; //!
  float m_em_probability; //!
  
  float m_ptCombined; //!
  float m_ptLC_D_ptCombined; //!
  float m_ptConstituent_D_ptCombined;//!
  float m_etaConstituent; //!
  
  float m_PanTauBDT_1p0n_vs_1p1n; //!
  float m_PanTauBDT_1p1n_vs_1pXn; //!
  float m_PanTauBDT_3p0n_vs_3pXn; //!
  float m_nTracks; //!
  float m_PFOEngRelDiff; //!
  
  // Spectators
  float m_truthPtVis; //!
  float m_pt; //!
  float m_ptPanTauCellBased; //!
  float m_ptDetectorAxis; //!
  float m_truthDecayMode; //!
  float m_PanTau_DecayMode; //!

  // for online calibration
  float m_etaDetectorAxis; //!
  float m_upsilon_cluster; //!
  float m_lead_cluster_frac; //!
};

#endif // TAURECTOOLSDEV_MVATESEVALUATOR_H
