/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************************

NAME:     MCTruthClassifier.cxx
PACKAGE:  atlasoff/PhysicsAnalysis/MCTruthClassifier

AUTHORS:  O. Fedin
CREATED:  Sep 2007

PURPOSE:  to classify  truth particles according to their origin. Based on
          the truth particle classification the tool provide classification of
          ID and combined muon tracks, egamma electrons (including forward
          electrons) and egamma photons. Both AOD and ESD files can be used.
          See  for details:
    https://twiki.cern.ch/twiki/bin/view/AtlasProtected/EgammaMCTruthClassifier
Updated:
********************************************************************************/

//
#include "MCTruthClassifier/MCTruthClassifier.h"
using namespace MCTruthPartClassifier;
using std::abs;

// -----------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------
MCTruthClassifier::MCTruthClassifier(const std::string& type)
  : asg::AsgTool(type)
{

#if !defined(XAOD_ANALYSIS) && !defined(GENERATIONBASE) // Add properties used/available only in Athena
  declareProperty("FwdElectronTruthExtrEtaCut",
                  m_FwdElectronTruthExtrEtaCut = 2.4,
                  "Cut on the eta of the truth Particles to be extrapolated for Fwd electrons");
  declareProperty(
    "FwdElectronTruthExtrEtaWindowCut",
    m_FwdElectronTruthExtrEtaWindowCut = 0.15,
    "Cut on the delta eta of the truth Particles to be extrapolated for Fwd electrons and the current FwdElectron");
  declareProperty("partExtrConePhi", m_partExtrConePhi = 0.4);
  declareProperty("partExtrConeEta", m_partExtrConeEta = 0.2);
  declareProperty("phtClasConePhi", m_phtClasConePhi = 0.05);
  declareProperty("phtClasConeEta", m_phtClasConeEta = 0.025);
  declareProperty("useCaching", m_useCaching = true);
  declareProperty("phtdRtoTrCut", m_phtdRtoTrCut = 0.1);
  declareProperty("fwrdEledRtoTrCut", m_fwrdEledRtoTrCut = 0.15);
  declareProperty("ROICone", m_ROICone = false);
#endif
#ifndef GENERATIONBASE // Add propertied not available in Generation only release
  declareProperty("deltaRMatchCut", m_deltaRMatchCut = 0.2);
  declareProperty("deltaPhiMatchCut", m_deltaPhiMatchCut = 0.2);
  declareProperty("NumOfSiHitsCut", m_NumOfSiHitsCut = 3);
  declareProperty("jetPartDRMatch", m_jetPartDRMatch = 0.4);
#endif
  //
  // Properties Available in all
  declareProperty("inclEgammaFwrdEle", m_inclEgammaFwrdEle = true);
  declareProperty("inclEgammaPhoton", m_inclEgammaPhoton = true);
  declareProperty("pTChargePartCut", m_pTChargePartCut = 1.0);
  declareProperty("pTNeutralPartCut", m_pTNeutralPartCut = 0.);
  declareProperty("inclG4part", m_inclG4part = false);
  declareProperty("LQpatch", m_LQpatch = false);
  declareProperty("barcodeG4Shift", m_barcodeG4Shift = 200001);

  // Other non-static variables init
  m_barcodeShift = 1000000;
}

// DESTRUCTOR:
MCTruthClassifier::~MCTruthClassifier() {}

// INITIALIZE METHOD:
StatusCode
MCTruthClassifier::initialize()
{

  ATH_MSG_INFO(" Initializing MCTruthClassifier");
  // define barcode scheme
  m_barcodeShift = 1000000;
  if (m_barcodeG4Shift == 1000001) {
    m_barcodeShift = 10000000;
  }

#ifndef XAOD_ANALYSIS
  // Only needed for GenParticle interface
  if (!m_truthLinkVecReadHandleKey.key().empty()) {
    ATH_CHECK(m_truthLinkVecReadHandleKey.initialize());
  }
  ATH_CHECK(m_truthParticleContainerKey.initialize());
#endif

#if !defined(XAOD_ANALYSIS) && !defined(GENERATIONBASE) // Tools making sense only for the Athena only environment
  if (!m_caloExtensionTool.empty() && m_caloExtensionTool.retrieve().isFailure()) {
    ATH_MSG_WARNING("Cannot retrieve extrapolateToCaloTool " << m_caloExtensionTool
                                                             << " - will not perform extrapolation.");
  }
  if (!m_truthInConeTool.empty() && m_truthInConeTool.retrieve().isFailure()) {
    ATH_MSG_ERROR("Cannot retrieve Truth in cone Tool " << m_truthInConeTool);
  }
#endif
  return StatusCode::SUCCESS;
}

