/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
//   Implementation file for class TauSeedTrackBasedTruthMatchTool
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Tool to match TauSeeds to TruthTaus
///////////////////////////////////////////////////////////////////
// sebastian.fleischmann@cern.ch
///////////////////////////////////////////////////////////////////

#include "TauTrackTools/TauSeedTrackBasedTruthMatchTool.h"
#include "TauTrackTools/TrackBasedTruthMatchUtility.h"

#include "StoreGate/StoreGateSvc.h"
#include "McParticleEvent/TruthParticleContainer.h"
#include "JetEvent/JetCollection.h"
#include "PanTauEvent/TauSeed.h"
#include "PanTauEvent/TauSeedCollection.h"
#include "JetEvent/Jet.h"
#include "PanTauEvent/SeedTruthMap.h"
#include "PanTauEvent/CorrectedEflowMomentum.h"
#include "ParticleTruth/TrackParticleTruth.h"
#include "ParticleTruth/TrackParticleTruthKey.h"
#include "ParticleTruth/TrackParticleTruthCollection.h"
#include "GeneratorObjects/HepMcParticleLink.h"
#include "eflowEvent/eflowObject.h"
#include "TauTrackEvent/TruthTau.h"

TauID::TauSeedTrackBasedTruthMatchTool::TauSeedTrackBasedTruthMatchTool(
    const std::string& ty,
    const std::string& na,
    const IInterface* pa ) :
        AthAlgTool(ty,na,pa),
        m_storeGate(0),
        m_matchMaxDR(0.2),
        m_maxAbsEta(2.5),
        m_minPt(2.*CLHEP::GeV),
        m_minTrackMatchProbability(0.5),
        m_inputTrackParticleTruthCollectionName("TrackParticleTruthCollection"),
        m_inputTrackParticleContainerName("TrackParticleCandidate"),
        m_doTruthJetMatching(true),
        m_inputTruthJetContainerName("Cone4TruthJets"),
        m_truthMatchUtil(0) {
    declareInterface<ITauSeedTruthMatchTool>(this);

    declareProperty("MatchMaxDR", m_matchMaxDR, "maximum delta R for match");
    declareProperty("MaxAbsEta",  m_maxAbsEta,  "maximum abs(eta)");
    declareProperty("MinPt", m_minPt, "minimum pT");
    declareProperty("MinTrackMatchProb",            m_minTrackMatchProbability, "minimum track match probability for matches between reco tracks and truth particles");
    declareProperty("TrackParticleContainer",       m_inputTrackParticleContainerName,  "SG key of the TrackParticleContainer used as input for TauSeeds");
    declareProperty("TrackParticleTruthCollection", m_inputTrackParticleTruthCollectionName,    "SG key of input TrackParticle truth collection");
    declareProperty("DoTruthJetMatching",           m_doTruthJetMatching,           "match fake jets to truth jets?");
    declareProperty("TruthJetContainer",            m_inputTruthJetContainerName,   "SG key of input TruthJet container (needed for matching of fake jets to TruthJets)");

}

TauID::TauSeedTrackBasedTruthMatchTool::~TauSeedTrackBasedTruthMatchTool() {
}

StatusCode TauID::TauSeedTrackBasedTruthMatchTool::initialize() {

    StatusCode sc = AthAlgTool::initialize();
    ATH_MSG_INFO( name() << " initialize()" );

    m_truthMatchUtil = new TauID::TrackBasedTruthMatchUtility(m_matchMaxDR, m_maxAbsEta, m_minPt, m_minTrackMatchProbability, msg());

    // retrieve the StoreGate Service
    sc = service("StoreGateSvc",m_storeGate);
    if (sc.isFailure())
        ATH_MSG_ERROR( "Could not retrieve StoreGateSvc!" );
    else
        ATH_MSG_INFO( "StoreGateSvc retrieved!" );

    return sc;

}


StatusCode TauID::TauSeedTrackBasedTruthMatchTool::finalize() {
    StatusCode sc = AlgTool::finalize();
    delete m_truthMatchUtil;
    return sc;
}

