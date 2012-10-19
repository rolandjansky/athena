/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TruthTauCreatorAlg.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "TauTrackTools/TruthTauCreatorAlg.h"

#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/MsgStream.h"

#include "TauTrackEvent/TruthTau.h"
#include "McParticleEvent/TruthParticleContainer.h"
#include "McParticleEvent/TruthParticle.h"


//================ Constructor =================================================

TauID::TruthTauCreatorAlg::TruthTauCreatorAlg(const std::string& name, ISvcLocator* pSvcLocator)
        :
        AthAlgorithm(name,pSvcLocator),
        m_inputContainerName("SpclMC"),
        m_outputContainerName("TruthTaus"),
        m_maxRadialDistance(1200.),
        m_maxZDistance(3500.) {
    declareProperty("InputTruthParticleContainer",      m_inputContainerName,   "SG key of input TrackParticle container");
    declareProperty("OutputTauTruthParticleContainer",  m_outputContainerName,  "SG key of output TrackParticle container");
    declareProperty("MaxRadialDistance",                m_maxRadialDistance,    "max rho (cylinder coords) for particle origins (to suppress conversions out of the Inner Detector)");
    declareProperty("MaxZDistance",                     m_maxZDistance,         "max z(cylinder coords) for particle origins (to suppress conversions out of the Inner Detector)");
}

//================ Destructor =================================================

TauID::TruthTauCreatorAlg::~TruthTauCreatorAlg() {}


//================ Initialisation =================================================

StatusCode TauID::TruthTauCreatorAlg::initialize() {
    ATH_MSG_INFO( name() << " initialize()" );
    return StatusCode::SUCCESS;
}

//================ Finalisation =================================================

StatusCode TauID::TruthTauCreatorAlg::finalize() {
    return StatusCode::SUCCESS;
}

//================ Execution ====================================================
StatusCode TauID::TruthTauCreatorAlg::execute() {

    /** get the MC truth particle container from StoreGate */
    const TruthParticleContainer*  inputTruthPartCont = 0;
    StatusCode sc = evtStore()->retrieve(inputTruthPartCont, m_inputContainerName);
    if ( !inputTruthPartCont || sc.isFailure() ) {
        ATH_MSG_ERROR( " Truth particle container (" << m_inputContainerName << ") not found in StoreGate" );
        return StatusCode::FAILURE;
    }

    TruthParticleContainer*  outputTruthTaus = prepareCollection(inputTruthPartCont);

    // record the TruthParticleContainer to StoreGate
    sc = evtStore()->record(outputTruthTaus, m_outputContainerName, false);
    if (sc.isFailure()) {
        ATH_MSG_ERROR( "Could not record taus" );
    } else {
        ATH_MSG_VERBOSE(    "Saved "<< outputTruthTaus->size() << " taus (to " << m_outputContainerName
                            << "), from input (" << m_inputContainerName << ") of " << inputTruthPartCont->size()
                            <<" particles." );
    }
      return StatusCode::SUCCESS;
}


