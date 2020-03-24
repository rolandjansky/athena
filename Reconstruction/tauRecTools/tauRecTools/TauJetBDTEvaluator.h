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
    
  StatusCode initialize() override;
  StatusCode execute(xAOD::TauJet& xTau) override
  {
    return static_cast<const TauJetBDTEvaluator*>(this)->execute(xTau);
  }
  StatusCode execute(xAOD::TauJet& xTau) const;
  StatusCode finalize() override;
  
 private:

  Gaudi::Property<std::string> m_weightsFile{this, "weightsFile", ""};
  Gaudi::Property<std::string> m_outputVarName{this, "outputVarName", "BDTJetScore"};

  std::unique_ptr<tauRecTools::BDTHelper> m_mvaBDT;
  Gaudi::Property<int> m_minNTracks{this, "minNTracks", 0};
  Gaudi::Property<int> m_maxNTracks{this, "maxNTracks", 999};
  Gaudi::Property<float> m_minAbsTrackEta{this, "minAbsTrackEta", -1};
  Gaudi::Property<float> m_maxAbsTrackEta{this, "maxAbsTrackEta", -1};
  Gaudi::Property<float> m_dummyValue{this, "defaultValue", -1111, "if no weightsFile, then set all taus to this value nTrack/eta ignored"};
};
#endif
