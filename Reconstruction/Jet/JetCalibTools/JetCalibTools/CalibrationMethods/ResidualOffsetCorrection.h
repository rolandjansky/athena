/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETCALIBTOOLS_RESIDUALOFFSETCORRECTION_H
#define JETCALIBTOOLS_RESIDUALOFFSETCORRECTION_H 1

/* Implementation of ResidualOffsetCorrection class
 * This class will apply the residual offset pile up correction
 *
 * Author: Joe Taenzer (joseph.taenzer@cern.ch)
 * Date: August 15 2013
 */

#include <TEnv.h>
#include <TAxis.h>

#include "JetCalibTools/IJetCalibrationTool.h"
#include "JetCalibTools/JetCalibrationToolBase.h"
#include "JetCalibTools/CalibrationMethods/NPVBeamspotCorrection.h"

class ResidualOffsetCorrection 
  : virtual public ::IJetCalibrationTool,
    virtual public ::JetCalibrationToolBase
{

  ASG_TOOL_CLASS( ResidualOffsetCorrection, IJetCalibrationTool )

 public:
  ResidualOffsetCorrection();
  ResidualOffsetCorrection(const std::string& name);
  ResidualOffsetCorrection(const std::string& name, TEnv * config, TString jetAlgo, TString calibAreaTag, bool isData, bool dev);
  virtual ~ResidualOffsetCorrection();

  //virtual bool initializeTool(TEnv * config, TString jetAlgo, bool isData);
  virtual StatusCode initializeTool(const std::string& name);

  double GetResidualOffset ( double abseta, double mu, double NPV, int nJet, bool MuOnly, bool NOnly) const;

 private:
  double GetResidualOffsetET(double abseta, double mu, double NPV, int nJet, bool MuOnly, bool NOnly,
                             std::vector<double> OffsetMu, std::vector<double> OffsetNPV, std::vector<double> OffsetNjet,
                             TAxis *OffsetBins) const;

  double GetNPVBeamspotCorrection(double NPV) const;
 
 private:
  TEnv * m_config;
  TString m_jetAlgo, m_calibAreaTag;
  bool m_dev;
  bool m_isData;

  NPVBeamspotCorrection * m_npvBeamspotCorr;
  JetEventInfo m_eventInfo;

  TString m_resOffsetDesc;
  TAxis * m_resOffsetBins;
  bool m_applyNPVBeamspotCorrection;
  double m_muSF;
  double m_mu_ref, m_NPV_ref, m_nJet_ref;
  bool m_useNjet;

  std::vector<double> m_resOffsetMu, m_resOffsetNPV, m_resOffsetNjet;

};

#endif
