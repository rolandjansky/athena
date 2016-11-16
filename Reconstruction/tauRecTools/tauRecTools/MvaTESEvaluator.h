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
  StatusCode execute(xAOD::TauJet& xTau);
  StatusCode finalize() { delete reader; return StatusCode::SUCCESS;}
  
 private:
   
  // Configurable properties
  std::string m_sWeightFileName;

  MVAUtils::BDT *reader; //!

  std::map<TString, float*> m_availableVars; //!< addresses of the floats below
    
  // MVA input variables (provide all variables in float)
  float mu; //!
  float nVtx; //!
    
  float center_lambda; //!
  float first_eng_dens; //!
  float second_lambda; //!
  float presampler_frac; //!
  float em_probability; //!
    
  float interpolPt; //!
  float LC_D_interpolPt; //!
  float pantau_D_interpolPt;//!
  float seedCalo_eta; //!
    
  float nTracks; //!
  float nPi0PFOs; //!
  float PFOEngRelDiff; //!
    
  float nMuSeg; //!
    
  // Spectators
  float truthPtVis; //!
};

#endif // TAURECTOOLSDEV_MVATESEVALUATOR_H