const PanTau::SeedTruthMap* TauID::TauSeedTrackBasedTruthMatchTool::matchSeedsToTruth(
    const TruthParticleContainer& truthTauCont,
    const PanTau::TauSeedCollection& tauSeedColl ) const {

    const DataVector<const TauID::TruthTau>* selectedTruthTaus = m_truthMatchUtil->selectTruthTaus(truthTauCont);

    const Rec::TrackParticleContainer* trackPartCont = 0;
    const TrackParticleTruthCollection* trackTruthParticles = 0;
    if (selectedTruthTaus->size()>0) {
        /** get the track particle container from StoreGate */
        StatusCode sc = m_storeGate->retrieve(trackPartCont, m_inputTrackParticleContainerName);
        if ( !trackPartCont || sc.isFailure() ) {
            ATH_MSG_ERROR( " Track particle container (" << m_inputTrackParticleContainerName << ") not found in StoreGate" );
            delete selectedTruthTaus;
            return 0;
        }
        /** get the track particle truth container (for track to truth link) from StoreGate */
        sc = m_storeGate->retrieve(trackTruthParticles, m_inputTrackParticleTruthCollectionName);
        if ( !trackTruthParticles || sc.isFailure() ) {
            ATH_MSG_ERROR( " Track Truth particle container (" << m_inputTrackParticleTruthCollectionName << ") not found in StoreGate" );
            delete selectedTruthTaus;
            return 0;
        }
    }

    /** get the MC truth jets container from StoreGate */
    const JetCollection* truthJetCont=0;
    if (m_doTruthJetMatching) {
        StatusCode sc = m_storeGate->retrieve(truthJetCont, m_inputTruthJetContainerName);
        if ( !truthJetCont || sc.isFailure() ) {
            ATH_MSG_ERROR( "Truth jet container ("<< m_inputTruthJetContainerName << ") not found in StoreGate" );
            delete selectedTruthTaus;
            return 0;
        }
    }

    PanTau::TauSeedCollection::const_iterator seedItr  = tauSeedColl.begin();
    PanTau::TauSeedCollection::const_iterator seedItrE = tauSeedColl.end();

    TauID::BarcodeIndexMap barcodeSeedMap;
    std::vector<unsigned int> nTracksInSeed(tauSeedColl.size(), 0);
    DataVector<const I4Momentum> seeds(SG::VIEW_ELEMENTS);
    // loop over all Seeds and get their reconstructed tracks
    // reconstructed tracks are used to get the barcode of the corresponding truth particle
    unsigned int seedIndex = 0;
    for (; seedItr != seedItrE; ++seedItr) {
        if (*seedItr) {
            ATH_MSG_VERBOSE( "Seed " << seedIndex << ": eta=" << (*seedItr)->eta() << " phi=" << (*seedItr)->phi() << " pT=" << (*seedItr)->pt() );
        } else {
            //seedIndex++;
            //seeds.push_back(*seedItr);
            continue;
        }
        seeds.push_back(*seedItr);
        if (selectedTruthTaus->size()>0) {
            // get reconstructed tracks used in seed:
            DataVector<PanTau::CorrectedEflowMomentum>::const_iterator chargedEflowIter = (*seedItr)->chargedEflowMomenta().begin();
            for (; chargedEflowIter != (*seedItr)->chargedEflowMomenta().end(); chargedEflowIter++){
                const eflowObject* efo = (*chargedEflowIter)->originalEflowObject();
                //for (unsigned int trackIndex = 0; trackIndex<efo->numTrack(); trackIndex++)
                if (efo->numTrack() > 1) {
                    ATH_MSG_WARNING( "Eflow object contains more than one track, use the first one only!" );
                }
                if (efo->numTrack() < 1) {
                    ATH_MSG_WARNING( "Charged eflow object contains no track, ignore it!" );
                    continue;
                }
                const Rec::TrackParticle* track = efo->track(0);
                //eflowTrack_iterator trackIter = efo->track_begin();
                if (!track) {
                    ATH_MSG_WARNING( "Could not get track from eflow object, ignore it!" );
                    continue;
                }
                ATH_MSG_DEBUG( "TrackParticle: pT=" << track->pt() << "\t eta=" << track->eta() << "\t phi=" << track->phi()  << " eflow type=" << efo->eflowType() ); //<< (efo->conversion() ? " conversion!" : "")
                const HepMC::GenParticle* genPart = m_truthMatchUtil->getTruth(track, trackTruthParticles, trackPartCont);
                if (genPart) {
                    const int barcode = genPart->barcode();
                    ATH_MSG_DEBUG( "Barcode assigned to TrackParticle: " << barcode << " PDG ID: " << genPart->pdg_id() );
                    //barcodeSeedMap.insert(std::make_pair(barcode, (*seedItr) ));
                    barcodeSeedMap.insert(std::make_pair(barcode, seedIndex ));
                    nTracksInSeed[seedIndex]++;
                }
            }// end for (chargedEflowIter)
        } // end if (selectedTruthTaus->size()>0)
        seedIndex++;
    } // end for (seedIter)

    // create output map:
    PanTau::SeedTruthMap* seedTruthMap = new PanTau::SeedTruthMap();

    const std::multimap<const I4Momentum*, const TauID::TruthTau*>* truthMap = m_truthMatchUtil->matchToTruth(
            *selectedTruthTaus,
            barcodeSeedMap,
            seeds,
            nTracksInSeed,
            truthJetCont );

    std::multimap<const I4Momentum*, const TauID::TruthTau*>::const_iterator truthMapIter = truthMap->begin();
    for ( ; truthMapIter != truthMap->end(); truthMapIter++) {
//         const I4Momentum* seed = (*truthMapIter).first;
//         const TauID::TruthTau* truthTau = (*truthMapIter).second;
        seedTruthMap->insert(std::make_pair(dynamic_cast<const PanTau::TauSeed*>((*truthMapIter).first), (*truthMapIter).second));
    }
    delete truthMap;
    delete selectedTruthTaus;

    return seedTruthMap;
}
