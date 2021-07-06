/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// EGCrackVetoCleaningTool.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
//

#include "DerivationFrameworkEGamma/EGCrackVetoCleaningTool.h"
#include "xAODEgamma/EgammaxAODHelpers.h"

namespace DerivationFramework {

EGCrackVetoCleaningTool::EGCrackVetoCleaningTool(const std::string& t,
                                                 const std::string& n,
                                                 const IInterface* p)
  : AthAlgTool(t, n, p)
  , m_sgName("")
{
  declareInterface<DerivationFramework::IAugmentationTool>(this);
  declareProperty("StoreGateEntryName", m_sgName);
}

StatusCode
EGCrackVetoCleaningTool::initialize()
{
  if (m_sgName.empty()) {
    ATH_MSG_ERROR(
      "No SG name provided for the output of EGCrackVetoCleaningTool!");
    return StatusCode::FAILURE;
  }
  ATH_CHECK(m_containerName.initialize());
  m_decoratorPass = m_containerName.key() + "." + m_sgName;
  ATH_CHECK(m_decoratorPass.initialize());
  return StatusCode::SUCCESS;
}

StatusCode
EGCrackVetoCleaningTool::addBranches() const
{

  // retrieve container
  const EventContext& ctx = Gaudi::Hive::currentContext();
  SG::ReadHandle<xAOD::EgammaContainer> particles{ m_containerName, ctx };
  SG::WriteDecorHandle<xAOD::EgammaContainer, char> decoratorPass(
    m_decoratorPass, ctx);

  // Write mask for each element and record to SG for subsequent selection
  for (const xAOD::Egamma* eg : *particles) {
    const xAOD::CaloCluster* clu = eg->caloCluster();
    bool maybeBug = clu->hasSampling(CaloSampling::EMB2) &&
                    clu->hasSampling(CaloSampling::EME2);
    const std::vector<const xAOD::CaloCluster*> assocC =
      xAOD::EgammaHelpers::getAssociatedTopoClusters(clu);
    double dRsatMax = -1.;
    if (assocC.size() > 1) {
      for (const xAOD::CaloCluster* sclu : assocC) {
        double dR = clu->p4().DeltaR(sclu->p4());
        if (dR > dRsatMax)
          dRsatMax = dR;
      }
    }
    decoratorPass(*eg) = !(maybeBug && assocC.size() > 1 &&
                           dRsatMax > 0.16); // (or dPhi > 0.15 better ?)
  }

  return StatusCode::SUCCESS;
}
} // end namespace Derivation
