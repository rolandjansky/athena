// this file is -*- C++ -*- 

/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
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

#include <string>
#include "fastjet/FunctionOfPseudoJet.hh"
#include "TFile.h"
#include "TH1D.h"
#include "TH2D.h"


class ConstituentSubtractorTool: public JetConstituentModifierBase {
ASG_TOOL_CLASS(ConstituentSubtractorTool, IJetConstituentModifier) 
public:
	
  ConstituentSubtractorTool(const std::string & name); 

  // Check that the configuration is sane
  StatusCode initialize();

private:
  // Implement the correction
  StatusCode process_impl(xAOD::IParticleContainer* cont) const; 

  float m_maxDeltaR = 0.25;
  float m_alpha = 0.0;
  float m_maxEta = 4.5;
  float m_maxRapForRhoComputation = 2.0;
  float m_gridSize = 0.5;
  float m_ghostArea = 0.01;
  bool m_commonBgeForRhoAndRhom = false;
  bool m_ignoreChargedPFOs = true;
  bool m_useWorkaroundForBugInFastjetContrib=true;
  bool m_doRapidityRescaling = false;
  bool m_doRapidityPhiRescaling = false;
  std::string m_fileRescaling;
  std::string m_histogramRescaling;
  std::unique_ptr<fastjet::FunctionOfPseudoJet<double> > m_rescaling=nullptr;
  std::unique_ptr<TH1D> m_hist=nullptr;
  std::unique_ptr<TH2D> m_hist2D=nullptr;
};

#endif
