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

#include "GeneratorFilters/xAODTruthParticleSlimmerPhoton.h"

xAODTruthParticleSlimmerPhoton::xAODTruthParticleSlimmerPhoton(const std::string &name, ISvcLocator *svcLoc)
    : AthAlgorithm(name, svcLoc)
{
    declareProperty("xAODTruthParticleContainerName", m_xaodTruthParticleContainerName = "TruthParticles");
    declareProperty("xAODTruthParticleContainerNamePhoton", m_xaodTruthParticleContainerNamePhoton = "TruthPhotons");
    declareProperty("xAODTruthEventContainerName", m_xaodTruthEventContainerName = "TruthEvents");
}

StatusCode xAODTruthParticleSlimmerPhoton::initialize()
{
    ATH_MSG_INFO("xAOD input TruthParticleContainer name = " << m_xaodTruthParticleContainerName);
    ATH_MSG_INFO("xAOD output TruthParticleContainerPhoton name = " << m_xaodTruthParticleContainerNamePhoton);
    ATH_MSG_INFO("xAOD input xAODTruthEventContainerName name = " << m_xaodTruthEventContainerName);
    return StatusCode::SUCCESS;
}

StatusCode xAODTruthParticleSlimmerPhoton::execute()
{
    // If the containers already exists then assume that nothing needs to be done
    if (evtStore()->contains<xAOD::TruthParticleContainer>(m_xaodTruthParticleContainerNamePhoton))
    {
        ATH_MSG_WARNING("xAOD Photon Truth Particles are already available in the event");
        return StatusCode::SUCCESS;
    }

    // Create new output container
    xAOD::TruthParticleContainer *xTruthParticleContainerPhoton = new xAOD::TruthParticleContainer();
    CHECK(evtStore()->record(xTruthParticleContainerPhoton, m_xaodTruthParticleContainerNamePhoton));
    xAOD::TruthParticleAuxContainer *xTruthParticleAuxContainerPhoton = new xAOD::TruthParticleAuxContainer();
    CHECK(evtStore()->record(xTruthParticleAuxContainerPhoton, m_xaodTruthParticleContainerNamePhoton + "Aux."));
    xTruthParticleContainerPhoton->setStore(xTruthParticleAuxContainerPhoton);
    ATH_MSG_INFO("Recorded TruthParticleContainerPhoton with key: " << m_xaodTruthParticleContainerNamePhoton);

    // Retrieve full TruthEventContainer container
    const xAOD::TruthEventContainer *xTruthEventContainer=NULL;
    if (evtStore()->retrieve(xTruthEventContainer, m_xaodTruthEventContainerName).isFailure())
    {
        ATH_MSG_ERROR("No TruthEvent collection with name " << m_xaodTruthEventContainerName << " found in StoreGate!");
        return StatusCode::FAILURE;
    }
    // Set up decorators if needed
    xAOD::TruthEventContainer::const_iterator itr;
    for (itr = xTruthEventContainer->begin(); itr!=xTruthEventContainer->end(); ++itr) {

        unsigned int nPart = (*itr)->nTruthParticles();
        for (unsigned int iPart = 0; iPart < nPart; ++iPart) {
            const xAOD::TruthParticle* theParticle =  (*itr)->truthParticle(iPart);

            int this_absPdgID = theParticle->absPdgId();
            int this_status = theParticle->status();

            //Save stable Photons 
            if (this_status == 1 && this_absPdgID == 22)
            {
                xAOD::TruthParticle *xTruthParticle = new xAOD::TruthParticle();
                xTruthParticleContainerPhoton->push_back( xTruthParticle );

                // Fill with numerical content
                xTruthParticle->setPdgId(theParticle->pdgId());
                xTruthParticle->setBarcode(theParticle->barcode());
                xTruthParticle->setStatus(theParticle->status());
                xTruthParticle->setM(theParticle->m());
                xTruthParticle->setPx(theParticle->px());
                xTruthParticle->setPy(theParticle->py());
                xTruthParticle->setPz(theParticle->pz());
                xTruthParticle->setE(theParticle->e());
            }   
        }
    }

    return StatusCode::SUCCESS;
}
