/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */

#include "topoEgammaBuilder.h"

#include "AthenaKernel/errorcheck.h"
#include "GaudiKernel/EventContext.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/ServiceHandle.h"
#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"

#include "CaloDetDescr/CaloDetDescrManager.h"
#include "xAODCaloEvent/CaloClusterContainer.h"

#include "egammaRecEvent/egammaRecContainer.h"
#include "xAODEgamma/EgammaContainer.h"
#include "xAODEgamma/Electron.h"
#include "xAODEgamma/ElectronAuxContainer.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/Photon.h"
#include "xAODEgamma/PhotonAuxContainer.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/VertexContainer.h"

// INCLUDE GAUDI HEADER FILES:
#include <algorithm>
#include <cmath>

topoEgammaBuilder::topoEgammaBuilder(const std::string& name,
                                     ISvcLocator* pSvcLocator)
  : AthReentrantAlgorithm(name, pSvcLocator)
{}

StatusCode
topoEgammaBuilder::initialize()
{
  // the data handle keys
  ATH_CHECK(m_electronSuperClusterRecContainerKey.initialize(m_doElectrons));
  ATH_CHECK(m_photonSuperClusterRecContainerKey.initialize(m_doPhotons));
  ATH_CHECK(m_electronOutputKey.initialize());
  ATH_CHECK(m_photonOutputKey.initialize());
  // retrieve tools
  ATH_CHECK(m_clusterTool.retrieve());
  ATH_CHECK(m_ShowerTool.retrieve());
  ATH_CHECK(m_egammaTools.retrieve());

  if (m_doElectrons) {
    ATH_CHECK(m_electronTools.retrieve());
  }
  if (m_doPhotons) {
    ATH_CHECK(m_photonTools.retrieve());
  }
  m_doOQ = !m_egammaOQTool.empty();
  if (m_doOQ) {
    ATH_CHECK(m_egammaOQTool.retrieve());
  }else{
    m_egammaOQTool.disable();
  }

  //do we actually do ambiguity 
  m_doAmbiguity = !m_ambiguityTool.empty();
  if (m_doElectrons && m_doPhotons && m_doAmbiguity) {
    ATH_CHECK(m_ambiguityTool.retrieve());
  }else{
    m_ambiguityTool.disable();
  }

  return StatusCode::SUCCESS;
}

StatusCode
topoEgammaBuilder::finalize()
{
  return StatusCode::SUCCESS;
}

