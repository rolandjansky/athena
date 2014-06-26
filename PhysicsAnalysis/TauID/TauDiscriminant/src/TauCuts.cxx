/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TauDiscriminant/TauCuts.h"
#include "xAODTau/TauJetContainer.h"
#include "xAODTau/TauJet.h"
// #include "tauEvent/TauCommonDetails.h"
// #include "tauEvent/TauPID.h"
// #include "tauEvent/TauJetParameters.h"
#include "xAODTau/TauDefs.h"
#include <PathResolver/PathResolver.h>

using namespace xAOD;
//-----------------------------------------------------------------------------
// Initializer
//-----------------------------------------------------------------------------
StatusCode TauCuts::prepare(const TauDetailsManager& manager)
{
    m_cutsManager = new MethodCuts("TauCuts");
    m_cutsManager->setDetails(manager);

    string cutsPath = PathResolver::find_file(m_fileName, "DATAPATH");

    if ( !m_cutsManager->build(cutsPath) ) {
        ATH_MSG_FATAL("unable to build cuts manager");
        return StatusCode::FAILURE;
    }

    return StatusCode::SUCCESS;
}

//-----------------------------------------------------------------------------
// Finalizer
//-----------------------------------------------------------------------------
StatusCode TauCuts::finalize()
{
    delete m_cutsManager;
    return StatusCode::SUCCESS;
}

//-----------------------------------------------------------------------------
// Execution
//-----------------------------------------------------------------------------
StatusCode TauCuts::execute(xAOD::TauJet* tau, FakeTauBits* /*bits*/, FakeTauScores* /*scores*/)
{
    bool loose(false), medium(false), tight(false);

    if (tau->nTracks() > 0)
    {
        loose = m_cutsManager->response(0);
        medium = m_cutsManager->response(1);
        tight = m_cutsManager->response(2); 
    }

//     Analysis::TauPID *p_tauid = tau->tauID();

    tau->setIsTau(TauJetParameters::TauCutLoose, loose);
    tau->setIsTau(TauJetParameters::TauCutMedium, medium);
    tau->setIsTau(TauJetParameters::TauCutTight, tight);
    
    // Verbose messaging for extra info
    if (msgLvl(MSG::VERBOSE))
    {
        if (!((!loose && !medium && !tight) || (loose && !medium && !tight) || (loose && medium && !tight) || (loose && medium && tight)))
        {
            msg(MSG::VERBOSE) << "Bad cuts!" << endreq;
        }
        msg(MSG::VERBOSE) << "TauCutLoose: " << tau->isTau(TauJetParameters::TauCutLoose) << endreq;
        msg(MSG::VERBOSE) << "TauCutMedium: " << tau->isTau(TauJetParameters::TauCutMedium) << endreq;
        msg(MSG::VERBOSE) << "TauCutTight: " << tau->isTau(TauJetParameters::TauCutTight) << endreq;
    }
   
    return StatusCode::SUCCESS;
} 
