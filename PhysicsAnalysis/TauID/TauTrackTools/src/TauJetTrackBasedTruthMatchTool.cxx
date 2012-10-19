/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
//   Implementation file for class TauJetTrackBasedTruthMatchTool
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Tool to match TauSeeds to TruthTaus
///////////////////////////////////////////////////////////////////
// sebastian.fleischmann@cern.ch
///////////////////////////////////////////////////////////////////

#include "TauTrackTools/TauJetTrackBasedTruthMatchTool.h"
#include "TauTrackTools/TrackBasedTruthMatchUtility.h"
#include "McParticleEvent/TruthParticleContainer.h"
#include "JetEvent/JetCollection.h"
#include "JetEvent/Jet.h"
#include "tauEvent/TauJetContainer.h"
#include "tauEvent/TauJet.h"
#include "TauTrackEvent/TauJetTruthMap.h"
#include "ParticleTruth/TrackParticleTruth.h"
#include "ParticleTruth/TrackParticleTruthKey.h"
#include "ParticleTruth/TrackParticleTruthCollection.h"
#include "GeneratorObjects/HepMcParticleLink.h"
#include "TauTrackEvent/TruthTau.h"

TauID::TauJetTrackBasedTruthMatchTool::TauJetTrackBasedTruthMatchTool(
    const std::string& ty,
    const std::string& na,
    const IInterface* pa ) :
        AthAlgTool(ty,na,pa),
        m_matchMaxDR(0.2),
        m_maxAbsEta(2.5),
        m_minPt(5.*CLHEP::GeV),
        m_minTrackMatchProbability(0.5),
        m_inputTrackParticleTruthCollectionName("TrackParticleTruthCollection"),
        m_inputTrackParticleContainerName("TrackParticleCandidate"),
        m_doTruthJetMatching(false),
        m_inputTruthJetContainerName("Cone4TruthJets"),
        m_truthMatchUtil(0) {
    declareInterface<ITauJetTruthMatchTool>(this);

    declareProperty("MatchMaxDR", m_matchMaxDR, "maximum delta R for match");
    declareProperty("MaxAbsEta",  m_maxAbsEta,  "maximum abs(eta)");
    declareProperty("MinPt",    m_minPt, "minimum pT");
    declareProperty("MinTrackMatchProb",            m_minTrackMatchProbability, "minimum track match probability for matches between reco tracks and truth particles");
    declareProperty("TrackParticleContainer",       m_inputTrackParticleContainerName,  "SG key of the TrackParticleContainer used as input for TauJets");
    declareProperty("TrackParticleTruthCollection", m_inputTrackParticleTruthCollectionName,    "SG key of input TrackParticle truth collection");
    declareProperty("DoTruthJetMatching",           m_doTruthJetMatching,           "match fake jets to truth jets?");
    declareProperty("TruthJetContainer",            m_inputTruthJetContainerName,   "SG key of input TruthJet container (needed for matching of fake jets to TruthJets)");

}

TauID::TauJetTrackBasedTruthMatchTool::~TauJetTrackBasedTruthMatchTool() {
}

StatusCode TauID::TauJetTrackBasedTruthMatchTool::initialize() {

    StatusCode sc = AlgTool::initialize();

    m_truthMatchUtil = new TrackBasedTruthMatchUtility(m_matchMaxDR, m_maxAbsEta, m_minPt, m_minTrackMatchProbability, msg());

    return sc;

}


StatusCode TauID::TauJetTrackBasedTruthMatchTool::finalize() {
    StatusCode sc = AlgTool::finalize();
    delete m_truthMatchUtil;
    return sc;
}


