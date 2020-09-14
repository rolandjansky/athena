/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */

/******************************************************************************
 * @package Trigger/TrigAlgorithms/TrigEFMissingET
 * @file CellFex.cxx
 *
 * Implementation of the cell fex class
 * @author Jon Burr
 *****************************************************************************/

#include "CellFex.h"
#include "TrigEFMissingET/METComponent.h"
#include "TrigEFMissingET/SignedKinematics.h"
#include "StoreGate/ReadCondHandle.h"
#include "CaloGeoHelpers/CaloSampling.h"
#include <array>
#include <numeric>

// TODO - we should check a couple of things
// - What is the 'prefetch' used by the TopoClusterMaker? Do we need to use it?
// - Should we be using the 2-Gaussian noise estimate or not? (Right now we do)
// - Should we be skipping 'bad' cells in the loop? (Right now we include them
// in the sum)
// - Previously we had the bytestream error flag in the output objects. I can't
// tell if there's any equivalent in the new access mechanism

namespace {
  /// The number of samplings that we use in this algorithm. We do not use the
  /// 'mini FCal' samplings (that appear after FCAL2 in the enum) so we ensure
  /// that we stop after the FCAL
  static constexpr std::size_t N_SAMPLINGS{CaloSampling::FCAL2+1};
} //> end anonymous namespace

namespace HLT { namespace MET {
  CellFex::CellFex(const std::string& name, ISvcLocator* pSvcLocator) :
    FexBase(name, pSvcLocator)
  {}

  StatusCode CellFex::initialize()
  {
    CHECK( m_cellsKey.initialize() );
    CHECK( m_noiseCDOKey.initialize() );
    CHECK( detStore()->retrieve(m_caloCellID, "CaloCell_ID") );
    // Build up the list of component names
    std::vector<std::string> componentNames;
    componentNames.reserve(N_SAMPLINGS);
    for (std::size_t ii = 0; ii < N_SAMPLINGS; ++ii)
      componentNames.push_back(CaloSampling::getSamplingName(ii) );
    return initializeBase(componentNames);
  }

  StatusCode CellFex::fillMET(
      xAOD::TrigMissingET& met,
      const EventContext& context,
      MonGroupBuilder&) const
  {
    // Retrieve the inputs
    auto cells = SG::makeHandle(m_cellsKey, context);
      if (!cells.isValid())
      {
        ATH_MSG_ERROR("Failed to retrieve " << m_cellsKey);
        return StatusCode::FAILURE;
      }
    // NB - there's no makeHandle overload for ReadCondHandle
    SG::ReadCondHandle noiseCDO(m_noiseCDOKey, context);
      if (!noiseCDO.isValid())
      {
        ATH_MSG_ERROR("Failed to retrieve " << m_noiseCDOKey);
        return StatusCode::FAILURE;
      }

    // Prepare the individual components
    std::array<METComponent, N_SAMPLINGS> sums;
    // Iterate over the calorimeter cells
    for (const CaloCell* icell : *cells) {
      // Get the noise. The two different calls are equivalent for LAr cells,
      // but do differ for the TileCal. As far as I can see, the 'two Gaussian'
      // approach is the more recommended one.
      float noise = m_doTwoGaussianNoise ? 
        noiseCDO->getEffectiveSigma(icell->ID(), icell->gain(), icell->energy() ) :
        noiseCDO->getNoise(icell->ID(), icell->gain() );
      // Noise selections, first |E| < T1*S
      if (m_absNoiseThreshold > 0 &&
          std::abs(icell->energy() ) < m_absNoiseThreshold*noise)
        continue;
      // Then E > -T2*S
      if (m_negNoiseThreshold > 0 &&
          icell->energy() < -m_negNoiseThreshold*noise)
        continue;
      // What about bad cells?
      if (const CaloDetDescrElement* dde = icell->caloDDE() ) {
        // Get the right component
        METComponent& sum = sums.at(dde->getSampling() );
        sum += SignedKinematics::fromEnergyEtaPhi(
            icell->energy(), dde->eta(), dde->phi() );
      }
      else {
        auto id = icell->ID();
        METComponent& sum = sums.at(m_caloCellID->sampling(id) );
        sum += SignedKinematics::fromEnergyEtaPhi(
            icell->energy(), m_caloCellID->eta(id), m_caloCellID->phi(id) );
      }
    }
    // Save the full sum
    std::accumulate(sums.begin(), sums.end(), METComponent{}).fillMET(met);
    // Save each component
    for (std::size_t ii = 0; ii < N_SAMPLINGS; ++ii)
      sums.at(ii).fillMETComponent(ii, met);

    return StatusCode::SUCCESS;
  }
} } //> end namespace HLT::MET
