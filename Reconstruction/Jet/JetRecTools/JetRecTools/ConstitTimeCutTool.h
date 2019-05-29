/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETRECTOOLS_CONSTITTIMECUTTOOL_H
#define JETRECTOOLS_CONSTITTIMECUTTOOL_H

// \class ConstitTimeCutTool
// \author Jennifer Roloff
// \date October 2016
//
// This tool places cuts on constituents based on their timing information


#include "JetRecTools/JetConstituentModifierBase.h"
#include "xAODBase/IParticleContainer.h"

#include <string>

class ConstitTimeCutTool : public JetConstituentModifierBase{
  ASG_TOOL_CLASS(ConstitTimeCutTool, IJetConstituentModifier)

  public:
  
  ConstitTimeCutTool(const std::string& name);

  // Check that the configuration is sane
  StatusCode initialize();

  private:
  StatusCode process_impl(xAOD::IParticleContainer* cont) const; 
  StatusCode applyTimingCut(xAOD::IParticle* part, const float& time, const float& quality, const float& lambda_center) const; 

  // Properties.
  float m_lambdaCalDivide; 
  float m_qualityCut; 
  float m_timeCutLargeQ;
  float m_timeCutSmallQ; 
  float m_etaMax;
		
};


#endif
