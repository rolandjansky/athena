/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMATOOLS_EMPIDBUILDER_H
#define EGAMMATOOLS_EMPIDBUILDER_H
/**
  @class EMPIDBuilder
  egamma tool to add selector outputs to the egamma objects 
  Note:  Subject to change after the PAT selectors evolve
 
  @author Jovan Mitrevski Jovan.Mitrevski@cern.ch

*/
// INCLUDE HEADER FILES: 
#include "GaudiKernel/ToolHandle.h"
#include "egammaBaseTool.h"

#include <string>
#include <vector>

class IAsgSelectionTool;
class IAsgElectronIsEMSelector;
class IAsgElectronLikelihoodTool;
class IAsgPhotonIsEMSelector;
class ILumiBlockMuTool;

class EMPIDBuilder : public egammaBaseTool
{
 public:

  /** @brief Default constructor*/
  EMPIDBuilder(const std::string& type,
	       const std::string& name,
	       const IInterface* parent);
   
  /** @brief Destructor*/
  ~EMPIDBuilder();
	
  /** @brief initialize method*/
  StatusCode initialize();
  /** @brief standard execute method */
  virtual StatusCode execute(xAOD::Egamma*);
  /** @brief finalize method*/
  StatusCode finalize();

 protected:
  /** Handle to the selectors */

  ToolHandleArray<IAsgElectronIsEMSelector> m_electronIsEMselectors;
  std::vector<std::string> m_electronIsEMselectorResultNames;
 
  ToolHandleArray<IAsgElectronLikelihoodTool> m_electronLHselectors;
  std::vector<std::string> m_electronLHselectorResultNames;
  
  ToolHandleArray<IAsgSelectionTool> m_genericIsEMselectors;
  std::vector<std::string> m_genericIsEMselectorResultNames;
  
  ToolHandleArray<IAsgPhotonIsEMSelector> m_photonIsEMselectors;
  std::vector<std::string> m_photonIsEMselectorResultNames;
 
  ToolHandle<ILumiBlockMuTool>  m_lumiBlockMuTool;

  std::string m_LHValueName;

 private:
  bool m_UselumiBlockMuTool;


  
};

#endif









