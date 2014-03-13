/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ****************************************************************************
// ----------------------------------------------------------------------------
// JpsiFinder
// James Catmore <James.Catmore@cern.ch>
// Evelina Bouhova-Thacker <e.bouhova@cern.ch>
// Results returned as a vector of ExtendedVxCandidates
// ----------------------------------------------------------------------------
// ****************************************************************************

#include "JpsiUpsilonTools/JpsiFinder.h"
#include "Particle/TrackParticle.h"
#include "muonEvent/MuonContainer.h"
#include "VxVertex/VxContainer.h"
#include "VxVertex/ExtendedVxCandidate.h"
#include "VxVertex/VxTrackAtVertex.h"
#include "TrkVertexAnalysisUtils/V0Tools.h"
#include "TrkVertexFitterInterfaces/IVertexFitter.h"
#include "TrkVKalVrtFitter/TrkVKalVrtFitter.h"
#include "TrkV0Fitter/TrkV0VertexFitter.h"
#include "TrkParticleBase/TrackParticleBase.h"
#include "TrkParticleBase/LinkToTrackParticleBase.h"
#include "InDetConversionFinderTools/ConversionFinderUtils.h"
#include "InDetConversionFinderTools/VertexPointEstimator.h"
#include "TrkToolInterfaces/ITrackSelectorTool.h"
#include "EventPrimitives/EventPrimitives.h"
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/IPartPropSvc.h"
#include "DataModel/ElementLink.h"

namespace Analysis {
    
    StatusCode JpsiFinder::initialize() {
        
        // retrieving vertex Fitter
        if ( m_iVertexFitter.retrieve().isFailure() ) {
            ATH_MSG_FATAL("Failed to retrieve tool " << m_iVertexFitter);
            return StatusCode::FAILURE;
        } else {
            ATH_MSG_INFO("Retrieved tool " << m_iVertexFitter);
        }
        
        // retrieving V0 Fitter
        if ( m_iV0VertexFitter.retrieve().isFailure() ) {
            ATH_MSG_FATAL("Failed to retrieve tool " << m_iV0VertexFitter);
            return StatusCode::FAILURE;
        } else {
            ATH_MSG_INFO("Retrieved tool " << m_iV0VertexFitter);
        }
        
        // Get the track selector tool from ToolSvc
        if ( m_trkSelector.retrieve().isFailure() ) {
            ATH_MSG_FATAL("Failed to retrieve tool " << m_trkSelector);
            return StatusCode::FAILURE;
        } else {
            ATH_MSG_INFO("Retrieved tool " << m_trkSelector);
        }
        
        // uploading the V0 tools
        if ( m_V0Tools.retrieve().isFailure() ) {
            ATH_MSG_FATAL("Failed to retrieve tool " << m_V0Tools);
            return StatusCode::FAILURE;
        } else {
            ATH_MSG_INFO("Retrieved tool " << m_V0Tools);
        }
        
        // Get the vertex point estimator tool from ToolSvc
        if ( m_vertexEstimator.retrieve().isFailure() ) {
            ATH_MSG_FATAL("Failed to retrieve tool " << m_vertexEstimator);
            return StatusCode::FAILURE;
        } else {
            ATH_MSG_INFO("Retrieved tool " << m_vertexEstimator);
        }
        
        // Get the helpertool from ToolSvc
        if ( m_helpertool.retrieve().isFailure() ) {
            ATH_MSG_FATAL("Failed to retrieve tool " << m_helpertool);
            return StatusCode::FAILURE;
        } else {
            ATH_MSG_INFO("Retrieved tool " << m_helpertool);
        }
        
        // Get the Particle Properties Service
        IPartPropSvc* partPropSvc = 0;
        StatusCode sc = service("PartPropSvc", partPropSvc, true);
        if (sc.isFailure()) {
            ATH_MSG_ERROR("Could not initialize Particle Properties Service");
            return StatusCode::SUCCESS;
        } else {
            m_particleDataTable = partPropSvc->PDT();
            const HepPDT::ParticleData* pd_mu = m_particleDataTable->particle(PDG::mu_minus);
            if (m_diMuons) {m_trk1M = pd_mu->mass(); m_trk2M = pd_mu->mass();}
        }
        
        if (m_doTagAndProbe) ATH_MSG_WARNING("You have requested tag and probe mode. Duplicate mu+trk pairs WILL be allowed, charge ordering WILL NOT be done. Tag track will be first in each candidate");
        
        ATH_MSG_INFO("Initialize successful");
        
        return StatusCode::SUCCESS;
        
    }
    
    StatusCode JpsiFinder::finalize() {
        
        ATH_MSG_INFO("Finalize successful");
        return StatusCode::SUCCESS;
        
    }
    JpsiFinder::JpsiFinder(const std::string& t, const std::string& n, const IInterface* p)  : AthAlgTool(t,n,p),
    m_mumu(true),
    m_mutrk(false),
    m_trktrk(false),
    m_allMuons(false),
    m_combOnly(false),
    m_atLeastOneComb(true),
    m_useCombMeasurement(false),
    m_useV0Fitter(false),
    m_diMuons(true),
    m_trk1M(105.66),
    m_trk2M(105.66),
    m_particleDataTable(0),
    m_thresholdPt(0.0),
    m_higherPt(0.0),
    m_trkThresholdPt(0.0),
    m_invMassUpper(100000.0),
    m_invMassLower(0.0),
    m_collAngleTheta(0.0),
    m_collAnglePhi(0.0),
    m_Chi2Cut(50.),
    m_oppChOnly(true),
    m_sameChOnly(false),
    m_allChCombs(false),
    m_muonCollectionKey("StacoMuonCollection"),
    m_TrkParticleCollection("TrackParticleCandidate"),
    m_iVertexFitter("Trk::TrkVKalVrtFitter"),
    m_iV0VertexFitter("Trk::V0VertexFitter"),
    m_V0Tools("Trk::V0Tools"),
    m_trkSelector("InDet::TrackSelectorTool"),
    m_helpertool("InDet::ConversionFinderUtils"),
    m_vertexEstimator("InDet::VertexPointEstimator"),
    m_mcpCuts(true),
    m_doTagAndProbe(false)
    
