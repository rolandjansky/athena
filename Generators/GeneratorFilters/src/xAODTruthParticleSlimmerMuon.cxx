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

#include "GeneratorFilters/xAODTruthParticleSlimmerMuon.h"

xAODTruthParticleSlimmerMuon::xAODTruthParticleSlimmerMuon(const std::string &name, ISvcLocator *svcLoc)
    : AthAlgorithm(name, svcLoc)
{
    declareProperty("xAODTruthParticleContainerName", m_xaodTruthParticleContainerName = "TruthParticles");
    declareProperty("xAODTruthParticleContainerNameMuon", m_xaodTruthParticleContainerNameMuon = "TruthMuons");
    declareProperty("xAODTruthEventContainerName", m_xaodTruthEventContainerName = "TruthEvents");
}

StatusCode xAODTruthParticleSlimmerMuon::initialize()
{
    ATH_MSG_INFO("xAOD input TruthParticleContainer name = " << m_xaodTruthParticleContainerName);
    ATH_MSG_INFO("xAOD output TruthParticleContainerMuon name = " << m_xaodTruthParticleContainerNameMuon);
    ATH_MSG_INFO("xAOD input xAODTruthEventContainerName name = " << m_xaodTruthEventContainerName);
    return StatusCode::SUCCESS;
}

StatusCode xAODTruthParticleSlimmerMuon::execute()
{
    // If the containers already exists then assume that nothing needs to be done
    if (evtStore()->contains<xAOD::TruthParticleContainer>(m_xaodTruthParticleContainerNameMuon))
    {
        ATH_MSG_WARNING("xAOD Muon Truth Particles are already available in the event");
        return StatusCode::SUCCESS;
    }

    // Create new output container
    xAOD::TruthParticleContainer *xTruthParticleContainerMuon = new xAOD::TruthParticleContainer();
    CHECK(evtStore()->record(xTruthParticleContainerMuon, m_xaodTruthParticleContainerNameMuon));
    xAOD::TruthParticleAuxContainer *xTruthParticleAuxContainerMuon = new xAOD::TruthParticleAuxContainer();
    CHECK(evtStore()->record(xTruthParticleAuxContainerMuon, m_xaodTruthParticleContainerNameMuon + "Aux."));
    xTruthParticleContainerMuon->setStore(xTruthParticleAuxContainerMuon);
    ATH_MSG_INFO("Recorded TruthParticleContainerMuon with key: " << m_xaodTruthParticleContainerNameMuon);

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

            //Save stable Muons 
            if (this_status == 1 && this_absPdgID == 13)
            {
                xAOD::TruthParticle *xTruthParticle = new xAOD::TruthParticle();
                xTruthParticleContainerMuon->push_back( xTruthParticle );

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
