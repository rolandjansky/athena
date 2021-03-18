/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "DerivationFrameworkEGamma/BkgElectronClassification.h"
#include "GaudiKernel/EventContext.h"
#include "MCTruthClassifier/IMCTruthClassifier.h"
#include "xAODEgamma/EgammaTruthxAODHelpers.h"
#include "xAODEgamma/Electron.h"
#include "xAODTruth/TruthParticle.h"

namespace DerivationFramework {

BkgElectronClassification::BkgElectronClassification(const std::string& t,
                                                     const std::string& n,
                                                     const IInterface* p)
  : AthAlgTool(t, n, p)
{

  declareInterface<DerivationFramework::IAugmentationTool>(this);

  declareProperty("MCTruthClassifierTool",
                  m_mcTruthClassifier,
                  "Handle of MCTruthClassifier");
  declareProperty("barcodeCut",
                  m_barcodecut = 200e3,
                  "Cut on the barcode for the xAODEgammaTruthHelpers");
}

StatusCode
BkgElectronClassification::initialize()
{

  ATH_MSG_DEBUG("Initializing " << name() << "...");
  ATH_CHECK(m_mcTruthClassifier.retrieve());
  ATH_MSG_DEBUG("Retrieved tool " << m_mcTruthClassifier);

  ATH_CHECK(m_electronContainer.initialize());
  ATH_CHECK(m_truthContainer.initialize());

  const std::string baseName = m_electronContainer.key();
  m_truthPdgId = baseName + ".truthPdgId";
  //
  m_firstEgMotherTruthType = baseName + ".firstEgMotherTruthType";
  m_firstEgMotherTruthOrigin = baseName + ".firstEgMotherTruthOrigin";
  m_firstEgMotherTruthParticleLink =
    baseName + ".firstEgMotherTruthParticleLink";
  m_firstEgMotherPdgId = baseName + ".firstEgMotherPdgId";
  //
  m_lastEgMotherTruthType = baseName + ".lastEgMotherTruthType";
  m_lastEgMotherTruthOrigin = baseName + ".lastEgMotherTruthOrigin";
  m_lastEgMotherTruthParticleLink = baseName + ".lastEgMotherTruthParticleLink";
  m_lastEgMotherPdgId = baseName + ".lastEgMotherPdgId";

  ATH_CHECK(m_truthPdgId.initialize());
  // 
  ATH_CHECK(m_firstEgMotherTruthType.initialize());
  ATH_CHECK(m_firstEgMotherTruthOrigin.initialize());
  ATH_CHECK(m_firstEgMotherTruthParticleLink.initialize());
  ATH_CHECK(m_firstEgMotherPdgId.initialize());
  //
  ATH_CHECK(m_lastEgMotherTruthType.initialize());
  ATH_CHECK(m_lastEgMotherTruthOrigin.initialize());
  ATH_CHECK(m_lastEgMotherTruthParticleLink.initialize());
  ATH_CHECK(m_lastEgMotherPdgId.initialize());
  
  ATH_MSG_DEBUG("Initialization successful");

  return StatusCode::SUCCESS;
}

StatusCode
BkgElectronClassification::addBranches() const
{

  const EventContext& ctx = Gaudi::Hive::currentContext();
  SG::ReadHandle<xAOD::ElectronContainer> electrons{ m_electronContainer, ctx };
  SG::ReadHandle<xAOD::TruthParticleContainer> truthContainer{ m_truthContainer,
                                                               ctx };

  // Access for the already existing info
  static const SG::AuxElement::Accessor<int> tT("truthType");
  static const SG::AuxElement::Accessor<int> tO("truthOrigin");
  static const SG::AuxElement::Accessor<
    ElementLink<xAOD::TruthParticleContainer>>
    tPL("truthParticleLink");

  // pdg iD
  SG::WriteDecorHandle<xAOD::ElectronContainer, int> tPdgID(m_truthPdgId, ctx);
  // first mother decorations
  SG::WriteDecorHandle<xAOD::ElectronContainer, int> firstEgMotherTT(
    m_firstEgMotherTruthType, ctx);
  SG::WriteDecorHandle<xAOD::ElectronContainer, int> firstEgMotherTO(
    m_firstEgMotherTruthOrigin, ctx);
  SG::WriteDecorHandle<xAOD::ElectronContainer, int> firstEgMotherPdgID(
    m_firstEgMotherPdgId, ctx);
  SG::WriteDecorHandle<xAOD::ElectronContainer,
                       ElementLink<xAOD::TruthParticleContainer>>
    firstEgMotherTPL(m_firstEgMotherTruthParticleLink, ctx);

  // last mother decorations
  SG::WriteDecorHandle<xAOD::ElectronContainer, int> lastEgMotherTT(
    m_lastEgMotherTruthType, ctx);
  SG::WriteDecorHandle<xAOD::ElectronContainer, int> lastEgMotherTO(
    m_lastEgMotherTruthOrigin, ctx);
  SG::WriteDecorHandle<xAOD::ElectronContainer, int> lastEgMotherPdgID(
    m_lastEgMotherPdgId, ctx);
  SG::WriteDecorHandle<xAOD::ElectronContainer,
                       ElementLink<xAOD::TruthParticleContainer>>
    lastEgMotherTPL(m_lastEgMotherTruthParticleLink, ctx);
  //

  for (const xAOD::Electron* el : *electrons) {
    tPdgID(*el) = 0;
    if (tPL.isAvailable(*el) && tPL(*el).isValid()) {
      tPdgID(*el) = (*tPL(*el))->pdgId();
    }
    // Use the Helpers for electron from electron or photon
    // Add Extra Decoration from Egamma helpers in case of BkgElectron (Electron
    // coming for a photon) Go back to the first/last electron/photon Generator
    // mother and classify this one
    // First the one entering the Geant, the first we meet on the way back
    firstEgMotherTT(*el) = 0;
    firstEgMotherTO(*el) = 0;
    firstEgMotherTPL(*el) = ElementLink<xAOD::TruthParticleContainer>();
    firstEgMotherPdgID(*el) = 0;
    const xAOD::TruthParticle* firstElTruth =
      xAOD::EgammaHelpers::getBkgElectronMother(el, m_barcodecut);

    IMCTruthClassifier::Info mcinfo(ctx);
    if (firstElTruth) {
      auto res =
        m_mcTruthClassifier->particleTruthClassifier(firstElTruth, &mcinfo);
      firstEgMotherTT(*el) = res.first;
      firstEgMotherTO(*el) = res.second;
      firstEgMotherPdgID(*el) = firstElTruth->pdgId();
      ElementLink<xAOD::TruthParticleContainer> link(
        firstElTruth, *truthContainer, ctx);
      firstEgMotherTPL(*el) = link;
    }

    // The last electron / photon  we meet on the way back towards the Generator
    // vertex
    lastEgMotherTT(*el) = 0;
    lastEgMotherTO(*el) = 0;
    lastEgMotherTPL(*el) = ElementLink<xAOD::TruthParticleContainer>();
    lastEgMotherPdgID(*el) = 0;
    const xAOD::TruthParticle* lastElTruth =
      xAOD::EgammaHelpers::getBkgElectronMother(el);

    if (lastElTruth) {
      auto res =
        m_mcTruthClassifier->particleTruthClassifier(lastElTruth, &mcinfo);
      lastEgMotherTT(*el) = res.first;
      lastEgMotherTO(*el) = res.second;
      lastEgMotherPdgID(*el) = lastElTruth->pdgId();
      ElementLink<xAOD::TruthParticleContainer> link(
        lastElTruth, *truthContainer, ctx);
      lastEgMotherTPL(*el) = link;
    }
  }
  return StatusCode::SUCCESS;
}
}
