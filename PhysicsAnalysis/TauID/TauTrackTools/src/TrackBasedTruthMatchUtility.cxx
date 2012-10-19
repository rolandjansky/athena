/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
//   Implementation file for class TrackBasedTruthMatchUtility
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Tool to match TauJets to TruthTaus
///////////////////////////////////////////////////////////////////
// sebastian.fleischmann@cern.ch
///////////////////////////////////////////////////////////////////

#include "TauTrackTools/TrackBasedTruthMatchUtility.h"
#include "GaudiKernel/MsgStream.h"
#include "TauTrackEvent/TruthTau.h"
#include "McParticleEvent/TruthParticleContainer.h"
#include "JetEvent/JetCollection.h"
#include "JetEvent/Jet.h"
#include "ParticleTruth/TrackParticleTruth.h"
#include "ParticleTruth/TrackParticleTruthKey.h"
#include "ParticleTruth/TrackParticleTruthCollection.h"
#include "GeneratorObjects/HepMcParticleLink.h"

TauID::TrackBasedTruthMatchUtility::TrackBasedTruthMatchUtility(double matchMaxDR, double maxAbsEta, double minPt, double minTrackMatchProbability, MsgStream log) :
        m_log(log),
        m_matchMaxDR(matchMaxDR),
        m_maxAbsEta(maxAbsEta),
        m_minPt(minPt),
        m_minTrackMatchProbability(minTrackMatchProbability)
{
}

TauID::TrackBasedTruthMatchUtility::~TrackBasedTruthMatchUtility() {
}

