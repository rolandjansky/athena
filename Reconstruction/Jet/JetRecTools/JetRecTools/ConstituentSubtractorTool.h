// this file is -*- C++ -*- 

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/////////////////////////////////////////////////////////////////////////////
/// \class ConstituentSubtractorTool
/// \brief Constituent modifier implementing the Constituent Substraction technique
///
/// \author P-A Delsart
///////////////////////////////////////////////////////////////////////

#ifndef CALOCLUSTERCONSTITUENTSUBSTRACTOR_H
#define CALOCLUSTERCONSTITUENTSUBSTRACTOR_H

#include "AsgTools/AsgTool.h" 
#include "JetRecTools/JetConstituentModifierBase.h"
#include "xAODBase/IParticleContainer.h"
#include "xAODCaloEvent/CaloClusterContainer.h"

class ConstituentSubtractorTool: public JetConstituentModifierBase {
ASG_TOOL_CLASS(ConstituentSubtractorTool, IJetConstituentModifier) 
public:
	
  ConstituentSubtractorTool(const std::string & name); 

  StatusCode process_impl(xAOD::IParticleContainer* cont) const; 

public:

  float m_maxDeltaR;
  float m_alpha;
  float m_maxEta;
  float m_maxRapForRhoComputation;
};

#endif