StatusCode
topoEgammaBuilder::execute(const EventContext& ctx) const
{

  // Chrono name for each Tool
  const EgammaRecContainer* inputElRecs = nullptr;
  const EgammaRecContainer* inputPhRecs = nullptr;
  xAOD::ElectronContainer* electrons = nullptr;
  xAOD::PhotonContainer* photons = nullptr;
  /*
   * From here one if a Read/Write handle is retrieved the above will be !=
   * nullptr for electron or photons or both
   */
  if (m_doElectrons) {
    SG::ReadHandle<EgammaRecContainer> electronSuperRecs(m_electronSuperClusterRecContainerKey, ctx);
    inputElRecs = electronSuperRecs.ptr();
  }
  if (m_doPhotons) {
    SG::ReadHandle<EgammaRecContainer> photonSuperRecs(m_photonSuperClusterRecContainerKey, ctx);
    inputPhRecs = photonSuperRecs.ptr();
  }

  SG::WriteHandle<xAOD::ElectronContainer> electronContainer(m_electronOutputKey, ctx);
  
  ATH_CHECK(electronContainer.record(std::make_unique<xAOD::ElectronContainer>(),
                                     std::make_unique<xAOD::ElectronAuxContainer>()));

  SG::WriteHandle<xAOD::PhotonContainer> photonContainer(m_photonOutputKey,ctx);
  
  ATH_CHECK(photonContainer.record(std::make_unique<xAOD::PhotonContainer>(),
                                   std::make_unique<xAOD::PhotonAuxContainer>()));
  
  electrons = electronContainer.ptr();
  photons = photonContainer.ptr();

  /*
   * Now fill the electrons and photons
   */
  if (m_doElectrons) {
    for (const egammaRec* electronRec : *inputElRecs) {
      // in case for some reasons we reach here with no trackparticles
      if (electronRec->getNumberOfTrackParticles() == 0) {
        continue;
      }
      unsigned int author = xAOD::EgammaParameters::AuthorElectron;
      xAOD::AmbiguityTool::AmbiguityType type = xAOD::AmbiguityTool::electron;
      if (m_doPhotons) {
        // get the hottest cell
        const xAOD::CaloCluster* const elClus = electronRec->caloCluster();
        const double elEta0 = elClus->eta0();
        const double elPhi0 = elClus->phi0();
        for (const egammaRec* photonRec : *inputPhRecs) {
          const xAOD::CaloCluster* const phClus = photonRec->caloCluster();
          // See if they have the same hottest cell
          if (elEta0 == phClus->eta0() && elPhi0 == phClus->phi0()) {
            if (m_doAmbiguity) { // should be the default
              author =
                m_ambiguityTool->ambiguityResolve(elClus,
                                                  photonRec->vertex(),
                                                  electronRec->trackParticle(),
                                                  type);
            } else { // in case the ambiguity tool is not set ambiguity is not
                     // resolved
              author = xAOD::EgammaParameters::AuthorAmbiguous;
            }
            break;
          }
        }
      }
      // Create Electron xAOD objects
      if (author == xAOD::EgammaParameters::AuthorElectron ||
          author == xAOD::EgammaParameters::AuthorAmbiguous) {
        if (!getElectron(electronRec, electrons, author, type)) {
          return StatusCode::FAILURE;
        }
      }
    }
  }

  if (m_doPhotons) {
    for (const egammaRec* photonRec : *inputPhRecs) {
      unsigned int author = xAOD::EgammaParameters::AuthorPhoton;
      xAOD::AmbiguityTool::AmbiguityType type = xAOD::AmbiguityTool::photon;
      if (m_doElectrons) {
        // get the hottest cell
        const xAOD::CaloCluster* const phClus = photonRec->caloCluster();
        const double phEta0 = phClus->eta0();
        const double phPhi0 = phClus->phi0();
        for (const egammaRec* electronRec : *inputElRecs) {
          const xAOD::CaloCluster* const elClus = electronRec->caloCluster();
          // See if they have the same hottest cell
          if (phEta0 == elClus->eta0() && phPhi0 == elClus->phi0()) {
            if (m_doAmbiguity) { // should be the default
              author =
                m_ambiguityTool->ambiguityResolve(elClus,
                                                  photonRec->vertex(),
                                                  electronRec->trackParticle(),
                                                  type);
            } else { // in case the ambiguity tool is not set ambiguity is not
                     // resolved
              author = xAOD::EgammaParameters::AuthorAmbiguous;
            }
            break;
          }
        }
      }
      // Create Photon xAOD objects
      if (author == xAOD::EgammaParameters::AuthorPhoton ||
          author == xAOD::EgammaParameters::AuthorAmbiguous) {
        if (!getPhoton(photonRec, photons, author, type)) {
          return StatusCode::FAILURE;
        }
      }
    }
  }

  const CaloDetDescrManager* calodetdescrmgr = nullptr;
  ATH_CHECK(detStore()->retrieve(calodetdescrmgr, "CaloMgr"));

  // Shower Shapes
  if (m_doElectrons) {
    for (xAOD::Electron* electron : *electronContainer) {
      ATH_CHECK(m_ShowerTool->execute(ctx, *calodetdescrmgr, electron));
    }
  }
  if (m_doPhotons) {
    for (xAOD::Photon* photon : *photonContainer) {
      ATH_CHECK(m_ShowerTool->execute(ctx, *calodetdescrmgr, photon));
    }
  }

  // Object Quality
  if (m_doOQ) {
    if (m_doElectrons) {
      for (xAOD::Electron* electron : *electronContainer) {
        ATH_CHECK(m_egammaOQTool->execute(ctx, *calodetdescrmgr, *electron));
      }
    }
    if (m_doPhotons) {
      for (xAOD::Photon* photon : *photonContainer) {
        ATH_CHECK(m_egammaOQTool->execute(ctx, *calodetdescrmgr, *photon));
      }
    }
  }

  // Calibration
  if (m_clusterTool->contExecute(ctx, *calodetdescrmgr, electrons, photons)
        .isFailure()) {
    ATH_MSG_ERROR("Problem executing the " << m_clusterTool << " tool");
    return StatusCode::FAILURE;
  }

  // Tools for ToolHandleArrays
  // First common photon/electron tools*/
  for (auto& tool : m_egammaTools) {
    ATH_CHECK(CallTool(ctx, tool, electrons, photons));
  }
  // Tools for only electrons
  if (m_doElectrons) {
    for (auto& tool : m_electronTools) {
      ATH_CHECK(CallTool(ctx, tool, electrons, nullptr));
    }
  }
  // Tools for only photons
  if (m_doPhotons) {
    for (auto& tool : m_photonTools) {
      ATH_CHECK(CallTool(ctx, tool, nullptr, photons));
    }
  }
  // Do the ambiguity Links
  if (m_doElectrons && m_doPhotons) {
    ATH_CHECK(doAmbiguityLinks(electrons, photons));
  }

  return StatusCode::SUCCESS;
}

