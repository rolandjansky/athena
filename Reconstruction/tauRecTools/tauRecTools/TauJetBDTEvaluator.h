/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAURECTOOLS_TAUJETBDTEVALUATOR_H
#define TAURECTOOLS_TAUJETBDTEVALUATOR_H

// tauRecTools include(s)
#include "tauRecTools/TauRecToolBase.h"
#include "tauRecTools/BDTHelper.h"

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
    
  virtual StatusCode initialize() override;
  virtual StatusCode execute(xAOD::TauJet& xTau) const override;
  virtual StatusCode finalize() override;
  
 private:
  std::unique_ptr<tauRecTools::BDTHelper> m_mvaBDT;
  std::string m_weightsFile;
  std::string m_outputVarName;
  int m_minNTracks;
  int m_maxNTracks;
  float m_minAbsTrackEta;
  float m_maxAbsTrackEta;
  float m_dummyValue;
};
#endif