const std::multimap<const I4Momentum*, const TauID::TruthTau*>* TauID::TrackBasedTruthMatchUtility::matchToTruth(
    const DataVector<const TauID::TruthTau>& selectedTruthTaus,
    const BarcodeIndexMap& barcodeSeedMap,
    const DataVector<const I4Momentum>& tauSeedColl,
    const std::vector<unsigned int>& nTracksInSeed,
    const JetCollection* truthJetCont ) const {

    DataVector<const I4Momentum>::const_iterator seedItr  = tauSeedColl.begin();
    DataVector<const I4Momentum>::const_iterator seedItrE = tauSeedColl.end();

    // create output map:
    std::multimap<const I4Momentum*, const TauID::TruthTau*>* seedTruthMap = new std::multimap<const I4Momentum*, const TauID::TruthTau*>();
    // do the matching only if at least one truth tau exists...
    if (selectedTruthTaus.size() > 0) {
        std::vector<unsigned int> nTracksInTruthTau(selectedTruthTaus.size(), 0);
        std::vector< std::vector<unsigned int> > nMatched(selectedTruthTaus.size(), std::vector<unsigned int>(tauSeedColl.size(), 0));
    
        for (unsigned int truthTauIndex = 0; truthTauIndex < selectedTruthTaus.size(); ++truthTauIndex) {
            const TauID::TruthTau* truthTau = selectedTruthTaus[truthTauIndex];
            m_log << MSG::DEBUG << "TruthTau " << truthTauIndex << " contains " << truthTau->chargedProducts().size() << " charged products" << endreq;
            // loop over charged decay products to get barcodes
            unsigned int productIndex = 0;
            std::vector<const TruthParticle*>::const_iterator tauChargedProductsIter = truthTau->chargedProducts().begin();
            for (; tauChargedProductsIter != truthTau->chargedProducts().end(); tauChargedProductsIter++) {
                productIndex++;
                if (!(*tauChargedProductsIter)) continue;
                nTracksInTruthTau[truthTauIndex]++;
                m_log << MSG::VERBOSE << "-- BC:" << (*tauChargedProductsIter)->barcode() << " PDG:" << (*tauChargedProductsIter)->pdgId() << " eta=" << (*tauChargedProductsIter)->eta() << " phi=" << (*tauChargedProductsIter)->phi() << " pT=" << (*tauChargedProductsIter)->pt() << endreq;
    //             if (m_ignorePionsWithHadronicInteractions && truthTau->type(TruthTau::HadronicInteractions)) {
    //                 if (truthTau->chargedProductHadHadronicInteraction(productIndex-1)) {
    //                     m_log << MSG::VERBOSE << "---- pion had hadronic interaction: ignore!" << endreq;
    //                     continue;
    //                 }
    //             }
                std::pair<BarcodeIndexMapIterator, BarcodeIndexMapIterator> foundRange = barcodeSeedMap.equal_range((*tauChargedProductsIter)->barcode());
                BarcodeIndexMapIterator foundBarcodesIter = foundRange.first;
                for (; foundBarcodesIter != foundRange.second; foundBarcodesIter++ ) {
                    m_log << MSG::VERBOSE << "--- matching seed:" << (*foundBarcodesIter).second << endreq;
                    nMatched[truthTauIndex][(*foundBarcodesIter).second]++;
                } // end for (foundBarcodesIter)
            } // end for (tauChargedProductsIter)
        }// end for(mcpartItr)
    
        m_log << MSG::DEBUG << "Number of matching  tracks:" << endreq;
        m_log << MSG::DEBUG << "\n    tracks in seed \t";
        for (unsigned int truthTauIndex = 0; truthTauIndex < selectedTruthTaus.size(); ++truthTauIndex) {
            m_log << "tau " << truthTauIndex << "\t";
        }
        m_log << "\n";
        for (unsigned int seedIndex = 0; seedIndex < tauSeedColl.size(); ++seedIndex) {
            m_log << "    seed " << seedIndex << ": \t" << nTracksInSeed[seedIndex] << "\t";
            for (unsigned int truthTauIndex = 0; truthTauIndex < selectedTruthTaus.size(); ++truthTauIndex) {
                m_log << nMatched[truthTauIndex][seedIndex] << "\t";
            }
            m_log << "\n";
        }
        m_log << "    tracks in tau \t";
        for (unsigned int truthTauIndex = 0; truthTauIndex < selectedTruthTaus.size(); ++truthTauIndex) {
            m_log << nTracksInTruthTau[truthTauIndex] << "\t";
        }
        m_log << "\n";
    
        // truth taus without match will be investigated again using a delta R match:
        std::vector<const TauID::TruthTau*> nonMatchedTruthTaus;
        m_log << "  matched to seed \t";
        // find best matching seed using the number of matched tracks
        // (in this loop we insert only matched truth taus, because we try delta R matching on the remaining ones):
        for (unsigned int truthTauIndex = 0; truthTauIndex < selectedTruthTaus.size(); ++truthTauIndex) {
            const TauID::TruthTau* truthTau = selectedTruthTaus[truthTauIndex];
            const I4Momentum* matchedSeed = 0;
            //if (!truthTau) continue;
            unsigned int maxNumber = 0;
            unsigned int maxIndex = 0;
            for (unsigned int seedIndex = 0; seedIndex < tauSeedColl.size(); ++seedIndex) {
                if (nMatched[truthTauIndex][seedIndex] > maxNumber) {
                    maxNumber = nMatched[truthTauIndex][seedIndex];
                    maxIndex = seedIndex;
                }
            }
            if (maxNumber>0) {
                // we have a match!
                matchedSeed = tauSeedColl[maxIndex];
                // check if already inserted:
                if (seedTruthMap->find(matchedSeed) != seedTruthMap->end()) {
                    //m_log << MSG::WARNING << "matched seed already in map: "<< matchedSeed->dumpType() << endreq;
                    m_log << MSG::WARNING << "matched seed already in map: "<< matchedSeed << endreq;
                    // do not insert twice: TODO: assign to best matching truth tau
                    matchedSeed = 0;
                    nonMatchedTruthTaus.push_back(truthTau);
                } else {
                    seedTruthMap->insert(std::make_pair(matchedSeed, truthTau));
                    m_log << maxIndex << "\t";
                    //m_log << MSG::VERBOSE << "matched to seed: eta=" << matchedSeed->eta() << " phi=" << matchedSeed->phi() << " pT=" << matchedSeed->pt() << endreq;
                }
            } else {
                m_log << "-" << "\t";
                nonMatchedTruthTaus.push_back(truthTau);
            }
        }
        m_log << "\n" << endreq;
    
        // try to find matching seed using delta R for remaining truth taus
        // important for 1-prong with hadronic interaction which often lack any reconstructed track
        // (in this loop we insert all truth taus, even those without match [in this case assigned to NULL])
        for (unsigned int truthTauIndex = 0; truthTauIndex < nonMatchedTruthTaus.size(); ++truthTauIndex) {
            const TauID::TruthTau* truthTau = nonMatchedTruthTaus[truthTauIndex];
            const CLHEP::HepLorentzVector& visTrueMom = truthTau->hlvVis();
            // find matching TauSeed using delta R:
            seedItr  = tauSeedColl.begin();
            double minDR = 99999999.;
            const I4Momentum* matchedSeed = 0;
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
                    m_log << MSG::WARNING << "matched seed already in map: "<< matchedSeed << endreq;
                    // do not insert twice: TODO: assign to best matching truth tau
                    matchedSeed = 0;
                } else {
                    m_log << MSG::VERBOSE << "matched to seed using dR: eta=" << matchedSeed->eta() << " phi=" << matchedSeed->phi() << " pT=" << matchedSeed->pt() << endreq;
                }
            }
            seedTruthMap->insert(std::make_pair(matchedSeed, truthTau));
        } // end for (nonMatchedTruthTaus)
    } // end if (selectedTruthTaus.size() > 0)
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
            m_log << MSG::VERBOSE << "Seed: eta=" << (*seedItr)->eta() << " phi=" << (*seedItr)->phi() << " pT=" << (*seedItr)->pt() << endreq;
        }
        // is seed already in the map?
        if (seedTruthMap->find(*seedItr) == seedTruthMap->end()) {
            //find matching TruthJet
            if (truthJetCont) {
                mcjetItr = truthJetCont->begin();
                double minDR = 99999999.;
                //const I4Momentum* matchedFake = 0;
                for (; mcjetItr != mcjetItrE; ++mcjetItr) {
                    const CLHEP::HepLorentzVector& mcJet = (*mcjetItr)->hlv();
                    //ignore if mc jet outside kinematic range
                    // TODO: decide: ignore jets outside kinematic range or match to all
                    // TruthJets first and do cut on match TruthJet??
                    if ( fabs(mcJet.eta()) > m_maxAbsEta || mcJet.perp() < m_minPt ) continue;
                    double dR = mcJet.deltaR( (*seedItr)->hlv() );
                    if (dR < minDR ) {
                        minDR = dR;
                        //matchedFake = (*seedItr);
                    }
                }// end for(mcjetItr)
                if (minDR < m_matchMaxDR) {
                    // insert into map with NULL TruthTau only if matched to truthJet
                    seedTruthMap->insert(std::make_pair( (*seedItr), truthTau ));
                }
            } else {
                // check, if outside of considered range based on reconstructed momenta:
                if ( (fabs((*seedItr)->eta()) > m_maxAbsEta) || ((*seedItr)->pt() < m_minPt) ) continue;
                // insert into map with NULL TruthTau
                seedTruthMap->insert(std::make_pair( (*seedItr), truthTau ));
            } // end if (truthJetCont)
        } // end if (not already in map)
    } // end for (seedItr)

    return seedTruthMap;
}

