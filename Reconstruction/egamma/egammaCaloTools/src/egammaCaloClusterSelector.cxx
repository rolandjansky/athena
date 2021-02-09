/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#include "egammaCaloClusterSelector.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "CaloUtils/CaloCellList.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include <CLHEP/Units/SystemOfUnits.h>

#include <cmath>

using CLHEP::MeV;

egammaCaloClusterSelector::egammaCaloClusterSelector(const std::string& type,
                                                     const std::string& name,
                                                     const IInterface* parent) :
  base_class(type, name, parent)
{
}

StatusCode egammaCaloClusterSelector::initialize()
{
  if (!m_egammaCheckEnergyDepositTool.empty()) {
    ATH_CHECK( m_egammaCheckEnergyDepositTool.retrieve() );
  } else {
    m_egammaCheckEnergyDepositTool.disable();
  }

  //Shower Shape tools
  //Initialize tools when non-default values are given
  m_doReta = (m_RetaCut != RETA_DEFAULT_NO_CUT);
  m_doHadLeak = (m_HadLeakCut != HAD_LEAK_DEFAULT_NO_CUT);
  ATH_CHECK(m_cellsKey.initialize(m_doReta||m_doHadLeak));
  if (m_doReta) {
    ATH_CHECK(m_egammaMiddleShape.retrieve());
  } else {
    m_egammaMiddleShape.disable();
  }

  if (m_doHadLeak) {
    ATH_CHECK(m_HadronicLeakageTool.retrieve());
  } else {
    m_HadronicLeakageTool.disable();
  }

  return StatusCode::SUCCESS;
}

StatusCode egammaCaloClusterSelector::finalize()
{
  return StatusCode::SUCCESS;
}

bool egammaCaloClusterSelector::passSelection(
    const xAOD::CaloCluster* cluster, const CaloDetDescrManager& cmgr) const {

  // Minimum Cluster energy
  if (cluster->et() < m_ClusterEtCut) {
    ATH_MSG_DEBUG("Cluster failed Energy Cut: dont make ROI");
    return false;
  }

  // Check energy deposit if requested
  if (!m_egammaCheckEnergyDepositTool.empty() &&
      !m_egammaCheckEnergyDepositTool->checkFractioninSamplingCluster(cluster)) {
    ATH_MSG_DEBUG("Cluster failed sample check: dont make ROI");
    return false;
  }

  // Check second sampling is present
  if (!cluster->hasSampling(CaloSampling::EMB2) && !cluster->hasSampling(CaloSampling::EME2)){
      return false;
  }
  const double eta2 = std::abs(cluster->etaBE(2));
  constexpr double ETA2_CUT = 10;
  if (eta2 > ETA2_CUT) {
    return false;
  }
  // minimum energy reconstructed in 2nd sampling
  constexpr double EM2ENERGY_CUT = 50 * MeV;
  if (cluster->energyBE(2) < EM2ENERGY_CUT){
      return false;
  }
  // use the egamma definition of EMFrac
  // (includes presampler and TileGap3/E4, helps with eff in the crack)
  static const SG::AuxElement::ConstAccessor<float> acc("EMFraction");
  const double emFrac = acc.isAvailable(*cluster) ? acc(*cluster) : 0.;
  const double EMEnergy = cluster->e() * emFrac;
  double EMEt = EMEnergy / std::cosh(eta2);
  // cluster is in crack region
  // if at least one clustered cell in EMB and EMEC
  if (cluster->inBarrel() && cluster->inEndcap()) {
    EMEt /= m_EMEtSplittingFraction;  // rescale EMEt for clusters in the crack
  }

  // EMEt cut
  if (EMEt < m_EMEtCut) {
    ATH_MSG_DEBUG("Cluster failed EMEt cut: don't make ROI");
    return false;
  }

  // EM fraction cut
  if (emFrac < m_EMFCut) {
    ATH_MSG_DEBUG("Cluster failed EM Fraction cut: don't make ROI");
    return false;
  }

  // Reta and Rhad cuts
  if (m_doReta || m_doHadLeak) {
    // retrieve the cell containers
    SG::ReadHandle<CaloCellContainer> cellcoll(m_cellsKey);
    if (m_doReta) {
      IegammaMiddleShape::Info info;
      StatusCode sc = m_egammaMiddleShape->execute(*cluster, cmgr, *cellcoll, info);
      if ( sc.isFailure() ) {
        ATH_MSG_WARNING("call to Middle shape returns failure for execute");
        return false;
      }
      if (info.e277<0 || info.e237<0) {
        ATH_MSG_DEBUG("Negative e277 or e237: dont make ROI");
        return false;
      }
      if (info.e277 != 0. && info.e237/info.e277 < m_RetaCut) {
        ATH_MSG_DEBUG("Cluster failed Reta test: dont make ROI");
        return false;
      }
    }

    if (m_doHadLeak) {
      // calculate information concerning just the hadronic leakage
      IegammaIso::Info info;
      StatusCode sc = m_HadronicLeakageTool->execute(*cluster, cmgr, *cellcoll, info);
      if ( sc.isFailure() ) {
        ATH_MSG_WARNING("call to Iso returns failure for execute");
        return false;
      }
      const double ethad1 = info.ethad1;
      const double ethad = info.ethad;
      const double raphad1 = EMEt != 0. ? ethad1 / EMEt : 0.;
      const double raphad = EMEt != 0. ? ethad / EMEt : 0.;
      if (eta2 >= 0.8 && eta2 < 1.37) {
        if (raphad > m_HadLeakCut) {
          ATH_MSG_DEBUG("Cluster failed Hadronic Leakage test: dont make ROI");
          return false;
        }
      }
      else if (raphad1 > m_HadLeakCut){
        ATH_MSG_DEBUG("Cluster failed Hadronic Leakage test: dont make ROI");
        return false;
      }
    }
  }

  return true;
}
