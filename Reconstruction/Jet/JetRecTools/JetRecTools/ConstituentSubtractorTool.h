// this file is -*- C++ -*- 

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


/////////////////////////////////////////////////////////////////////////////
/// \class ConstituentSubtractorTool
/// \brief Constituent modifier implementing the Constituent Substraction technique
///
/// \author P-A Delsart, T.J. Khoo, others ??
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
#include "fastjet/FunctionOfPseudoJet.hh"
#include "TH1.h"
#include "TH2.h"

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
  Gaudi::Property<float> m_maxEta    = {this, "MaxEta",5.5,""};

  Gaudi::Property<float> m_maxRapForRhoComputation = {this, "MaxRapForRhoComputation", 2.0, ""};
  Gaudi::Property<float> m_gridSize = {this, "GridSize", 0.5, ""};  // grid size (not area) for GridMedianBackgroundEstimator
  Gaudi::Property<bool> m_commonBgeForRhoAndRhom = {this, "CommonBgeForRhoAndRhom",false, ""};
  Gaudi::Property<bool> m_doRapidityRescaling = {this, "DoRapidityRescaling",false, ""};
  Gaudi::Property<bool> m_doRapidityPhiRescaling = {this, "DoRapidityPhiRescaling",false, ""};
  Gaudi::Property<std::string> m_rescalingFileName = {this, "RescalingFileName","", ""};
  Gaudi::Property<std::string> m_rescalingHistogramName = {this, "RescalingHistogramName","", ""};

  Gaudi::Property<bool> m_ignoreChargedPFOs = {this, "IgnoreChargedPFO",0.,"Option to disregard cPFOs in the weight calculation"};

  std::unique_ptr<fastjet::FunctionOfPseudoJet<double> > m_rescaling=nullptr;
  std::unique_ptr<TH1D> m_hist=nullptr;
  std::unique_ptr<TH2D> m_hist2D=nullptr;


  
};

#endif
