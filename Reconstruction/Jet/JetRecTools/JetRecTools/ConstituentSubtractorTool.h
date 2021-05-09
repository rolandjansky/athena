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

#include "xAODEventInfo/EventInfo.h"
#include "xAODBase/IParticleContainer.h"
#include "xAODCaloEvent/CaloClusterContainer.h"

#include "AsgTools/AsgTool.h" 
#include "JetRecTools/JetConstituentModifierBase.h"
#include "AsgDataHandles/ReadHandleKey.h"
#include "AsgTools/PropertyWrapper.h"

class ConstituentSubtractorTool: public JetConstituentModifierBase {
ASG_TOOL_CLASS(ConstituentSubtractorTool, IJetConstituentModifier) 
public:
	
  ConstituentSubtractorTool(const std::string & name); 

  // Check that the configuration is sane
  StatusCode initialize();

private:
  // Implement the correction
  StatusCode process_impl(xAOD::IParticleContainer* cont) const; 

   /// Handle to EventInfo. This is used to get the evt&run number to set the Ghost area random seeds.
  SG::ReadHandleKey<xAOD::EventInfo> m_eventinfokey{"EventInfo"};
    
  Gaudi::Property<float> m_maxDeltaR = {this, "MaxDeltaR",0.25,""};
  Gaudi::Property<float> m_alpha     = {this, "Alpha",0.,""};
  Gaudi::Property<float> m_ghostArea = {this, "GhostArea",0.01,""};
  Gaudi::Property<float> m_maxEta    = {this, "MaxEta",2.5,""};

  Gaudi::Property<bool> m_ignoreChargedPFOs = {this, "IgnoreChargedPFO",0.,"Option to disregard cPFOs in the weight calculation"};

};

#endif
