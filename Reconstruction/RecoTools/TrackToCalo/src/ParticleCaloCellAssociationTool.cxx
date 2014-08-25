/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ParticleCaloCellAssociationTool.h"
// forward declares
#include "RecoToolInterfaces/IParticleCaloExtensionTool.h"


namespace Trk {

  ParticleCaloCellAssociationTool::ParticleCaloCellAssociationTool(const std::string& t, const std::string& n, const IInterface*  p )
    : AthAlgTool(t,n,p),
      m_caloExtensionTool("Trk::ParticleCaloExtensionTool/ParticleCaloExtensionTool") {

    declareInterface<IParticleCaloCellAssociationTool>(this);
    declareProperty("ParticleCaloExtensionTool",   m_caloExtensionTool );
  }

  ParticleCaloCellAssociationTool::~ParticleCaloCellAssociationTool() {}

  StatusCode ParticleCaloCellAssociationTool::initialize() {
    /* Retrieve track extrapolator from ToolService */
    ATH_CHECK( m_caloExtensionTool.retrieve() );

    return StatusCode::SUCCESS;
  }

  StatusCode ParticleCaloCellAssociationTool::finalize() {
    return StatusCode::SUCCESS;
  }

  const xAOD::ParticleCaloExtension* ParticleCaloCellAssociationTool::caloAssociation( xAOD::TrackParticle& trackParticle ) const {
  
    // get the extrapolation into the calo
    const xAOD::ParticleCaloExtension* caloExtension = m_caloExtensionTool->caloExtension(trackParticle);
    if( !caloExtension ) return 0;

    // now collect the cells
  
    return caloExtension;
  }

} // end of namespace Trk
