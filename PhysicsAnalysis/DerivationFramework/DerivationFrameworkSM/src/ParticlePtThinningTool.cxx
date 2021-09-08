// Author: ebakos 

#include "DerivationFrameworkSM/ParticlePtThinningTool.h"
#include "AthenaKernel/IThinningSvc.h"
#include <xAODBase/IParticleContainer.h>

#include <vector>
#include <string>

// Constructor
DerivationFramework::ParticlePtThinningTool::ParticlePtThinningTool( 	const std::string& t,
                                                 		const std::string& n,
                                                 		const IInterface* p) : 
  AthAlgTool(t,n,p),
  m_thinningSvc("ThinningSvc",n),
  //variables
  m_PtCut(20.0),
  m_ContainerName("")
  {
    declareInterface<DerivationFramework::IThinningTool>(this);
    declareProperty("ThinningService", m_thinningSvc, "Which thinningservice to use.");
    declareProperty("PtCut", m_PtCut, "Height of the ptcut in MeV.");
    declareProperty("ContainerName", m_ContainerName, "What container name to use, for example 'Photons'.");		
  }
  
// Destructor
DerivationFramework::ParticlePtThinningTool::~ParticlePtThinningTool() {
}  

// Athena initialize and finalize
StatusCode DerivationFramework::ParticlePtThinningTool::initialize()
{
     ATH_MSG_VERBOSE("initialize() ...");
     return StatusCode::SUCCESS;
}
StatusCode DerivationFramework::ParticlePtThinningTool::finalize()
{
     ATH_MSG_VERBOSE("finalize() ...");
     return StatusCode::SUCCESS;
}

// The thinning itself
StatusCode DerivationFramework::ParticlePtThinningTool::doThinning() const
{
     // Get the particle container
     auto *particles = evtStore()->retrieve<const xAOD::IParticleContainer>(m_ContainerName.c_str());
     
     if (!particles) {
          ATH_MSG_ERROR ("Couldn't retrieve Container with key " << m_ContainerName);
          return StatusCode::FAILURE;
     }

     // Loop over particles, see if they pass, set mask
     std::vector<bool> particle_mask;
     particle_mask.reserve(particles->size());

     for (
          xAOD::IParticleContainer::const_iterator it = particles->begin(); 
          it != particles->end(); 
          ++it
     ) {
          particle_mask.push_back((*it)->pt() > m_PtCut);
     }

     if (m_thinningSvc->filter(*particles, particle_mask, IThinningSvc::Operator::Or).isFailure()) {
          ATH_MSG_ERROR("Application of thinning service failed! ");
          return StatusCode::FAILURE;
     }

     return StatusCode::SUCCESS;
}  
  
