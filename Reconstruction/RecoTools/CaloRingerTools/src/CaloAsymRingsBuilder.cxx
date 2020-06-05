/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// =================================================================================
#include "CaloAsymRingsBuilder.h"
//#include "CaloRingsBuilder.h"

#include "AthenaKernel/errorcheck.h"

// Cell includes:
#include "CaloUtils/CaloCellList.h"
#include "CaloGeoHelpers/CaloSampling.h"
#include "CaloEvent/CaloCellContainer.h"

// RingSet includes:
#include "xAODCaloRings/RingSetContainer.h"
#include "xAODCaloRings/RingSetAuxContainer.h"

// CaloRings includes:
#include "xAODCaloRings/CaloRingsContainer.h"
#include "xAODCaloRings/CaloRingsAuxContainer.h"

// Ringer Conf include:
#include "xAODCaloRings/RingSetConf.h"

// Other xAOD includes:
#include "xAODBase/IParticle.h"
#include "xAODCaloEvent/CaloCluster.h"

// STL
#include <algorithm>
#include <math.h>
#include <cfloat>
#include <sstream>

namespace Ringer {

// =================================================================================
CaloAsymRingsBuilder::CaloAsymRingsBuilder(const std::string& type,
                                 const std::string& name,
                                 const ::IInterface* parent)
  : CaloRingsBuilder(type, name, parent)//AthAlgTool(type, name, parent)
{

  // declare interface
  declareInterface<ICaloRingsBuilder>(this);

  declareProperty("DoEtaAxesDivision", m_doEtaAxesDivision = false,
      "Eta Axes can be divide in two.");
  declareProperty("DoPhiAxesDivision", m_doPhiAxesDivision = false,
      "Phi Axes can be divide in two.");
}

// =====================================================================================
CaloAsymRingsBuilder::~CaloAsymRingsBuilder()
{ }

// =====================================================================================
StatusCode CaloAsymRingsBuilder::initialize()
{
  ATH_MSG_DEBUG("Initializing " << name() );

  m_nRingSets = m_nRings.size();

  auto itr = m_layers.begin();

  // Build RingSets configuration:
  for (size_t rsConfIdx = 0; rsConfIdx < m_nRingSets; ++rsConfIdx) {

    const auto rsNLayers = m_nLayers[rsConfIdx];

    auto end_itr = itr + rsNLayers;

    // Unfortunately, as we can't declare property to a
    // std::vector<CaloSampling::CaloSample>, we will have to do this to
    // convert the types:
    const auto& caloSampleItr = reinterpret_cast<
        std::vector<CaloSampling::CaloSample>::iterator&
      >(itr);
    const auto& caloSampleEndItr = reinterpret_cast<
        std::vector<CaloSampling::CaloSample>::iterator&
      >(end_itr);

    std::vector<CaloSampling::CaloSample> rsLayers( caloSampleItr ,
        caloSampleEndItr);

    itr += rsNLayers;

    const auto rawConf = xAOD::RingSetConf::RawConf(
          m_nRings[rsConfIdx],
          rsLayers,
          m_etaWidth[rsConfIdx], m_phiWidth[rsConfIdx],
          m_cellMaxDEtaDist, m_cellMaxDPhiDist,
          xAOD::RingSetConf::whichLayer(rsLayers),
          xAOD::RingSetConf::whichSection(rsLayers),
          m_doEtaAxesDivision, m_doPhiAxesDivision
        );

    // Build our raw configuration structure:
    m_rsRawConfCol.push_back(rawConf);
  }

  // We have finished filling the main raw configuration properties, now we add
  // it bounderies:
  try {
    xAOD::RingSetConf::addRawConfColBounderies(m_rsRawConfCol);
  } catch ( const std::runtime_error &e) {
    ATH_MSG_ERROR("Could not add collection bounderies due to: " << e.what() );
    ATH_MSG_ERROR("RawConfCollection is: ");
    std::ostringstream str;
    xAOD::RingSetConf::print(m_rsRawConfCol, str);
    ATH_MSG_ERROR(str.str());
    return StatusCode::FAILURE;
  }

  // Print our collection
  if (msgLevel() <= MSG::DEBUG){
    std::ostringstream str;
    xAOD::RingSetConf::print(m_rsRawConfCol, str);
    ATH_MSG_DEBUG(str.str());
  }

  return StatusCode::SUCCESS;
}

// =====================================================================================
StatusCode CaloAsymRingsBuilder::finalize()
{
  return StatusCode::SUCCESS;
}

// =================================================================================
StatusCode CaloAsymRingsBuilder::buildRingSet(
    const xAOD::RingSetConf::RawConf &rawConf,
    const AtlasGeoPoint &seed,
    xAOD::RingSet *rs)
{
  // Retrieve CaloCells
  SG::ReadHandle<CaloCellContainer> cellsCont(m_cellsContName);
  // check is only used for serial running; remove when MT scheduler used
  if(!cellsCont.isValid()) {
    ATH_MSG_FATAL("Failed to retrieve "<< m_cellsContName.key());
    return StatusCode::FAILURE;
  }
  CaloCellList cells( cellsCont.ptr() );

  // Get this RingSet size:
  const auto nRings = rawConf.nRings;
  // loop over cells
  for ( const int layer : rawConf.layers) { // We use int here because the
    // cells.select() method needs int as param
    // FIXME This could have its speed improved by selecting only the truly
    // needed window:
    // If the nRings * eta/phi width is lower than cellMaxDEtaDist, then use it
    // instead
    cells.select(seed.eta(), seed.phi(), m_cellMaxDEtaDist, m_cellMaxDPhiDist, layer );
    for ( const CaloCell *cell : cells ) {
      unsigned int ringNumber(0);

      // calculate the normalised difference in eta
      float deltaEta = (cell->eta() - seed.eta())
                             / rawConf.etaWidth;
      bool etaPositive = ( deltaEta > 0 )?true:false;
      deltaEta = fabs(deltaEta);

      // TODO Add phi asimetry...
      // calculate the normalised difference in phi
      float deltaPhi = m_phiHelper.diff(cell->phi(), seed.phi());

      bool phiPositive = ( deltaPhi > 0)?true:false;
      deltaPhi = fabs(deltaPhi)/rawConf.phiWidth;

      // The biggest difference indicates the ring number (we are using
      // squared-shape rings)
      const float deltaGreater = std::max(deltaEta, deltaPhi);

      // Round to nearest integer:
      ringNumber = static_cast<unsigned int>(std::floor(deltaGreater + .5));

      // Correct position in which we shall fill the ring:
      if ( m_doEtaAxesDivision && m_doPhiAxesDivision && ringNumber ){
        if (phiPositive){
          if (etaPositive){
            ringNumber = (ringNumber * 4) - 3;
          }
          else{
            ringNumber = (ringNumber * 4) - 2;
          }
        }
        else if (etaPositive){
          ringNumber = (ringNumber * 4) - 1;
        }
        else{
          ringNumber = (ringNumber * 4);
        }
      } else if ((m_doEtaAxesDivision || m_doPhiAxesDivision) && ringNumber){
        if (m_doEtaAxesDivision){
          ringNumber = (etaPositive)?(ringNumber * 2):
                                    ((ringNumber * 2) - 1);
        }
        else {
          ringNumber = (phiPositive)?(ringNumber * 2):
                                    ((ringNumber * 2) - 1);
        }
      }

      if ( ringNumber < nRings ){
        rs->at(ringNumber) += cell->energy()/cosh(cell->eta());
      }

    }
  }
  return StatusCode::SUCCESS;
}


} // namespace Ringer



