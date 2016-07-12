/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// FastCaloSimSvcV2.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// class header include
#include "FastCaloSimSvcV2.h"

// FastCaloSim includes
#include "ISF_FastCaloSimEvent/TFCSSimulationState.h"

/** Constructor **/
ISF::FastCaloSimSvcV2::FastCaloSimSvcV2(const std::string& name,ISvcLocator* svc) :
  BaseSimulationSvc(name, svc),
  m_epara("etest", "etest")
{
}

ISF::FastCaloSimSvcV2::~FastCaloSimSvcV2() 
{}

/** framework methods */
StatusCode ISF::FastCaloSimSvcV2::initialize()
{
    ATH_MSG_INFO(m_screenOutputPrefix << "Initializing ...");

    std::string samplename = "pions";
    TFile* file = TFile::Open(Form("/afs/cern.ch/atlas/groups/Simulation/FastCaloSimV2/secondPCA_%s_bin0.root",samplename.c_str()));
    m_epara.loadInputs(file);
    file->Close();
    delete file;

    return StatusCode::SUCCESS;
}

/** framework methods */
StatusCode ISF::FastCaloSimSvcV2::finalize()
{
    ATH_MSG_INFO(m_screenOutputPrefix << "Finalizing ...");
    return StatusCode::SUCCESS;
}

StatusCode ISF::FastCaloSimSvcV2::setupEvent()
{ 
    ATH_MSG_DEBUG(m_screenOutputPrefix << "setup Event");
    return StatusCode::SUCCESS;
}

StatusCode ISF::FastCaloSimSvcV2::releaseEvent()
{ 
    ATH_MSG_DEBUG(m_screenOutputPrefix << "release Event");
    return StatusCode::SUCCESS; 
}

/** Simulation Call */
StatusCode ISF::FastCaloSimSvcV2::simulate(const ISF::ISFParticle& isfp)
{
    ATH_MSG_INFO("FastCaloSimSvcV2 called with ISFParticle: " << isfp);

    int energybin = 0; // TODO: determine from ISFParticle energy
    ::TFCSSimulationState simulstate;
    simulstate.set_Ebin(energybin);
    m_epara.simulate(simulstate, nullptr, nullptr);

    ATH_MSG_INFO("Energy returned: " << simulstate.E());
    return StatusCode::SUCCESS;
}
