/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//! This class implements a tool to calculate ID input variables and add them to the tau aux store
/*!
 * Tau ID input variable calculator tool
 *
 * Author: Lorenz Hauswald
 */

#ifndef TAUIDVARCALCULATOR_H
#define TAUIDVARCALCULATOR_H

#include "tauRecTools/TauRecToolBase.h"
#include "AsgDataHandles/ReadHandleKey.h"

class TauIDVarCalculator: public TauRecToolBase
{
  ASG_TOOL_CLASS2(TauIDVarCalculator, TauRecToolBase, ITauToolBase)

    public:
  
  TauIDVarCalculator(const std::string& name = "TauIDVarCalculator");
  
  virtual ~TauIDVarCalculator() {}

  virtual StatusCode initialize() override;
  virtual StatusCode execute(xAOD::TauJet&) const override;

  static const float LOW_NUMBER;
  
 private:

  SG::ReadHandleKey<xAOD::VertexContainer> m_vertexInputContainer{this,"Key_vertexInputContainer", "PrimaryVertices", "input vertex container key"};

  bool m_incShowerSubtr;
};

#endif
