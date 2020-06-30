/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// =================================================================================
#include "CaloRingsBuilder.h"

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
CaloRingsBuilder::CaloRingsBuilder(const std::string& type,
                                 const std::string& name,
                                 const ::IInterface* parent)
  : ::AthAlgTool(type, name, parent),
    m_rsCont(nullptr),
    m_crCont(nullptr),
    m_nRingSets(0)
{
  declareInterface<ICaloRingsBuilder>(this);
}

// =====================================================================================
CaloRingsBuilder::~CaloRingsBuilder()
{ }

// =====================================================================================
StatusCode CaloRingsBuilder::initialize()
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
          xAOD::RingSetConf::whichSection(rsLayers)
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

  // This will check that the properties were initialized properly
  // by job configuration.
  ATH_CHECK( m_crContName.initialize() );
  ATH_CHECK( m_rsContName.initialize() );
  ATH_CHECK( m_cellsContName.initialize() );

  return StatusCode::SUCCESS;
}

// =====================================================================================
StatusCode CaloRingsBuilder::finalize()
{
  return StatusCode::SUCCESS;
}

// =====================================================================================
StatusCode CaloRingsBuilder::preExecute( xAOD::CaloRingsContainer* crCont
                                       , xAOD::RingSetContainer* rsCont
                                       , std::size_t nReserve )
{
  if ( crCont && rsCont ){
    m_crCont = crCont;
    m_rsCont = rsCont;
  } else {
    ATH_MSG_ERROR( "Attempted to set CaloRingsContainer and/or RingSetContainer to invalid pointers");
    return StatusCode::FAILURE;
  }
  // Reserve container space if required:
  if (nReserve) {
    // Reserve one CaloRings per particle
    m_crCont->reserve( nReserve );
    // We need to reserve more space for the RingSet container, there will be
    // the number of RawConfig available in our raw configuration collection.
    m_rsCont->reserve( nReserve * m_nRingSets );
  }

  return StatusCode::SUCCESS;
}

// =====================================================================================
StatusCode CaloRingsBuilder::execute(const xAOD::CaloCluster &cluster,
    ElementLink<xAOD::CaloRingsContainer> &clRings)
{
  double et(0.);
  const double eta2 = std::fabs(cluster.etaBE(2));
  const double energy =  cluster.e();
  if ( eta2 < 999.) {
    const double cosheta = std::cosh(eta2);
    et = (cosheta != 0.) ? energy /cosheta : 0.;
  }
  if ( et > m_minEnergy )
  {
    return executeTemp(cluster, clRings);
  } else {
    ATH_MSG_DEBUG( "Skipping cluster with low energy (" << et << " MeV) .");
    return StatusCode::SUCCESS;
  }
}

// =====================================================================================
StatusCode CaloRingsBuilder::execute(
    const xAOD::IParticle &particle,
    ElementLink<xAOD::CaloRingsContainer> &clRings)
{
  double et = particle.pt();
  if ( et > m_minEnergy )
  {
    return executeTemp(particle, clRings);
  } else {
    ATH_MSG_DEBUG( "Skipping particle with low energy (" << et << " MeV) .");
    return StatusCode::SUCCESS;
  }
}

// =====================================================================================
// Local execute
// =====================================================================================
template<typename T>
StatusCode CaloRingsBuilder::executeTemp(
    const T &input,
    ElementLink<xAOD::CaloRingsContainer> &clRingsLink)
{

  ATH_MSG_DEBUG("Entering "<< name() << " execute.");

  // Create structure to hold rings:
  xAOD::CaloRings *clRings = new xAOD::CaloRings();

  // Add the CaloRings to the container:
  m_crCont->push_back(clRings);

  // Set elementLink reference to created CaloRings:
  clRingsLink.toContainedElement(*m_crCont, clRings);

  // If not using shower barycenter, we need to reset last valid seed to avoid
  // any possible issue:
  if ( !m_useShowShapeBarycenter ){
    m_lastValidSeed = AtlasGeoPoint(input.eta(),input.phi());
  }

  // Build this CaloRings RingSets:
  for ( const auto &rawConf : m_rsRawConfCol ) {

    // Create new RingSet and add it to the container:
    xAOD::RingSet* rs = new xAOD::RingSet( rawConf.nRings );
    m_rsCont->push_back(rs);

    // Get RingSet seed:
    AtlasGeoPoint seed;
    CHECK( getRingSetSeed( rawConf, input, seed ) );

    // Build it:
    CHECK( buildRingSet( rawConf, seed, rs ) );

    // Get the ElementLink and add it to our RingSet holder:
    ElementLink< xAOD::RingSetContainer > rsEL( rs, *m_rsCont );
    clRings->addRingSetEL( rsEL );
  }

  // Print CaloRings with DEBUG level:
  if (msgLevel() <= MSG::DEBUG){
    std::ostringstream str;
    clRings->print(str);
    ATH_MSG_DEBUG(str.str());
  }

  return StatusCode::SUCCESS;
}