TruthParticleContainer* TauID::TruthTauCreatorAlg::prepareCollection(const TruthParticleContainer* inputTruthPartCont) {
    //create TruthTau container
    TruthParticleContainer*  outputTruthTaus = new TruthParticleContainer();
    if (!inputTruthPartCont) return outputTruthTaus;

    /** Iterate over list of MC particles looking for taus */
    TruthParticleContainer::const_iterator mcpartItr  = inputTruthPartCont->begin();
    TruthParticleContainer::const_iterator mcpartItrE = inputTruthPartCont->end();

    //loop over MC particles
    for (; mcpartItr != mcpartItrE; ++mcpartItr) {
        // ignore G4 particles
        if( ! isGenerator(*mcpartItr) )
            continue;
        // ignore taus which decay to tau and generator comments
        if( ! isTau(*mcpartItr) )
            continue;

        if (!((*mcpartItr)->genParticle())) {
            ATH_MSG_WARNING( "Could not get GenParticle for tau TruthParticle!" );
            continue;
        }
        // assign types
        std::bitset<TruthTau::NumberOfTruthTauTypes> typePattern(0);

        const TruthParticle* resonance = 0;
        CLHEP::HepLorentzVector* hlvVis = new CLHEP::HepLorentzVector( (*mcpartItr)->hlv() );
        // loop over children:
        const std::size_t nChildren = (*mcpartItr)->nDecay();
        for ( std::size_t iChild = 0; iChild != nChildren; ++iChild ) {
            const TruthParticle* child = (*mcpartItr)->child(iChild);
            if (!child) continue;
            unsigned int absID = abs(child->pdgId());
            if (absID == 16 || absID == 14 || absID == 12) {
                // substract momentum of neutrino:
                (*hlvVis) -= child->hlv();
            }
            if (absID == 11){
                typePattern.set(TruthTau::Electron);
            }
            if (absID == 13) {
                typePattern.set(TruthTau::Muon);
            }
        } // end for (iChild)

        unsigned int nChargedPions = 0;
        unsigned int nChargedKaons = 0;
        unsigned int nPi0 = 0;
        std::vector<const TruthParticle*>* convertedPhotons = new std::vector<const TruthParticle*>(0);
        std::vector<const TruthParticle*>* chargedProducts = new std::vector<const TruthParticle*>(0);
        std::vector<unsigned int>*         pionsWithHadronicInteractions = new std::vector<unsigned int>(0);
        std::vector<const TruthParticle*>* neutralProducts = new std::vector<const TruthParticle*>(0);
        // count number of charged and neutral pions from decay
        findPions((*mcpartItr), nChargedPions, nChargedKaons, nPi0, resonance, *convertedPhotons, *chargedProducts, *pionsWithHadronicInteractions, *neutralProducts);
        ATH_MSG_DEBUG( "Found " << nChargedPions << " charged pions and " << nChargedKaons << " charged Kaons with " << pionsWithHadronicInteractions->size() << " hadronic interactions and " << nPi0 << " neutral pions with " << convertedPhotons->size() << " conversions." );
        int resIdAbs = resonance ? abs( resonance->pdgId() ) : 0;
        if (resIdAbs == 130 || resIdAbs == 310 || resIdAbs == 311 || resIdAbs == 321 || resIdAbs == 313 || resIdAbs == 323) {
            // Decays with Kaons: K0L, K0S, K0, K+, K*(892)0, K*(892)+
            // TODO: investigate number of charged tracks within the ID volume
            typePattern.set(TruthTau::Kaon);
            typePattern.set(TruthTau::Hadronic);
            if ((nChargedPions + nChargedKaons)  == 1)
                typePattern.set(TruthTau::OneProng);
            if ((nChargedPions + nChargedKaons) == 3)
                typePattern.set(TruthTau::ThreeProng);
            if ((nChargedPions + nChargedKaons) == 5)
                typePattern.set(TruthTau::FiveProng);
        } else {
            if (nChargedPions == 1)
                typePattern.set(TruthTau::OneProng);
            if (nChargedPions == 3)
                typePattern.set(TruthTau::ThreeProng);
            if (nChargedPions == 5)
                typePattern.set(TruthTau::FiveProng);
        }
        if (nChargedPions > 0) {
            typePattern.set(TruthTau::Hadronic);
        }
        if (nPi0 > 0)
            typePattern.set(TruthTau::Neutrals);
        if (convertedPhotons->size() > 0 ) typePattern.set(TruthTau::Conversions);
        if (pionsWithHadronicInteractions->size() > 0) {
            typePattern.set(TruthTau::HadronicInteractions);
        } else {
            delete pionsWithHadronicInteractions;
            pionsWithHadronicInteractions = 0;
        }

        TruthTau* truthTau = new TruthTau(  (*mcpartItr)->genParticle(),
                                            typePattern,
                                            hlvVis,
                                            nPi0,
                                            resonance,
                                            inputTruthPartCont,  // take input truth particle container such that mothers and daughters are still valid...
                                            convertedPhotons,
                                            chargedProducts,
                                            pionsWithHadronicInteractions,
                                            neutralProducts );
        outputTruthTaus->push_back(truthTau);
        ATH_MSG_DEBUG( "Created TruthTau of type " << truthTau->dumpType() << " resonance: " << truthTau->resonancePdgId() << " barcode: " << truthTau->barcode() );

    } // end for (mcpartItr)

   
    return outputTruthTaus;
}

