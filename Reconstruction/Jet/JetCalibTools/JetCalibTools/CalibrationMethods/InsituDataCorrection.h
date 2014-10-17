/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
#include <TAxis.h>
#include <TH1D.h>
#include <TH2D.h>

#include "JetCalibTools/IJetCalibrationTool.h"
#include "JetCalibTools/JetCalibrationToolBase.h"

class InsituDataCorrection 
  : virtual public ::IJetCalibrationTool,
    virtual public ::JetCalibrationToolBase
{

  ASG_TOOL_CLASS( InsituDataCorrection, IJetCalibrationTool )

 public:
  InsituDataCorrection();
  InsituDataCorrection(const std::string& name);
  InsituDataCorrection(const std::string& name, TEnv * config, TString jetAlgo);
  virtual ~InsituDataCorrection();

  virtual StatusCode initializeTool(const std::string& name);

 protected:
  virtual StatusCode calibrateImpl(xAOD::Jet& jet, JetEventInfo&) const;

 private:
  double getInsituCorr(double pt, double eta) const;
  TH2D * combineCalibration(TH2D *h2d, TH1D *h);
 
 private:
  TEnv * m_config;
  TString m_jetAlgo;

  TH2D * m_insituCorr;
  double m_insituEtaMax, m_insituPtMin, m_insituPtMax;

  std::vector<double> m_resOffsetMu, m_resOffsetNPV;

};

#endif