// adapted from PhysicsAnalysis/AnalysisCommon/AnalysisExamples/src/JetTagAna.cxx
const HepMC::GenParticle* TauID::TrackBasedTruthMatchUtility::getTruth(const Rec::TrackParticle* trackParticle,
                                              const TrackParticleTruthCollection* mcpartColl,
                                              const Rec::TrackParticleContainer* trackColl) const
{
    //this method returns pointer to truth particle linked to given track

    const HepMC::GenParticle* genPart =0;

    ElementLink<Rec::TrackParticleContainer> trackPrtlink;
    trackPrtlink.setElement(const_cast<Rec::TrackParticle*>(trackParticle));
    trackPrtlink.setStorableObject(*trackColl);

    std::map<Rec::TrackParticleTruthKey,TrackParticleTruth>::const_iterator tempTrackPrtTruthItr = mcpartColl->find(trackPrtlink);

    if (tempTrackPrtTruthItr != mcpartColl->end()) {
        const HepMcParticleLink& tempHepMcLink = (*tempTrackPrtTruthItr).second.particleLink();
        if (tempHepMcLink.isValid() && (*tempTrackPrtTruthItr).second.probability() > m_minTrackMatchProbability) {
            long theBarcode = tempHepMcLink.barcode();
            if (theBarcode!=0) genPart = tempHepMcLink.cptr();
        }
    }
    return genPart;
}

const DataVector<const TauID::TruthTau>* TauID::TrackBasedTruthMatchUtility::selectTruthTaus(const TruthParticleContainer& truthTauCont) const {
    //TauSeedCollection selectedSeeds(SG::VIEW_ELEMENTS);
    DataVector<const TauID::TruthTau>* selectedTruthTaus = new DataVector<const TauID::TruthTau>(SG::VIEW_ELEMENTS);
    /** Iterate over list of MC particles to select truth taus inside cuts */
    //loop over MC particles
    TruthParticleContainer::const_iterator mcpartItr  = truthTauCont.begin();
    TruthParticleContainer::const_iterator mcpartItrE = truthTauCont.end();
    for (; mcpartItr != mcpartItrE; ++mcpartItr) {
        // use TruthTaus only
        const TauID::TruthTau* truthTau = dynamic_cast<const TauID::TruthTau*>(*mcpartItr);
        if (!truthTau) continue;
        const CLHEP::HepLorentzVector& visTrueMom = truthTau->hlvVis();
        m_log << MSG::VERBOSE << "Truth tau " << selectedTruthTaus->size() << " vis: eta=" << visTrueMom.eta() << " phi=" << visTrueMom.phi() << " ET=" << visTrueMom.et() << ": " << truthTau->dumpType() << endreq;
        // ignore, if outside kinematic range
        if (fabs(visTrueMom.eta()) > m_maxAbsEta || visTrueMom.perp() < m_minPt) {
            m_log << MSG::VERBOSE << "   outside kinmatic range: ignore!" << endreq;
            continue;
        }
        selectedTruthTaus->push_back(truthTau);
    }
    return selectedTruthTaus;
}
