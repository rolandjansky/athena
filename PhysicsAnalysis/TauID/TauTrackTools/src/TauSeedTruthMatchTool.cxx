/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
//   Implementation file for class TauSeedTruthMatchTool
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Tool to match TauSeeds to TruthTaus
///////////////////////////////////////////////////////////////////
// sebastian.fleischmann@cern.ch
///////////////////////////////////////////////////////////////////

#include "TauTrackTools/TauSeedTruthMatchTool.h"
#include "StoreGate/StoreGateSvc.h"
#include "McParticleEvent/TruthParticleContainer.h"
#include "JetEvent/JetCollection.h"
#include "PanTauEvent/TauSeedCollection.h"
#include "JetEvent/Jet.h"
#include "PanTauEvent/TauSeed.h"
#include "PanTauEvent/TauSeedCollection.h"
#include "PanTauEvent/SeedTruthMap.h"
#include "TauTrackEvent/TruthTau.h"

TauID::TauSeedTruthMatchTool::TauSeedTruthMatchTool(
    const std::string& ty,
    const std::string& na,
    const IInterface* pa ) :
        AthAlgTool(ty,na,pa),
        m_storeGate(0),
        m_matchMaxDR(0.2),
        m_maxAbsEta(2.5),
        m_minPt(2.*CLHEP::GeV),
        m_doTruthJetMatching(true),
        m_inputTruthJetContainerName("Cone4TruthJets") {
    declareInterface<ITauSeedTruthMatchTool>(this);

    declareProperty("MatchMaxDR", m_matchMaxDR, "maximum delta R for match");
    declareProperty("MaxAbsEta",  m_maxAbsEta,  "maximum abs(eta)");
    declareProperty("MinPt", m_minPt, "minimum pT");
    declareProperty("DoTruthJetMatching",           m_doTruthJetMatching,           "match fake jets to truth jets?");
    declareProperty("TruthJetContainer",            m_inputTruthJetContainerName,   "SG key of input TruthJet container (needed for matching of fake jets to TruthJets)");

}

TauID::TauSeedTruthMatchTool::~TauSeedTruthMatchTool() {
}

StatusCode TauID::TauSeedTruthMatchTool::initialize() {

    StatusCode sc = AthAlgTool::initialize();
    ATH_MSG_INFO( name() << " initialize()" );

    // retrieve the StoreGate Service
    sc = service("StoreGateSvc",m_storeGate);
    if (sc.isFailure())
        ATH_MSG_ERROR( "Could not retrieve StoreGateSvc!" );
    else
        ATH_MSG_INFO( "StoreGateSvc retrieved!" );

    return sc;

}


StatusCode TauID::TauSeedTruthMatchTool::finalize() {
    StatusCode sc = AlgTool::finalize();
    return sc;
}