    {
        declareInterface<JpsiFinder>(this);
        declareProperty("muAndMu",m_mumu);
        declareProperty("muAndTrack",m_mutrk);
        declareProperty("TrackAndTrack",m_trktrk);
        declareProperty("allMuons",m_allMuons);
        declareProperty("combOnly",m_combOnly);
        declareProperty("atLeastOneComb",m_atLeastOneComb);
        declareProperty("useCombinedMeasurement",m_useCombMeasurement);
        declareProperty("useV0Fitter",m_useV0Fitter);
        declareProperty("assumeDiMuons",m_diMuons);
        declareProperty("track1Mass",m_trk1M);
        declareProperty("track2Mass",m_trk2M);
        declareProperty("muonThresholdPt",m_thresholdPt);
        declareProperty("higherPt",m_higherPt);
        declareProperty("trackThresholdPt",m_trkThresholdPt);
        declareProperty("invMassUpper",m_invMassUpper);
        declareProperty("invMassLower",m_invMassLower);
        declareProperty("collAngleTheta",m_collAngleTheta);
        declareProperty("collAnglePhi",m_collAnglePhi);
        declareProperty("Chi2Cut",m_Chi2Cut);
        declareProperty("oppChargesOnly",m_oppChOnly);
        declareProperty("sameChargesOnly",m_sameChOnly);
        declareProperty("allChargeCombinations",m_allChCombs);
        declareProperty("muonCollectionKey",m_muonCollectionKey);
        declareProperty("TrackParticleCollection",m_TrkParticleCollection);
        declareProperty("MuonTrackKeys",m_MuonTrackKeys);
        declareProperty("TrkVertexFitterTool",m_iVertexFitter);
        declareProperty("V0VertexFitterTool",m_iV0VertexFitter);
        declareProperty("V0Tools",m_V0Tools);
        declareProperty("TrackSelectorTool",m_trkSelector);
        declareProperty("ConversionFinderHelperTool",m_helpertool);
        declareProperty("VertexPointEstimator",m_vertexEstimator);
        declareProperty("useMCPCuts",m_mcpCuts);
        declareProperty("doTagAndProbe",m_doTagAndProbe);
    }
    
    JpsiFinder::~JpsiFinder() { }
    
