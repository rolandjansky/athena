/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETCALIBTOOLS_ETAJESCORRECTION_H
#define JETCALIBTOOLS_ETAJESCORRECTION_H 1

/* Implementation of EtaJESCorrection class
 * This class will apply the absolute EtaJES correction
 *
 * Author: Joe Taenzer (joseph.taenzer@cern.ch)
 * Date: August 19 2013
 */

#include <TEnv.h>
#include <TAxis.h>

#include "JetCalibTools/IJetCalibrationTool.h"
#include "JetCalibTools/JetCalibrationToolBase.h"

class EtaJESCorrection 
  : virtual public ::IJetCalibrationTool,
    virtual public ::JetCalibrationToolBase
{

  ASG_TOOL_CLASS( EtaJESCorrection, IJetCalibrationTool )

 public:
  EtaJESCorrection();  
  EtaJESCorrection(const std::string& name);
  EtaJESCorrection(const std::string& name, TEnv * config, TString jetAlgo, TString calibAreaTag, bool mass, bool dev);
  virtual ~EtaJESCorrection();

  //virtual bool initializeTool(TEnv * config, TString jetAlgo);
  virtual StatusCode initializeTool(const std::string& name);

 protected:
  virtual StatusCode calibrateImpl(xAOD::Jet& jet, JetEventInfo&) const;
 
 private:
  double getJES(double E_uncorr, double eta_det) const;
  double getLowPtJES(double E_uncorr, double eta_det) const;
  double getEtaCorr(double E_corr, double eta_det) const;
  double getMassCorr(double E_corr, double eta_det) const;
  double getLogPolN(const double *factors, double x) const;
  double getLogPolNSlope(const double *factors, double x) const;
  int getEtaBin(double eta_det) const;

 private:
  TEnv * m_config;
  TString m_jetAlgo;
  TString m_calibAreaTag;
  bool m_mass;
  bool m_dev;
  bool m_freezeJESatHighE;

  TString m_jesDesc;
  double m_minPt_JES, m_minPt_EtaCorr, m_maxE_EtaCorr;
  unsigned int m_lowPtExtrap;
  double m_lowPtMinR;
  bool m_applyMassCorrection;
  bool m_useSecondaryminPt_JES;
  double m_etaSecondaryminPt_JES;
  double m_secondaryminPt_JES;

  TAxis * m_etaBinAxis;

  // 90 eta bins, and up to 9 parameter for the pol-fit
  const static unsigned int s_nEtaBins=90;
  const static unsigned int s_nParMin=7;
  const static unsigned int s_nParMax=9;
  unsigned int m_nPar; // number of parameters in config file
  double m_JESFactors[s_nEtaBins][s_nParMax];
  double m_JES_MinPt_Slopes[s_nEtaBins];
  double m_JES_MinPt_E[s_nEtaBins];
  double m_JES_MinPt_R[s_nEtaBins];
  //double m_JES_MinPt_Rmin[s_nEtaBins];
  double m_JES_MinPt_Param1[s_nEtaBins];
  double m_JES_MinPt_Param2[s_nEtaBins];
  double m_etaCorrFactors[s_nEtaBins][s_nParMax];
  double m_JMSFactors[s_nEtaBins][s_nParMax];
  double m_energyFreezeJES[s_nEtaBins];

};

#endif
