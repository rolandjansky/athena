/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETCALIBTOOLS_JMSCORRECTION_H
#define JETCALIBTOOLS_JMSCORRECTION_H 1

 /*
 *  Class definition of JMSCorrection - see Root/JMSCorrection.cxx for more details
 *  Jonathan Bossio (jbossios@cern.ch) , July 2015
 */

#include <TEnv.h>
#include <TAxis.h>
#include <TH2F.h>
#include <TH2D.h>

#include "JetCalibTools/IJetCalibrationTool.h"
#include "JetCalibTools/JetCalibrationToolBase.h"

class JMSCorrection 
  : virtual public ::IJetCalibrationTool,
    virtual public ::JetCalibrationToolBase
{

  ASG_TOOL_CLASS( JMSCorrection, IJetCalibrationTool )

 public:
  //Some convenient typedefs
  typedef std::vector<TH2F*> VecTH2F;
  typedef std::vector<TH2D*> VecTH2D;
  typedef std::vector<double> VecD;
  typedef unsigned int uint;

  JMSCorrection();
  JMSCorrection(const std::string& name);
  JMSCorrection(const std::string& name, TEnv * config, TString jetAlgo, TString calibAreaTag, bool dev);
  virtual ~JMSCorrection();

  virtual StatusCode initializeTool(const std::string& name);

 protected:
  virtual StatusCode calibrateImpl(xAOD::Jet& jet, JetEventInfo&) const;

 private:
  float getMassCorr(double pT_uncorr, double mass_uncorr, int etabin) const;
  float getTrackAssistedMassCorr(double pT_uncorr, double mass_uncorr, int etabin) const;
  float getRelCalo(double pT_uncorr, double mass_over_pt_uncorr, int etabin) const;
  float getRelTA(double pT_uncorr, double mass_over_pt_uncorr, int etabin) const;
  float getRho(double pT_uncorr, double mass_over_pt_uncorr, int etabin) const;

  void setMassEtaBins(VecD etabins) { 
    if (etabins.size()==0) ATH_MSG_ERROR("Please check that the mass eta binning is properly set in your config file");
    m_massEtaBins=etabins;
  }

  void setMassCombinationEtaBins(VecD etabins) { 
    if (etabins.size()==0) ATH_MSG_ERROR("Please check that the mass combination eta binning is properly set in your config file");
    m_massCombinationEtaBins=etabins;
  }

 private:

  //Private members set in the constructor
  TEnv * m_config;
  TString m_jetAlgo, m_calibAreaTag, m_jetOutScale;
  bool m_dev;

  double m_pTMinCorr;

  bool m_trackAssistedJetMassCorr;

  mutable int m_warning_counter_mTACorr;

  bool m_pTfixed; // false: pT will be corrected (large-R), if true: the energy will be corrected and pT will be fixed (small-R)

  bool m_combination; // Mass Combination of calo mass with track-assisted mass
  bool m_useCorrelatedWeights; 

  // Control the binning using a private class enum
  enum class BinningParam { pt_mass_eta, e_LOGmOe_eta, e_LOGmOet_eta, e_LOGmOpt_eta };
  BinningParam m_binParam;


  //Private members set during initialization
  VecTH2F m_respFactorsMass;
  VecD m_massEtaBins;
  VecTH2F m_respFactorsTrackAssistedMass;
  VecD m_massCombinationEtaBins;
  VecTH2D m_caloResolutionMassCombination; // Calo Mass Resolution
  VecTH2D m_taResolutionMassCombination;   // Track-Assisted Mass Resolution
  VecTH2D m_correlationMapMassCombination;   // Correlation Map for mass combination (rho)
  
};

#endif