// FINALIZE METHOD:
StatusCode
MCTruthClassifier::finalize()
{

  return StatusCode::SUCCESS;
}
// Now that we use TLorentzVector for the momentum base class, this is straightforward
double
MCTruthClassifier::deltaR(const xAOD::TruthParticle& v1, const xAOD::Jet& v2)
{
  // Should this use delta y though?
  return v1.p4().DeltaR(v2.p4());
}
void
MCTruthClassifier::findJetConstituents(const xAOD::Jet* jet,
                                       std::set<const xAOD::TruthParticle*>& constituents,
                                       bool DR) const
{

  if (DR) {
    // use a DR matching scheme (default)
    // retrieve collection and get a pointer

    SG::ReadHandle<xAOD::TruthParticleContainer> truthParticleContainerReadHandle(m_truthParticleContainerKey);

    if (!truthParticleContainerReadHandle.isValid()) {
      ATH_MSG_WARNING(
        " Invalid ReadHandle for xAOD::TruthParticleContainer with key: " << truthParticleContainerReadHandle.key());
      return;
    }

    ATH_MSG_DEBUG("xAODTruthParticleContainer with key  " << truthParticleContainerReadHandle.key()
                                                          << " has valid ReadHandle ");

    // find the matching truth particles
    for (const auto thePart : *truthParticleContainerReadHandle) {
      // match truth particles to the jet
      if (thePart->status() == 1 && deltaR((*thePart), (*jet)) < m_jetPartDRMatch) {
        constituents.insert(thePart);
      }
    }
  } // end if DR
  else {
    xAOD::JetConstituentVector vec = jet->getConstituents();
    for (auto particle0 : vec) {
      const xAOD::TruthParticle* thePart = dynamic_cast<const xAOD::TruthParticle*>(particle0->rawConstituent());
      if (thePart->status() == 1) {
        constituents.insert(thePart);
      }
    }
  } // end if !DR
  }

void
MCTruthClassifier::findParticleDaughters(const xAOD::TruthParticle* thePart,
                                         std::set<const xAOD::TruthParticle*>& daughters) const
{

  // Get descendants
  const xAOD::TruthVertex* endVtx = thePart->decayVtx();
  if (endVtx != nullptr) {
    for (unsigned int i = 0; i < endVtx->nOutgoingParticles(); i++) {
      const xAOD::TruthParticle* theDaughter = endVtx->outgoingParticle(i);
      if (theDaughter == nullptr)
        continue;
      if (theDaughter->status() == 1 && theDaughter->barcode() < 200000) {
        // Add descendants with status code 1
        daughters.insert(theDaughter);
      }
      findParticleDaughters(theDaughter, daughters);
    }
  }
  }

double
MCTruthClassifier::fracParticleInJet(const xAOD::TruthParticle* thePart,
                                     const xAOD::Jet* jet,
                                     bool DR,
                                     bool nparts) const
{

  // Get jet constituents
  std::set<const xAOD::TruthParticle*> constituents;
  constituents.clear();
  findJetConstituents(jet, constituents, DR);

  // Get all particle daughters
  std::set<const xAOD::TruthParticle*> daughters;
  daughters.clear();
  findParticleDaughters(thePart, daughters);
  if (daughters.empty())
    daughters.insert(thePart);

  // Get the intersection of constituents and daughters
  std::set<const xAOD::TruthParticle*> intersect;
  std::set_intersection(constituents.begin(),
                        constituents.end(),
                        daughters.begin(),
                        daughters.end(),
                        std::inserter(intersect, intersect.begin()));

  double frac = 0;
  if (nparts) {
    frac = 1.0 * intersect.size() / daughters.size();
  } else {
    double tot = 0;
    for (auto daughter : daughters) {
      tot += 1.0 * daughter->pt();
    }
    for (auto particle : intersect) {
      frac += 1.0 * particle->pt() / tot;
    }
  }

  return frac;
}


