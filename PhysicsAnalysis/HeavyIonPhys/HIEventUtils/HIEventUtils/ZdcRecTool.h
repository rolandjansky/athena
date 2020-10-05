/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HIEVENTUTILS_ZDCRECTOOL_H
#define HIEVENTUTILS_ZDCRECTOOL_H

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
    virtual StatusCode initializeTool() override;
    virtual StatusCode initialize() override {return initializeTool();}
    virtual StatusCode recoZdcModule(const xAOD::ZdcModule& module) override;
    virtual StatusCode recoZdcModules(const xAOD::ZdcModuleContainer& moduleContainer) override;
    virtual StatusCode reprocessZdc() override;

    // methods for processing, used for decoration
    // TODO: documentaton about the arguments of these methods must be written by experts
    bool sigprocMaxFinder(const std::vector<unsigned short>& adc, float deltaT, float& amp, float& time, float& qual);
    bool sigprocPeakFitter(const std::vector<unsigned short>& adc, float deltaT, float& amp, float& time, float& qual);
    bool sigprocSincInterp(const std::vector<unsigned short>& adc, float deltaT, float& amp, float& time, float& qual);

  private:

    // internal functions - moved to private accoding to T.Bold request while merging into master
    TF1* m_tf1SincInterp;
    TF1* m_tf1FermiExpFit;

    // Name of the ZDC rec tool
    std::string m_name;
    // Boolean to flag initialization
    bool m_init;
    // Path to the configuring file
    std::string m_zdcRecConfigPath;

    mutable bool m_eventReady;
    std::string m_zdcModuleContainerName;
    const xAOD::ZdcModuleContainer* m_zdcModules;

  };

} // namespace ZDC

#endif
