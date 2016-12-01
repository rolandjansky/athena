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