    //-------------------------------------------------------------------------------------
    // Find the candidates
    //-------------------------------------------------------------------------------------
    VxContainer* JpsiFinder::performSearch()
    {
        ATH_MSG_DEBUG( "JpsiFinder::performSearch" );
        VxContainer* JpsiContainer = new VxContainer;
        
        // Counter of good muon pairs BEFORE vertexing
        m_nGoodPairs = 0;
        
        // Get the ToolSvc
        IToolSvc* toolsvc;
        StatusCode sc1=service("ToolSvc",toolsvc);
        if (sc1.isFailure() ) {
            ATH_MSG_WARNING("Problem loading tool service. JpsiCandidates will be EMPTY!");
            return JpsiContainer;
        };
        
        // Check that the user's settings are sensible
        bool illogicalOptions(false);
        if ( (m_mumu && m_mutrk) || (m_mumu && m_trktrk) || (m_mutrk && m_trktrk) ) {
            ATH_MSG_WARNING("You are requesting incompatible combinations of muons and tracks in the pairs. JpsiCandidates will be EMPTY!");
            illogicalOptions=true;
        };
        if ( (m_doTagAndProbe && m_mumu) || (m_doTagAndProbe && m_trktrk) ) {
            ATH_MSG_WARNING("You are requesting Tag and Probe analysis but have not requested mu+trk mode. This is impossible. JpsiCandidates will be EMPTY!");
            illogicalOptions=true;
        };
        if ( (m_mutrk ||  m_trktrk ) && m_useCombMeasurement ) {
            ATH_MSG_WARNING("You are requesting a combined muon measurement to be used with pairs involving non-muon tracks. Not permitted. JpsiCandidates will be EMPTY!");
            illogicalOptions=true;
        };
        if ( (m_allMuons && m_combOnly) || (m_allMuons && m_atLeastOneComb) || (m_combOnly && m_atLeastOneComb) ) {
            ATH_MSG_WARNING("You are requesting incompatible combinations of combined muons non-combined muons in the pairs. JpsiCandidates will be EMPTY!");
            illogicalOptions=true;
        };
        if (m_atLeastOneComb && m_doTagAndProbe) {
            ATH_MSG_WARNING("You are requesting at least one muon to be combined in a tag and probe analysis. This doesn't make sense. JpsiCandidates will be EMPTY!");
            illogicalOptions=true;
        };
        if ( (m_oppChOnly && m_sameChOnly) || (m_oppChOnly && m_allChCombs) || (m_sameChOnly && m_allChCombs) ) {
            ATH_MSG_WARNING("You are requesting incompatible combinations of charges in the pairs. JpsiCandidates will be EMPTY!");
            illogicalOptions=true;
        };
        if ( (m_sameChOnly && m_doTagAndProbe) || (m_allChCombs && m_doTagAndProbe) ) {
            ATH_MSG_WARNING("You are requesting same-sign or all-sign combinations in a tag and probe analysis. This doesn't make sense. JpsiCandidates will be EMPTY!");
            illogicalOptions=true;
        }
        if (illogicalOptions) return JpsiContainer;
        
        // Get the muons from StoreGate
        const Analysis::MuonContainer* importedMuonCollection;
        StatusCode sc = evtStore()->retrieve(importedMuonCollection,m_muonCollectionKey);
        if(sc.isFailure()){
            ATH_MSG_WARNING("No muon collection with key " << m_muonCollectionKey << " found in StoreGate. JpsiCandidates will be EMPTY!");
            return JpsiContainer;
        }else{
            ATH_MSG_DEBUG("Found muon collections with key "<<m_muonCollectionKey);
        }
        ATH_MSG_DEBUG("Muon container size "<<importedMuonCollection->size());
        
        // Get muon tracks if combined measurement requested
        std::vector<const Trk::TrackParticleBaseCollection*> importedMuonTrackCollections;
        if (m_useCombMeasurement) {
            std::vector<std::string>::iterator strItr;
            for (strItr=m_MuonTrackKeys.begin(); strItr!=m_MuonTrackKeys.end(); ++strItr) {
                const Trk::TrackParticleBaseCollection* importedMuonTrackCollection;
                sc = evtStore()->retrieve(importedMuonTrackCollection,*strItr);
                if(sc.isFailure() || importedMuonTrackCollection==NULL){
                    ATH_MSG_WARNING("No muon TrackParticle collection with name " << *strItr  << " found in StoreGate!");
                    return JpsiContainer;
                } else {
                    ATH_MSG_DEBUG("Found muon TrackParticle collection " << *strItr << " in StoreGate!");
                    ATH_MSG_DEBUG("Muon TrackParticle container size "<< importedMuonTrackCollection->size());
                    importedMuonTrackCollections.push_back(importedMuonTrackCollection);
                }
            }
        }
        
        // Get ID tracks
        const Trk::TrackParticleBaseCollection* importedTrackCollection(0);
        sc = evtStore()->retrieve(importedTrackCollection,m_TrkParticleCollection);
        if(sc.isFailure()){
            ATH_MSG_WARNING("No TrackParticle collection with name " << m_TrkParticleCollection << " found in StoreGate!");
            return JpsiContainer;
        } else {
            ATH_MSG_DEBUG("Found TrackParticle collection " << m_TrkParticleCollection << " in StoreGate!");
        }
        ATH_MSG_DEBUG("ID TrackParticle container size "<< importedTrackCollection->size());
        
        // Typedef for vectors of tracks and muons
        typedef std::vector<const Rec::TrackParticle*> TrackBag;
        typedef std::vector<const Analysis::Muon*> MuonBag;
        
        // Select the inner detector tracks
        const Trk::Vertex* vx = 0;
        TrackBag theIDTracksAfterSelection;
        if (m_trktrk || m_mutrk) {
            Trk::TrackParticleBaseCollection::const_iterator trkCItr;
            for (trkCItr=importedTrackCollection->begin(); trkCItr!=importedTrackCollection->end(); ++trkCItr) {
                const Rec::TrackParticle* TP = dynamic_cast<const Rec::TrackParticle*>(*trkCItr);
                if ( fabs(getPt(TP))<m_trkThresholdPt ) continue;
                if ( !m_trkSelector->decision(*TP, vx) ) continue;
                theIDTracksAfterSelection.push_back(TP);
            }
            if (theIDTracksAfterSelection.size() == 0) return JpsiContainer;
            ATH_MSG_DEBUG("Number of tracks after ID track selection: " << theIDTracksAfterSelection.size());
        }
        
        // Select the muons
        MuonBag theMuonsAfterSelection;
        Analysis::MuonContainer::const_iterator muItr;
        if (m_mumu || m_mutrk) {
            for (muItr=importedMuonCollection->begin(); muItr!=importedMuonCollection->end(); ++muItr) {
                if ( *muItr == NULL ) continue;
                if (!(*muItr)->hasInDetTrackParticle()) continue; // No muons without ID tracks
                const Rec::TrackParticle* muonTrk(0);
                muonTrk = (*muItr)->inDetTrackParticle();
                if ( muonTrk==NULL) continue;
                if ( !m_trkSelector->decision(*muonTrk, vx) ) continue; // all ID tracks must pass basic tracking cuts
                if ( fabs(getPt(muonTrk))<m_thresholdPt ) continue; // higher pt cut if needed
                if ( m_mcpCuts && !passesMCPCuts(muonTrk)) continue; // MCP cuts
                if ( m_combOnly && !(*muItr)->isCombinedMuon() ) continue; // require combined muons
                if ( (*muItr)->isSiliconAssociatedForwardMuon() && !m_useCombMeasurement) continue;
                theMuonsAfterSelection.push_back(*muItr);
            }
            if (theMuonsAfterSelection.size() == 0) return JpsiContainer;
            ATH_MSG_DEBUG("Number of muons after selection: " << theMuonsAfterSelection.size());
        }
        
        // Sort into pairs - end result will be a vector of JpsiCandidate structs
        std::vector<JpsiCandidate> jpsiCandidates;
        if (m_mumu) jpsiCandidates = getPairs(theMuonsAfterSelection);
        if (m_trktrk) jpsiCandidates = getPairs(theIDTracksAfterSelection);
        if (m_mutrk) jpsiCandidates = getPairs2Colls(theIDTracksAfterSelection,theMuonsAfterSelection,m_doTagAndProbe);
        
        // Pair-wise selections
        std::vector<JpsiCandidate>::iterator jpsiItr;
        
        // (1) Enforce one combined muon
        if (m_atLeastOneComb) {
            int index(0);
            std::vector<int> listToDelete;
            std::vector<int>::reverse_iterator ii;
            for (jpsiItr = jpsiCandidates.begin(); jpsiItr!=jpsiCandidates.end(); ++jpsiItr,++index) {
                if ( ((*jpsiItr).muonTypes==TT)) listToDelete.push_back(index);
            }
            for (ii=listToDelete.rbegin(); ii!=listToDelete.rend(); ++ii) {
                jpsiCandidates.erase(jpsiCandidates.begin() + (*ii) );
            }
            ATH_MSG_DEBUG("Number of candidates after requirement of at least 1 combined muon: " << jpsiCandidates.size() );
        }
        
        // (2) Establish track content for candidates
        // and set the appropriate track collections for the combined muon tracks where appropriate (for saving to persistency later)
        
        // mu+trk or trk+trk - always ID track collection
        if (m_mutrk || m_trktrk) {
            for (jpsiItr=jpsiCandidates.begin(); jpsiItr!=jpsiCandidates.end(); ++jpsiItr) {
                (*jpsiItr).collection1 = importedTrackCollection;
                (*jpsiItr).collection2 = importedTrackCollection;
            }
        }
        
        const Rec::TrackParticle* combTrk1(0);
        const Rec::TrackParticle* combTrk2(0);
        std::vector<const Trk::TrackParticleBaseCollection*>::iterator muTrkCollItr;
        if (m_mumu) {
            for (jpsiItr=jpsiCandidates.begin(); jpsiItr!=jpsiCandidates.end(); ++jpsiItr) {
                if ( (m_combOnly && !m_useCombMeasurement) || m_atLeastOneComb || m_allMuons) {
                    (*jpsiItr).trackParticle1 = (*jpsiItr).muon1->inDetTrackParticle();
                    (*jpsiItr).trackParticle2 = (*jpsiItr).muon2->inDetTrackParticle();
                    (*jpsiItr).collection1 = importedTrackCollection;
                    (*jpsiItr).collection2 = importedTrackCollection;
                }
                if (m_combOnly && m_useCombMeasurement) {
                    combTrk1 = (*jpsiItr).muon1->combinedMuonTrackParticle();
                    combTrk2 = (*jpsiItr).muon2->combinedMuonTrackParticle();
                    if (combTrk1==NULL) {
                        (*jpsiItr).trackParticle1 = (*jpsiItr).muon1->inDetTrackParticle();
                        (*jpsiItr).collection1 = importedTrackCollection;
                    }
                    if (combTrk2==NULL) {
                        (*jpsiItr).trackParticle2 = (*jpsiItr).muon2->inDetTrackParticle();
                        (*jpsiItr).collection2 = importedTrackCollection;
                    }
                    if (combTrk1!=NULL) {
                        (*jpsiItr).trackParticle1 = combTrk1;
                        bool foundCollection(false);
                        // Look for correct muon track container
                        for (muTrkCollItr=importedMuonTrackCollections.begin(); muTrkCollItr!=importedMuonTrackCollections.end(); ++muTrkCollItr) {
                            if (isContainedIn(combTrk1,*muTrkCollItr)) { (*jpsiItr).collection1 = *muTrkCollItr; foundCollection=true; break;}
                        }
                        if (!foundCollection) { // didn't find the correct muon track container so go back to ID
                            (*jpsiItr).trackParticle1 = (*jpsiItr).muon1->inDetTrackParticle();
                            (*jpsiItr).collection1 = importedTrackCollection;
                            ATH_MSG_WARNING("Muon track from muon of author " << (*jpsiItr).muon1->author() << " not found in muon track collections you have provided.");
                            ATH_MSG_WARNING("Defaulting to ID track collection - combined measurement will not be used");
                        }
                    }
                    if (combTrk2!=NULL) {
                        (*jpsiItr).trackParticle2 = combTrk2;
                        bool foundCollection(false);
                        // Look for correct muon track container
                        for (muTrkCollItr=importedMuonTrackCollections.begin(); muTrkCollItr!=importedMuonTrackCollections.end(); ++muTrkCollItr) {
                            if (isContainedIn(combTrk2,*muTrkCollItr)) { (*jpsiItr).collection2 = *muTrkCollItr; foundCollection=true; break;}
                        }
                        if (!foundCollection) { // didn't find the correct muon track container so go back to ID
                            (*jpsiItr).trackParticle2 = (*jpsiItr).muon2->inDetTrackParticle();
                            (*jpsiItr).collection2 = importedTrackCollection;
                            ATH_MSG_WARNING("Muon track from muon of author " << (*jpsiItr).muon2->author() << " not found in muon track collections you have provided.");
                            ATH_MSG_WARNING("Defaulting to ID track collection - combined measurement will not be used");
                        }
                    }
                } // combined measurement
            } // iteration over candidates
        }
        
        
        // (3) Enforce higher track pt if requested
        if (m_higherPt>0.0) {
            int index(0);
            std::vector<int> listToDelete;
            std::vector<int>::reverse_iterator ii;
            for(jpsiItr=jpsiCandidates.begin(); jpsiItr!=jpsiCandidates.end();++jpsiItr,++index) {
                if( (fabs(getPt((*jpsiItr).trackParticle1)) < m_higherPt) && (fabs(getPt((*jpsiItr).trackParticle2)) < m_higherPt) ) listToDelete.push_back(index);
            }
            for (ii=listToDelete.rbegin(); ii!=listToDelete.rend(); ++ii) {
                jpsiCandidates.erase(jpsiCandidates.begin() + (*ii) );
            }
            ATH_MSG_DEBUG("Number of candidates after higherPt cut: " << jpsiCandidates.size() );
        }
        
        // (4) Select all opp/same charged track pairs
        std::vector<JpsiCandidate> sortedJpsiCandidates;
        if (m_oppChOnly) sortedJpsiCandidates = selectCharges(jpsiCandidates,"OPPOSITE");
        if (m_sameChOnly) sortedJpsiCandidates = selectCharges(jpsiCandidates,"SAME");
        if (m_allChCombs) sortedJpsiCandidates = selectCharges(jpsiCandidates,"ALL");
        ATH_MSG_DEBUG("Number of candidates after charge selection: " << sortedJpsiCandidates.size() );
        
        // (5) Select for decay angle, if requested
        if (m_collAnglePhi>0.0 && m_collAngleTheta>0.0) {
            int index(0);
            std::vector<int> listToDelete;
            std::vector<int>::reverse_iterator ii;
            for(jpsiItr=sortedJpsiCandidates.begin(); jpsiItr!=sortedJpsiCandidates.end();++jpsiItr,++index) {
                double deltatheta = fabs( (*jpsiItr).trackParticle1->measuredPerigee()->parameters()[Trk::theta] -
                                         (*jpsiItr).trackParticle2->measuredPerigee()->parameters()[Trk::theta] );
                // -3.14 < phi < +3.14 ==> correction
                double deltaphi = (*jpsiItr).trackParticle1->measuredPerigee()->parameters()[Trk::phi0] -
                (*jpsiItr).trackParticle1->measuredPerigee()->parameters()[Trk::phi0];
                while ( fabs(deltaphi) > M_PI ) deltaphi += ( deltaphi > 0. ) ? -2.*M_PI : 2.*M_PI;
                deltaphi = fabs(deltaphi);
                // perform the angle cuts
                if ((deltatheta > m_collAngleTheta) || (deltaphi > m_collAnglePhi)) listToDelete.push_back(index);
            }
            for (ii=listToDelete.rbegin(); ii!=listToDelete.rend(); ++ii) {
                sortedJpsiCandidates.erase(sortedJpsiCandidates.begin() + (*ii) );
            }
            ATH_MSG_DEBUG("Number of collimated candidates: " << sortedJpsiCandidates.size() );
        }
        
        // (6) Select for invariant mass, if requested
        std::vector<double> trkMasses;
        trkMasses.push_back(m_trk1M);
        trkMasses.push_back(m_trk2M);
        if ( (m_invMassLower > 0.0) || (m_invMassUpper > 0.0) ) {
            int index(0);
            std::vector<int> listToDelete;
            std::vector<int>::reverse_iterator ii;
            for(jpsiItr=sortedJpsiCandidates.begin(); jpsiItr!=sortedJpsiCandidates.end(); ++jpsiItr,++index) {
                double invMass = getInvariantMass(*jpsiItr,trkMasses);
                if ( invMass < m_invMassLower || invMass > m_invMassUpper ) {
                    listToDelete.push_back(index);
                }
            }
            for (ii=listToDelete.rbegin(); ii!=listToDelete.rend(); ++ii) {
                sortedJpsiCandidates.erase(sortedJpsiCandidates.begin() + (*ii) );
            }
            ATH_MSG_DEBUG("Number of candidates passing invariant mass selection: " << sortedJpsiCandidates.size() );
        }
        
        ATH_MSG_DEBUG("Number of pairs passing all selections and going to vertexing: " << sortedJpsiCandidates.size() );
        if (sortedJpsiCandidates.size() == 0) return JpsiContainer;
        
        // Some applications want the number of candidates (or the track pairs themselves) before vertexing;
        // the next lines facilitate this
        m_nGoodPairs = sortedJpsiCandidates.size();
        for (jpsiItr=sortedJpsiCandidates.begin(); jpsiItr!=sortedJpsiCandidates.end(); ++jpsiItr) {
            TrackBag tmpTrkBag;
            tmpTrkBag.push_back( (*jpsiItr).trackParticle1);
            tmpTrkBag.push_back( (*jpsiItr).trackParticle2);
            m_goodPairs.push_back(tmpTrkBag);
        }
        
        // Fit each pair of tracks to a vertex
        for(jpsiItr=sortedJpsiCandidates.begin(); jpsiItr!=sortedJpsiCandidates.end(); ++jpsiItr) {
            std::vector<const Rec::TrackParticle*> theTracks; theTracks.clear();
            theTracks.push_back((*jpsiItr).trackParticle1);
            theTracks.push_back((*jpsiItr).trackParticle2);
            Trk::VxCandidate* myVxCandidate = fit(theTracks); // This line actually does the fitting and object making
            bool keepVx(false);
            if (myVxCandidate != 0) {
                // In the end myVxCandidate must either be stored or deleted
                const std::vector<Trk::VxTrackAtVertex*>& trackAtVertexVec = *((myVxCandidate)->vxTrackAtVertex());
                // Linking of original tracks with the ExtendedVxCandidate object. Original container pointer needed
                for (unsigned int i = 0; i < trackAtVertexVec.size(); i++) {
                    Trk::LinkToTrackParticleBase* newLinkToTrackPB = new Trk::LinkToTrackParticleBase();
                    if(i == 0) {
                        newLinkToTrackPB->setElement(theTracks[0]);
                        const Trk::TrackParticleBaseCollection* storableCollection = (*jpsiItr).collection1;
                        newLinkToTrackPB->setStorableObject(*storableCollection);
                    }
                    if(i == 1) {
                        newLinkToTrackPB->setElement(theTracks[1]);
                        const Trk::TrackParticleBaseCollection* storableCollection = (*jpsiItr).collection2;
                        newLinkToTrackPB->setStorableObject(*storableCollection);
                    }
                    trackAtVertexVec[i]->setOrigTrack(newLinkToTrackPB);
                }
                // Chi2 cut if requested
                double chi2 = myVxCandidate->recVertex().fitQuality().chiSquared();
                ATH_MSG_DEBUG("chi2 is: " << chi2);
                if (m_Chi2Cut == 0.0 || chi2 <= m_Chi2Cut) {
                    Trk::ExtendedVxCandidate* jpsiVertexCandidate = dynamic_cast<Trk::ExtendedVxCandidate* >(myVxCandidate);
                    if (!jpsiVertexCandidate) { // Needed in case Billoir fitter used
                        keepVx = false;
                        jpsiVertexCandidate = new Trk::ExtendedVxCandidate(myVxCandidate->recVertex(), *(myVxCandidate->vxTrackAtVertex()), 0);
                    } else {
                        keepVx=true;
                    }
                    if (jpsiVertexCandidate!=NULL){
                        JpsiContainer->push_back(jpsiVertexCandidate);
                    }
                } else { // chi2 cut failed
                    keepVx=true; // just to avoid double delete at end
                    delete myVxCandidate;
                }
            } else { // fit failed
                ATH_MSG_DEBUG("Fitter failed!");
                delete myVxCandidate;
            }
            if (myVxCandidate && !keepVx) {
                ATH_MSG_DEBUG("Fitter succeeded but no ExtendedVxCandidate");
                if (myVxCandidate->vxTrackAtVertex()) myVxCandidate->vxTrackAtVertex()->clear();
                delete myVxCandidate;
            }
        }
        ATH_MSG_DEBUG("JpsiContainer size " << JpsiContainer->size());
        
        return JpsiContainer;
    }
    
