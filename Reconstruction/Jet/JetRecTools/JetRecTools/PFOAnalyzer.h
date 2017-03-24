// this file is -*- C++ -*- 

#ifndef JETRECTOOLS_PFOANALYZER_H
#define JETRECTOOLS_PFOANALYZER_H

////////////////////////////////////////////
/// \class PFOAnalyzer
///
/// \author John Stupak
//////////////////////////////////////////////////

#include <string>
#include "JetRecTools/JetConstituentModifierBase.h"
#include "xAODBase/IParticleContainer.h"
#include "xAODPFlow/PFOContainer.h"
#include "AsgTools/ToolHandle.h"

class PFOAnalyzer : public JetConstituentModifierBase{
  ASG_TOOL_CLASS(PFOAnalyzer, IJetConstituentModifier)

  std::string m_jetCollectionName;

  public:
  
  PFOAnalyzer(const std::string& name);
  StatusCode process(xAOD::IParticleContainer* cont) const; 
  StatusCode process(xAOD::PFOContainer* cont) const;
	
};

#endif