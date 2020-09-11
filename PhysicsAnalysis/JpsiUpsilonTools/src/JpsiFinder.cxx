/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// ****************************************************************************
// ----------------------------------------------------------------------------
// JpsiFinder
// James Catmore <James.Catmore@cern.ch>
// Evelina Bouhova-Thacker <e.bouhova@cern.ch>
// Daniel.Scheirich <daniel.scheirich@cern.ch>
// Migration to xAOD
// ----------------------------------------------------------------------------
// ****************************************************************************

#include "JpsiUpsilonTools/JpsiFinder.h"
#include "xAODBPhys/BPhysHelper.h"
#include "TrkVertexFitterInterfaces/IVertexFitter.h"
#include "TrkVKalVrtFitter/TrkVKalVrtFitter.h"
#include "TrkV0Fitter/TrkV0VertexFitter.h"
#include "InDetConversionFinderTools/VertexPointEstimator.h"
#include "TrkToolInterfaces/ITrackSelectorTool.h"
#include "GaudiKernel/IPartPropSvc.h"
#include "AthLinks/ElementLink.h"
#include "InDetConversionFinderTools/ConversionFinderUtils.h"
#include "xAODTracking/Vertex.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/VertexAuxContainer.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "EventKernel/PdtPdg.h"

namespace Analysis {
    
    StatusCode JpsiFinder::initialize() {
        
        // retrieving vertex Fitter
        ATH_CHECK(m_iVertexFitter.retrieve());
        
        // retrieving V0 Fitter
        ATH_CHECK(m_iV0VertexFitter.retrieve());
        
        // Get the track selector tool from ToolSvc
        ATH_CHECK(m_trkSelector.retrieve());
        
        // Get the vertex point estimator tool from ToolSvc
        ATH_CHECK(m_vertexEstimator.retrieve());


        ATH_CHECK(m_muonCollectionKey.initialize());
        ATH_CHECK(m_TrkParticleCollection.initialize());
        ATH_CHECK(m_MuonTrackKeys.initialize(m_MuonTrackKeys.size() != 0));

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


//        // Check that the user's settings are sensible
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
        if (illogicalOptions) return StatusCode::FAILURE;;


        ATH_MSG_INFO("Initialize successful");
        
        return StatusCode::SUCCESS;
        
    }
    
    StatusCode JpsiFinder::finalize() {
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
    m_iVertexFitter("Trk::TrkVKalVrtFitter"),
    m_iV0VertexFitter("Trk::V0VertexFitter"),
    m_trkSelector("InDet::TrackSelectorTool"),
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
        declareProperty("TrkVertexFitterTool",m_iVertexFitter);
        declareProperty("V0VertexFitterTool",m_iV0VertexFitter);
        declareProperty("TrackSelectorTool",m_trkSelector);
        declareProperty("VertexPointEstimator",m_vertexEstimator);
        declareProperty("useMCPCuts",m_mcpCuts);
        declareProperty("doTagAndProbe",m_doTagAndProbe);
        declareProperty("MuonTrackKeys",m_MuonTrackKeys);
    }
    
    JpsiFinder::~JpsiFinder() { }
    
