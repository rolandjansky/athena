/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//! This class implements a tool to flatten a discriminant score
/*!
 * Tool to transform discriminant score to be flat in nVtx and pT
 *
 * Author: Lorenz Hauswald
 */

#ifndef TAUSCOREFLATTENINGTOOL_H
#define TAUSCOREFLATTENINGTOOL_H

#include "tauRecTools/TauRecToolBase.h"
#include "TauDiscriminant/TauCutGraph.h"
#include "TDirectory.h"

class TauScoreFlatteningTool: virtual public TauRecToolBase
{
  ASG_TOOL_CLASS2(TauScoreFlatteningTool, TauRecToolBase, ITauToolBase)

    public:
  
  TauScoreFlatteningTool(const std::string& name = "TauScoreFlatteningTool");
  
  virtual ~TauScoreFlatteningTool() {}

  virtual StatusCode initialize();

  virtual StatusCode execute(xAOD::TauJet&);

  virtual StatusCode finalize();

  virtual StatusCode eventInitialize() { return StatusCode::SUCCESS; }
  virtual StatusCode executeShotFinder(xAOD::TauJet&, xAOD::CaloClusterContainer&, xAOD::PFOContainer&) { return StatusCode::SUCCESS; }
  virtual StatusCode executePi0CreateROI(xAOD::TauJet&, CaloCellContainer&) { return StatusCode::SUCCESS; }
  virtual StatusCode executePi0ClusterCreator(xAOD::TauJet&, xAOD::PFOContainer&, xAOD::PFOContainer&, xAOD::CaloClusterContainer&) { return StatusCode::SUCCESS; }
  virtual StatusCode executeVertexVariables(xAOD::TauJet&, xAOD::VertexContainer&) { return StatusCode::SUCCESS; }
  virtual StatusCode executePi0ClusterScaler(xAOD::TauJet&, xAOD::PFOContainer&, xAOD::PFOContainer&) { return StatusCode::SUCCESS; }
  virtual StatusCode executePi0nPFO(xAOD::TauJet&, xAOD::PFOContainer&) { return StatusCode::SUCCESS; }
  virtual StatusCode executePanTau(xAOD::TauJet&, xAOD::ParticleContainer&) { return StatusCode::SUCCESS; }
  virtual StatusCode eventFinalize() { return StatusCode::SUCCESS; }




 private:
  std::string m_inputFile;
  std::vector<TauCutGraph> m_graphs1p; //!
  std::vector<TauCutGraph> m_graphs3p; //!
  double m_effVeryLoose1P;
  double m_effLoose1P;
  double m_effMedium1P;
  double m_effTight1P;

  double m_effVeryLoose3P;
  double m_effLoose3P;
  double m_effMedium3P;
  double m_effTight3P;

  bool m_defineWP;

 private:
  static bool passesCut(const TauCutGraph& graph, const xAOD::TauJet& tau);
  void fetchInput(TDirectory* directory, std::vector<TauCutGraph>& graphvector);
};

#endif
