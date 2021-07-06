/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// EGInvariantMassTool.h, (c) ATLAS Detector software
// author: giovanni.marchiori@cern.ch
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_EGINVARIANTMASSTOOL_H
#define DERIVATIONFRAMEWORK_EGINVARIANTMASSTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"
//
#include "ExpressionEvaluation/ExpressionParserUser.h"
#include "GaudiKernel/EventContext.h"
#include "StoreGate/ReadHandleKey.h"
#include "xAODBase/IParticleContainer.h"
//
#include <string>
#include <vector>
namespace DerivationFramework {

enum EEGInvariantMassToolParser
{
  kParser1,
  kParser2,
  kNumEGInvariantMassToolParser
};
class EGInvariantMassTool
  : public ExpressionParserUser<AthAlgTool, kNumEGInvariantMassToolParser>
  , public IAugmentationTool
{
public:
  EGInvariantMassTool(const std::string& t,
                      const std::string& n,
                      const IInterface* p);

  virtual StatusCode initialize() override final;
  virtual StatusCode addBranches() const override final;

private:
  StatusCode getInvariantMasses(const EventContext& ctx,
                                std::vector<float>&) const;

  std::string m_expression1, m_expression2;
  SG::WriteHandleKey<std::vector<float>> m_sgName{ this,
                                                   "StoreGateEntryName",
                                                   "",
                                                   "SG key of output object" };

  SG::ReadHandleKey<xAOD::IParticleContainer> m_container1Name{
    this,
    "Container1Name",
    "",
    "SG key of first  container"
  };
  SG::ReadHandleKey<xAOD::IParticleContainer> m_container2Name{
    this,
    "Container2Name",
    "",
    "SG key of second container"
  };

  SG::ReadHandleKey<std::vector<float>> m_pt1BranchName{
    this,
    "Pt1BranchName",
    "",
    "Pt1 if different than default"
  };

  SG::ReadHandleKey<std::vector<float>> m_eta1BranchName{
    this,
    "Eta1BranchName",
    "",
    "Eta1 if different than default"
  };

  SG::ReadHandleKey<std::vector<float>> m_phi1BranchName{
    this,
    "Phi1BranchName",
    "",
    "Phi1 if different than default"
  };

  SG::ReadHandleKey<std::vector<float>> m_pt2BranchName{
    this,
    "Pt2BranchName",
    "",
    "Pt2 if different than default"
  };

  SG::ReadHandleKey<std::vector<float>> m_eta2BranchName{
    this,
    "Eta2BranchName",
    "",
    "Eta2 if different than default"
  };

  SG::ReadHandleKey<std::vector<float>> m_phi2BranchName{
    this,
    "Phi2BranchName",
    "",
    "Phi2 if different than default"
  };

  float m_mass1Hypothesis, m_mass2Hypothesis;
  float m_mindR;
  bool m_checkCharge;
  bool m_doTransverseMass;
};
}

#endif // DERIVATIONFRAMEWORK_EGINVARIANTMASSTOOL_H
