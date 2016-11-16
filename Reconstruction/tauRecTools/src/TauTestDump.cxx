/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAOD_ANALYSIS
//-----------------------------------------------------------------------------
// file:        TauTestDump.cxx
// package:     Reconstruction/tauRec
// authors:     Felix Friedrich
// date:        2012-11-05
// 
//-----------------------------------------------------------------------------

//#include <GaudiKernel/IToolSvc.h>
//#include <GaudiKernel/ListItem.h>

//#include "FourMomUtils/P4Helpers.h"
//#include "FourMom/P4EEtaPhiM.h"
//#include "CLHEP/Vector/LorentzVector.h"
//#include "Particle/TrackParticle.h"


#include "tauRecTools/TauEventData.h"
//#include "tauEvent/TauCommonDetails.h"
//#include "tauEvent/TauPi0Details.h"
//#include "tauEvent/TauPi0Cluster.h"
#include "xAODTau/TauJet.h"

//#include "tauEvent/TauJetParameters.h"        

#include "TauTestDump.h"

//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------

TauTestDump::TauTestDump(const std::string &name ) :
TauRecToolBase(name) {
}

//-----------------------------------------------------------------------------
// Destructor
//-----------------------------------------------------------------------------

TauTestDump::~TauTestDump() {
}


//-----------------------------------------------------------------------------
// Initializer
//-----------------------------------------------------------------------------

StatusCode TauTestDump::initialize() {
    return StatusCode::SUCCESS;
}


//-----------------------------------------------------------------------------
// Finalizer
//-----------------------------------------------------------------------------

StatusCode TauTestDump::finalize() {
    return StatusCode::SUCCESS;
}


//-----------------------------------------------------------------------------
// Execution
//-----------------------------------------------------------------------------
StatusCode TauTestDump::execute(xAOD::TauJet& pTau) {

    ATH_MSG_INFO("=== TAU TEST DUMP BEGIN ==================== ");
  
    ATH_MSG_INFO("Tau recorded with: pt="<< pTau.pt() <<", eta=" <<pTau.eta()<<", phi="<<pTau.phi());
    float tfFlightPathSig;
    float ipSigLeadTrk;
    pTau.detail(xAOD::TauJetParameters::ipSigLeadTrk, ipSigLeadTrk);
    pTau.detail(xAOD::TauJetParameters::trFlightPathSig,tfFlightPathSig );
    ATH_MSG_INFO(" numTrack="<<pTau.nTracks() << ", tfFlightPathSig=" << tfFlightPathSig <<", ipSigLeadTrk=" << ipSigLeadTrk );
    
    //stop here
    return StatusCode::SUCCESS;

    /* Unreachable due to the above return statement. Disabled to shut coverity up.
    if (pTau.nTracks() != 1) {
      // Pi0 calculated only for 1p taus --> leave test case
      ATH_MSG_INFO("Pi0 calculated only for 1p taus --> leave test case");
      return StatusCode::SUCCESS;
    }

    // Default PFO pi0
    ATH_MSG_INFO("dumping pi0 standard");
    if (pTau.nPi0PFOs()>0) {
	    for (unsigned int i=0; i<pTau.nPi0PFOs();++i) ATH_MSG_INFO(pTau.pi0PFO(i)->e()<< " ");
    }
    else ATH_MSG_INFO("no pi0 cand");

    // Cell-based PFO pi0
    ATH_MSG_INFO("dumping pi0 cell-based");
    if (pTau.nProtoPi0PFOs()>0) {
	    for (unsigned int i=0; i<pTau.nProtoPi0PFOs();++i) ATH_MSG_INFO(pTau.protoPi0PFO(i)->e()<< " ");
    }
    else ATH_MSG_INFO("no pi0 cand");

    ATH_MSG_INFO("end dumping pi0");
    
    ATH_MSG_INFO("=== TAU TEST DUMP END ==================== ");

    return StatusCode::SUCCESS;
    */
}


#endif