StatusCode
topoEgammaBuilder::doAmbiguityLinks(
  xAOD::ElectronContainer* electronContainer,
  xAOD::PhotonContainer* photonContainer) const
{

  /// Needs the same logic as the ambiguity after building the objects (make
  /// sure they are all valid)
  static const SG::AuxElement::Accessor<
    std::vector<ElementLink<xAOD::CaloClusterContainer>>>
    caloClusterLinks("constituentClusterLinks");

  static const SG::AuxElement::Accessor<ElementLink<xAOD::EgammaContainer>>
    ELink("ambiguityLink");

  ElementLink<xAOD::EgammaContainer> dummylink;
  for (xAOD::Photon* photon : *photonContainer) {
    ELink(*photon) = dummylink;
    if (photon->author() != xAOD::EgammaParameters::AuthorAmbiguous) {
      continue;
    }

    for (size_t electronIndex = 0; electronIndex < electronContainer->size();
         ++electronIndex) {

      xAOD::Electron* electron = electronContainer->at(electronIndex);
      if (electron->author() != xAOD::EgammaParameters::AuthorAmbiguous) {
        continue;
      }

      if (caloClusterLinks(*(electron->caloCluster())).at(0) ==
          caloClusterLinks(*(photon->caloCluster())).at(0)) {
        ElementLink<xAOD::EgammaContainer> link(*electronContainer,
                                                electronIndex);
        ELink(*photon) = link;
        break;
      }
    }
  }

  for (xAOD::Electron* electron : *electronContainer) {
    ELink(*electron) = dummylink;
    if (electron->author() != xAOD::EgammaParameters::AuthorAmbiguous) {
      continue;
    }
    for (size_t photonIndex = 0; photonIndex < photonContainer->size();
         ++photonIndex) {

      xAOD::Photon* photon = photonContainer->at(photonIndex);
      if (photon->author() != xAOD::EgammaParameters::AuthorAmbiguous) {
        continue;
      }

      if (caloClusterLinks(*(electron->caloCluster())).at(0) ==
          caloClusterLinks(*(photon->caloCluster())).at(0)) {
        ElementLink<xAOD::EgammaContainer> link(*photonContainer, photonIndex);
        ELink(*electron) = link;
        break;
      }
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode
topoEgammaBuilder::CallTool(
  const EventContext& ctx,
  const ToolHandle<IegammaBaseTool>& tool,
  xAOD::ElectronContainer* electronContainer /* = 0*/,
  xAOD::PhotonContainer* photonContainer /* = 0*/) const
{

  if (electronContainer) {
    for (xAOD::Electron* electron : *electronContainer) {
      if (tool->execute(ctx, electron).isFailure()) {
        ATH_MSG_ERROR("Problem executing tool on electrons: " << tool);
        return StatusCode::FAILURE;
      }
    }
  }
  if (photonContainer) {
    for (xAOD::Photon* photon : *photonContainer) {
      if (tool->execute(ctx, photon).isFailure()) {
        ATH_MSG_ERROR("Problem executing tool on photons: " << tool);
        return StatusCode::FAILURE;
      }
    }
  }
  return StatusCode::SUCCESS;
}

bool
topoEgammaBuilder::getElectron(const egammaRec* egRec,
                               xAOD::ElectronContainer* electronContainer,
                               const unsigned int author,
                               const uint8_t type) const
{

  if (!egRec || !electronContainer){
    return false;
  }

  xAOD::Electron* electron = new xAOD::Electron();
  electronContainer->push_back(electron);
  electron->setAuthor(author);

  static const SG::AuxElement::Accessor<uint8_t> acc("ambiguityType");
  acc(*electron) = type;

  std::vector<ElementLink<xAOD::CaloClusterContainer>> clusterLinks;
  for (size_t i = 0; i < egRec->getNumberOfClusters(); ++i) {
    clusterLinks.push_back(egRec->caloClusterElementLink(i));
  }
  electron->setCaloClusterLinks(clusterLinks);

  std::vector<ElementLink<xAOD::TrackParticleContainer>> trackLinks;
  for (size_t i = 0; i < egRec->getNumberOfTrackParticles(); ++i) {
    trackLinks.push_back(egRec->trackParticleElementLink(i));
  }
  electron->setTrackParticleLinks(trackLinks);

  electron->setCharge(electron->trackParticle()->charge());
  // Set DeltaEta, DeltaPhi , DeltaPhiRescaled
  std::array<double, 4> deltaEta = egRec->deltaEta();
  std::array<double, 4> deltaPhi = egRec->deltaPhi();
  std::array<double, 4> deltaPhiRescaled = egRec->deltaPhiRescaled();

  electron->setTrackCaloMatchValue(static_cast<float>(deltaEta[0]),
                                   xAOD::EgammaParameters::deltaEta0);
  electron->setTrackCaloMatchValue(static_cast<float>(deltaPhi[0]),
                                   xAOD::EgammaParameters::deltaPhi0);
  electron->setTrackCaloMatchValue(static_cast<float>(deltaPhiRescaled[0]),
                                   xAOD::EgammaParameters::deltaPhiRescaled0);

  electron->setTrackCaloMatchValue(static_cast<float>(deltaEta[1]),
                                   xAOD::EgammaParameters::deltaEta1);
  electron->setTrackCaloMatchValue(static_cast<float>(deltaPhi[1]),
                                   xAOD::EgammaParameters::deltaPhi1);
  electron->setTrackCaloMatchValue(static_cast<float>(deltaPhiRescaled[1]),
                                   xAOD::EgammaParameters::deltaPhiRescaled1);

  electron->setTrackCaloMatchValue(static_cast<float>(deltaEta[2]),
                                   xAOD::EgammaParameters::deltaEta2);
  electron->setTrackCaloMatchValue(static_cast<float>(deltaPhi[2]),
                                   xAOD::EgammaParameters::deltaPhi2);
  electron->setTrackCaloMatchValue(static_cast<float>(deltaPhiRescaled[2]),
                                   xAOD::EgammaParameters::deltaPhiRescaled2);

  electron->setTrackCaloMatchValue(static_cast<float>(deltaEta[3]),
                                   xAOD::EgammaParameters::deltaEta3);
  electron->setTrackCaloMatchValue(static_cast<float>(deltaPhi[3]),
                                   xAOD::EgammaParameters::deltaPhi3);
  electron->setTrackCaloMatchValue(static_cast<float>(deltaPhiRescaled[3]),
                                   xAOD::EgammaParameters::deltaPhiRescaled3);

  float deltaPhiLast = static_cast<float>(egRec->deltaPhiLast());
  electron->setTrackCaloMatchValue(
    deltaPhiLast, xAOD::EgammaParameters::deltaPhiFromLastMeasurement);

  return true;
}

bool
topoEgammaBuilder::getPhoton(const egammaRec* egRec,
                             xAOD::PhotonContainer* photonContainer,
                             const unsigned int author,
                             const uint8_t type) const
{
  if (!egRec || !photonContainer){
    return false;
  }

  xAOD::Photon* photon = new xAOD::Photon();
  photonContainer->push_back(photon);
  photon->setAuthor(author);
  static const SG::AuxElement::Accessor<uint8_t> acc("ambiguityType");
  acc(*photon) = type;

  // Transfer the links to the clusters
  std::vector<ElementLink<xAOD::CaloClusterContainer>> clusterLinks;
  for (size_t i = 0; i < egRec->getNumberOfClusters(); ++i) {
    clusterLinks.push_back(egRec->caloClusterElementLink(i));
  }
  photon->setCaloClusterLinks(clusterLinks);

  // Transfer the links to the vertices
  std::vector<ElementLink<xAOD::VertexContainer>> vertexLinks;
  for (size_t i = 0; i < egRec->getNumberOfVertices(); ++i) {
    vertexLinks.push_back(egRec->vertexElementLink(i));
  }
  photon->setVertexLinks(vertexLinks);

  // Transfer deltaEta/Phi info
  float deltaEta = egRec->deltaEtaVtx();
  float deltaPhi = egRec->deltaPhiVtx();
  if (!photon->setVertexCaloMatchValue(
        deltaEta, xAOD::EgammaParameters::convMatchDeltaEta1)) {
    ATH_MSG_WARNING("Could not transfer deltaEta to photon");
    return false;
  }

  if (!photon->setVertexCaloMatchValue(
        deltaPhi, xAOD::EgammaParameters::convMatchDeltaPhi1)) {
    ATH_MSG_WARNING("Could not transfer deltaPhi to photon");
    return false;
  }
  return true;
}
