/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CaloAsymRingsBuilder.cxx 695505 2015-09-17 16:59:37Z wsfreund $
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

namespace Ringer {

// =================================================================================
CaloAsymRingsBuilder::CaloAsymRingsBuilder(const std::string& type,
                                 const std::string& name,
                                 const ::IInterface* parent)
  : CaloRingsBuilder(type, name, parent)//AthAlgTool(type, name, parent)
{

  // declare interface
  declareInterface<ICaloRingsBuilder>(this); 
  
  declareProperty("RingSetContainerName", 
      m_rsContName = "RingSets",
      "Name of the RingSets Container");

  declareProperty("CaloRingsContainerName", 
      m_crContName = "CaloRings",
      "Name of the CaloRings Container");

  declareProperty("CellsContainerName", m_cellsContName = "AllCalo",
      "Key to obtain the cell's container");

  // RingSet configuration properties:
  declareProperty("EtaWidth", m_etaWidth,
      "Each RingSet ring eta width");
  declareProperty("PhiWidth", m_phiWidth,
      "Each RingSet ring phi width");
  declareProperty("MaxCellDEtaDist", m_maxCellDEtaDist,
      "Maximum cell distance in eta to seed");
  declareProperty("MaxCellDPhiDist", m_maxCellDPhiDist,
      "Maximum cell distance in phi to seed");
  declareProperty("NRings", m_nRings,
      "Each RingSet number of rings");
  declareProperty("Layers", m_layers,
      "Concatenated list of layers which will be used " 
      "to build the RingSets");
  declareProperty("RingSetNLayers", m_nLayers,
      "Each RingSet number of layers from the Layers "
      "configurable property to use.");
  declareProperty("DoEtaAxesDivision", m_doEtaAxesDivision = false,
      "Eta Axes can be divide in two.");
  declareProperty("DoPhiAxesDivision", m_doPhiAxesDivision = false,
      "Phi Axes can be divide in two.");
  // Whether to use layer centers or cluster center:
  declareProperty("useShowerShapeBarycenter", m_useShowShapeBarycenter = false, 
      "Switch to use shower barycenter for each layer, "
      "instead of the cluster center.");
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
          m_maxCellDEtaDist, m_maxCellDPhiDist,
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
    xAOD::RingSetConf::print(m_rsRawConfCol, msg(), MSG::ERROR);
    return StatusCode::FAILURE;
  }

  // Print our collection
  xAOD::RingSetConf::print(m_rsRawConfCol, msg(), MSG::DEBUG);

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
  // Get this RingSet size:
  const auto nRings = rawConf.nRings; 
  CaloCellList cells( m_cellCont );
  // loop over cells
  for ( const int layer : rawConf.layers) { // We use int here because the
    // cells.select() method needs int as param
    // FIXME This could have its speed improved by selecting only the truly
    // needed window:
    // If the nRings * eta/phi width is lower than maxCellDEtaDist, then use it
    // instead
    cells.select(seed.eta(), seed.phi(), m_maxCellDEtaDist, m_maxCellDPhiDist, layer ); 
    for ( const CaloCell *cell : cells ) {
      unsigned int ringNumber(0);

      // calculate the normalised difference in eta
      float deltaEta = (cell->eta() - seed.eta())
                             / rawConf.etaWidth;
      bool etaPositive = ( deltaEta > 0 )?true:false;
      deltaEta = fabs(deltaEta);

      // TODO Add phi asimetry...
      // calculate the normalised difference in phi
      const float deltaPhi = fabs(
          m_phiHelper.diff(cell->phi(), seed.phi())
          ) / rawConf.phiWidth;
      // The biggest difference indicates the ring number (we are using
      // squared-shape rings)
      const float deltaGreater = std::max(deltaEta, deltaPhi);

      // Round to nearest integer:
      ringNumber = static_cast<unsigned int>(std::floor(deltaGreater + .5));

      // Correct position in which we shall fill the ring:
      if ( m_doEtaAxesDivision && ringNumber ){
        ringNumber = (etaPositive)?(ringNumber * 2):
                                   ((ringNumber * 2) - 1);
      }

      if ( ringNumber < nRings ){
        rs->at(ringNumber) += cell->energy()/cosh(cell->eta());
      }

    }
  }
  return StatusCode::SUCCESS;
}


} // namespace Ringer



