/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "DerivationFrameworkEGamma/TruthCaloShowerDecorator.h"
#include "CaloCalibHitRec/CalibHitToCaloCellTool.h"
#include "xAODEgamma/EgammaTruthxAODHelpers.h"
#include "xAODEgamma/Electron.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODTruth/TruthParticle.h"

namespace DerivationFramework {

TruthCaloShowerDecorator::TruthCaloShowerDecorator(const std::string& t,
                                                   const std::string& n,
                                                   const IInterface* p)
  : AthAlgTool(t, n, p)
{
  declareInterface<DerivationFramework::IAugmentationTool>(this);
  declareProperty("SingleParticleBarcode",
                  m_singleParticleBarcode = 10001,
                  "Barcode of single particle");
}

StatusCode
TruthCaloShowerDecorator::initialize()
{
  ATH_MSG_DEBUG("Initializing " << name() << "...");

  if (m_calibhitToCaloCellTool.retrieve().isFailure()) {
    ATH_MSG_FATAL("Failed to retrieve tool " << m_calibhitToCaloCellTool);
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO("Retrieved tool " << m_calibhitToCaloCellTool);

  ATH_CHECK(m_truthParticleContainerName.initialize());
  ATH_CHECK(m_truthClusterContainerEtot.initialize());
  ATH_CHECK(m_truthClusterContainerEvis.initialize());
  ATH_CHECK(m_truthClusterContainerEem.initialize());

  const std::string baseName = m_truthParticleContainerName.key();
  m_linkDecoratorClusterEtot = baseName + ".truthLArClusterEtotLink";
  m_linkDecoratorClusterEvis = baseName + ".truthLArClusterEvisLink";
  m_linkDecoratorClusterEem = baseName + ".truthLArClusterEemLink";

  ATH_CHECK(m_linkDecoratorClusterEtot.initialize());
  ATH_CHECK(m_linkDecoratorClusterEvis.initialize());
  ATH_CHECK(m_linkDecoratorClusterEem.initialize());

  return StatusCode::SUCCESS;
}

StatusCode
TruthCaloShowerDecorator::finalize()
{
  return StatusCode::SUCCESS;
}

StatusCode
TruthCaloShowerDecorator::addBranches() const
{
  const EventContext& ctx = Gaudi::Hive::currentContext();

  SG::ReadHandle<xAOD::TruthParticleContainer> truthPartContainerReadHandle{
    m_truthParticleContainerName, ctx
  };
  const xAOD::TruthParticleContainer* truthPartContainer =
    truthPartContainerReadHandle.ptr();

  // create truth clusters
  ATH_MSG_DEBUG("Creating truth clusters");
  int singleElectronBarcode = 10001;
  if (!m_calibhitToCaloCellTool
         ->processCalibHitsFromParticle(singleElectronBarcode)
         .isSuccess()) {
    ATH_MSG_FATAL("Tool " << m_calibhitToCaloCellTool << " failed.");
    return StatusCode::FAILURE;
  }

  ATH_MSG_DEBUG("Retrieving truth clusters");
  SG::ReadHandle<xAOD::CaloClusterContainer>
    truthClusterContainerEtotReadHandle{ m_truthClusterContainerEtot, ctx };
  SG::ReadHandle<xAOD::CaloClusterContainer>
    truthClusterContainerEvisReadHandle{ m_truthClusterContainerEvis, ctx };
  SG::ReadHandle<xAOD::CaloClusterContainer> truthClusterContainerEemReadHandle{
    m_truthClusterContainerEem, ctx
  };
  const xAOD::CaloClusterContainer* truthClusterContainerEtot =
    truthClusterContainerEtotReadHandle.ptr();
  const xAOD::CaloClusterContainer* truthClusterContainerEvis =
    truthClusterContainerEvisReadHandle.ptr();
  const xAOD::CaloClusterContainer* truthClusterContainerEem =
    truthClusterContainerEemReadHandle.ptr();

  if (!truthClusterContainerEtot || !truthClusterContainerEvis ||
      !truthClusterContainerEem) {
    ATH_MSG_ERROR("Failed to retrieve truth cluster container");
    return StatusCode::FAILURE;
  }

  SG::WriteDecorHandle<xAOD::TruthParticleContainer,
                       ElementLink<xAOD::CaloClusterContainer>>
    linkDecoratorClusterEtot(m_linkDecoratorClusterEtot, ctx);
  SG::WriteDecorHandle<xAOD::TruthParticleContainer,
                       ElementLink<xAOD::CaloClusterContainer>>
    linkDecoratorClusterEvis(m_linkDecoratorClusterEvis, ctx);
  SG::WriteDecorHandle<xAOD::TruthParticleContainer,
                       ElementLink<xAOD::CaloClusterContainer>>
    linkDecoratorClusterEem(m_linkDecoratorClusterEem, ctx);

  ElementLink<xAOD::CaloClusterContainer> truthClusterEtot(
    *truthClusterContainerEtot, 0);
  ElementLink<xAOD::CaloClusterContainer> truthClusterEvis(
    *truthClusterContainerEvis, 0);
  ElementLink<xAOD::CaloClusterContainer> truthClusterEem(
    *truthClusterContainerEem, 0);

  ATH_MSG_DEBUG("Decorating truth parts with truth cluster energy");
  for (const auto* const truthPart : *truthPartContainer) {
    if (!truthPart)
      continue;
    if (truthPart->barcode() != m_singleParticleBarcode)
      continue;
    linkDecoratorClusterEtot(*truthPart) = truthClusterEtot;
    linkDecoratorClusterEvis(*truthPart) = truthClusterEvis;
    linkDecoratorClusterEem(*truthPart) = truthClusterEem;
  }
  return StatusCode::SUCCESS;
}
}
