/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "AthenaKernel/errorcheck.h"
#include "AthLinks/ElementLink.h"

#include "GeneratorObjects/xAODTruthParticleLink.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/DataSvc.h"
#include "GaudiKernel/PhysicalConstants.h"

#include "EventInfo/EventStreamInfo.h"

#include "xAODTruth/TruthParticle.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthParticleAuxContainer.h"

#include "xAODTruth/TruthEvent.h"
#include "xAODTruth/TruthEventContainer.h"

#include "GeneratorFilters/xAODTruthParticleSlimmerLightLepton.h"

xAODTruthParticleSlimmerLightLepton::xAODTruthParticleSlimmerLightLepton(const std::string &name, ISvcLocator *svcLoc)
    : AthReentrantAlgorithm(name, svcLoc)
{
}

StatusCode xAODTruthParticleSlimmerLightLepton::initialize()
{   
    ATH_CHECK(m_xaodTruthEventContainerNameReadHandleKey.initialize());
    ATH_CHECK(m_xaodTruthParticleContainerNameLightLeptonKey.initialize());
    return StatusCode::SUCCESS;
}

StatusCode xAODTruthParticleSlimmerLightLepton::execute(const EventContext& context) const
{
    // If the containers already exists then assume that nothing needs to be done
    if (evtStore()->contains<xAOD::TruthParticleContainer>(m_xaodTruthParticleContainerNameLightLeptonKey.key()))
    {
        ATH_MSG_WARNING("xAOD LightLeptons Truth Particles are already available in the event");
        return StatusCode::SUCCESS;
    }

    // Create new output container
    SG::WriteHandle<xAOD::TruthParticleContainer> xTruthParticleContainerLightLepton(m_xaodTruthParticleContainerNameLightLeptonKey, context);
	ATH_CHECK(xTruthParticleContainerLightLepton.record(std::make_unique<xAOD::TruthParticleContainer>(), std::make_unique<xAOD::TruthParticleAuxContainer>()));
        
    SG::ReadHandle<xAOD::TruthEventContainer> xTruthEventContainerReadHandle(m_xaodTruthEventContainerNameReadHandleKey, context);
    if (!xTruthEventContainerReadHandle.isValid()) {
	  ATH_MSG_ERROR("Could not retrieve xAOD::TruthEventContainer with key:" << 
			m_xaodTruthEventContainerNameReadHandleKey.key());
	  return StatusCode::FAILURE;
	}

    xAOD::TruthEventContainer::const_iterator itr;
    for (itr = xTruthEventContainerReadHandle->begin(); itr!=xTruthEventContainerReadHandle->end(); ++itr) {

        unsigned int nPart = (*itr)->nTruthParticles();
        for (unsigned int iPart = 0; iPart < nPart; ++iPart) {
            const xAOD::TruthParticle* particle =  (*itr)->truthParticle(iPart);
            
            int this_absPdgID = particle->absPdgId();
            int this_status = particle->status();

            //Save stable Electrons & Muons
            if (this_status == 1 && (this_absPdgID == 11 || this_absPdgID == 13) )
            {
                xAOD::TruthParticle *xTruthParticle = new xAOD::TruthParticle();
                xTruthParticleContainerLightLepton->push_back( xTruthParticle );

                // Fill with numerical content
                xTruthParticle->setPdgId(particle->pdgId());
                xTruthParticle->setBarcode(particle->barcode());
                xTruthParticle->setStatus(particle->status());
                xTruthParticle->setM(particle->m());
                xTruthParticle->setPx(particle->px());
                xTruthParticle->setPy(particle->py());
                xTruthParticle->setPz(particle->pz());
                xTruthParticle->setE(particle->e());
            }
        }

    }

    return StatusCode::SUCCESS;
}