    // *********************************************************************************
    
    // ---------------------------------------------------------------------------------
    // fit - does the fit
    // ---------------------------------------------------------------------------------
    
    Trk::VxCandidate* JpsiFinder::fit(std::vector<const Rec::TrackParticle*> inputTracks) {
        
        Trk::TrkV0VertexFitter* concreteVertexFitter=0;
        if (m_useV0Fitter) {
            // making a concrete fitter for the V0Fitter
            concreteVertexFitter = dynamic_cast<Trk::TrkV0VertexFitter * >(&(*m_iV0VertexFitter));
            if(concreteVertexFitter == 0) {
                ATH_MSG_FATAL("The vertex fitter passed is not a V0 Vertex Fitter");
                return NULL;
            }
        }
        
        const Trk::Perigee * aPerigee1 = inputTracks[0]->measuredPerigee();
        const Trk::Perigee * aPerigee2 = inputTracks[1]->measuredPerigee();
        int sflag = 0;
        int errorcode = 0;
        Amg::Vector3D startingPoint = m_vertexEstimator->getCirclesIntersectionPoint(aPerigee1,aPerigee2,sflag,errorcode);
        if (errorcode != 0) {startingPoint(0) = 0.0; startingPoint(1) = 0.0; startingPoint(2) = 0.0;}
        Trk::Vertex vertex(startingPoint);
        const Trk::ParametersBase<5,Trk::Charged>* track1 = m_helpertool->getTrkParticleParameters((inputTracks[0]));
        const Trk::ParametersBase<5,Trk::Charged>* track2 = m_helpertool->getTrkParticleParameters((inputTracks[1]));
        std::vector<const Trk::ParametersBase<5,Trk::Charged>*> vertexTracks; vertexTracks.clear();
        vertexTracks.push_back(track1);
        vertexTracks.push_back(track2);
        if (m_useV0Fitter) {
            Trk::VxCandidate* myVxCandidate = concreteVertexFitter->fit(vertexTracks, vertex);
            return myVxCandidate;
        } else {
            Trk::VxCandidate* myVxCandidate = m_iVertexFitter->fit(vertexTracks, vertex);
            return myVxCandidate;
        }
        
    } // End of fit method
    
    
    // *********************************************************************************
    
    
    // ---------------------------------------------------------------------------------
    // getPt: returns the pT on being passed a const TrackParticle
    // ---------------------------------------------------------------------------------
    
