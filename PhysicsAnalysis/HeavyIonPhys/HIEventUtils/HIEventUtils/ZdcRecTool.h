/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __ZDCRECTOOL_H__
#define __ZDCRECTOOL_H__

#include "AsgTools/AsgTool.h"
#include "HIEventUtils/IZdcRecTool.h"
#include "xAODForward/ZdcModuleContainer.h"
#include "xAODTrigL1Calo/TriggerTowerContainer.h"

#include "TF1.h"
#include "TMath.h"

namespace ZDC
{

double SincInterp(double* xvec, double* pvec);
double FermiExpFit(double* xvec, double* pvec);

class ZdcRecTool : public virtual IZdcRecTool, public asg::AsgTool
{
  
  ASG_TOOL_CLASS(ZdcRecTool,IZdcRecTool)
  
 public:
  ZdcRecTool(const std::string& name);
  virtual ~ZdcRecTool();

  //interface from AsgTool
  StatusCode initializeTool();
  StatusCode initialize() {return initializeTool();}
  StatusCode recoZdcModule(const xAOD::ZdcModule& module);
  StatusCode recoZdcModules(const xAOD::ZdcModuleContainer& moduleContainer);
  StatusCode reprocessZdc();

  // methods for processing, used for decoration
  bool sigprocMaxFinder(const std::vector<unsigned short>& adc, float deltaT, float& amp, float& time, float& qual);
  bool sigprocPeakFitter(const std::vector<unsigned short>& adc, float deltaT, float& amp, float& time, float& qual);
  bool sigprocSincInterp(const std::vector<unsigned short>& adc, float deltaT, float& amp, float& time, float& qual);

  // internal functions
  TF1* tf1SincInterp;
  TF1* tf1FermiExpFit;

 private:

  std::string m_name;
  bool m_init;
  std::string m_zdcRecConfigPath;

  mutable bool m_eventReady;
  float m_highLowTransition[2][4];
  float m_moduleWeights[2][4];
  std::string m_zdcModuleContainerName;
  const xAOD::ZdcModuleContainer* m_zdcModules;

};

} // namespace ZDC

#endif



