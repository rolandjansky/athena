/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// EGInvariantMassTool.cxx, (c) ATLAS Detector software
// Author: Giovanni Marchiori (giovanni.marchiori@cern.ch)
///////////////////////////////////////////////////////////////////

#include "DerivationFrameworkEGamma/EGInvariantMassTool.h"
#include "xAODEgamma/Electron.h"
#include "xAODMuon/Muon.h"
#include <cmath>
using std::abs;
using std::sqrt;

#include "TLorentzVector.h"

namespace DerivationFramework {

EGInvariantMassTool::EGInvariantMassTool(const std::string& t,
                                         const std::string& n,
                                         const IInterface* p)
  : ExpressionParserUser<AthAlgTool, kNumEGInvariantMassToolParser>(t, n, p)
  , m_expression1("true")
  , m_expression2("true")
  , m_mass1Hypothesis(0.0)
  , m_mass2Hypothesis(0.0)
  , m_mindR(0.0)
  , m_checkCharge(true)
  , m_doTransverseMass(false)
{
  declareInterface<DerivationFramework::IAugmentationTool>(this);
  declareProperty("Object1Requirements", m_expression1);
  declareProperty("Object2Requirements", m_expression2);
  declareProperty("Mass1Hypothesis", m_mass1Hypothesis);
  declareProperty("Mass2Hypothesis", m_mass2Hypothesis);
  declareProperty("CheckCharge", m_checkCharge);
  declareProperty("MinDeltaR", m_mindR);
  declareProperty("DoTransverseMass", m_doTransverseMass);
}

StatusCode
EGInvariantMassTool::initialize()
{
  if (m_sgName.key().empty()) {
    ATH_MSG_ERROR("No SG name provided for the output of EGInvariantMassTool!");
    return StatusCode::FAILURE;
  }
  ATH_CHECK(m_sgName.initialize());

  if (!m_container1Name.key().empty()) {
    ATH_CHECK(m_container1Name.initialize());
  }
  if (!m_container2Name.key().empty()) {
    ATH_CHECK(m_container2Name.initialize());
  }
  if (!m_pt1BranchName.key().empty()) {
    ATH_CHECK(m_pt1BranchName.initialize());
  }
  if (!m_eta1BranchName.key().empty()) {
    ATH_CHECK(m_eta1BranchName.initialize());
  }
  if (!m_phi1BranchName.key().empty()) {
    ATH_CHECK(m_phi1BranchName.initialize());
  }
  if (!m_pt2BranchName.key().empty()) {
    ATH_CHECK(m_pt2BranchName.initialize());
  }
  if (!m_eta2BranchName.key().empty()) {
    ATH_CHECK(m_eta2BranchName.initialize());
  }
  if (!m_phi2BranchName.key().empty()) {
    ATH_CHECK(m_phi2BranchName.initialize());
  }

  ATH_CHECK(initializeParser({ m_expression1, m_expression2 }));

  return StatusCode::SUCCESS;
}

StatusCode
EGInvariantMassTool::addBranches() const
{

  const EventContext& ctx = Gaudi::Hive::currentContext();
  SG::WriteHandle<std::vector<float>> writeHandle{ m_sgName, ctx };

  // create the vector which will hold the values invariant masses
  auto masses = std::make_unique<std::vector<float>>();
  // compute the invariant mass values
  ATH_CHECK(getInvariantMasses(ctx,*masses));

  ATH_CHECK(writeHandle.record(std::move(masses)));

  return StatusCode::SUCCESS;
}

StatusCode
EGInvariantMassTool::getInvariantMasses(const EventContext& ctx,
                                        std::vector<float>& masses) const
{
  // Get optional payload
  const std::vector<float>* pt1 = nullptr;
  if (!m_pt1BranchName.key().empty()) {
    SG::ReadHandle<std::vector<float>> readHandle{ m_pt1BranchName, ctx };
    pt1 = readHandle.ptr();
  }
  const std::vector<float>* pt2 = nullptr;
  if (!m_pt2BranchName.key().empty()) {
    SG::ReadHandle<std::vector<float>> readHandle{ m_pt2BranchName, ctx };
    pt2 = readHandle.ptr();
  }

  const std::vector<float>* eta1 = nullptr;
  if (!m_eta1BranchName.key().empty()) {
    SG::ReadHandle<std::vector<float>> readHandle{ m_eta1BranchName, ctx };
    eta1 = readHandle.ptr();
  }
  const std::vector<float>* eta2 = nullptr;
  if (!m_eta2BranchName.key().empty()) {
    SG::ReadHandle<std::vector<float>> readHandle{ m_eta2BranchName, ctx };
    eta2 = readHandle.ptr();
  }

  const std::vector<float>* phi1 = nullptr;
  if (!m_phi1BranchName.key().empty()) {
    SG::ReadHandle<std::vector<float>> readHandle{ m_phi1BranchName, ctx };
    phi1 = readHandle.ptr();
  }
  const std::vector<float>* phi2 = nullptr;
  if (!m_phi2BranchName.key().empty()) {
    SG::ReadHandle<std::vector<float>> readHandle{ m_phi2BranchName, ctx };
    phi2 = readHandle.ptr();
  }
  // Get the input particles
  SG::ReadHandle<xAOD::IParticleContainer> inputParticles1{ m_container1Name,
                                                            ctx };
  SG::ReadHandle<xAOD::IParticleContainer> inputParticles2{ m_container2Name,
                                                            ctx };
  const xAOD::IParticleContainer* particles1 = inputParticles1.ptr();
  const xAOD::IParticleContainer* particles2 = inputParticles2.ptr();

  // get the positions of the elements which pass the requirement
  std::vector<int> entries1 = m_parser[kParser1]->evaluateAsVector();
  unsigned int nEntries1 = entries1.size();
  std::vector<int> entries2 = m_parser[kParser2]->evaluateAsVector();
  unsigned int nEntries2 = entries2.size();

  // if there are no particles in one of the two lists to combine,
  // just leave the function
  if (nEntries1 == 0 || nEntries2 == 0) {
    return StatusCode::SUCCESS;
  }

  // check the sizes are compatible
  if (particles1->size() != nEntries1) {
    ATH_MSG_ERROR("Branch sizes incompatible - returning zero");
    return StatusCode::FAILURE;
  }
  if (particles2->size() != nEntries2) {
    ATH_MSG_ERROR("Branch sizes incompatible - returning zero");
    return StatusCode::FAILURE;
  }
  if ((pt1 && pt1->size() != nEntries1) ||
      ((!m_doTransverseMass || (m_mindR > 0.0)) && eta1 &&
       eta1->size() != nEntries1) ||
      (phi1 && phi1->size() != nEntries1)) {
    ATH_MSG_ERROR("Branch sizes incompatible - returning zero");
    return StatusCode::FAILURE;
  }
  if ((pt2 && pt2->size() != nEntries2) ||
      ((!m_doTransverseMass || (m_mindR > 0.0)) && eta2 &&
       eta2->size() != nEntries2) ||
      (phi2 && phi2->size() != nEntries2)) {
    ATH_MSG_ERROR("Branch sizes incompatible - returning zero");
    return StatusCode::FAILURE;
  }

  // Double loop to get the pairs for which the mass should be calculated
  unsigned int outerIt, innerIt;
  std::vector<std::vector<int>> pairs;
  for (outerIt = 0; outerIt < nEntries1; ++outerIt) {
    for (innerIt = 0; innerIt < nEntries2; ++innerIt) {
      std::vector<int> tmpPair;
      if (entries1[outerIt] == 1 && entries2[innerIt] == 1) {
        tmpPair.push_back(outerIt);
        tmpPair.push_back(innerIt);
        pairs.push_back(tmpPair);
      }
    }
  }

  // since IParticle interface does not provide charge() method
  // we need to identify the particle type in case we want to check
  // its charge (through a type cast)
  xAOD::Type::ObjectType type1 = xAOD::Type::Other;
  xAOD::Type::ObjectType type2 = xAOD::Type::Other;
  if (m_checkCharge) {
    type1 = ((*particles1)[0])->type();
    type2 = ((*particles2)[0])->type();
    if ((type1 != xAOD::Type::Electron && type1 != xAOD::Type::Muon) ||
        (type2 != xAOD::Type::Electron && type2 != xAOD::Type::Muon)) {
      ATH_MSG_WARNING(
        "Cannot check charge for particles not of type electron or muon");
      return StatusCode::FAILURE;
    }
  }

  for (const auto& pair : pairs) {
    unsigned int first = pair[0];
    unsigned int second = pair[1];
    float apt1 = pt1 ? (*pt1)[first] : ((*particles1)[first])->p4().Pt();
    float apt2 = pt2 ? (*pt2)[second] : ((*particles2)[second])->p4().Pt();
    float aeta1(-999.), aeta2(-999.);
    if (!m_doTransverseMass || (m_mindR > 0.0)) {
      aeta1 = eta1 ? (*eta1)[first] : ((*particles1)[first])->p4().Eta();
      aeta2 = eta2 ? (*eta2)[second] : ((*particles2)[second])->p4().Eta();
    }
    float aphi1 = phi1 ? (*phi1)[first] : ((*particles1)[first])->p4().Phi();
    float aphi2 = phi2 ? (*phi2)[second] : ((*particles2)[second])->p4().Phi();

    if (m_mindR > 0.0) {
      float deta = aeta1 - aeta2;
      float dphi = abs(aphi1 - aphi2);
      if (dphi > TMath::Pi()) {
        dphi = TMath::TwoPi() - dphi;
      }
      if (sqrt(deta * deta + dphi * dphi) < m_mindR) {
        continue;
      }
    }

    if (m_checkCharge) {
      float q1(0.), q2(0.);
      if (type1 == xAOD::Type::Electron) {
        q1 = ((xAOD::Electron*)((*particles1)[first]))->charge();
      } else if (type1 == xAOD::Type::Muon) {
        q1 = ((xAOD::Muon*)((*particles1)[first]))
               ->primaryTrackParticle()
               ->charge();
      }
      if (type2 == xAOD::Type::Electron) {
        q2 = ((xAOD::Electron*)((*particles2)[second]))->charge();
      } else if (type2 == xAOD::Type::Muon) {
        q2 = ((xAOD::Muon*)((*particles2)[second]))
               ->primaryTrackParticle()
               ->charge();
      }
      if (q1 * q2 > 0.) {
        continue;
      }
    }
    TLorentzVector v1, v2, v;
    if (m_doTransverseMass) {
      v1.SetPtEtaPhiM(apt1, 0., aphi1, m_mass1Hypothesis);
      v2.SetPtEtaPhiM(apt2, 0., aphi2, m_mass2Hypothesis);
    } else {
      v1.SetPtEtaPhiM(apt1, aeta1, aphi1, m_mass1Hypothesis);
      v2.SetPtEtaPhiM(apt2, aeta2, aphi2, m_mass2Hypothesis);
    }
    float mass = (v1 + v2).M();
    masses.push_back(mass);
  }
  return StatusCode::SUCCESS;
}
} //end namespace DerivationFramework