    double JpsiFinder::getPt(const Rec::TrackParticle* trackIn){
        
        double pt=0.,theta=0.,qOverP=0.;
        qOverP=trackIn->measuredPerigee()->parameters()[Trk::qOverP];
        theta=trackIn->measuredPerigee()->parameters()[Trk::theta];
        pt=(1.0/qOverP)*sin(theta);
        
        return pt;
    }
    
    // ---------------------------------------------------------------------------------
    // getEta: returns the pseudorapidity on being passed a const TrackParticle*
    // ---------------------------------------------------------------------------------
    double JpsiFinder::getEta(const Rec::TrackParticle* trackIn){
        double theta,eta;
        theta=trackIn->measuredPerigee()->parameters()[Trk::theta];
        eta=-1.0*log(tan(0.5*theta));
        return eta;
    }
    
    // *********************************************************************************
    
    // *********************************************************************************
    
    // ---------------------------------------------------------------------------------
    // getPairs: forms up 2-plets of tracks
    // ---------------------------------------------------------------------------------
    
    std::vector<JpsiCandidate> JpsiFinder::getPairs(std::vector<const Rec::TrackParticle*> TracksIn){
        
        std::vector<JpsiCandidate> myPairs;
        JpsiCandidate pair;
        std::vector<const Rec::TrackParticle*>::const_iterator outerItr;
        std::vector<const Rec::TrackParticle*>::const_iterator innerItr;
        
        if(TracksIn.size()>=2){
            for(outerItr=TracksIn.begin();outerItr<TracksIn.end();outerItr++){
                for(innerItr=(outerItr+1);innerItr!=TracksIn.end();innerItr++){
                    pair.trackParticle1 = *innerItr;
                    pair.trackParticle2 = *outerItr;
                    pair.pairType = TRKTRK;
                    myPairs.push_back(pair);
                }
            }
        }
        
        return(myPairs);
    }
    
