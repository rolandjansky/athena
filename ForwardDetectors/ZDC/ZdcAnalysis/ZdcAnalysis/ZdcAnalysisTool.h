/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ZDCANALYSIS_ZDCANALYSISTOOL_H
#define ZDCANALYSIS_ZDCANALYSISTOOL_H

#include "AsgTools/AsgTool.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODForward/ZdcModuleContainer.h"
#include "xAODTrigL1Calo/TriggerTowerContainer.h"

#include "ZdcAnalysis/ZDCDataAnalyzer.h"
#include "ZdcAnalysis/ZDCTriggerEfficiency.h"
#include "ZdcAnalysis/IZdcAnalysisTool.h"

#include "TF1.h"
#include "TMath.h"

namespace ZDC
{

class ZdcAnalysisTool : public virtual IZdcAnalysisTool, public asg::AsgTool
{
  
  ASG_TOOL_CLASS(ZdcAnalysisTool, ZDC::IZdcAnalysisTool)
  
 public:
  ZdcAnalysisTool(const std::string& name);
  virtual ~ZdcAnalysisTool();

  //interface from AsgTool
  virtual StatusCode initializeTool() override;
  virtual StatusCode initialize() override {return initializeTool();}
  void initialize80MHz();
  void initialize40MHz();
  void initializeTriggerEffs(unsigned int runNumber);

  virtual StatusCode recoZdcModule(const xAOD::ZdcModule& module) override;
  virtual StatusCode recoZdcModules(const xAOD::ZdcModuleContainer& moduleContainer) override;

  // methods for processing, used for decoration
  bool sigprocMaxFinder(const std::vector<unsigned short>& adc, float deltaT, float& amp, float& time, float& qual);
  bool sigprocSincInterp(const std::vector<unsigned short>& adc, float deltaT, float& amp, float& time, float& qual);

  void setEnergyCalibrations(unsigned int runNumber);
  void setTimeCalibrations(unsigned int runNumber);

  float getModuleSum(int side);

  //float getModuleFitAmplitude(int side, int imod);
  //float getModuleFitT0(int side, int imod);
  //float getModuleAmplitude(int side, int imod);
  //float getModuleTime(int side, int imod);
  //float getModuleChisq(int side, int imod);
  //float getModuleStatus(int side, int imod);

  //float getModuleCalibAmplitude(int side, int imod);
  //float getModuleCalibTime(int side, int imod);

  float getCalibModuleSum(int side);
  float getAverageTime(int side);
  bool  sideFailed(int side);
  unsigned int getModuleMask();

  float getTriggerEfficiency(int side);

  const ZDCDataAnalyzer* getDataAnalyzer() {return m_zdcDataAnalyzer;}

 private:
  // Private methods
  //
  ZDCDataAnalyzer* initializeDefault();
  ZDCDataAnalyzer* initializepPb2016();

  StatusCode configureNewRun(unsigned int runNumber);

  // Data members
  //
  std::string m_name;
  std::string m_configuration;
  std::string m_zdcAnalysisConfigPath;
  std::string m_zdcEnergyCalibFileName;
  std::string m_zdcTimeCalibFileName;
  std::string m_zdcTriggerEffParamsFileName;

  bool m_writeAux;
  std::string m_auxSuffix;

  unsigned int m_runNumber;
  unsigned int m_lumiBlock;

  // internal functions
  TF1* m_tf1SincInterp;

  SG::ReadHandleKey<xAOD::EventInfo>           m_eventInfoKey;
  SG::WriteHandleKey<xAOD::ZdcModuleContainer> m_ZdcModuleWriteKey;
  bool m_flipEMDelay;
  bool m_lowGainOnly;
  bool m_combineDelay;
  bool m_doCalib;
  int m_forceCalibRun;
  int m_forceCalibLB;

  //  Parameters that control the pulse fitting analysis
  //	
  unsigned int m_numSample;
  float m_deltaTSample;
  unsigned int m_presample;
  unsigned int m_peakSample;
  float m_Peak2ndDerivThresh;
  float m_t0;
  float m_delayDeltaT;
  float m_tau1;
  float m_tau2;
  bool m_fixTau1;
  bool m_fixTau2;
  float m_deltaTCut;
  float m_ChisqRatioCut;

  std::array<std::array<TSpline*, 4>, 2> m_splines;

  ZDCDataAnalyzer* m_zdcDataAnalyzer;
  ZDCDataAnalyzer* m_zdcDataAnalyzer_40MHz;
  ZDCDataAnalyzer* m_zdcDataAnalyzer_80MHz;
  ZDCDataAnalyzer::ZDCModuleFloatArray m_peak2ndDerivMinSamples;
  ZDCDataAnalyzer::ZDCModuleFloatArray m_peak2ndDerivMinThresholdsHG;
  ZDCDataAnalyzer::ZDCModuleFloatArray m_peak2ndDerivMinThresholdsLG;


  ZDCTriggerEfficiency* m_zdcTriggerEfficiency;

};

} // namespace ZDC

#endif



