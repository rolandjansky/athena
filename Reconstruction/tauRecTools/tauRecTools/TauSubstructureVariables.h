/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAURECTOOLS_TAUSUBSTRUCTUREVARIABLES_H
#define TAURECTOOLS_TAUSUBSTRUCTUREVARIABLES_H

#include "tauRecTools/TauRecToolBase.h"

#include <string>

/**
 * @brief Calculate variables from the tau substructure.
 * 
 * @author M. Trottier-McDonald
 * @author Felix Friedrich
 * 
 */

class TauSubstructureVariables : public TauRecToolBase {

public: 

  ASG_TOOL_CLASS2(TauSubstructureVariables, TauRecToolBase, ITauToolBase)
  
  TauSubstructureVariables(const std::string& name="TauSubstructureVariables");

  virtual ~TauSubstructureVariables() = default;

  virtual StatusCode execute(xAOD::TauJet& tau) const override;

  static const float DEFAULT;

private:

  bool m_doVertexCorrection;
};

#endif // TAURECTOOLS_TAUSUBSTRUCTUREVARIABLES_H