    // *********************************************************************************
    
    // ---------------------------------------------------------------------------------
    // getPairs: forms up 2-plets of muons
    // ---------------------------------------------------------------------------------
    
    std::vector<JpsiCandidate> JpsiFinder::getPairs(std::vector<const Analysis::Muon*> muonsIn){
        
        std::vector<JpsiCandidate> myPairs;
        JpsiCandidate pair;
        std::vector<const Analysis::Muon*>::const_iterator outerItr;
        std::vector<const Analysis::Muon*>::const_iterator innerItr;
        
        if(muonsIn.size()>=2){
            for(outerItr=muonsIn.begin();outerItr<muonsIn.end();outerItr++){
                for(innerItr=(outerItr+1);innerItr!=muonsIn.end();innerItr++){
                    pair.muon1 = *innerItr;
                    pair.muon2 = *outerItr;
                    pair.pairType = MUMU;
                    bool mu1Comb( (*innerItr)->isCombinedMuon() );
                    bool mu2Comb( (*outerItr)->isCombinedMuon() );
                    if (mu1Comb && mu2Comb) pair.muonTypes = CC;
                    if ( (mu1Comb && !mu2Comb) || (!mu1Comb && mu2Comb) ) pair.muonTypes = CT;
                    if (!mu1Comb && !mu2Comb) pair.muonTypes = TT;
                    myPairs.push_back(pair);
                }
            }
        }
        
        return(myPairs);
    }
    
