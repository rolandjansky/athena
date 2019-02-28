/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAURECTOOLS_TAUJETBDTEVALUATOR_H
#define TAURECTOOLS_TAUJETBDTEVALUATOR_H

// tauRecTools include(s)
#include "tauRecTools/TauRecToolBase.h"
#include "tauRecTools/HelperFunctions.h"

/**
 * @brief Implementation of a generic BDT for tau ID
 * @brief Can support dynamic loading of floats attached
 * @brief to the tau object, wih a few modifications to
 * @brief HelperFunctions TRTBDT, should handle ints too
 * @author Justin Griffiths
 *                                                                              
 */

class TauJetBDTEvaluator
: public TauRecToolBase
{
 public:
  ASG_TOOL_CLASS2(TauJetBDTEvaluator, TauRecToolBase, ITauToolBase)

  TauJetBDTEvaluator(const std::string& name="TauJetBDTEvaluator");
  virtual ~TauJetBDTEvaluator() { }
    
  StatusCode initialize();
  StatusCode execute(xAOD::TauJet& xTau);
  StatusCode executeShotFinder(xAOD::TauJet&, xAOD::CaloClusterContainer&, xAOD::PFOContainer&) { return StatusCode::SUCCESS; }
  StatusCode executePi0CreateROI(xAOD::TauJet&, CaloCellContainer&) { return StatusCode::SUCCESS; }
  StatusCode executePi0ClusterCreator(xAOD::TauJet&, xAOD::PFOContainer&, xAOD::PFOContainer&, xAOD::CaloClusterContainer&) { return StatusCode::SUCCESS; }
  StatusCode executeVertexVariables(xAOD::TauJet&, xAOD::VertexContainer&) { return StatusCode::SUCCESS; }
  StatusCode executePi0ClusterScaler(xAOD::TauJet&, xAOD::PFOContainer&, xAOD::PFOContainer&) { return StatusCode::SUCCESS; }
  StatusCode executePi0nPFO(xAOD::TauJet&, xAOD::PFOContainer&) { return StatusCode::SUCCESS; }
  StatusCode executePanTau(xAOD::TauJet&, xAOD::ParticleContainer&) { return StatusCode::SUCCESS; }
  StatusCode finalize();// { delete myBdt; delete m_outputVar; return StatusCode::SUCCESS;}
  
 private:

  std::string m_weightsFile;
  std::string m_outputVarName;

  tauRecTools::TRTBDT* m_myBdt=0;
  int m_minNTracks;
  int m_maxNTracks;
  float m_minAbsTrackEta;
  float m_maxAbsTrackEta;
  float m_dummyValue;// in case no configs are set, set a dummy value.

  bool m_isGrad;
};
#endif