//============================================================================================


bool TauID::TruthTauCreatorAlg::isGenerator( const TruthParticle * part ) {

    return ( part->barcode() < 100000 && ( part->status()<200 ||
                                           part->status()%1000 == 1 ||
                                           part->status()%1000 == 2  ) );
}


bool TauID::TruthTauCreatorAlg::isTau( const TruthParticle * part ) {
    PDG::pidType id = part->pdgId();
    int ida = abs( (int) id );
    return ( ida == PDG::tau_minus && part->status() != 3  && !part->hasChild(id) );
}


void TauID::TruthTauCreatorAlg::findPions(  const TruthParticle * part,
                                            unsigned int& nChargedPions,
                                            unsigned int& nChargedKaons,
                                            unsigned int& nPi0,
                                            const TruthParticle*& resonance,
                                            std::vector<const TruthParticle*>& convertedPhotons,
                                            std::vector<const TruthParticle*>& chargedProducts,
                                            std::vector<unsigned int>& pionsWithHadronicInteractions,
                                            std::vector<const TruthParticle*>& neutralProducts,
                                            std::string indentationDepth) {
    if (!part) return;
    if (part->nDecay()==1 && part->hasChild(part->pdgId()) ) {
        ATH_MSG_DEBUG( indentationDepth  << "child has identical daughter: moving down the chain" );
        findPions(part->child(0), nChargedPions, nChargedKaons, nPi0, resonance, convertedPhotons, chargedProducts, pionsWithHadronicInteractions, neutralProducts, indentationDepth+"-");
        return;
    }
    const HepMC::GenVertex *decayVertex = part->genParticle() ? part->genParticle()->end_vertex() : 0;
    if (decayVertex) {
        double decRho   = decayVertex->point3d().perp();
        double decZ     = decayVertex->point3d().z();
        if (decRho > m_maxRadialDistance) {
            ATH_MSG_VERBOSE( indentationDepth  << "decay vertex at rho=" << decRho << ": ignore! " );
            return;
        }
        if (fabs(decZ) > m_maxZDistance) {
            ATH_MSG_VERBOSE( indentationDepth  << "decay vertex at z=" << decZ << ": ignore! " );
            return;
        }
        ATH_MSG_VERBOSE( indentationDepth  << "decay vertex at rho=" << decRho << ", z=" << decZ );
    }
    // loop over children:
    const std::size_t nChildren = part->nDecay();
    for ( std::size_t iChild = 0; iChild != nChildren; ++iChild ) {
        const TruthParticle * child = part->child(iChild);
        if (!child ) continue;
        ATH_MSG_VERBOSE( indentationDepth  << "child is a : " << child->pdgId() << " status: " << child->status() );
        if (child->status() == 3) continue;
//        if( !isGenerator(child) ) {
//            ATH_MSG_VERBOSE( "G4 particle: ignore! " );
//            continue;
//        }
        if (child->status() == 1 && fabs(child->charge()) > 0) {
            // found charged final state particle, add to list of charged products:
            chargedProducts.push_back(child);
        }
        int ida = abs( child->pdgId() );
        // neutral pions (neutral pions have status 2, because they are decayed into photons
        // by the generator) and other final state neutrals:
        if (ida == 111) {
            neutralProducts.push_back(child);
        } else if (child->status() == 1 && (fabs(child->charge()) < 0.01)) {
            //found neutral final state particle, add to list of neutral products:
            if( (ida != 16) && (ida != 14) && (ida != 12) ) {
                //do not add neutrinos to list of neutral products
                neutralProducts.push_back(child);
            }
        }
        if (ida == 22) {
            ATH_MSG_VERBOSE( indentationDepth  << "direct photon" );
            continue;
        }
        if (ida == 111) {
            nPi0++;
            //std::cout << indentationDepth  << child->dump(std::cout) << std::endl;
            // go on to find converted photons from pi0 decay:
            findConversions(child, convertedPhotons, indentationDepth+"-");
            continue;

        }
        if (ida == 211) {
            nChargedPions++;
            // investigate hadronic interactions
            const HepMC::GenVertex *pionDecayVertex = child->genParticle() ? child->genParticle()->end_vertex() : 0;
            if (pionDecayVertex) {
                double decRho   = pionDecayVertex->point3d().perp();
                double decZ     = pionDecayVertex->point3d().z();
                ATH_MSG_VERBOSE( indentationDepth << "  pion (child) decay vertex at rho=" << decRho << ", z=" << decZ );
                if ((decRho < m_maxRadialDistance) || (fabs(decZ) < m_maxZDistance)) {
                    // decay/interation inside Inner Detector
                    //nPionsWithHadronicInteractions++;
                    // add index of the charged particle:
                    pionsWithHadronicInteractions.push_back(chargedProducts.size()-1);
                }
            } // end if (pionDecayVertex)
            continue;
        }
        if (!resonance && ida > 100) {
            resonance = child;
        }
        // FIXME: How do we handle decays and hadronic interactions from K0??? Count their decay products?
        // For the moment: Stop and do not count their products as they are not prompt:
        if (ida == 310) {
            // K0s
            continue;
        }
        if (ida == 321) {
            // K+/- (do not go further down the chain, but look for hadronic interactions)
            nChargedKaons++;
            // investigate hadronic interactions
            const HepMC::GenVertex *pionDecayVertex = child->genParticle() ? child->genParticle()->end_vertex() : 0;
            if (pionDecayVertex) {
                double decRho   = pionDecayVertex->point3d().perp();
                double decZ     = pionDecayVertex->point3d().z();
                ATH_MSG_VERBOSE( indentationDepth << "  K+/- (child) decay vertex at rho=" << decRho << ", z=" << decZ );
                if ((decRho < m_maxRadialDistance) || (fabs(decZ) < m_maxZDistance)) {
                    // decay/interation inside Inner Detector
                    //nPionsWithHadronicInteractions++;
                    // add index of the charged particle:
                    pionsWithHadronicInteractions.push_back(chargedProducts.size()-1);
                }
            } // end if (pionDecayVertex)
            continue;
        }
//         if (ida > 10000000) {
//             // strange!
//             std::cout << indentationDepth  << child->dump(std::cout) << std::endl;
//         }
        if (ida > 22) {
            // go down the chain:
            findPions(child, nChargedPions, nChargedKaons, nPi0, resonance, convertedPhotons, chargedProducts, pionsWithHadronicInteractions, neutralProducts, indentationDepth+"-");
        }
    } // end for (iChild)
}

