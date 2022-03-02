/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETCALIBTOOLS_INSITUDATACORRECTION_H
#define JETCALIBTOOLS_INSITUDATACORRECTION_H 1

/* Implementation of InsituDataCorrection class
 * This class will apply the residual offset pile up correction
 *
 * Author: Joe Taenzer (joseph.taenzer@cern.ch)
 * Date: August 15 2013
 */

#include <TROOT.h>
#include <TEnv.h>
#include <TFile.h>
#include <TAxis.h>
#include <TH1.h>
#include <TH2.h>

#include "JetCalibTools/JetCalibrationStep.h"

class InsituDataCorrection 
  : virtual public ::JetCalibrationStep
{

 public:
  InsituDataCorrection();
  // optional first and last run arguments are for time-dependent in situ calibration.
  // If lastRun is 0, all runs are calibrated (i.e. not time-dependent).
  InsituDataCorrection(const std::string& name, TEnv * config, TString jetAlgo, TString calibAreaTag, bool dev, unsigned int firstRun = 0, unsigned int lastRun = 0);
  virtual ~InsituDataCorrection();

  virtual StatusCode initialize() override;
  virtual StatusCode calibrate(xAOD::Jet& jet, JetEventInfo& jetEventInfo) const override;

  inline double getRelHistoPtMax(){return m_relhistoPtMax;}
  inline double getAbsHistoPtMax(){return m_abshistoPtMax;}

 private:
  double getInsituCorr(double pt, double eta, std::string calibstep) const;
  double getInsituCorr_JMS(double pt, double mass, double eta, std::string calibstep, bool isTAmass) const;
  std::unique_ptr<const TH2> combineCalibration(const TH2* h2d, const TH1* h);
  std::unique_ptr<const TH2> invertHistogram(const TH2* h2d);
 
 private:
  TEnv * m_config;
  TString m_jetAlgo, m_calibAreaTag;
  bool m_dev;

  std::unique_ptr<const TH2> m_insituCorr;
  std::unique_ptr<const TH2> m_insituCorr_JMS;
  std::unique_ptr<const TH2> m_insituCorr_JMS_TA;
  double m_insituEtaMax, m_insituPtMin, m_insituPtMax, m_insituEtaMax_JMS, m_insituPtMin_JMS, m_insituPtMax_JMS, m_insituMassMin_JMS, m_insituMassMax_JMS;
  double m_relhistoPtMax, m_abshistoPtMax;
  std::unique_ptr<const TH2> m_insituCorr_ResidualMCbased;
  double m_insituEtaMax_ResidualMCbased, m_insituPtMin_ResidualMCbased, m_insituPtMax_ResidualMCbased;

  bool m_applyRelativeandAbsoluteInsitu;
  bool m_applyEtaRestrictionRelativeandAbsolute;

  bool m_applyResidualMCbasedInsitu;
  bool m_applyEtaRestrictionResidualMCbased;

  bool m_applyInsituCaloTAjets;
  bool m_applyInsituJMS;

  unsigned int m_firstRun;
  unsigned int m_lastRun;
};

#endif
