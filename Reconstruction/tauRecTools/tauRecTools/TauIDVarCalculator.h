/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
#include "xAODTau/TauJet.h"
#include <string>

class TauIDVarCalculator: public TauRecToolBase
{
  ASG_TOOL_CLASS2(TauIDVarCalculator, TauRecToolBase, ITauToolBase)

    public:
  
  TauIDVarCalculator(const std::string& name = "TauIDVarCalculator");
  
  virtual ~TauIDVarCalculator() {}

  virtual StatusCode eventInitialize();
  
  virtual StatusCode initialize();

  virtual StatusCode execute(xAOD::TauJet&);

  virtual StatusCode finalize();

  static const float LOW_NUMBER;
  
 private:

  std::string m_vertexContainerKey;
  int m_nVtx;
  float m_mu;
};

#endif
