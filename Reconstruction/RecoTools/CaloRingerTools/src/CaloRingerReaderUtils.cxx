/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

// =====================================================================================
#include "CaloRingerReaderUtils.h"


// Kernel includes:
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ToolHandle.h"

// xAOD includes:
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODCaloRings/CaloRingsContainer.h"
#include "xAODCaloRings/RingSetContainer.h"
#include "xAODCaloRings/CaloRingsAuxContainer.h"
#include "xAODCaloRings/RingSetAuxContainer.h"

namespace Ringer {

// =====================================================================================
StatusCode BuildCaloRingsFctorBase::prepareToLoopFor( std::size_t nParticles ) {
  ATH_MSG_DEBUG(" ---------- Preparing to loop for " << nParticles << " particles");
  // Reset counters, set number of particles to nParticles:
  m_part_size = nParticles;
  m_part_counter = 0;

  // Make and set the CaloRings and RingSets container
  m_crContH = new SG::WriteHandle<xAOD::CaloRingsContainer >(m_builder->crContName());
  ATH_CHECK(m_crContH->record(std::make_unique<xAOD::CaloRingsContainer>(),
                              std::make_unique<xAOD::CaloRingsAuxContainer>()));
  m_rsContH = new SG::WriteHandle<xAOD::RingSetContainer>(m_builder->rsContName());
  ATH_CHECK(m_rsContH->record(std::make_unique<xAOD::RingSetContainer>(),
                              std::make_unique<xAOD::RingSetAuxContainer>()));

  // Set containers which will hold the build CaloRings and reserve one slot per
  // particle as initial guess:
  return m_builder->preExecute( m_crContH->ptr(), m_rsContH->ptr(), nParticles );
}

// =====================================================================================
void BuildCaloRingsFctorBase::checkRelease() {
  if ( m_part_counter == m_part_size ) {
    delete m_crContH; m_crContH = nullptr;
    delete m_rsContH; m_rsContH = nullptr;
  }
}

} // namespace Ringer
