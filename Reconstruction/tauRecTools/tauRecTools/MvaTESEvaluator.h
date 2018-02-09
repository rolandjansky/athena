/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
    
  StatusCode initialize();
  StatusCode eventInitialize();
  StatusCode execute(xAOD::TauJet& xTau);
  StatusCode finalize() { delete reader; return StatusCode::SUCCESS;}
  
 private:
   
  // Configurable properties
  std::string m_sWeightFileName;

  MVAUtils::BDT *reader; //!

  std::map<TString, float*> m_availableVars; //!< addresses of the floats below
  
  // MVA input variables (provide all variables in float)
  float mu; //!
  float nVtxPU; //!
  
  float center_lambda; //!
  float first_eng_dens; //!
  float second_lambda; //!
  float presampler_frac; //!
  float em_probability; //!
  
  float ptCombined; //!
  float ptLC_D_ptCombined; //!
  float ptConstituent_D_ptCombined;//!
  float etaConstituent; //!
  
  float PanTauBDT_1p0n_vs_1p1n; //!
  float PanTauBDT_1p1n_vs_1pXn; //!
  float PanTauBDT_3p0n_vs_3pXn; //!
  float nTracks; //!
  float PFOEngRelDiff; //!
  
  // Spectators
  float truthPtVis; //!
  float pt; //!
  float ptPanTauCellBased; //!
  // ptDetectorAxis is a primary input for online MVA TES
  float ptDetectorAxis; //!
  float truthDecayMode; //!
  float PanTau_DecayMode; //!

  // for online calibration
  float etaDetectorAxis; //!
  float upsilon_cluster; //!
  float lead_cluster_frac; //!

};

#endif // TAURECTOOLSDEV_MVATESEVALUATOR_H
