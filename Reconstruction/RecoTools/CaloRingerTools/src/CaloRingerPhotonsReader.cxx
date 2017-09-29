/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CaloRingerPhotonsReader.cxx 752568 2016-06-03 16:03:21Z ssnyder $
// =============================================================================
#include "CaloRingerPhotonsReader.h"

#include <algorithm>

namespace Ringer {

// =============================================================================
CaloRingerPhotonsReader::CaloRingerPhotonsReader(const std::string& type,
                                 const std::string& name,
                                 const ::IInterface* parent)
  : CaloRingerInputReader(type, name, parent),
    m_container(nullptr),
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

  if ( m_builderAvailable ) {
    // Initialize our fctor 
    m_clRingsBuilderPhotonFctor = 
      new BuildCaloRingsFctor<xAOD::Photon>(
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
  if ( evtStore()->retrieve(m_container, m_inputKey).isFailure() )
  {
    ATH_MSG_ERROR("Cannot retrieve photon container " << m_inputKey );
    return StatusCode::FAILURE;
  }


  if ( m_builderAvailable ) {
    m_clRingsBuilderPhotonFctor->prepareToLoopFor(m_container->size());

    // loop over our particles:
    std::for_each( 
        m_container->begin(), 
        m_container->end(), 
        *m_clRingsBuilderPhotonFctor );
  }


  return StatusCode::SUCCESS;

}

} // namespace Ringer