    // *********************************************************************************
    
    // ---------------------------------------------------------------------------------
    // getPairs2Colls: forms up 2-plets of tracks from two independent collections
    // ---------------------------------------------------------------------------------
    
    std::vector<JpsiCandidate> JpsiFinder::getPairs2Colls(std::vector<const Rec::TrackParticle*> tracks, std::vector<const Analysis::Muon*> muons, bool tagAndProbe){
        
        std::vector<JpsiCandidate> myPairs;
        JpsiCandidate pair;
        std::vector<const Rec::TrackParticle*>::const_iterator trkItr;
        std::vector<const Analysis::Muon*>::const_iterator muItr;
        
        // Unless user is running in tag and probe mode, remove tracks which are also identified as muons
        std::vector<const Rec::TrackParticle*> tracksToKeep;
        if (!tagAndProbe) {
            if(tracks.size()>=1 && muons.size()>=1){
                for(trkItr=tracks.begin();trkItr<tracks.end();trkItr++){
                    bool trackIsMuon(false);
                    for(muItr=muons.begin();muItr<muons.end();muItr++){
                        if ( (*muItr)->inDetTrackParticle()==(*trkItr) ) {trackIsMuon=true; break;}
                    }
                    if (!trackIsMuon) tracksToKeep.push_back(*trkItr);
                }
            }
        } else {tracksToKeep = tracks;}
        
        if(tracksToKeep.size()>=1 && muons.size()>=1){
            for(trkItr=tracksToKeep.begin();trkItr<tracksToKeep.end();trkItr++){
                for(muItr=muons.begin();muItr<muons.end();muItr++){
                    pair.muon1 = *muItr;
                    // Muon track 1st
                    pair.trackParticle1 = (*muItr)->inDetTrackParticle();
                    pair.trackParticle2 = *trkItr;
                    pair.pairType = MUTRK;
                    myPairs.push_back(pair);
                }
            }
        }
        
        return(myPairs);
    }
    
    
    
    // *********************************************************************************
    
    // ---------------------------------------------------------------------------------
    // getInvariantMass: returns invariant mass
    // ---------------------------------------------------------------------------------
    
    double JpsiFinder::getInvariantMass(JpsiCandidate jpsiIn,std::vector<double> massHypotheses){
        
        const Rec::TrackParticle* track1 = jpsiIn.trackParticle1;
        const Rec::TrackParticle* track2 = jpsiIn.trackParticle2;
        double mass1 = massHypotheses.at(0);
        double mass2 = massHypotheses.at(1);
        
        std::vector<double> px,py,pz,e;
        
        double px1 = track1->px();
        double py1 = track1->py();
        double pz1 = track1->pz();
        double px2 = track2->px();
        double py2 = track2->py();
        double pz2 = track2->pz();
        double e1 = sqrt(px1*px1+py1*py1+pz1*pz1+mass1*mass1);
        double e2 = sqrt(px2*px2+py2*py2+pz2*pz2+mass2*mass2);
        
        double pxSum = px1+px2;
        double pySum = py1+py2;
        double pzSum = pz1+pz2;
        double eSum = e1+e2;
        double M=sqrt((eSum*eSum)-(pxSum*pxSum)-(pySum*pySum)-(pzSum*pzSum));
        
        return M;
    }
    
