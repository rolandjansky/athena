/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMATOOLS_EMPIDBUILDER_H
#define EGAMMATOOLS_EMPIDBUILDER_H
/**
  @class EMPIDBuilder
  egamma tool to add selector outputs to the egamma objects
  Note:  Subject to change after the PAT selectors evolve

  @author Jovan Mitrevski Jovan.Mitrevski@cern.ch

*/
#include "GaudiKernel/EventContext.h"
#include "GaudiKernel/ToolHandle.h"
#include "egammaBaseTool.h"

#include <string>
#include <vector>

#include "EgammaAnalysisInterfaces/IAsgElectronIsEMSelector.h"
#include "EgammaAnalysisInterfaces/IAsgElectronLikelihoodTool.h"
#include "EgammaAnalysisInterfaces/IAsgPhotonIsEMSelector.h"
#include "LumiBlockComps/ILumiBlockMuTool.h"

class EMPIDBuilder : public egammaBaseTool
{
public:
  /** @brief Default constructor*/
  EMPIDBuilder(const std::string& type,
               const std::string& name,
               const IInterface* parent);

  /** @brief Destructor*/
  virtual ~EMPIDBuilder() = default;

  /** @brief initialize method*/
  virtual StatusCode initialize() override final;
  /** @brief standard execute method */
  virtual StatusCode execute(const EventContext& ctx,
                             xAOD::Egamma*) const override final;

private:
  /** Handle to the selectors */
  ToolHandleArray<IAsgElectronIsEMSelector> m_electronIsEMselectors{
    this,
    "electronIsEMselectors",
    {},
    "The selectors that we need to apply to the Electron object"
  };
  Gaudi::Property<std::vector<std::string>> m_electronIsEMselectorResultNames{
    this,
    "electronIsEMselectorResultNames",
    {},
    "The selector result names"
  };

  ToolHandleArray<IAsgElectronLikelihoodTool> m_electronLHselectors{
    this,
    "electronLHselectors",
    {},
    "The selectors that we need to apply to the LH electron object"
  };
  Gaudi::Property<std::vector<std::string>> m_electronLHselectorResultNames{
    this,
    "electronLHselectorResultNames",
    {},
    "The selector result names"
  };

  ToolHandleArray<IAsgSelectionTool> m_genericIsEMselectors{
    this,
    "genericIsEMselectors",
    {},
    "The selectors that we need to apply to the generic object"
  };

  Gaudi::Property<std::vector<std::string>> m_genericIsEMselectorResultNames{
    this,
    "genericIsEMselectorResultNames",
    {},
    "The selector result names"
  };

  ToolHandleArray<IAsgPhotonIsEMSelector> m_photonIsEMselectors{
    this,
    "photonIsEMselectors",
    {},
    "The selectors that we need to apply to the pothon object"
  };
  Gaudi::Property<std::vector<std::string>> m_photonIsEMselectorResultNames{
    this,
    "photonIsEMselectorResultNames",
    {},
    "The selector result names"
  };

  ToolHandle<ILumiBlockMuTool> m_lumiBlockMuTool{
    this,
    "LuminosityTool",
    "LumiBlockMuTool/LumiBlockMuTool",
    "Luminosity Tool"
  };

  Gaudi::Property<std::string> m_LHValueName{ this,
                                              "LHValueName",
                                              "LHValue",
                                              "The LH Value name" };

  Gaudi::Property<bool> m_UselumiBlockMuTool{
    this,
    "UseLuminosityTool",
    false,
    "Use Luminosity Tool instead of value stored in xAOD"
  };
};

#endif

