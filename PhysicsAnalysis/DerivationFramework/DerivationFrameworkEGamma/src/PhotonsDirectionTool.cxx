/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// PhotonsDirectionTool.cxx, (c) ATLAS Detector software
// compute photon et = E(cluster)/cosh(eta of 2nd sampling)
// eventually E will be after recalibration
///////////////////////////////////////////////////////////////////
// Author: Giovanni Marchiori (giovanni.marchiori@cern.ch)
//

#include "DerivationFrameworkEGamma/PhotonsDirectionTool.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "xAODEgamma/PhotonContainer.h"

namespace DerivationFramework {

PhotonsDirectionTool::PhotonsDirectionTool(const std::string& t,
                                           const std::string& n,
                                           const IInterface* p)
  : AthAlgTool(t, n, p)
{
  declareInterface<DerivationFramework::IAugmentationTool>(this);
}

StatusCode
PhotonsDirectionTool::initialize()
{
  ATH_CHECK(m_collName.initialize());

  m_doEta = !(m_sgEta.key().empty());
  m_doPhi = !(m_sgPhi.key().empty());
  m_doEt = !(m_sgEt.key().empty());
  m_doE = !(m_sgE.key().empty());
  if (!m_doEta && !m_doPhi && !m_doE && !m_doEt) {
    ATH_MSG_ERROR("You are requesting the PhotonsDirectionTool but have "
                  "provided no SG names for any of the results");
    return StatusCode::FAILURE;
  }
  ATH_CHECK(m_sgEta.initialize(m_doEta));
  ATH_CHECK(m_sgPhi.initialize(m_doPhi));
  ATH_CHECK(m_sgEt.initialize(m_doEt));
  ATH_CHECK(m_sgE.initialize(m_doE));
  return StatusCode::SUCCESS;
}

StatusCode
PhotonsDirectionTool::addBranches() const
{
  const EventContext& ctx = Gaudi::Hive::currentContext();
  // Retrieve photon container
  SG::ReadHandle<xAOD::PhotonContainer> photons(m_collName, ctx);
  // define the pointers to vectors which will hold the additional payloads
  auto recEta = std::make_unique<std::vector<float>>();
  auto recPhi = std::make_unique<std::vector<float>>();
  auto recEt = std::make_unique<std::vector<float>>();
  auto recE = std::make_unique<std::vector<float>>();

  // Loop over electrons and fill the vectors
  for (const xAOD::Photon* ph : *photons) {
    // Prepare variables
    float eta(0.0), phi(0.0), e(0.0), et(0.0);

    if (ph->nCaloClusters() > 0) {
      const xAOD::CaloCluster* gCluster = ph->caloCluster(0);
      eta = gCluster->etaBE(2);
      phi = gCluster->phi();
      e = gCluster->e();
      et = e / cosh(eta);
    } else {
      ATH_MSG_WARNING(
        "Couldn't retrieve photon cluster, will use photon 4-momentum");
      eta = ph->eta();
      phi = ph->phi();
      e = ph->e();
      et = ph->pt();
    }
    recEta->push_back(eta);
    recPhi->push_back(phi);
    recEt->push_back(et);
    recE->push_back(e);
  } // end of loop over photons

  // Write payload to StoreGate for downstream client access
  if (m_doEta) {
    SG::WriteHandle<std::vector<float>> etas(m_sgEta, ctx);
    ATH_CHECK(etas.record(std::move(recEta)));
  }
  if (m_doPhi) {
    SG::WriteHandle<std::vector<float>> phis(m_sgPhi, ctx);
    ATH_CHECK(phis.record(std::move(recPhi)));
  }
  if (m_doEt) {
    SG::WriteHandle<std::vector<float>> ets(m_sgEt, ctx);
    ATH_CHECK(ets.record(std::move(recEt)));
  }
  if (m_doE) {
    SG::WriteHandle<std::vector<float>> es(m_sgE, ctx);
    ATH_CHECK(es.record(std::move(recE)));
  }
  return StatusCode::SUCCESS;
}

} // end of namespace