    // ---------------------------------------------------------------------------------
    // selectCharges: selects track pairs with opposite charge / store + before -
    // Boolean argument is to decide whether to accept oppositely or identically charged
    // particles (true for oppositely charged)
    // ---------------------------------------------------------------------------------
    
    std::vector<JpsiCandidate> JpsiFinder::selectCharges(std::vector<JpsiCandidate> jpsisIn, std::string selection) {
        
        bool opposite(false),same(false),all(false);
        if (selection=="OPPOSITE") opposite=true;
        if (selection=="SAME") same=true;
        if (selection=="ALL") all=true;
        
        JpsiCandidate tmpJpsi;
        std::vector<JpsiCandidate> jpsis;
        std::vector<JpsiCandidate>::iterator jpsiItr;
        double qOverP1=0.;
        double qOverP2=0.;
        for(jpsiItr=jpsisIn.begin();jpsiItr!=jpsisIn.end();jpsiItr++){
            bool oppCh(false),sameCh(false);
            tmpJpsi = *jpsiItr;
            qOverP1=(*jpsiItr).trackParticle1->measuredPerigee()->parameters()[Trk::qOverP];
            qOverP2=(*jpsiItr).trackParticle2->measuredPerigee()->parameters()[Trk::qOverP];
            if(qOverP1*qOverP2<0.0) oppCh=true; // product charge < 0
            if(qOverP1*qOverP2>0.0) sameCh=true; // product charge > 0
            // +ve should be first so swap
            // Don't do it for tag and probe analyses (because tag muon must not change position)
            if (oppCh && qOverP1<0.0 && !m_doTagAndProbe) {
            	tmpJpsi.trackParticle1 = (*jpsiItr).trackParticle2;
            	tmpJpsi.trackParticle2 = (*jpsiItr).trackParticle1;
            	tmpJpsi.muon1 = (*jpsiItr).muon2;
            	tmpJpsi.muon2 = (*jpsiItr).muon1;
            	tmpJpsi.collection1 = (*jpsiItr).collection2;
            	tmpJpsi.collection2 = (*jpsiItr).collection1;
            }
            if (oppCh && (opposite || all) ) jpsis.push_back(tmpJpsi);
            if (sameCh && (same || all) ) jpsis.push_back(tmpJpsi);
            
        } // end of for loop
        
        return(jpsis);
    }
    
    // ---------------------------------------------------------------------------------
    // Apply the current cuts of the MCP group recommendation.
    // ---------------------------------------------------------------------------------
    
    bool JpsiFinder::passesMCPCuts(const Rec::TrackParticle* tp) {
        
        const Trk::TrackSummary* ts=tp->trackSummary();
        if (ts==NULL) return false;
        //if (ts->get(Trk::expectBLayerHit) && ts->get(Trk::numberOfBLayerHits)==0) return false;
        if (ts->get(Trk::numberOfPixelHits)+ts->get(Trk::numberOfPixelDeadSensors)==0) return false;
        if (ts->get(Trk::numberOfSCTHits)+ts->get(Trk::numberOfSCTDeadSensors)<5) return false;
        if (ts->get(Trk::numberOfPixelHoles)+ts->get(Trk::numberOfSCTHoles)>2) return false;
        int nTRTHits = ts->get(Trk::numberOfTRTHits);
        int nTRTOutliers = ts->get(Trk::numberOfTRTOutliers);
        int n = nTRTHits + nTRTOutliers;
        double dn = static_cast<double>(n);
        if (fabs(getEta(tp))>0.1 && fabs(getEta(tp))<1.9) {
            if (n<6 || nTRTOutliers >= 0.9*dn) return false;
        }
        //if ((fabs(getEta(tp)<=0.1) || fabs(getEta(tp))>=1.9) && n>5) {
        //    if (nTRTOutliers > 0.9*dn) return false;
        //}
        return true;
        
    }
    
    // ---------------------------------------------------------------------------------
    // Checks whether a TPB is in the collection
    // ---------------------------------------------------------------------------------
    
    bool JpsiFinder::isContainedIn(const Trk::TrackParticleBase* theTrack, const Trk::TrackParticleBaseCollection* theCollection) {
        
        bool isContained(false);
        Trk::TrackParticleBaseCollection::const_iterator tpbIt;
        for (tpbIt=theCollection->begin(); tpbIt!=theCollection->end(); ++tpbIt) {
            if ( *tpbIt == theTrack ) {
                isContained=true;
                break;
            }
        }
        return(isContained);
    }
    
    bool JpsiFinder::isContainedIn(const Rec::TrackParticle* theTrack, std::vector<const Rec::TrackParticle*> theCollection) {
        
        bool isContained(false);
        std::vector<const Rec::TrackParticle*>::iterator tpbIt;
        for (tpbIt=theCollection.begin(); tpbIt!=theCollection.end(); ++tpbIt) {
            if ( *tpbIt == theTrack ) {
                isContained=true;
                break;
            }
        }
        return(isContained);
    }
    
    
    
    
    
    
    
    
    
}
