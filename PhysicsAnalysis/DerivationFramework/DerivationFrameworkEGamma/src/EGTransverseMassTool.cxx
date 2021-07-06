/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// EGTransverseMassTool.cxx, (c) ATLAS Detector software
// Author: Giovanni Marchiori (giovanni.marchiori@cern.ch)
//
// The tool computes the transverse mass between the MET and the
// particles belonging to some collection.
// The MET met and phi values can either be stored in SG under
// keys specified with METPtBranchName and METPhiBranchName,
// otherwise the met() and phi() of the MET object stored in
// SG under the METContainerName (default = MET_LocHadTopo)
// will be used.
// A MET>METmin cut can be applied.
// For the particles, only those passing the ObjectRequirements
// selection are retained. Their pt and phi can be stored
// in SG under keys specified with ObjectPtBranchName and
// ObjectPhiBranchName, otherwise the Pt() and Phi() methods
// of the IParticles object of the containred stored in SG
// with the key specified by ObjectContainerName will be used
///////////////////////////////////////////////////////////////////

#include "DerivationFrameworkEGamma/EGTransverseMassTool.h"

#include <cmath>
using std::abs;
using std::sqrt;

#include "TLorentzVector.h"

namespace DerivationFramework {

EGTransverseMassTool::EGTransverseMassTool(const std::string& t,
                                           const std::string& n,
                                           const IInterface* p)
  : ExpressionParserUser<AthAlgTool>(t, n, p)
  , m_expression1("true")
  , m_METmin(-999.)
  , m_mass1Hypothesis(0.0)
{
  declareInterface<DerivationFramework::IAugmentationTool>(this);
  declareProperty("ObjectRequirements", m_expression1);
  declareProperty("METmin", m_METmin);
  declareProperty("ObjectMassHypothesis", m_mass1Hypothesis);
}

StatusCode
EGTransverseMassTool::initialize()
{
  if (m_sgName.key().empty()) {
    ATH_MSG_ERROR(
      "No SG name provided for the output of the transverse mass tool!");
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
  if (!m_phi1BranchName.key().empty()) {
    ATH_CHECK(m_phi1BranchName.initialize());
  }
  if (!m_pt2BranchName.key().empty()) {
    ATH_CHECK(m_pt2BranchName.initialize());
  }
  if (!m_phi2BranchName.key().empty()) {
    ATH_CHECK(m_phi2BranchName.initialize());
  }

  ATH_CHECK(initializeParser(m_expression1));

  return StatusCode::SUCCESS;
}

StatusCode
EGTransverseMassTool::addBranches() const
{
  const EventContext& ctx = Gaudi::Hive::currentContext();
  SG::WriteHandle<std::vector<float>> writeHandle{ m_sgName, ctx };

  // create the vector which will hold the values invariant masses
  auto masses = std::make_unique<std::vector<float>>();
  // compute the invariant mass values
  ATH_CHECK(getTransverseMasses(ctx, *masses));

  ATH_CHECK(writeHandle.record(std::move(masses)));

  return StatusCode::SUCCESS;
}

StatusCode
EGTransverseMassTool::getTransverseMasses(const EventContext& ctx,
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

  // Get the input particle and MET
  SG::ReadHandle<xAOD::IParticleContainer> inputParticles1{ m_container1Name,
                                                            ctx };
  SG::ReadHandle<xAOD::MissingETContainer> inputParticles2{ m_container2Name,
                                                            ctx };
  const xAOD::IParticleContainer* particles1 = inputParticles1.ptr();
  const xAOD::MissingETContainer* particles2 = inputParticles2.ptr();

  // compute MET
  if (particles2->empty()) {
    if (!pt2) {
      ATH_MSG_WARNING("No MET info found");
      return StatusCode::SUCCESS;
    }
    if (pt2->empty()) {
      ATH_MSG_WARNING("No MET info found");
      return StatusCode::SUCCESS;
    }
  }

  float MET = pt2 ? (*pt2)[0] : particles2->at(0)->met();
  float MET_phi = phi2 ? (*phi2)[0] : particles2->at(0)->phi();

  // apply MET requirement
  if (MET < m_METmin)
    return StatusCode::SUCCESS;

  // get the positions of the particles which pass the requirement
  std::vector<int> entries1 = m_parser->evaluateAsVector();
  unsigned int nEntries1 = entries1.size();

  // if there are no particles in one of the two lists to combine, just leave
  // function
  if (nEntries1 == 0)
    return StatusCode::SUCCESS;

  // check that the sizes are compatible
  if (particles1->size() != nEntries1) {
    ATH_MSG_ERROR("Branch sizes incompatible - returning zero");
    return StatusCode::FAILURE;
  }
  if ((pt1 && pt1->size() != nEntries1) ||
      (phi1 && phi1->size() != nEntries1)) {
    ATH_MSG_ERROR("Branch sizes incompatible - returning zero");
    return StatusCode::FAILURE;
  }

  // Loop over the objects and calculate the transverse mass
  unsigned int iter;
  for (iter = 0; iter < nEntries1; ++iter) {
    if (entries1[iter] != 1)
      continue;
    float apt1 = pt1 ? (*pt1)[iter] : ((*particles1)[iter])->p4().Pt();
    float aphi1 = phi1 ? (*phi1)[iter] : ((*particles1)[iter])->p4().Phi();

    TLorentzVector v1, v2;
    v1.SetPtEtaPhiM(apt1, 0., aphi1, m_mass1Hypothesis);
    v2.SetPtEtaPhiM(MET, 0., MET_phi, 0.);
    float mass = (v1 + v2).M();
    masses.push_back(mass);
  }
  return StatusCode::SUCCESS;
}
}