const TauID::TauJetTruthMap* TauID::TauJetTrackBasedTruthMatchTool::matchCandidatesToTruth(
    const TruthParticleContainer& truthTauCont,
    const Analysis::TauJetContainer& tauCandColl) const {

    const DataVector<const TauID::TruthTau>* selectedTruthTaus = m_truthMatchUtil->selectTruthTaus(truthTauCont);

    const Rec::TrackParticleContainer* trackPartCont = 0;
    const TrackParticleTruthCollection* trackTruthParticles = 0;
    if (selectedTruthTaus->size()>0) {
        /** get the track particle container from StoreGate */
        StatusCode sc = evtStore()->retrieve(trackPartCont, m_inputTrackParticleContainerName);
        if ( !trackPartCont || sc.isFailure() ) {
            ATH_MSG_ERROR( " Track particle container (" << m_inputTrackParticleContainerName << ") not found in StoreGate" );
            delete selectedTruthTaus;
            return 0;
        }
        /** get the track particle truth container (for track to truth link) from StoreGate */
        sc = evtStore()->retrieve(trackTruthParticles, m_inputTrackParticleTruthCollectionName);
        if ( !trackTruthParticles || sc.isFailure() ) {
            ATH_MSG_ERROR( " Track Truth particle container (" << m_inputTrackParticleTruthCollectionName << ") not found in StoreGate" );
            delete selectedTruthTaus;
            return 0;
        }
    }
    /** get the MC truth jets container from StoreGate */
    const JetCollection* truthJetCont=0;
    if (m_doTruthJetMatching) {
        StatusCode sc = evtStore()->retrieve(truthJetCont, m_inputTruthJetContainerName);
        if ( !truthJetCont || sc.isFailure() ) {
            ATH_MSG_ERROR( "Truth jet container ("<< m_inputTruthJetContainerName << ") not found in StoreGate" );
            delete selectedTruthTaus;
            return 0;
        }
    }

    Analysis::TauJetContainer::const_iterator tauJetItr  = tauCandColl.begin();
    Analysis::TauJetContainer::const_iterator tauJetItrE = tauCandColl.end();

    BarcodeIndexMap barcodeSeedMap;
    std::vector<unsigned int> nTracksInTauJet(tauCandColl.size(), 0);
    DataVector<const I4Momentum> tauJets(SG::VIEW_ELEMENTS);
    // loop over all Seeds and get their reconstructed tracks
    // (tracks are not needed, if no truth taus were selected, because nothing to match)
    // reconstructed tracks are used to get the barcode of the corresponding truth particle
    unsigned int tauJetIndex = 0;
    for (; tauJetItr != tauJetItrE; ++tauJetItr) {
        if (*tauJetItr) {
            ATH_MSG_VERBOSE( "Seed " << tauJetIndex << ": eta=" << (*tauJetItr)->eta() << " phi=" << (*tauJetItr)->phi() << " pT=" << (*tauJetItr)->pt() );
        } else {
            //tauJetIndex++;
            //tauJets.push_back(*tauJetItr);
            continue;
        }
        tauJets.push_back(*tauJetItr);
        if (selectedTruthTaus->size()>0) {
            // get reconstructed tracks used in tauJet:
            if ( (*tauJetItr)->numTrack() < 1) {
                    ATH_MSG_INFO( "TauJet has no associated tracks. Truth matching will be done on Delta R and may be inaccurate" );
            }
            // FIXME: Use const ElementLinkVector< Rec::TrackParticleContainer > TauJet::trackLinkVector() 
            // to get the TrackParticleContainer directly from the TauJet.
            for (unsigned int trackIndex = 0; trackIndex < (*tauJetItr)->numTrack(); trackIndex++){
                const Rec::TrackParticle* track = (*tauJetItr)->track(trackIndex);
                if (!track) {
                    ATH_MSG_WARNING( "Could not get track from TauJet, ignore this track!" );
                    continue;
                }
                ATH_MSG_DEBUG( "TrackParticle: pT=" << track->pt() << "\t eta=" << track->eta() << "\t phi=" << track->phi() );
                const HepMC::GenParticle* genPart = m_truthMatchUtil->getTruth(track, trackTruthParticles, trackPartCont);
                if (genPart) {
                    const int barcode = genPart->barcode();
                    ATH_MSG_DEBUG( "Barcode assigned to TrackParticle: " << barcode );
                    barcodeSeedMap.insert(std::make_pair(barcode, tauJetIndex ));
                    nTracksInTauJet[tauJetIndex]++;
                }
            }// end for (tracks)
        } // end if if (selectedTruthTaus.size()>0)
        tauJetIndex++;
    } // end for (tauJetIter)

    // create output map:
    TauJetTruthMap* candTruthMap = new TauJetTruthMap();

    const std::multimap<const I4Momentum*, const TauID::TruthTau*>* truthMap = m_truthMatchUtil->matchToTruth(
            *selectedTruthTaus,
            barcodeSeedMap,
            tauJets,
            nTracksInTauJet,
            truthJetCont );

    std::multimap<const I4Momentum*, const TauID::TruthTau*>::const_iterator truthMapIter = truthMap->begin();
    for ( ; truthMapIter != truthMap->end(); truthMapIter++) {
        candTruthMap->insert(std::make_pair(dynamic_cast<const Analysis::TauJet*>((*truthMapIter).first), (*truthMapIter).second));
    }
    delete truthMap;
    delete selectedTruthTaus;

    return candTruthMap;
}
