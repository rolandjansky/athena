/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETCALIBTOOLS_JETPILEUPCORRECTION_H
#define JETCALIBTOOLS_JETPILEUPCORRECTION_H 1

/* Implementation of JetAreaSubtraction class
 * This class will apply the jet area pile up correction
 *
 * Author: Joe Taenzer (joseph.taenzer@cern.ch)
 * Date: June 27 2013
 */

#include <TEnv.h>

#include "JetCalibTools/IJetCalibrationTool.h"
#include "JetCalibTools/JetCalibrationToolBase.h"
#include "JetCalibTools/CalibrationMethods/ResidualOffsetCorrection.h"

namespace PUCorrection {
  struct PU3DCorrectionHelper;
}

class JetPileupCorrection
  : virtual public ::JetCalibrationToolBase
{

  ASG_TOOL_CLASS( JetPileupCorrection, IJetCalibrationTool )

 public:
  JetPileupCorrection();
  JetPileupCorrection(const std::string& name);
  JetPileupCorrection(const std::string& name, TEnv * config, TString jetAlgo, TString calibAreaTag, bool doResidual, bool doJetArea, bool doOrigin, bool isData, bool dev);
  virtual ~JetPileupCorrection();

  //virtual bool initializeTool(const std::string& name, TEnv * config, TString jetAlgo, bool doResidual, bool isData);
  virtual StatusCode initializeTool(const std::string& name);

 protected:
  virtual StatusCode calibrateImpl(xAOD::Jet& jet, JetEventInfo& jetEventInfo) const;
 
 private:
  TEnv * m_config;
  TString m_jetAlgo;
  TString m_calibAreaTag;
  bool m_dev;
  bool m_doResidual;
  bool m_doJetArea;
  bool m_doOrigin;
  bool m_isData;
  bool m_doMuOnly;
  bool m_doNPVOnly;
  bool m_doNJetOnly;
  bool m_doSequentialResidual;

  bool m_do3Dcorrection;

  bool m_useFull4vectorArea;
  ResidualOffsetCorrection * m_residualOffsetCorr;

  std::unique_ptr<PUCorrection::PU3DCorrectionHelper> m_residual3DCorr;

  bool m_doOnlyResidual;

  std::string m_originScale;

};

#endif