void TauID::TruthTauCreatorAlg::findConversions(const TruthParticle * part, std::vector<const TruthParticle*>& convertedPhotons, std::string indentationDepth) {
    if (!part) return;
    const HepMC::GenVertex *decayVertex = part->genParticle() ? part->genParticle()->end_vertex() : 0;
    if (decayVertex) {
        double decRho   = decayVertex->point3d().perp();
        double decZ     = decayVertex->point3d().z();
        if (decRho > m_maxRadialDistance) {
            ATH_MSG_VERBOSE( indentationDepth  << "decay vertex at rho=" << decRho << ": ignore! " );
            return;
        }
        if (fabs(decZ) > m_maxZDistance) {
            ATH_MSG_VERBOSE( indentationDepth  << "decay vertex at z=" << decZ << ": ignore! " );
            return;
        }
        ATH_MSG_VERBOSE( indentationDepth  << "decay vertex at rho=" << decRho << ", z=" << decZ );
    }
    // loop over children:
    const std::size_t nChildren = part->nDecay();
    for ( std::size_t iChild = 0; iChild != nChildren; ++iChild ) {
        const TruthParticle * child = part->child(iChild);
        if (!child ) continue;
        ATH_MSG_VERBOSE( indentationDepth  << "child is a : " << child->pdgId() << " status: " << child->status() );
        //std::cout << indentationDepth  << child->dump(std::cout) << std::endl;
        int ida = abs( child->pdgId() );
        if (ida == 11) {
            if (part->pdgId() == 22) {
                // found conversion electron
                convertedPhotons.push_back(part);
                ATH_MSG_VERBOSE( indentationDepth  << "found conversion" );
                return;
            }
        }
        findConversions(child, convertedPhotons, indentationDepth+"*");
    } // end for (iChild)
}


