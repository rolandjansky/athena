/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETCALIBTOOLS_BCIDOFFSETCORRECTION_H
#define JETCALIBTOOLS_BCIDOFFSETCORRECTION_H 1

/* Implementation of BcidOffsetCorrection class
 * This class will apply the bcid offset pile up correction
 *
 * Author: Chris Young (christopher.young@cern.ch)
 * Date: November 23 2020
 */

#include <TEnv.h>
#include <TAxis.h>

#include "JetCalibTools/IJetCalibrationTool.h"
#include "JetCalibTools/JetCalibrationToolBase.h"

#include <memory>

class BcidOffsetCorrection 
: virtual public ::IJetCalibrationTool,
  virtual public ::JetCalibrationToolBase
{

  ASG_TOOL_CLASS( BcidOffsetCorrection, IJetCalibrationTool )

    public:
  BcidOffsetCorrection();
  BcidOffsetCorrection(const std::string& name);
  BcidOffsetCorrection(const std::string& name, TEnv * config, TString jetAlgo, TString calibAreaTag, bool isData, bool dev);
  virtual ~BcidOffsetCorrection();

  //virtual bool initializeTool(TEnv * config, TString jetAlgo, bool isData);
  virtual StatusCode initializeTool(const std::string& name);

  //double GetResidualOffset ( double abseta, double mu, double NPV, int nJet, bool MuOnly, bool NOnly) const;

 private:
  //double GetResidualOffsetET(double abseta, double mu, double NPV, int nJet, bool MuOnly, bool NOnly,
  //                           std::vector<double> OffsetMu, std::vector<double> OffsetNPV, std::vector<double> OffsetNjet,
  //                           TAxis *OffsetBins) const;

  //double GetNPVBeamspotCorrection(double NPV) const;

 protected:
  virtual StatusCode calibrateImpl(xAOD::Jet& jet, JetEventInfo&) const;
 
 private:
  TEnv * m_config;
  TString m_jetAlgo, m_calibAreaTag;
  bool m_isData;

  bool m_doEMECIW2bcid;
  bool m_doEMECIW3bcid;
  bool m_doHEC0bcid;
  bool m_doHEC1bcid;
  bool m_doHEC2bcid;
  bool m_doHEC3bcid;
  bool m_doFCal0OuterBcid;
  bool m_doFCal1OuterBcid;
  bool m_doFCal2OuterBcid;
  bool m_doFCal0bcid;
  bool m_doFCal1bcid;
  bool m_doFCal2bcid;
  bool m_doFCal0InnerBcid;
  bool m_doFCal1InnerBcid;
  bool m_doFCal2InnerBcid;

  std::unique_ptr<const TH2> m_bcid_nCells_EMECIW2;
  std::unique_ptr<const TH2> m_bcid_nCells_HEC0;
  std::unique_ptr<const TH2> m_bcid_nCells_HEC1;
  std::unique_ptr<const TH2> m_bcid_nCells_FCal0;

  std::unique_ptr<const TH2> m_bcid_slope_EMECIW2_pos;
  std::unique_ptr<const TH2> m_bcid_slope_HEC0_pos;
  std::unique_ptr<const TH2> m_bcid_slope_HEC1_pos;

  std::unique_ptr<const TH2> m_bcid_slope_EMECIW2_neg;
  std::unique_ptr<const TH2> m_bcid_slope_HEC0_neg;
  std::unique_ptr<const TH2> m_bcid_slope_HEC1_neg;

  std::unique_ptr<const TH2> m_bcid_slope_FCal0;

  std::unique_ptr<const TH2> m_bcid_slope_EMECIW2_8b4e_pos;
  std::unique_ptr<const TH2> m_bcid_slope_HEC0_8b4e_pos;
  std::unique_ptr<const TH2> m_bcid_slope_HEC1_8b4e_pos;

  std::unique_ptr<const TH2> m_bcid_slope_EMECIW2_8b4e_neg;
  std::unique_ptr<const TH2> m_bcid_slope_HEC0_8b4e_neg;
  std::unique_ptr<const TH2> m_bcid_slope_HEC1_8b4e_neg;

  std::unique_ptr<const TH2> m_bcid_slope_FCal0_8b4e;

  JetEventInfo m_eventInfo;

};

#endif
