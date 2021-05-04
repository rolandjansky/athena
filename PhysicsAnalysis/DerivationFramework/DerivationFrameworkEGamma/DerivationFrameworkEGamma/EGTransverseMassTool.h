/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// EGTransverseMassTool.h, (c) ATLAS Detector software
// author: giovanni.marchiori@cern.ch
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_EGTRANSVERSEMASSTOOL_H
#define DERIVATIONFRAMEWORK_EGTRANSVERSEMASSTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"
//
#include "ExpressionEvaluation/ExpressionParserUser.h"
#include "GaudiKernel/EventContext.h"
#include "StoreGate/ReadHandleKey.h"
#include "xAODBase/IParticleContainer.h"
#include "xAODMissingET/MissingETContainer.h"
//
#include <string>
#include <vector>

namespace DerivationFramework {

class EGTransverseMassTool
  : public ExpressionParserUser<AthAlgTool>
  , public IAugmentationTool
{
public:
  EGTransverseMassTool(
    const std::string& t,
    const std::string& n,
    const IInterface* p);

  virtual StatusCode initialize() override final;
  virtual StatusCode addBranches() const override final;

private:
  StatusCode getTransverseMasses(const EventContext& ctx, std::vector<float>&)
    const;

  std::string m_expression1;
  float m_METmin;
  float m_mass1Hypothesis;

  SG::WriteHandleKey<std::vector<float>> m_sgName{ this,
                                                   "StoreGateEntryName",
                                                   "",
                                                   "SG key of output object" };

  SG::ReadHandleKey<xAOD::IParticleContainer> m_container1Name{
    this,
    "ObjectContainerName",
    "",
    "SG key of first container"
  };
  SG::ReadHandleKey<xAOD::MissingETContainer> m_container2Name{
    this,
    "METContainerName",
    "MET_LocHadTopo",
    "SG key of second container"
  };

  SG::ReadHandleKey<std::vector<float>> m_pt1BranchName{
    this,
    "ObjectPtBranchName",
    "",
    "Pt1 if different than default"
  };

  SG::ReadHandleKey<std::vector<float>> m_phi1BranchName{
    this,
    "ObjectPhiBranchName",
    "",
    "Phi1 if different than default"
  };

  SG::ReadHandleKey<std::vector<float>> m_pt2BranchName{
    this,
    "METPtBranchName",
    "",
    "Pt2 if different than default"
  };

  SG::ReadHandleKey<std::vector<float>> m_phi2BranchName{
    this,
    "METPhiBranchName",
    "",
    "Phi2 if different than default"
  };
};
}

#endif // DERIVATIONFRAMEWORK_EGTRANSVERSEMASSTOOL_H
