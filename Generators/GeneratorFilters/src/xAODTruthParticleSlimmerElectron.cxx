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

#include "GeneratorFilters/xAODTruthParticleSlimmerElectron.h"

xAODTruthParticleSlimmerElectron::xAODTruthParticleSlimmerElectron(const std::string &name, ISvcLocator *svcLoc)
    : AthAlgorithm(name, svcLoc)
{
    declareProperty("xAODTruthParticleContainerName", m_xaodTruthParticleContainerName = "TruthParticles");
    declareProperty("xAODTruthParticleContainerNameElectron", m_xaodTruthParticleContainerNameElectron = "TruthElectrons");
    declareProperty("el_pt_selection", m_el_pt_selection = 1. * Gaudi::Units::GeV); //User provides units in MeV!
    declareProperty("abseta_selection", m_abseta_selection = 5.);
}

StatusCode xAODTruthParticleSlimmerElectron::initialize()
{
    ATH_MSG_INFO("xAOD input TruthParticleContainer name = " << m_xaodTruthParticleContainerName);
    ATH_MSG_INFO("xAOD output TruthParticleContainerElectron name = " << m_xaodTruthParticleContainerNameElectron);
    return StatusCode::SUCCESS;
}

StatusCode xAODTruthParticleSlimmerElectron::execute()
{
    // If the containers already exists then assume that nothing needs to be done
    if (evtStore()->contains<xAOD::TruthParticleContainer>(m_xaodTruthParticleContainerNameElectron))
    {
        ATH_MSG_WARNING("xAOD Electron Truth Particles are already available in the event");
        return StatusCode::SUCCESS;
    }

    // Create new output container
    xAOD::TruthParticleContainer *xTruthParticleContainerElectron = new xAOD::TruthParticleContainer();
    CHECK(evtStore()->record(xTruthParticleContainerElectron, m_xaodTruthParticleContainerNameElectron));
    xAOD::TruthParticleAuxContainer *xTruthParticleAuxContainerElectron = new xAOD::TruthParticleAuxContainer();
    CHECK(evtStore()->record(xTruthParticleAuxContainerElectron, m_xaodTruthParticleContainerNameElectron + "Aux."));
    xTruthParticleContainerElectron->setStore(xTruthParticleAuxContainerElectron);
    ATH_MSG_INFO("Recorded TruthParticleContainerElectron with key: " << m_xaodTruthParticleContainerNameElectron);

    // Retrieve full TruthParticle container
    const xAOD::TruthParticleContainer *xTruthParticleContainer;
    if (evtStore()->retrieve(xTruthParticleContainer, m_xaodTruthParticleContainerName).isFailure())
    {
        ATH_MSG_ERROR("No TruthParticle collection with name " << m_xaodTruthParticleContainerName << " found in StoreGate!");
        return StatusCode::FAILURE;
    }

    // Set up decorators if needed

    // Loop over full TruthParticle container
    unsigned int nParticles = xTruthParticleContainer->size();
    for (unsigned int iPart = 0; iPart < nParticles; ++iPart)
    {
        const xAOD::TruthParticle *theParticle = (*xTruthParticleContainer)[iPart];

        int this_absPdgID = theParticle->absPdgId();
        float this_abseta = theParticle->abseta();
        float this_pt = theParticle->pt();
        int this_status = theParticle->status();

        //Save Electrons above 1 GeV, & within detector acceptance (4.5)
        if (this_status == 1 && this_absPdgID == 11 && this_pt >= m_el_pt_selection && this_abseta < m_abseta_selection)
        {
            xAOD::TruthParticle *xTruthParticle = new xAOD::TruthParticle();
            xTruthParticleContainerElectron->push_back( xTruthParticle );

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

    return StatusCode::SUCCESS;
}
