/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// INCLUDE HEADER FILES:
#include "egammaOQFlagsBuilder.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "xAODEgamma/Egamma.h"
#include "xAODEgamma/EgammaxAODHelpers.h"
#include <algorithm>
#include <cmath>
#include <vector>

#include "CLHEP/Units/SystemOfUnits.h"
#include "CaloConditions/CaloAffectedRegionInfoVec.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloIdentifier/LArEM_ID.h"
#include "FourMomUtils/P4Helpers.h"
#include "Identifier/HWIdentifier.h"
#include "StoreGate/ReadHandle.h"
#include "StoreGate/StoreGateSvc.h"

using CLHEP::GeV;

namespace {
bool
isbadtilecell(CaloCellList& ccl,
              const CaloDetDescrManager& cmgr,
              const float clusterEta,
              const float clusterPhi,
              const double sizeEta,
              const double sizePhi,
              const CaloSampling::CaloSample sample)
{

  bool isbadtilecell = false;
  ccl.select(cmgr, clusterEta, clusterPhi, sizeEta, sizePhi, sample);
  CaloCellList::list_iterator cclIter = ccl.begin();
  CaloCellList::list_iterator cclIterEnd = ccl.end();
  for (; cclIter != cclIterEnd; cclIter++) {
    const CaloCell* cell = (*cclIter);
    if (cell->badcell()) { // check of bad tile cell
      isbadtilecell = true;
      break;
    }
  }
  return isbadtilecell;
}
}

egammaOQFlagsBuilder::egammaOQFlagsBuilder(const std::string& type,
                                           const std::string& name,
                                           const IInterface* parent)
  : AthAlgTool(type, name, parent)
  , m_emHelper(nullptr)
{
  declareInterface<IegammaOQFlagsBuilder>(this);
  m_calocellId = nullptr;
}

egammaOQFlagsBuilder::~egammaOQFlagsBuilder() = default;