// =================================================================================
StatusCode CaloRingsBuilder::getRingSetSeed(
    const xAOD::RingSetConf::RawConf &rawConf,
    const xAOD::CaloCluster &cluster,
    AtlasGeoPoint &seed )
{
  if ( !m_useShowShapeBarycenter ){

    seed.setEta( cluster.eta() );
    seed.setPhi( cluster.phi() );

    return StatusCode::SUCCESS;

  } else {

    bool foundValid = false, foundMultipleValid = false;

    for ( const auto layer : rawConf.layers ) {

      AtlasGeoPoint seedCandidate(
          cluster.etaSample( layer ),
          cluster.phiSample( layer )
        );

      ATH_MSG_DEBUG( "This layer (" << CaloSampling::getSamplingName(layer) <<
          ") seedCandidate is: (" <<
          seedCandidate.eta() << "," <<
          seedCandidate.phi() << ")" );

      if ( seedCandidate.isValid() ){
        m_lastValidSeed = seedCandidate;
        if ( foundValid )
          foundMultipleValid = true;
        foundValid = true;
      }
    }

    // If we get here, set it to the last valid seed:
    seed = m_lastValidSeed;
    if ( foundMultipleValid ){
      ATH_MSG_WARNING( "Found multiple valid seeds. Set to last valid candidate.");
    }
    return StatusCode::SUCCESS;
  }
}

// =================================================================================
StatusCode CaloRingsBuilder::getRingSetSeed(
    const xAOD::RingSetConf::RawConf &/*rawConf*/,
    const xAOD::IParticle &part,
    AtlasGeoPoint &seed)
{

  seed.setEta( part.eta() );
  seed.setPhi( part.phi() );

  return StatusCode::SUCCESS;
}

// =================================================================================
StatusCode CaloRingsBuilder::buildRingSet(
    const xAOD::RingSetConf::RawConf &rawConf,
    const AtlasGeoPoint &seed,
    xAOD::RingSet *rs)
{
  // Get this RingSet size:
  const auto nRings = rawConf.nRings;

  // Retrieve CaloCells
  SG::ReadHandle<CaloCellContainer> cellsCont(m_cellsContName);
  // check is only used for serial running; remove when MT scheduler used
  if(!cellsCont.isValid()) {
    ATH_MSG_FATAL("Failed to retrieve "<< m_cellsContName.key());
    return StatusCode::FAILURE;
  }
  CaloCellList cells( cellsCont.ptr() );

  // loop over cells
  for ( const int layer : rawConf.layers) { // We use int here because the
    // cells.select() method needs int as param
    cells.select(seed.eta(), seed.phi(), m_cellMaxDEtaDist, m_cellMaxDPhiDist, layer );
    for ( const CaloCell *cell : cells ) {

      unsigned int ringNumber(0);

      // calculate the normalised difference in eta
      const float deltaEta = fabs(
          (cell->eta() - seed.eta())
          ) / rawConf.etaWidth;
      // calculate the normalised difference in phi
      const float deltaPhi = fabs(
          m_phiHelper.diff(cell->phi(), seed.phi())
          ) / rawConf.phiWidth;
      // The biggest difference indicates the ring number (we are using
      // squared-shape rings)
      const float deltaGreater = std::max(deltaEta, deltaPhi);

      // Round to nearest integer:
      ringNumber = static_cast<unsigned int>(std::floor(deltaGreater + .5));
      if ( ringNumber < nRings ){
        rs->at(ringNumber) += cell->energy()/cosh(cell->eta());
      }
    }
  }

  return StatusCode::SUCCESS;
}


} // namespace Ringer