const PanTau::SeedTruthMap* TauID::TauSeedTruthMatchTool::matchSeedsToTruth(
    const TruthParticleContainer& truthTauCont,
    const PanTau::TauSeedCollection& tauSeedColl ) const {

    ATH_MSG_DEBUG( "Starting TauSeedTruthMatchTool->matchSeedsToTruth" );
    /** get the MC truth jets container from StoreGate */
    const JetCollection* truthJetCont=0;
    if (m_doTruthJetMatching) {
        StatusCode sc = m_storeGate->retrieve(truthJetCont, m_inputTruthJetContainerName);
        if ( !truthJetCont || sc.isFailure() ) {
            ATH_MSG_ERROR( "Truth jet container ("<< m_inputTruthJetContainerName << ") not found in StoreGate" );
            return 0;
        }
    }

    // create output map:
    PanTau::SeedTruthMap* seedTruthMap = new PanTau::SeedTruthMap();

    /** Iterate over list of MC particles to find seeds matching to truth taus */
    TruthParticleContainer::const_iterator mcpartItr  = truthTauCont.begin();
    TruthParticleContainer::const_iterator mcpartItrE = truthTauCont.end();

    PanTau::TauSeedCollection::const_iterator seedItr  = tauSeedColl.begin();
    PanTau::TauSeedCollection::const_iterator seedItrE = tauSeedColl.end();

    //loop over MC particles
    for (; mcpartItr != mcpartItrE; ++mcpartItr) {
        // use TruthTaus only
        const TauID::TruthTau* truthTau = dynamic_cast<const TauID::TruthTau*>(*mcpartItr);
        if (!truthTau) continue;
        const CLHEP::HepLorentzVector& visTrueMom = truthTau->hlvVis();
        ATH_MSG_VERBOSE( "Truth tau vis: eta=" << visTrueMom.eta() << " phi=" << visTrueMom.phi() << " ET=" << visTrueMom.et() << ": " << truthTau->dumpType() );
        // ignore, if outside kinematic range
        if (fabs(visTrueMom.eta()) > m_maxAbsEta || visTrueMom.perp() < m_minPt) {
            ATH_MSG_VERBOSE( "   outside kinmatic range: ignore!" );
            continue;
        }
        // find matching TauSeed:
        seedItr  = tauSeedColl.begin();
        double minDR = 99999999.;
        const PanTau::TauSeed* matchedSeed = 0;
        for (; seedItr != seedItrE; ++seedItr) {
            double dR = visTrueMom.deltaR( (*seedItr)->hlv() );
            if (dR < minDR) {
                minDR = dR;
                matchedSeed = (*seedItr);
            }
        } // end for(seedItr)
        if (minDR > m_matchMaxDR) {
            matchedSeed = 0;
        }
        if (matchedSeed) {
            // check if already inserted:
            if (seedTruthMap->find(matchedSeed) != seedTruthMap->end()) {
                ATH_MSG_WARNING( "matched seed already in map: "<< matchedSeed->dumpType() );
                // do not insert twice: TODO: assign to best matching truth tau
                matchedSeed = 0;
            } else {
                ATH_MSG_VERBOSE( "matched to seed: eta=" << matchedSeed->eta() << " phi=" << matchedSeed->phi() << " pT=" << matchedSeed->pt() );
            }
        }
        seedTruthMap->insert(std::make_pair(matchedSeed, truthTau));
    }// end for(mcpartItr)


    /** Iterate over list of seeds to find seeds not assigned to any truth tau */
    seedItr  = tauSeedColl.begin();
    seedItrE = tauSeedColl.end();

    JetCollection::const_iterator mcjetItr;
    JetCollection::const_iterator mcjetItrE;

    if (truthJetCont) {
        mcjetItr  = truthJetCont->begin();
        mcjetItrE = truthJetCont->end();
    }

    // loop over seeds
    const TauID::TruthTau* truthTau = 0;
    for (; seedItr != seedItrE; ++seedItr) {
        if (*seedItr) {
            ATH_MSG_VERBOSE( "Seed: eta=" << (*seedItr)->eta() << " phi=" << (*seedItr)->phi() << " pT=" << (*seedItr)->pt() );
        }
        // is seed already in the map?
        if (seedTruthMap->find(*seedItr) == seedTruthMap->end()) {
            //find matching TruthJet
            if (truthJetCont) {
                mcjetItr = truthJetCont->begin();
                double minDR = 99999999.;
                const PanTau::TauSeed* matchedFake = 0;
                for (; mcjetItr != mcjetItrE; ++mcjetItr) {
                    const CLHEP::HepLorentzVector& mcJet = (*mcjetItr)->hlv();
                    //ignore if mc jet outside kinematic range
                    // TODO: decide: ignore jets outside kinematic range or match to all
                    // TruthJets first and do cut on match TruthJet??
                    if ( fabs(mcJet.eta()) > m_maxAbsEta || mcJet.perp() < m_minPt ) continue;
                    double dR = mcJet.deltaR( (*seedItr)->hlv() );
                    if (dR < minDR ) {
                        minDR = dR;
                        matchedFake = (*seedItr);
                    }
                }// end for(mcjetItr)
                if (minDR < m_matchMaxDR) {
                    // insert into map with NULL TruthTau only if matched to truthJet
                    seedTruthMap->insert(std::make_pair( matchedFake, truthTau ));
                }
            } else {
                // check, if outside of considered range based on reconstructed momenta:
                if (fabs((*seedItr)->eta()) > m_maxAbsEta || (*seedItr)->pt() < m_minPt ) continue;
                // insert into map with NULL TruthTau
                seedTruthMap->insert(std::make_pair( (*seedItr), truthTau ));
            } // end if (truthJetCont)
        } // end if (not already in map)
    } // end for (seedItr)

    return seedTruthMap;
}

