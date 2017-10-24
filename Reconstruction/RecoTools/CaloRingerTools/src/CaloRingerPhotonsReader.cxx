/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CaloRingerPhotonsReader.cxx 752568 2016-06-03 16:03:21Z ssnyder $
// =============================================================================
#include "CaloRingerPhotonsReader.h"
#include "StoreGate/ReadHandle.h"

#include <algorithm>

namespace Ringer {

// =============================================================================
CaloRingerPhotonsReader::CaloRingerPhotonsReader(const std::string& type,
                                 const std::string& name,
                                 const ::IInterface* parent)
  : CaloRingerInputReader(type, name, parent),
    m_clRingsBuilderPhotonFctor(0)
    //m_selectorAvailable(false)
{

  // declare interface
  declareInterface<ICaloRingerPhotonsReader>(this);

}

// =============================================================================
CaloRingerPhotonsReader::~CaloRingerPhotonsReader()
{ 
  if(m_clRingsBuilderPhotonFctor) delete m_clRingsBuilderPhotonFctor;
}

// =============================================================================
StatusCode CaloRingerPhotonsReader::initialize()
{

  CHECK( CaloRingerInputReader::initialize() );

  ATH_CHECK(m_inputPhotonContainerKey.initialize());

  if ( m_builderAvailable ) {
    // Initialize our fctor 
    m_clRingsBuilderPhotonFctor = 
      new BuildCaloRingsFctor<const xAOD::Photon>(
          m_crBuilder, 
          msg() );
  }

  return StatusCode::SUCCESS;
}

// =============================================================================
StatusCode CaloRingerPhotonsReader::finalize()
{
  return StatusCode::SUCCESS;
}

// =============================================================================
StatusCode CaloRingerPhotonsReader::execute()
{

  ATH_MSG_DEBUG("Entering " << name() << " execute.");

  // Retrieve photons 
  SG::ReadHandle<xAOD::PhotonContainer> photons(m_inputPhotonContainerKey);
  // check is only used for serial running; remove when MT scheduler used
  if(!photons.isValid()) {
    ATH_MSG_FATAL("Failed to retrieve "<< m_inputPhotonContainerKey.key());
    return StatusCode::FAILURE;
  }

  if ( m_builderAvailable ) {
    m_clRingsBuilderPhotonFctor->prepareToLoopFor(photons->size());

    // loop over our particles:
    std::for_each( 
        photons->begin(), 
        photons->end(), 
        *m_clRingsBuilderPhotonFctor );
  }


  return StatusCode::SUCCESS;

}

} // namespace Ringer