    //-------------------------------------------------------------------------------------
    // Find the candidates
    //-------------------------------------------------------------------------------------
    StatusCode JpsiFinder::performSearch(xAOD::VertexContainer*& vxContainer, xAOD::VertexAuxContainer*& vxAuxContainer)
    {
        ATH_MSG_DEBUG( "JpsiFinder::performSearch" );
        vxContainer = new xAOD::VertexContainer;
        vxAuxContainer = new xAOD::VertexAuxContainer;
        vxContainer->setStore(vxAuxContainer);

        
        SG::ReadHandle<xAOD::MuonContainer> muonhandle(m_muonCollectionKey);
        ATH_CHECK(muonhandle.isValid());
        // Get the muons from StoreGate
        const xAOD::MuonContainer* importedMuonCollection = muonhandle.cptr();
        ATH_MSG_DEBUG("Muon container size "<<importedMuonCollection->size());
        
        // Get muon tracks if combined measurement requested
        std::vector<const xAOD::TrackParticleContainer*> importedMuonTrackCollections;
        if (m_useCombMeasurement) {

            for (const auto& strItr : m_MuonTrackKeys) {
                SG::ReadHandle<xAOD::TrackParticleContainer> handle(strItr);
                if(!handle.isValid()){
                    ATH_MSG_WARNING("No muon TrackParticle collection with name " << strItr.key()  << " found in StoreGate!");
                    return StatusCode::FAILURE;
                } else {
                    ATH_MSG_DEBUG("Found muon TrackParticle collection " << strItr.key() << " in StoreGate!");
                    ATH_MSG_DEBUG("Muon TrackParticle container size "<< handle->size());
                    importedMuonTrackCollections.push_back(handle.cptr());
                }
            }
        }
        
        // Get ID tracks
        SG::ReadHandle<xAOD::TrackParticleContainer> handle(m_TrkParticleCollection);
        const xAOD::TrackParticleContainer* importedTrackCollection(0);
        if(!handle.isValid()){
            ATH_MSG_WARNING("No TrackParticle collection with name " << handle.key() << " found in StoreGate!");
            return StatusCode::FAILURE;
        } else {
            importedTrackCollection = handle.cptr();
            ATH_MSG_DEBUG("Found TrackParticle collection " << handle.key() << " in StoreGate!");
        }
        ATH_MSG_DEBUG("ID TrackParticle container size "<< handle->size());
        
        // Typedef for vectors of tracks and muons
        typedef std::vector<const xAOD::TrackParticle*> TrackBag;
        typedef std::vector<const xAOD::Muon*> MuonBag;
        
        // Select the inner detector tracks
        const xAOD::Vertex* vx = 0;
        TrackBag theIDTracksAfterSelection;
        if (m_trktrk || m_mutrk) {
            xAOD::TrackParticleContainer::const_iterator trkCItr;
            for (trkCItr=importedTrackCollection->begin(); trkCItr!=importedTrackCollection->end(); ++trkCItr) {
                const xAOD::TrackParticle* TP = (*trkCItr);
                if ( fabs(TP->pt())<m_trkThresholdPt ) continue;
                if ( !m_trkSelector->decision(*TP, vx) ) continue;
                theIDTracksAfterSelection.push_back(TP);
            }
            if (theIDTracksAfterSelection.size() == 0) return StatusCode::SUCCESS;;
            ATH_MSG_DEBUG("Number of tracks after ID track selection: " << theIDTracksAfterSelection.size());
        }
        
        // Select the muons
        MuonBag theMuonsAfterSelection;
        if (m_mumu || m_mutrk) {
            for (auto muItr=importedMuonCollection->begin(); muItr!=importedMuonCollection->end(); ++muItr) {
                if ( *muItr == NULL ) continue;
                if (!(*muItr)->inDetTrackParticleLink().isValid()) continue; // No muons without ID tracks
                const xAOD::TrackParticle* muonTrk = *((*muItr)->inDetTrackParticleLink());
                if ( muonTrk==NULL) continue;
                if ( !m_trkSelector->decision(*muonTrk, vx) ) continue; // all ID tracks must pass basic tracking cuts
                if ( fabs(muonTrk->pt())<m_thresholdPt ) continue; // higher pt cut if needed
                if ( m_mcpCuts && !passesMCPCuts(*muItr)) continue; // MCP cuts
                if ( m_combOnly && (*muItr)->muonType() != xAOD::Muon::Combined ) continue; // require combined muons
                if ( (*muItr)->muonType() == xAOD::Muon::SiliconAssociatedForwardMuon && !m_useCombMeasurement) continue;
                theMuonsAfterSelection.push_back(*muItr);
            }
            if (theMuonsAfterSelection.size() == 0) return StatusCode::SUCCESS;;
            ATH_MSG_DEBUG("Number of muons after selection: " << theMuonsAfterSelection.size());
        }
        
        // Sort into pairs - end result will be a vector of JpsiCandidate structs
        std::vector<JpsiCandidate> jpsiCandidates;
        if (m_mumu) jpsiCandidates = getPairs(theMuonsAfterSelection);
        if (m_trktrk) jpsiCandidates = getPairs(theIDTracksAfterSelection);
        if (m_mutrk) jpsiCandidates = getPairs2Colls(theIDTracksAfterSelection,theMuonsAfterSelection,m_doTagAndProbe);

        
        // (1) Enforce one combined muon
        if (m_atLeastOneComb) {
            std::vector<JpsiCandidate> selectCandidates;
            for(auto &cand : jpsiCandidates) { if(cand.muonTypes!=TT) selectCandidates.push_back(cand); }
            selectCandidates.swap(jpsiCandidates);
            ATH_MSG_DEBUG("Number of candidates after requirement of at least 1 combined muon: " << jpsiCandidates.size() );
        }
        
        // (2) Establish track content for candidates
        // and set the appropriate track collections for the combined muon tracks where appropriate (for saving to persistency later)
        
        // mu+trk or trk+trk - always ID track collection
        if (m_mutrk || m_trktrk) {
            for (auto jpsiItr=jpsiCandidates.begin(); jpsiItr!=jpsiCandidates.end(); ++jpsiItr) {
                (*jpsiItr).collection1 = importedTrackCollection;
                (*jpsiItr).collection2 = importedTrackCollection;
            }
        }
        
        if (m_mumu) {
            for (auto jpsiItr=jpsiCandidates.begin(); jpsiItr!=jpsiCandidates.end(); ++jpsiItr) {
                if ( (m_combOnly && !m_useCombMeasurement) || m_atLeastOneComb || m_allMuons) {
                  (*jpsiItr).trackParticle1 = (*jpsiItr).muon1->trackParticle( xAOD::Muon::InnerDetectorTrackParticle );
                  (*jpsiItr).trackParticle2 = (*jpsiItr).muon2->trackParticle( xAOD::Muon::InnerDetectorTrackParticle );
                  (*jpsiItr).collection1 = importedTrackCollection;
                  (*jpsiItr).collection2 = importedTrackCollection;
                }
                if (m_combOnly && m_useCombMeasurement) {
                    if (!(*jpsiItr).muon1->combinedTrackParticleLink().isValid()) {
                      (*jpsiItr).trackParticle1 = (*jpsiItr).muon1->trackParticle( xAOD::Muon::InnerDetectorTrackParticle );
                      (*jpsiItr).collection1 = importedTrackCollection;
                    }
                    
                    if (!(*jpsiItr).muon2->combinedTrackParticleLink().isValid()) {
                      (*jpsiItr).trackParticle2 = (*jpsiItr).muon2->trackParticle( xAOD::Muon::InnerDetectorTrackParticle );
                      (*jpsiItr).collection2 = importedTrackCollection;
                    }
                    
                    if ((*jpsiItr).muon1->combinedTrackParticleLink().isValid()) {
                        (*jpsiItr).trackParticle1 = (*jpsiItr).muon1->trackParticle( xAOD::Muon::CombinedTrackParticle );
                        bool foundCollection(false);
                        // Look for correct muon track container
                        for (auto muTrkCollItr=importedMuonTrackCollections.begin(); muTrkCollItr!=importedMuonTrackCollections.end(); ++muTrkCollItr) {
                            if (isContainedIn((*jpsiItr).trackParticle1,*muTrkCollItr)) { (*jpsiItr).collection1 = *muTrkCollItr; foundCollection=true; break;}
                        }
                        if (!foundCollection) { // didn't find the correct muon track container so go back to ID
                            (*jpsiItr).trackParticle1 = (*jpsiItr).muon1->trackParticle( xAOD::Muon::InnerDetectorTrackParticle );
                            (*jpsiItr).collection1 = importedTrackCollection;
                            ATH_MSG_WARNING("Muon track from muon of author " << (*jpsiItr).muon1->author() << " not found in muon track collections you have provided.");
                            ATH_MSG_WARNING("Defaulting to ID track collection - combined measurement will not be used");
                        }
                    }
                    
                    if ((*jpsiItr).muon2->combinedTrackParticleLink().isValid()) {
                        (*jpsiItr).trackParticle2 = (*jpsiItr).muon2->trackParticle( xAOD::Muon::CombinedTrackParticle );
                        bool foundCollection(false);
                        // Look for correct muon track container
                        for (auto muTrkCollItr=importedMuonTrackCollections.begin(); muTrkCollItr!=importedMuonTrackCollections.end(); ++muTrkCollItr) {
                            if (isContainedIn((*jpsiItr).trackParticle2,*muTrkCollItr)) { (*jpsiItr).collection2 = *muTrkCollItr; foundCollection=true; break;}
                        }
                        if (!foundCollection) { // didn't find the correct muon track container so go back to ID
                            (*jpsiItr).trackParticle2 = (*jpsiItr).muon2->trackParticle( xAOD::Muon::InnerDetectorTrackParticle );
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
            std::vector<JpsiCandidate> selectCandidates;
            for(auto& cand : jpsiCandidates){
                bool reject = (fabs(cand.trackParticle1->pt()) < m_higherPt) && (fabs(cand.trackParticle2->pt()) < m_higherPt);
                if(!reject) selectCandidates.push_back(cand);
            }
            selectCandidates.swap(jpsiCandidates);
            ATH_MSG_DEBUG("Number of candidates after higherPt cut: " << jpsiCandidates.size() );
        }
        
        // (4) Select all opp/same charged track pairs
        std::vector<JpsiCandidate> sortedJpsiCandidates;
        sortedJpsiCandidates = selectCharges(jpsiCandidates);

        ATH_MSG_DEBUG("Number of candidates after charge selection: " << sortedJpsiCandidates.size() );
        
        // (5) Select for decay angle, if requested
        if (m_collAnglePhi>0.0 && m_collAngleTheta>0.0) {
            std::vector<JpsiCandidate> selectCandidates;
            for(auto& cand : sortedJpsiCandidates){
                double deltatheta = fabs( cand.trackParticle1->theta() - cand.trackParticle2->theta() );
                double deltaphi = cand.trackParticle1->phi0() - cand.trackParticle2->phi0();
                while ( fabs(deltaphi) > M_PI ) deltaphi += ( deltaphi > 0. ) ? -2.*M_PI : 2.*M_PI;
                deltaphi = fabs(deltaphi);
                bool reject = (deltatheta > m_collAngleTheta) || (deltaphi > m_collAnglePhi);
                if(!reject) selectCandidates.push_back(cand);
            }
            sortedJpsiCandidates.swap(selectCandidates);
            ATH_MSG_DEBUG("Number of collimated candidates: " << sortedJpsiCandidates.size() );
        }
        
        // (6) Select for invariant mass, if requested
        std::vector<double> trkMasses;
        trkMasses.push_back(m_trk1M);
        trkMasses.push_back(m_trk2M);
        if ( (m_invMassLower > 0.0) || (m_invMassUpper > 0.0) ) {
            std::vector<JpsiCandidate> selectCandidates;
            for(auto& cand : sortedJpsiCandidates){
                double invMass = getInvariantMass(cand,trkMasses);
                bool reject = invMass < m_invMassLower || invMass > m_invMassUpper;
                if(!reject) selectCandidates.push_back(cand);
            }
            selectCandidates.swap(sortedJpsiCandidates);
            ATH_MSG_DEBUG("Number of candidates passing invariant mass selection: " << sortedJpsiCandidates.size() );
        }
        
        ATH_MSG_DEBUG("Number of pairs passing all selections and going to vertexing: " << sortedJpsiCandidates.size() );
        if (sortedJpsiCandidates.size() == 0) return StatusCode::SUCCESS;;
        std::vector<const xAOD::TrackParticle*> theTracks;
        std::vector<const xAOD::Muon*> theStoredMuons;
        // Fit each pair of tracks to a vertex
        for(auto jpsiItr=sortedJpsiCandidates.begin(); jpsiItr!=sortedJpsiCandidates.end(); ++jpsiItr) {
            theTracks.clear();
            theTracks.push_back((*jpsiItr).trackParticle1);
            theTracks.push_back((*jpsiItr).trackParticle2);
            xAOD::Vertex* myVxCandidate = fit(theTracks,importedTrackCollection); // This line actually does the fitting and object making
            if (myVxCandidate != 0) {
                // Chi2 cut if requested
                double chi2 = myVxCandidate->chiSquared();
                ATH_MSG_DEBUG("chi2 is: " << chi2);
                if (m_Chi2Cut <= 0.0 || chi2 <= m_Chi2Cut) {             
                	// decorate the candidate with refitted tracks and muons via the BPhysHelper
                	xAOD::BPhysHelper jpsiHelper(myVxCandidate);
                	bool validtrk = jpsiHelper.setRefTrks();
                    if(!validtrk) ATH_MSG_WARNING("Problem setting tracks " << __FILE__ << ':' << __LINE__);
                	if (m_mumu || m_mutrk) {
                         theStoredMuons.clear();
                	     theStoredMuons.push_back((*jpsiItr).muon1);
                	     if (m_mumu) theStoredMuons.push_back((*jpsiItr).muon2);
                	     bool valid = jpsiHelper.setMuons(theStoredMuons,importedMuonCollection);
                         if(!valid) ATH_MSG_WARNING("Problem setting muons " << __FILE__ << ':' << __LINE__);
                	}
                	// Retain the vertex
                    vxContainer->push_back(myVxCandidate);       
                } else { // chi2 cut failed
                    delete myVxCandidate;
                }
            } else { // fit failed
                ATH_MSG_DEBUG("Fitter failed!");
                // Don't try to delete the object, since we arrived here,
                // because this pointer is null...
                //delete myVxCandidate;
            }
        }
        ATH_MSG_DEBUG("vxContainer size " << vxContainer->size());
        
        return StatusCode::SUCCESS;;
    }
    
    // *********************************************************************************
    
    // ---------------------------------------------------------------------------------
    // fit - does the fit
    // ---------------------------------------------------------------------------------
    
    xAOD::Vertex* JpsiFinder::fit(const std::vector<const xAOD::TrackParticle*> &inputTracks,const xAOD::TrackParticleContainer* importedTrackCollection) const {
        
        const Trk::TrkV0VertexFitter* concreteVertexFitter=0;
        if (m_useV0Fitter) {
            // making a concrete fitter for the V0Fitter
            concreteVertexFitter = dynamic_cast<const Trk::TrkV0VertexFitter * >(m_iV0VertexFitter.get());
            if(concreteVertexFitter == 0) {
                ATH_MSG_FATAL("The vertex fitter passed is not a V0 Vertex Fitter");
                return NULL;
            }
        }
        
        const Trk::Perigee& aPerigee1 = inputTracks[0]->perigeeParameters();
        const Trk::Perigee& aPerigee2 = inputTracks[1]->perigeeParameters();
        int sflag = 0;
        int errorcode = 0;
        Amg::Vector3D startingPoint = m_vertexEstimator->getCirclesIntersectionPoint(&aPerigee1,&aPerigee2,sflag,errorcode);
        if (errorcode != 0) {startingPoint(0) = 0.0; startingPoint(1) = 0.0; startingPoint(2) = 0.0;}
        if (m_useV0Fitter) {
            xAOD::Vertex* myVxCandidate = concreteVertexFitter->fit(inputTracks, startingPoint);

            // Added by ASC
            if(myVxCandidate != 0){
            std::vector<ElementLink<DataVector<xAOD::TrackParticle> > > newLinkVector;
            for(unsigned int i=0; i< myVxCandidate->trackParticleLinks().size(); i++)
            { ElementLink<DataVector<xAOD::TrackParticle> > mylink=myVxCandidate->trackParticleLinks()[i]; //makes a copy (non-const) 
            mylink.setStorableObject(*importedTrackCollection, true); 
            newLinkVector.push_back( mylink ); }
            
            myVxCandidate->clearTracks();
            myVxCandidate->setTrackParticleLinks( newLinkVector );
            }
            


            return myVxCandidate;
        } else {
            xAOD::Vertex* myVxCandidate = m_iVertexFitter->fit(inputTracks, startingPoint);

            // Added by ASC
            if(myVxCandidate != 0){
            std::vector<ElementLink<DataVector<xAOD::TrackParticle> > > newLinkVector;
            for(unsigned int i=0; i< myVxCandidate->trackParticleLinks().size(); i++)
            { ElementLink<DataVector<xAOD::TrackParticle> > mylink=myVxCandidate->trackParticleLinks()[i]; //makes a copy (non-const) 
            mylink.setStorableObject(*importedTrackCollection, true); 
            newLinkVector.push_back( mylink ); }
            
            myVxCandidate->clearTracks();
            myVxCandidate->setTrackParticleLinks( newLinkVector );
            }


            return myVxCandidate;
        }
      


        return NULL;
        
    } // End of fit method
    
    
    // *********************************************************************************
      
    // ---------------------------------------------------------------------------------
    // getPairs: forms up 2-plets of tracks
    // ---------------------------------------------------------------------------------
    
    std::vector<JpsiCandidate> JpsiFinder::getPairs(const std::vector<const xAOD::TrackParticle*> &TracksIn) const {
        
        std::vector<JpsiCandidate> myPairs;
        JpsiCandidate pair;
        std::vector<const xAOD::TrackParticle*>::const_iterator outerItr;
        std::vector<const xAOD::TrackParticle*>::const_iterator innerItr;
        
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
    
    std::vector<JpsiCandidate> JpsiFinder::getPairs(const std::vector<const xAOD::Muon*> &muonsIn) const {
        
        std::vector<JpsiCandidate> myPairs;
        JpsiCandidate pair;
        std::vector<const xAOD::Muon*>::const_iterator outerItr;
        std::vector<const xAOD::Muon*>::const_iterator innerItr;
        
        if(muonsIn.size()>=2){
            for(outerItr=muonsIn.begin();outerItr<muonsIn.end();outerItr++){
                for(innerItr=(outerItr+1);innerItr!=muonsIn.end();innerItr++){
                    pair.muon1 = *innerItr;
                    pair.muon2 = *outerItr;
                    pair.pairType = MUMU;
                    bool mu1Comb( (*innerItr)->muonType() == xAOD::Muon::Combined );
                    bool mu2Comb( (*outerItr)->muonType() == xAOD::Muon::Combined );
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
    
    std::vector<JpsiCandidate> JpsiFinder::getPairs2Colls(const std::vector<const xAOD::TrackParticle*> &tracks, const std::vector<const xAOD::Muon*> &muons, bool tagAndProbe) const {
        
        std::vector<JpsiCandidate> myPairs;
        JpsiCandidate pair;
        std::vector<const xAOD::TrackParticle*>::const_iterator trkItr;
        std::vector<const xAOD::Muon*>::const_iterator muItr;
        
        // Unless user is running in tag and probe mode, remove tracks which are also identified as muons
        std::vector<const xAOD::TrackParticle*> tracksToKeep;
        if (!tagAndProbe) {
            if(tracks.size()>=1 && muons.size()>=1){
                for(trkItr=tracks.begin();trkItr<tracks.end();trkItr++){
                    bool trackIsMuon(false);
                    for(muItr=muons.begin();muItr<muons.end();muItr++){
                      auto& link = ( *muItr )->inDetTrackParticleLink();
                      if ( link.isValid() &&  *link == (*trkItr) ) {
                          trackIsMuon=true; 
                          break;
                        }
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
                    pair.trackParticle1 = (*muItr)->trackParticle( xAOD::Muon::InnerDetectorTrackParticle );
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
    
    double JpsiFinder::getInvariantMass(const JpsiCandidate &jpsiIn, const std::vector<double> &massHypotheses) const {
      double mass1 = massHypotheses[0];
      double mass2 = massHypotheses[1];
      
      // construct 4-vectors from track perigee parameters using given mass hypotheses.
      // NOTE: in new data model (xAOD) the defining parameters are expressed as perigee parameters w.r.t. the beamspot
      // NOTE2: TrackParticle::p4() method already returns TLorentzVector, however, we want to enforce our own mass hypothesis
      TLorentzVector mu1;
      TLorentzVector mu2;
      mu1.SetVectM(jpsiIn.trackParticle1->p4().Vect(), mass1);
      mu2.SetVectM(jpsiIn.trackParticle2->p4().Vect(), mass2);
      
      return (mu1+mu2).M();
        
    }
    
    // ---------------------------------------------------------------------------------
    // selectCharges: selects track pairs with opposite charge / store + before -
    // Boolean argument is to decide whether to accept oppositely or identically charged
    // particles (true for oppositely charged)
    // ---------------------------------------------------------------------------------
    
    std::vector<JpsiCandidate> JpsiFinder::selectCharges(const std::vector<JpsiCandidate> &jpsisIn) const {
        
        bool opposite(false),same(false),all(false);
        opposite=m_oppChOnly;
        same=m_sameChOnly;
        all=m_allChCombs;
        
        JpsiCandidate tmpJpsi;
        std::vector<JpsiCandidate> jpsis;
        double qOverP1=0.;
        double qOverP2=0.;
        for(auto jpsiItr=jpsisIn.cbegin();jpsiItr!=jpsisIn.cend();jpsiItr++){
            bool oppCh(false),sameCh(false);
            tmpJpsi = *jpsiItr;
            qOverP1=(*jpsiItr).trackParticle1->qOverP();
            qOverP2=(*jpsiItr).trackParticle2->qOverP();
            if(qOverP1*qOverP2<0.0) oppCh=true; // product charge < 0
            if(qOverP1*qOverP2>0.0) sameCh=true; // product charge > 0
            // +ve should be first so swap
            // Don't do it for tag and probe analyses (because tag muon must not change position)
            if (oppCh && qOverP1<0.0 && !m_doTagAndProbe && !m_mutrk) {
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
    
    bool JpsiFinder::passesMCPCuts(const xAOD::Muon* muon) const {
    
      return muon->passesIDCuts();
        
    }
    
    // ---------------------------------------------------------------------------------
    // Checks whether a TPB is in the collection
    // ---------------------------------------------------------------------------------
    
    bool JpsiFinder::isContainedIn(const xAOD::TrackParticle* theTrack, const xAOD::TrackParticleContainer* theCollection) const {
        return std::find(theCollection->begin(), theCollection->end(), theTrack) != theCollection->end();
    }
    
    // ---------------------------------------------------------------------------------
    // trackMomentum: returns refitted track momentum
    // ---------------------------------------------------------------------------------
    
    TVector3 JpsiFinder::trackMomentum(const xAOD::Vertex * vxCandidate, int trkIndex) const
    {
      double px = 0., py = 0., pz = 0.;
      if (0 != vxCandidate) {
        const Trk::TrackParameters* aPerigee = vxCandidate->vxTrackAtVertex()[trkIndex].perigeeAtVertex();
        px = aPerigee->momentum()[Trk::px];
        py = aPerigee->momentum()[Trk::py];
        pz = aPerigee->momentum()[Trk::pz];
      }
      TVector3 mom(px,py,pz);
      return mom;
    }
   
}
