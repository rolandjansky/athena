/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
//   Implementation file for class TauJetTruthMatchTool
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Tool to match TauSeeds to TruthTaus
///////////////////////////////////////////////////////////////////
// sebastian.fleischmann@cern.ch
///////////////////////////////////////////////////////////////////

#include "TauTrackTools/TauJetTruthMatchTool.h"
#include "McParticleEvent/TruthParticleContainer.h"
#include "JetEvent/JetCollection.h"
#include "JetEvent/Jet.h"
#include "tauEvent/TauJetContainer.h"
#include "tauEvent/TauJet.h"
#include "TauTrackEvent/TauJetTruthMap.h"
#include "TauTrackEvent/TruthTau.h"

TauID::TauJetTruthMatchTool::TauJetTruthMatchTool(
    const std::string& ty,
    const std::string& na,
    const IInterface* pa ) :
        AthAlgTool(ty,na,pa),
        m_matchMaxDR(0.2),
        m_maxAbsEta(2.5),
        m_minPt(5.*CLHEP::GeV),
        m_doTruthJetMatching(false),
        m_inputTruthJetContainerName("Cone4TruthJets") {
    declareInterface<ITauJetTruthMatchTool>(this);

    declareProperty("MatchMaxDR", m_matchMaxDR, "maximum delta R for match");
    declareProperty("MaxAbsEta",  m_maxAbsEta,  "maximum abs(eta)");
    declareProperty("MinPt", m_minPt, "minimum pT");
    declareProperty("DoTruthJetMatching",           m_doTruthJetMatching,           "match fake jets to truth jets?");
    declareProperty("TruthJetContainer",            m_inputTruthJetContainerName,   "SG key of input TruthJet container (needed for matching of fake jets to TruthJets)");

}

TauID::TauJetTruthMatchTool::~TauJetTruthMatchTool() {
}

StatusCode TauID::TauJetTruthMatchTool::initialize() {

    StatusCode sc = AlgTool::initialize();
    return sc;

}


StatusCode TauID::TauJetTruthMatchTool::finalize() {
    StatusCode sc = AlgTool::finalize();
    return sc;
}


const TauID::TauJetTruthMap* TauID::TauJetTruthMatchTool::matchCandidatesToTruth(
    const TruthParticleContainer& truthTauCont,
    const Analysis::TauJetContainer& tauCandColl ) const {

    // create output map:
    TauJetTruthMap* candTruthMap = new TauJetTruthMap();

    /** get the MC truth jets container from StoreGate */
    const JetCollection* truthJetCont=0;
    if (m_doTruthJetMatching) {
        StatusCode sc = evtStore()->retrieve(truthJetCont, m_inputTruthJetContainerName);
        if ( !truthJetCont || sc.isFailure() ) {
            ATH_MSG_ERROR( "Truth jet container ("<< m_inputTruthJetContainerName << ") not found in StoreGate" );
            return 0;
        }
    }

    /** Iterate over list of MC particles to find seeds matching to truth taus */
    TruthParticleContainer::const_iterator mcpartItr  = truthTauCont.begin();
    TruthParticleContainer::const_iterator mcpartItrE = truthTauCont.end();

    Analysis::TauJetContainer::const_iterator candItr  = tauCandColl.begin();
    Analysis::TauJetContainer::const_iterator candItrE = tauCandColl.end();

    //loop over MC particles
    for (; mcpartItr != mcpartItrE; ++mcpartItr) {

        // use TruthTaus only
        const TauID::TruthTau* truthTau = dynamic_cast<const TauID::TruthTau*>(*mcpartItr);
        if (!truthTau) continue;

        const CLHEP::HepLorentzVector& visTrueMom = truthTau->hlvVis();
        // ignore, if outside kinematic range
        if (fabs(visTrueMom.eta()) > m_maxAbsEta || visTrueMom.perp() < m_minPt) continue;

        // find matching TauSeed:
        candItr  = tauCandColl.begin();
        double minDR = 99999999.;
        const Analysis::TauJet* matchedCand = 0;

        for (; candItr != candItrE; ++candItr) {
            double dR = visTrueMom.deltaR( (*candItr)->hlv() );
            if (dR < minDR) {
                minDR = dR;
                matchedCand = (*candItr);
            }
        } // end for(candItr)
        if (minDR > m_matchMaxDR) {
            matchedCand = 0;
        }
        if (matchedCand) {
            // check if already inserted:
            if (candTruthMap->find(matchedCand) != candTruthMap->end()) {
                ATH_MSG_WARNING( "matched candidate already in map: ");
                // do not insert twice: TODO: assign to best matching truth tau
                matchedCand = 0;
            }
        }
        candTruthMap->insert(std::make_pair(matchedCand, truthTau));
    }// end for(mcpartItr)


    /** Iterate over list of seeds to find seeds not assigned to any truth tau */
    candItr  = tauCandColl.begin();
    candItrE = tauCandColl.end();

    JetCollection::const_iterator mcjetItr;
    JetCollection::const_iterator mcjetItrE;

    if (truthJetCont) {
        mcjetItr  = truthJetCont->begin();
        mcjetItrE = truthJetCont->end();
    }

    // loop over seeds
    const TauID::TruthTau* truthTau = 0;
    for (; candItr != candItrE; ++candItr) {
        // is seed already in the map?
        if (candTruthMap->find(*candItr) == candTruthMap->end()) {
            if (truthJetCont) {
                //find matching TruthJet
                mcjetItr = truthJetCont->begin();
                double minDR = 99999999.;
                //const Analysis::TauJet* matchedFake = 0;
                for (; mcjetItr != mcjetItrE; ++mcjetItr) {
                    const CLHEP::HepLorentzVector& mcJet = (*mcjetItr)->hlv();
                    //ignore if mc jet outside kinematic range
                    if ( fabs(mcJet.eta()) > m_maxAbsEta || mcJet.perp() < m_minPt ) continue;
                    double dR = mcJet.deltaR( (*candItr)->hlv() );
                    if (dR < minDR ) {
                        minDR = dR;
                        //matchedFake = (*candItr);
                    }
                }// end for(mcjetItr)
                if (minDR < m_matchMaxDR) {
                    // insert into map with NULL TruthTau only if matched to truthJet
                    candTruthMap->insert(std::make_pair( (*candItr), truthTau ));
                }
            } else {
                // check, if outside of considered range based on reconstructed momenta:
                if ( (fabs((*candItr)->eta()) > m_maxAbsEta) || ((*candItr)->pt() < m_minPt) ) continue;
                // insert into map with NULL TruthTau
                candTruthMap->insert(std::make_pair( (*candItr), truthTau ));
            }
        }
    } // end for (candItr)

    return candTruthMap;
}
