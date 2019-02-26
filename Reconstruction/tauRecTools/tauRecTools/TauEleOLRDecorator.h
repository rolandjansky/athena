/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//! This class implements a tool to decorate the tau with a link to the matched electron as well as its llh score and OLR decision(s)
/*!
 * Tau Electron Overlap Removal Decorator Tool
 *
 * Author: Dirk Duschinger
 * Modified: Lorenz Hauswald
 */

#ifndef TAUELEOLRDECORATOR_H
#define TAUELEOLRDECORATOR_H

#include "tauRecTools/TauRecToolBase.h"
#include "xAODEgamma/ElectronContainer.h"
#include "TH2F.h"

class AsgElectronLikelihoodTool;

class TauEleOLRDecorator: virtual public TauRecToolBase
{
  ASG_TOOL_CLASS2(TauEleOLRDecorator, TauRecToolBase, ITauToolBase)

    public:
  
  TauEleOLRDecorator(const std::string& name = "TauEleOLRDecorator");
  
  virtual ~TauEleOLRDecorator();

  virtual StatusCode eventInitialize();
  
  virtual StatusCode initialize();

  virtual StatusCode execute(xAOD::TauJet&);
  virtual StatusCode executeShotFinder(xAOD::TauJet&, xAOD::CaloClusterContainer&, xAOD::PFOContainer&) { return StatusCode::SUCCESS; }
  virtual StatusCode executePi0CreateROI(xAOD::TauJet&, CaloCellContainer&) { return StatusCode::SUCCESS; }
  virtual StatusCode executePi0ClusterCreator(xAOD::TauJet&, xAOD::PFOContainer&, xAOD::PFOContainer&, xAOD::CaloClusterContainer&) { return StatusCode::SUCCESS; }
  virtual StatusCode executePi0nPFO(xAOD::TauJet&, xAOD::PFOContainer&) { return StatusCode::SUCCESS; }
  virtual StatusCode executePanTau(xAOD::TauJet&, xAOD::ParticleContainer&) { return StatusCode::SUCCESS; }
  virtual StatusCode executeVertexVariables(xAOD::TauJet&, xAOD::VertexContainer&) { return StatusCode::SUCCESS; }
  virtual StatusCode executePi0ClusterScaler(xAOD::TauJet&, xAOD::PFOContainer&, xAOD::PFOContainer&) { return StatusCode::SUCCESS; }


  virtual StatusCode finalize();

 private:
  std::unique_ptr<AsgElectronLikelihoodTool> m_tEMLHTool;
  const xAOD::ElectronContainer* m_xElectronContainer;
  bool m_bElectonsAvailable;
  std::string m_sEleOLRFilePath;
  std::unique_ptr<TH2F> m_hCutValues;
#ifndef XAODTAU_VERSIONS_TAUJET_V3_H
  bool m_bEleOLRMatchAvailable=false;
  bool m_bEleOLRMatchAvailableChecked=false;
#endif
  
 private:
  float getCutVal(float fEta, float fPt);
  
  SG::ReadHandleKey<xAOD::ElectronContainer> m_electronInputContainer{this,"Key_electronInputContainer", "Electrons", "input electron container key"};
};

#endif
