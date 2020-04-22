/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// Asg wrapper around the MissingMassCalculator
// author Quentin Buat <quentin.buat@no.spam.cern.ch>
#ifndef DITAUMASSTOOLS_MISSINGMASSTOOLV2_H
#define DITAUMASSTOOLS_MISSINGMASSTOOLV2_H

// Framework include(s):
#include "AsgTools/AsgTool.h"

//Local include(s):
#include "DiTauMassTools/IMissingMassTool.h"
#include "DiTauMassTools/MissingMassCalculatorV2.h"
#include "DiTauMassTools/HelperFunctions.h"

#include <string>

namespace DiTauMassTools{

class MissingMassToolV2 : virtual public IMissingMassTool, virtual public asg::AsgTool
{

  /// Proper constructor for Athena
  ASG_TOOL_CLASS(MissingMassToolV2, IMissingMassTool)

 public:
  
  /// Standard constructor for standalone usage
  MissingMassToolV2(const std::string& name);
  /// Copy constructor for reflex in Athena
  MissingMassToolV2(const MissingMassToolV2& other);

  /// virtual destructor
  virtual ~MissingMassToolV2() { };

  /// Initialize the tool
  virtual StatusCode initialize();

  /// Initialize the tool
  virtual StatusCode finalize();


  // generic method
  virtual CP::CorrectionCode apply (const xAOD::EventInfo& ei,
				    const xAOD::IParticle* part1,
				    const xAOD::IParticle* part2,
				    const xAOD::MissingET* met,
				    const int & njets);

  virtual void calculate(const xAOD::EventInfo & ei, 
			 const TLorentzVector & vis_tau1,
			 const TLorentzVector & vis_tau2,
			 const int & tau1_decay_type,
			 const int & tau2_decay_type,
			 const xAOD::MissingET & met,
			 const int & njets){
	  ignore(ei); ignore(vis_tau1); ignore(vis_tau2);
	  ignore(tau1_decay_type); ignore(tau2_decay_type);
	  ignore(met); ignore(njets);}

  virtual MissingMassCalculatorV2* get() {return m_MMC;}
  virtual double GetFitStatus(int method) {(void) method; return m_MMC->OutputInfo.GetFitStatus();}
  virtual double GetFittedMass(int method) {return m_MMC->OutputInfo.GetFittedMass(method);}
  virtual double GetFittedMassErrorUp(int method) {return m_MMC->OutputInfo.GetFittedMassErrorUp(method);}
  virtual double GetFittedMassErrorLow(int method) {return m_MMC->OutputInfo.GetFittedMassErrorLow(method);}
  virtual TLorentzVector GetResonanceVec(int method) {return m_MMC->OutputInfo.GetResonanceVec(method);}
  virtual TVector2 GetFittedMetVec(int method) {return m_MMC->OutputInfo.GetFittedMetVec(method);}
  virtual TLorentzVector GetNeutrino4vec(int method, int index) {return m_MMC->OutputInfo.GetNeutrino4vec(method, index);}
  virtual TLorentzVector GetTau4vec(int method, int index) {return m_MMC->OutputInfo.GetTau4vec(method, index);}
  virtual int GetNNoSol() {return m_MMC->GetNNoSol();}
  virtual int GetNMetroReject() {return m_MMC->GetNMetroReject();}
  virtual int GetNSol() {return m_MMC->GetNSol();}

 private:

  MissingMassCalculatorV2* m_MMC;
  int m_alg_version;
  double m_n_sigma_met;
  int m_tail_cleanup;
  int m_use_verbose;
  int m_use_tau_probability;
  int m_niter_fit_2;
  int m_niter_fit_3;
  int m_use_defaults;
  int m_use_efficiency_recovery;
  std::string m_calib_set;
  std::string m_lfv_mode;
  bool m_decorate;
  bool m_float_stop;
  bool m_use_mnu_probability;
  bool m_use_met_param_dphiLL;
  std::string m_param_file_path;

};
} // namespace DiTauMassTools  

#endif