StatusCode
egammaOQFlagsBuilder::initialize()
{
  ATH_CHECK(m_cellsKey.initialize());
  ATH_CHECK(m_bcContKey.initialize());
  ATH_CHECK(m_affKey.initialize());

  // Get CaloAffectedTool
  StatusCode sc = m_affectedTool.retrieve();
  if (sc.isFailure()) {
    ATH_MSG_ERROR("Could not retrieve CaloAffectedTool " << m_affectedTool);
    return StatusCode::FAILURE;
  }

  sc = detStore()->retrieve(m_calocellId, "CaloCell_ID");
  if (sc.isFailure()) {
    ATH_MSG_WARNING("Cannot retrieve online_id");
    return StatusCode::FAILURE;
  }

  sc = detStore()->retrieve(m_emHelper);
  if (sc.isFailure()) {
    ATH_MSG_WARNING("Cannot retrieve online_id");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

StatusCode
egammaOQFlagsBuilder::finalize()
{
  return StatusCode::SUCCESS;
}

bool
egammaOQFlagsBuilder::findCentralCell(const xAOD::CaloCluster* cluster,
                                      Identifier& cellCentrId) const
{

  bool thereIsACentrCell = false;

  // LOOP OVER CLUSTER TO FIND THE CENTRAL CELL IN S2
  xAOD::CaloCluster::const_cell_iterator cellIter = cluster->cell_begin();
  xAOD::CaloCluster::const_cell_iterator cellIterEnd = cluster->cell_end();
  float clusEta = cluster->eta();
  float clusPhi = cluster->phi();
  float energymax = -999999.;

  for (; cellIter != cellIterEnd; cellIter++) {
    const CaloCell* cell = (*cellIter);
    if (!cell){
      continue;
    }
    float eta = cell->eta();
    float phi = cell->phi();
    float energy = cell->energy();
    CaloSampling::CaloSample layer = cell->caloDDE()->getSampling();
    if (fabs(eta - clusEta) < 0.025 &&
        fabs(P4Helpers::deltaPhi(phi, clusPhi)) < 0.025 &&
        (layer == CaloSampling::EMB2 || layer == CaloSampling::EME2) &&
        (energy > energymax)) {
      energymax = energy;
      cellCentrId = cellIter->ID();
      thereIsACentrCell = true;
    }
  }
  return thereIsACentrCell;
}

bool
egammaOQFlagsBuilder::isCore(
  const Identifier Id,
  const std::vector<IdentifierHash>& neighbourList) const
{
  const IdentifierHash hashId = m_calocellId->calo_cell_hash(Id);
  std::vector<IdentifierHash>::const_iterator it =
    std::find(neighbourList.begin(), neighbourList.end(), hashId);
  return (it != neighbourList.end());
}

std::vector<IdentifierHash>
egammaOQFlagsBuilder::findNeighbours(const Identifier cellCentrId) const
{
  std::vector<IdentifierHash> neighbourList;
  const IdentifierHash hashId = m_calocellId->calo_cell_hash(cellCentrId);
  m_emHelper->get_neighbours(hashId, LArNeighbours::all2D, neighbourList);
  return neighbourList;
}

StatusCode
egammaOQFlagsBuilder::execute(const EventContext& ctx,
                              const CaloDetDescrManager& cmgr,
                              xAOD::Egamma& eg) const
{
  // Protection against bad pointers
  const xAOD::CaloCluster* cluster = eg.caloCluster();
  if (!cluster) {
    return StatusCode::SUCCESS;
  }
  if (cluster->size() == 0) {
    return StatusCode::SUCCESS;
  }
  //
  const float clusterEta = cluster->eta();
  const float clusterPhi = cluster->phi();
  //
  // In case we have the sizes set during the cluster construction.
  int etaSize = cluster->getClusterEtaSize();
  int phiSize = cluster->getClusterPhiSize();
  // If no proper size could be found automatically, deduce by hand
  // for the known std cases
  if (etaSize == 0 && phiSize == 0) {
    bool isBarrel = xAOD::EgammaHelpers::isBarrel(cluster);
    if (xAOD::EgammaHelpers::isElectron(&eg)) {
      etaSize = (isBarrel ? 3 : 5);
      phiSize = (isBarrel ? 7 : 5);
    } else if (xAOD::EgammaHelpers::isConvertedPhoton(&eg)) {
      etaSize = (isBarrel ? 3 : 5);
      phiSize = (isBarrel ? 7 : 5);
    } else { // unconverted photons
      etaSize = (isBarrel ? 3 : 5);
      phiSize = (isBarrel ? 7 : 5);
    }
  }

  unsigned int iflag = 0;

  // Find the central cell in the middle layer
  Identifier cellCentrId;
  bool foundCentralCell =
    egammaOQFlagsBuilder::findCentralCell(cluster, cellCentrId);

  // Set timing bit
  const double absEnergyGeV = fabs(cluster->e() * (1. / GeV));
  if (absEnergyGeV != 0 &&
      fabs(cluster->time()) > m_TCut + m_TCutVsE / absEnergyGeV) {
    iflag |= (0x1 << xAOD::EgammaParameters::OutTime);
  }

  // Declare totE and badE for LarQ cleaning
  double totE = 0;
  double badE = 0;
  double energyCellMax = 0;

  if (foundCentralCell) {
    // Find the list of neighbours cells, to define the 3x3 cluster core
    std::vector<IdentifierHash> neighbourList = findNeighbours(cellCentrId);

    // Get Bad-channel info for this event
    SG::ReadCondHandle<LArBadChannelCont> larBadChanHdl{ m_bcContKey, ctx };
    const LArBadChannelCont* larBadChanCont = *larBadChanHdl;

    // Loop over all the Lar cluster cells
    xAOD::CaloCluster::const_cell_iterator cellIter = cluster->cell_begin();
    xAOD::CaloCluster::const_cell_iterator cellIterEnd = cluster->cell_end();
    for (; cellIter != cellIterEnd; cellIter++) {
      const CaloCell* cell = (*cellIter);
      if (!cell) {
        continue;
      }
      // Check we are not tile
      if (cell->caloDDE()->is_tile()) {
        continue;
      }
      // Find cell parameters and properties
      float eta = cell->eta();
      // float phi = cell->phi(); // no longer used
      float qual = cell->quality();
      CaloSampling::CaloSample layer = cell->caloDDE()->getSampling();

      // Calculate badE et totE
      if ((cell->provenance() & 0x2000) && !(cell->provenance() & 0x0800)) {
        totE += cell->e();
        if (cell->e() > energyCellMax)
          energyCellMax = cell->e();
        if (cell->quality() > m_QCellCut)
          badE += cell->e();
      }
      bool isACoreCell = false;
      isACoreCell = isCore(cell->ID(), neighbourList);

      bool isStripCoreCell = false;
      if ((layer == CaloSampling::EMB1 || layer == CaloSampling::EME1) &&
          fabs(eta - clusterEta) < 0.025 / 2.) {
        isStripCoreCell = true;
      }

      // Set HEC bit
      if (layer >= CaloSampling::HEC0 && layer <= CaloSampling::HEC3 &&
          qual > m_QCellHECCut) {
        iflag |= (0x1 << xAOD::EgammaParameters::HECHighQ);
      }

      // Set LAr bits
      const LArBadChannel bc = larBadChanCont->offlineStatus(cell->ID());
      //
      if (isACoreCell) {
        if ((cell->provenance() & 0x0A00) == 0x0A00) {
          iflag |= (0x1 << xAOD::EgammaParameters::MissingFEBCellCore);
        }
        if ((cell->provenance() & 0x0A00) == 0x0800) {
          iflag |= (0x1 << xAOD::EgammaParameters::MaskedCellCore);
        }
        if (bc.sporadicBurstNoise() && qual < m_QCellSporCut) {
          iflag |= (0x1 << xAOD::EgammaParameters::SporadicNoiseLowQCore);
        }
        if (bc.deadCalib() || bc.lowNoiseHG() || bc.lowNoiseMG() ||
            bc.lowNoiseLG() || bc.distorted() || bc.unstable() ||
            bc.unstableNoiseHG() || bc.unstableNoiseMG() ||
            bc.unstableNoiseLG() || bc.peculiarCalibrationLine() ||
            bc.almostDead() || bc.shortProblem()) {
          iflag |= (0x1 << xAOD::EgammaParameters::AffectedCellCore);
        }
        if (qual >= 4000) {
          iflag |= (0x1 << xAOD::EgammaParameters::HighQCore);
        }
      } else {
        if ((cell->provenance() & 0x0A00) == 0x0A00) {

          if (layer == CaloSampling::PreSamplerB ||
              layer == CaloSampling::PreSamplerE) {
            iflag |= (0x1 << xAOD::EgammaParameters::MissingFEBCellEdgePS);
          } else if (layer == CaloSampling::EMB1 ||
                     layer == CaloSampling::EME1) {
            iflag |= (0x1 << xAOD::EgammaParameters::MissingFEBCellEdgeS1);
            if (isStripCoreCell)
              iflag |= (0x1 << xAOD::EgammaParameters::BadS1Core);
          } else if (layer == CaloSampling::EMB2 ||
                     layer == CaloSampling::EME2) {
            iflag |= (0x1 << xAOD::EgammaParameters::MissingFEBCellEdgeS2);
          } else if (layer == CaloSampling::EMB3 ||
                     layer == CaloSampling::EME3) {
            iflag |= (0x1 << xAOD::EgammaParameters::MissingFEBCellEdgeS3);
          }
        }
        if ((cell->provenance() & 0x0A00) == 0x0800) {

          if (layer == CaloSampling::PreSamplerB ||
              layer == CaloSampling::PreSamplerE) {
            iflag |= (0x1 << xAOD::EgammaParameters::MaskedCellEdgePS);
          } else if (layer == CaloSampling::EMB1 ||
                     layer == CaloSampling::EME1) {
            iflag |= (0x1 << xAOD::EgammaParameters::MaskedCellEdgeS1);
            if (isStripCoreCell) {
              iflag |= (0x1 << xAOD::EgammaParameters::BadS1Core);
            }
          } else if (layer == CaloSampling::EMB2 ||
                     layer == CaloSampling::EME2) {
            iflag |= (0x1 << xAOD::EgammaParameters::MaskedCellEdgeS2);
          } else if (layer == CaloSampling::EMB3 ||
                     layer == CaloSampling::EME3) {
            iflag |= (0x1 << xAOD::EgammaParameters::MaskedCellEdgeS3);
          }
        }
        if (bc.sporadicBurstNoise() && qual < m_QCellSporCut) {
          iflag |= (0x1 << xAOD::EgammaParameters::SporadicNoiseLowQEdge);
        }

        if (bc.deadCalib() || bc.lowNoiseHG() || bc.lowNoiseMG() ||
            bc.lowNoiseLG() || bc.distorted() || bc.unstable() ||
            bc.unstableNoiseHG() || bc.unstableNoiseMG() ||
            bc.unstableNoiseLG() || bc.peculiarCalibrationLine() ||
            bc.almostDead() || bc.shortProblem()) {

          if (layer == CaloSampling::PreSamplerB ||
              layer == CaloSampling::PreSamplerE) {
            iflag |= (0x1 << xAOD::EgammaParameters::AffectedCellEdgePS);
          } else if (layer == CaloSampling::EMB1 ||
                     layer == CaloSampling::EME1) {
            iflag |= (0x1 << xAOD::EgammaParameters::AffectedCellEdgeS1);
          } else if (layer == CaloSampling::EMB2 ||
                     layer == CaloSampling::EME2) {
            iflag |= (0x1 << xAOD::EgammaParameters::AffectedCellEdgeS2);
          } else if (layer == CaloSampling::EMB3 ||
                     layer == CaloSampling::EME3) {
            iflag |= (0x1 << xAOD::EgammaParameters::AffectedCellEdgeS3);
          }
        }
        if (qual >= 4000) {
          iflag |= (0x1 << xAOD::EgammaParameters::HighQEdge);
        }
      }
    } // end loop over LAr cells

    // Set LArQCleaning bit
    double egammaLArQCleaning = 0;
    if (totE != 0)
      egammaLArQCleaning = badE / totE;
    if (egammaLArQCleaning > m_LArQCut) {
      iflag |= (0x1 << xAOD::EgammaParameters::LArQCleaning);
    }
    // Set HighRcell bit//
    double ratioCell = 0;
    if (totE != 0)
      ratioCell = energyCellMax / totE;
    if (ratioCell > m_RcellCut) {
      iflag |= (0x1 << xAOD::EgammaParameters::HighRcell);
    }
  } // close if found central cell

  // Check the HV components
  float deta = 0;
  float dphi = 0;

  // Get affected info for this event
  SG::ReadCondHandle<CaloAffectedRegionInfoVec> affHdl{ m_affKey, ctx };
  const CaloAffectedRegionInfoVec* affCont = *affHdl;
  if (!affCont) {
    ATH_MSG_WARNING("Do not have affected regions info, is this expected ?");
  }

  //--------------> PRE SAMPLER
  deta = 0.5 * 0.025 * etaSize;
  dphi = 0.5 * 0.025 * phiSize;

  bool isNonNominalHVPS = (m_affectedTool->isAffected(cluster,
                                                      affCont,
                                                      deta,
                                                      dphi,
                                                      CaloSampling::PreSamplerE,
                                                      CaloSampling::PreSamplerE,
                                                      1) ||
                           m_affectedTool->isAffected(cluster,
                                                      affCont,
                                                      deta,
                                                      dphi,
                                                      CaloSampling::PreSamplerB,
                                                      CaloSampling::PreSamplerB,
                                                      1));
  if (isNonNominalHVPS) {
    iflag |= (0x1 << xAOD::EgammaParameters::NonNominalHVPS);
  }

  bool isDeadHVPS = (m_affectedTool->isAffected(cluster,
                                                affCont,
                                                deta,
                                                dphi,
                                                CaloSampling::PreSamplerE,
                                                CaloSampling::PreSamplerE,
                                                2) ||
                     m_affectedTool->isAffected(cluster,
                                                affCont,
                                                deta,
                                                dphi,
                                                CaloSampling::PreSamplerB,
                                                CaloSampling::PreSamplerB,
                                                2));
  if (isDeadHVPS) {
    iflag |= (0x1 << xAOD::EgammaParameters::DeadHVPS);
  }

  //---------------> SAMPLING 2 : CLUSTER CORE
  deta = 0.5 * 0.025 * 3.;
  dphi = 0.5 * 0.025 * 3.;
  bool isDeadHVS2Core = (m_affectedTool->isAffected(cluster,
                                                    affCont,
                                                    deta,
                                                    dphi,
                                                    CaloSampling::EMB2,
                                                    CaloSampling::EMB2,
                                                    2) ||
                         m_affectedTool->isAffected(cluster,
                                                    affCont,
                                                    deta,
                                                    dphi,
                                                    CaloSampling::EME2,
                                                    CaloSampling::EME2,
                                                    2));

  if (isDeadHVS2Core) {
    iflag |= (0x1 << xAOD::EgammaParameters::DeadHVS1S2S3Core);
  }
  //----------------> SAMPLINGS 1,2,3 : CLUSTER EDGE
  deta = 0.5 * 0.025 * etaSize;
  dphi = 0.5 * 0.025 * phiSize;

  bool isNonNominalHVS1S2S3 = (m_affectedTool->isAffected(cluster,
                                                          affCont,
                                                          deta,
                                                          dphi,
                                                          CaloSampling::EMB1,
                                                          CaloSampling::EMB1,
                                                          1) ||
                               m_affectedTool->isAffected(cluster,
                                                          affCont,
                                                          deta,
                                                          dphi,
                                                          CaloSampling::EMB2,
                                                          CaloSampling::EMB2,
                                                          1) ||
                               m_affectedTool->isAffected(cluster,
                                                          affCont,
                                                          deta,
                                                          dphi,
                                                          CaloSampling::EMB3,
                                                          CaloSampling::EMB3,
                                                          1) ||
                               m_affectedTool->isAffected(cluster,
                                                          affCont,
                                                          deta,
                                                          dphi,
                                                          CaloSampling::EME1,
                                                          CaloSampling::EME1,
                                                          1) ||
                               m_affectedTool->isAffected(cluster,
                                                          affCont,
                                                          deta,
                                                          dphi,
                                                          CaloSampling::EME2,
                                                          CaloSampling::EME2,
                                                          1) ||
                               m_affectedTool->isAffected(cluster,
                                                          affCont,
                                                          deta,
                                                          dphi,
                                                          CaloSampling::EME3,
                                                          CaloSampling::EME3,
                                                          1));
  if (isNonNominalHVS1S2S3) {
    iflag |= (0x1 << xAOD::EgammaParameters::NonNominalHVS1S2S3);
  }

  bool isDeadHVS1S2S3Edge = (m_affectedTool->isAffected(cluster,
                                                        affCont,
                                                        deta,
                                                        dphi,
                                                        CaloSampling::EMB1,
                                                        CaloSampling::EMB1,
                                                        2) ||
                             m_affectedTool->isAffected(cluster,
                                                        affCont,
                                                        deta,
                                                        dphi,
                                                        CaloSampling::EMB2,
                                                        CaloSampling::EMB2,
                                                        2) ||
                             m_affectedTool->isAffected(cluster,
                                                        affCont,
                                                        deta,
                                                        dphi,
                                                        CaloSampling::EMB3,
                                                        CaloSampling::EMB3,
                                                        2) ||
                             m_affectedTool->isAffected(cluster,
                                                        affCont,
                                                        deta,
                                                        dphi,
                                                        CaloSampling::EME1,
                                                        CaloSampling::EME1,
                                                        2) ||
                             m_affectedTool->isAffected(cluster,
                                                        affCont,
                                                        deta,
                                                        dphi,
                                                        CaloSampling::EME2,
                                                        CaloSampling::EME2,
                                                        2) ||
                             m_affectedTool->isAffected(cluster,
                                                        affCont,
                                                        deta,
                                                        dphi,
                                                        CaloSampling::EME3,
                                                        CaloSampling::EME3,
                                                        2));

  if (isDeadHVS1S2S3Edge) {
    iflag |= (0x1 << xAOD::EgammaParameters::DeadHVS1S2S3Edge);
  }

  // Check the tile component
  // Get CaloCellContainer
  SG::ReadHandle<CaloCellContainer> cellcoll(m_cellsKey, ctx);
  // check is only used for serial running; remove when MT scheduler used
  if (!cellcoll.isValid()) {
    ATH_MSG_ERROR("Failed to retrieve cell container: " << m_cellsKey.key());
    return StatusCode::FAILURE;
  }
  CaloCell_ID::SUBCALO HADCal =
    static_cast<CaloCell_ID::SUBCALO>(CaloCell_ID::TILE);
  CaloCellList ccl(cellcoll.cptr(), HADCal);
  double size = 0.12;
  // TileBar0  or TileExt0
  bool isDeadCellTileS0 =
    (isbadtilecell(
       ccl, cmgr, clusterEta, clusterPhi, size, size, CaloSampling::TileBar0) ||
     isbadtilecell(
       ccl, cmgr, clusterEta, clusterPhi, size, size, CaloSampling::TileExt0));

  if (isDeadCellTileS0) {
    iflag |= (0x1 << xAOD::EgammaParameters::DeadCellTileS0);
  }
  // TileBar1 TileExt1 TileGap1 TileBar2 TileExt2 TileGap2
  bool isDeadCellTileS1S2 =
    (isbadtilecell(
       ccl, cmgr, clusterEta, clusterPhi, size, size, CaloSampling::TileBar1) ||
     isbadtilecell(
       ccl, cmgr, clusterEta, clusterPhi, size, size, CaloSampling::TileExt1) ||
     isbadtilecell(
       ccl, cmgr, clusterEta, clusterPhi, size, size, CaloSampling::TileGap1) ||
     isbadtilecell(
       ccl, cmgr, clusterEta, clusterPhi, size, size, CaloSampling::TileBar2) ||
     isbadtilecell(
       ccl, cmgr, clusterEta, clusterPhi, size, size, CaloSampling::TileExt2) ||
     isbadtilecell(
       ccl, cmgr, clusterEta, clusterPhi, size, size, CaloSampling::TileGap2));
  if (isDeadCellTileS1S2) {
    iflag |= (0x1 << xAOD::EgammaParameters::DeadCellTileS1S2);
  }

  eg.setOQ(iflag);
  ATH_MSG_DEBUG("Executing egammaOQFlagsBuilder::execute");
  return StatusCode::SUCCESS;
}

