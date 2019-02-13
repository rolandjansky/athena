/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// ****************************************************************************
// ----------------------------------------------------------------------------
// JpsiFinder_ee
// James Catmore <James.Catmore@cern.ch>
// Evelina Bouhova-Thacker <e.bouhova@cern.ch>
// Daniel.Scheirich <daniel.scheirich@cern.ch>
// Migration to xAOD
// ----------------------------------------------------------------------------
// ****************************************************************************

#include "JpsiUpsilonTools/JpsiFinder_ee.h"
#include "xAODBPhys/BPhysHelper.h"
#include "TrkVertexFitterInterfaces/IVertexFitter.h"
#include "TrkVKalVrtFitter/TrkVKalVrtFitter.h"
#include "TrkV0Fitter/TrkV0VertexFitter.h"
#include "InDetConversionFinderTools/VertexPointEstimator.h"
#include "TrkToolInterfaces/ITrackSelectorTool.h"
#include "GaudiKernel/IPartPropSvc.h"
#include "DataModel/ElementLink.h"

#include "xAODTracking/Vertex.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/VertexAuxContainer.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODEgamma/ElectronContainer.h"
#include "EventKernel/PdtPdg.h"
namespace Analysis {
    
    StatusCode JpsiFinder_ee::initialize() {
        
        // retrieving vertex Fitter
        if ( m_iVertexFitter.retrieve().isFailure() ) {
            ATH_MSG_FATAL("Failed to retrieve tool " << m_iVertexFitter);
            return StatusCode::FAILURE;
        } else {
            ATH_MSG_DEBUG("Retrieved tool " << m_iVertexFitter);
        }
        
        // retrieving V0 Fitter
        if ( m_iV0VertexFitter.retrieve().isFailure() ) {
            ATH_MSG_FATAL("Failed to retrieve tool " << m_iV0VertexFitter);
            return StatusCode::FAILURE;
        } else {
            ATH_MSG_DEBUG("Retrieved tool " << m_iV0VertexFitter);
        }
        
        // Get the track selector tool from ToolSvc
        if ( m_trkSelector.retrieve().isFailure() ) {
            ATH_MSG_FATAL("Failed to retrieve tool " << m_trkSelector);
            return StatusCode::FAILURE;
        } else {
            ATH_MSG_DEBUG("Retrieved tool " << m_trkSelector);
        }
        
       
        // Get the vertex point estimator tool from ToolSvc
        if ( m_vertexEstimator.retrieve().isFailure() ) {
            ATH_MSG_FATAL("Failed to retrieve tool " << m_vertexEstimator);
            return StatusCode::FAILURE;
        } else {
            ATH_MSG_DEBUG("Retrieved tool " << m_vertexEstimator);
        }

        
        // Get the Particle Properties Service
        IPartPropSvc* partPropSvc = 0;
        StatusCode sc = service("PartPropSvc", partPropSvc, true);
        if (sc.isFailure()) {
            ATH_MSG_ERROR("Could not initialize Particle Properties Service");
            return StatusCode::SUCCESS;
        } else {
            m_particleDataTable = partPropSvc->PDT();
            const HepPDT::ParticleData* pd_el = m_particleDataTable->particle(PDG::e_minus);
            if (m_diElectrons) {m_trk1M = pd_el->mass(); m_trk2M = pd_el->mass();}
        }
        
        if (m_doTagAndProbe) ATH_MSG_WARNING("You have requested tag and probe mode. Duplicate mu+trk pairs WILL be allowed, charge ordering WILL NOT be done. Tag track will be first in each candidate");
        

//        // Check that the user's settings are sensible
        bool illogicalOptions(false);
        if ( (m_elel && m_eltrk) || (m_elel && m_trktrk) || (m_eltrk && m_trktrk) ) {
            ATH_MSG_WARNING("You are requesting incompatible combinations of muons and tracks in the pairs. JpsiEECandidates will be EMPTY!");
            illogicalOptions=true;
        };
        if ( (m_doTagAndProbe && m_elel) || (m_doTagAndProbe && m_trktrk) ) {
           ATH_MSG_WARNING("You are requesting Tag and Probe analysis but have not requested mu+trk mode. This is impossible. JpsiEECandidates will be EMPTY!");
            illogicalOptions=true;
        };
        if ( (m_oppChOnly && m_sameChOnly) || (m_oppChOnly && m_allChCombs) || (m_sameChOnly && m_allChCombs) ) {
            ATH_MSG_WARNING("You are requesting incompatible combinations of charges in the pairs. JpsiEECandidates will be EMPTY!");
            illogicalOptions=true;
        };
        if ( (m_sameChOnly && m_doTagAndProbe) || (m_allChCombs && m_doTagAndProbe) ) {
            ATH_MSG_WARNING("You are requesting same-sign or all-sign combinations in a tag and probe analysis. This doesn't make sense. JpsiEECandidates will be EMPTY!");
            illogicalOptions=true;
        }
        if (illogicalOptions) return StatusCode::FAILURE;



        ATH_MSG_INFO("Initialize successful");
        
        return StatusCode::SUCCESS;
        
    }
    
    StatusCode JpsiFinder_ee::finalize() {
        
        ATH_MSG_DEBUG("Finalize successful, i ran on " << m_numberOfEventsWithJpsi << " events that had jpis from Bs");
        return StatusCode::SUCCESS;
        
    }
    JpsiFinder_ee::JpsiFinder_ee(const std::string& t, const std::string& n, const IInterface* p)  : AthAlgTool(t,n,p),
    m_elel(true),
    m_eltrk(false),
    m_trktrk(false),
    m_allElectrons(false),
    m_useTrackMeasurement(true),
    m_useV0Fitter(false),
    m_diElectrons(true),
    m_trk1M(0.511),
    m_trk2M(0.511),
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
    m_electronCollectionKey("Electrons"),
    m_TrkParticleCollection("InDetTrackParticles"),
    m_iVertexFitter("Trk::TrkVKalVrtFitter"),
    m_iV0VertexFitter("Trk::V0VertexFitter"),
    m_trkSelector("InDet::TrackSelectorTool"),
    m_vertexEstimator("InDet::VertexPointEstimator"),
    m_egammaCuts(true),
    m_elSelection("LHLoose"),
    m_doTagAndProbe(false),
    m_numberOfEventsWithJpsi(0)
    
    {
        declareInterface<JpsiFinder_ee>(this);
        declareProperty("elAndEl",m_elel);
        declareProperty("elAndTrack",m_eltrk);
        declareProperty("TrackAndTrack",m_trktrk);
        declareProperty("allElectrons",m_allElectrons);
        declareProperty("useElectronTrackMeasurement",m_useTrackMeasurement);
        declareProperty("useV0Fitter",m_useV0Fitter);
        declareProperty("assumeDiElectrons",m_diElectrons);
//        declareProperty("electronLHValue",m_electronLHValue);
        declareProperty("track1Mass",m_trk1M);
        declareProperty("track2Mass",m_trk2M);
        declareProperty("elThresholdPt",m_thresholdPt);
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
        declareProperty("electronCollectionKey",m_electronCollectionKey);
        declareProperty("TrackParticleCollection",m_TrkParticleCollection);
        declareProperty("TrkVertexFitterTool",m_iVertexFitter);
        declareProperty("V0VertexFitterTool",m_iV0VertexFitter);
        declareProperty("TrackSelectorTool",m_trkSelector);
        declareProperty("VertexPointEstimator",m_vertexEstimator);
        declareProperty("useEgammaCuts",m_egammaCuts);
        declareProperty("doTagAndProbe",m_doTagAndProbe);
        declareProperty("ElectronSelection",m_elSelection);
    }
    
    JpsiFinder_ee::~JpsiFinder_ee() { }
    
    //-------------------------------------------------------------------------------------
    // Find the candidates
    //-------------------------------------------------------------------------------------
 
  
  StatusCode JpsiFinder_ee::performSearch(xAOD::VertexContainer*& vxContainer, xAOD::VertexAuxContainer*& vxAuxContainer)
    {
        ATH_MSG_DEBUG( "JpsiFinder_ee::performSearch" );
        vxContainer = new xAOD::VertexContainer;
        vxAuxContainer = new xAOD::VertexAuxContainer;
        vxContainer->setStore(vxAuxContainer);
        
        // Get the ToolSvc
        IToolSvc* toolsvc;
        StatusCode sc1=service("ToolSvc",toolsvc);
        if (sc1.isFailure() ) {
            ATH_MSG_WARNING("Problem loading tool service. JpsiEECandidates will be EMPTY!");
            return StatusCode::SUCCESS;;
        };
        
        // Get the electrons from StoreGate
        const xAOD::ElectronContainer* importedElectronCollection;
        StatusCode sc = evtStore()->retrieve(importedElectronCollection,m_electronCollectionKey);
        if(sc.isFailure()){
            ATH_MSG_WARNING("No electron collection with key " << m_electronCollectionKey << " found in StoreGate. JpsiEECandidates will be EMPTY!");
            return StatusCode::SUCCESS;;
        }else{
            ATH_MSG_DEBUG("Found electron collections with key "<<m_electronCollectionKey);
        }
        ATH_MSG_DEBUG("Electron container size "<<importedElectronCollection->size());       

        // Get ID tracks
        const xAOD::TrackParticleContainer* importedTrackCollection(0);
        sc = evtStore()->retrieve(importedTrackCollection,m_TrkParticleCollection);
        if(sc.isFailure()){
            ATH_MSG_WARNING("No TrackParticle collection with name " << m_TrkParticleCollection << " found in StoreGate!");
            return StatusCode::SUCCESS;;
        } else {
        }
        
        // Typedef for vectors of tracks and muons
        typedef std::vector<const xAOD::TrackParticle*> TrackBag;
        typedef std::vector<const xAOD::Electron*> ElectronBag;
        
        // Select the inner detector tracks
        const xAOD::Vertex* vx = 0;
        TrackBag theIDTracksAfterSelection;
        if (m_trktrk || m_eltrk) {
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
        ElectronBag theElectronsAfterSelection;
        xAOD::ElectronContainer::const_iterator elItr;
        if (m_elel || m_eltrk) {
            for (elItr=importedElectronCollection->begin(); elItr!=importedElectronCollection->end(); ++elItr) {
                if ( *elItr == NULL ) continue;
	        if (!(*elItr)->trackParticleLink().isValid()) continue; // No electrons without ID tracks
	        const xAOD::TrackParticle* elTrk(0);
                elTrk = (*elItr)->trackParticleLink().cachedElement();
                
                if ( elTrk==NULL) continue;
                if ( !m_trkSelector->decision(*elTrk, vx) ) continue; // all ID tracks must pass basic tracking cuts
                if ( fabs(elTrk->pt())<m_thresholdPt ) continue; // higher pt cut if needed
        
                if ( m_egammaCuts && !passesEgammaCuts(*elItr)) continue; // egamma cuts
                theElectronsAfterSelection.push_back(*elItr);
            }
            if (theElectronsAfterSelection.size() == 0) return StatusCode::SUCCESS;;
            ATH_MSG_DEBUG("Number of electrons after selection: " << theElectronsAfterSelection.size());
        }
        
        // Sort into pairs - end result will be a vector of JpsiEECandidate structs
        std::vector<JpsiEECandidate> jpsiCandidates;
        if (m_elel) jpsiCandidates = getPairs(theElectronsAfterSelection);
        if (m_trktrk) jpsiCandidates = getPairs(theIDTracksAfterSelection);
        if (m_eltrk) jpsiCandidates = getPairs2Colls(theIDTracksAfterSelection,theElectronsAfterSelection,m_doTagAndProbe);
        if (jpsiCandidates.size() > 0) m_numberOfEventsWithJpsi++;

        ATH_MSG_DEBUG("Number of pairs with ee from a B decay: " << jpsiCandidates.size() );

        // Pair-wise selections
        std::vector<JpsiEECandidate>::iterator jpsiItr;
        
        // (1) Enforce one combined muon [deleted, no electron equivalent]
        
        // (2) Establish track content for candidates
        // and set the appropriate track collections for the combined muon tracks where appropriate (for saving to persistency later)
        
        // el+trk or trk+trk - always ID track collection
        if (m_eltrk || m_trktrk) {
            for (jpsiItr=jpsiCandidates.begin(); jpsiItr!=jpsiCandidates.end(); ++jpsiItr) {
                (*jpsiItr).collection1 = importedTrackCollection;
                (*jpsiItr).collection2 = importedTrackCollection;
            }
        }
        
        std::vector<const xAOD::TrackParticleContainer*>::iterator muTrkCollItr;
        if (m_elel) {
            for (jpsiItr=jpsiCandidates.begin(); jpsiItr!=jpsiCandidates.end(); ++jpsiItr) {
                if ( m_useTrackMeasurement ) {
                  (*jpsiItr).trackParticle1 = (*jpsiItr).el1->trackParticleLink().cachedElement();
                  (*jpsiItr).trackParticle2 = (*jpsiItr).el2->trackParticleLink().cachedElement();
                  (*jpsiItr).collection1 = importedTrackCollection;
                  (*jpsiItr).collection2 = importedTrackCollection;
                } else {
		   ATH_MSG_WARNING("Not setup for non-track electron measurements yet....");
		}
            } // iteration over candidates
        }
        
        
        // (3) Enforce higher track pt if requested
        if (m_higherPt>0.0) {
            int index(0);
            std::vector<int> listToDelete;
            std::vector<int>::reverse_iterator ii;
            for(jpsiItr=jpsiCandidates.begin(); jpsiItr!=jpsiCandidates.end();++jpsiItr,++index) {
              if( (fabs((*jpsiItr).trackParticle1->pt()) < m_higherPt) && (fabs((*jpsiItr).trackParticle2->pt()) < m_higherPt) ) listToDelete.push_back(index);
            }
            for (ii=listToDelete.rbegin(); ii!=listToDelete.rend(); ++ii) {
                jpsiCandidates.erase(jpsiCandidates.begin() + (*ii) );
            }
            ATH_MSG_DEBUG("Number of candidates after higherPt cut: " << jpsiCandidates.size() );
        }
        
        // (4) Select all opp/same charged track pairs
        std::vector<JpsiEECandidate> sortedJpsiEECandidates;
        if (m_oppChOnly) sortedJpsiEECandidates = selectCharges(jpsiCandidates,"OPPOSITE");
        if (m_sameChOnly) sortedJpsiEECandidates = selectCharges(jpsiCandidates,"SAME");
        if (m_allChCombs) sortedJpsiEECandidates = selectCharges(jpsiCandidates,"ALL");
        ATH_MSG_DEBUG("Number of candidates after charge selection: " << sortedJpsiEECandidates.size() );
        
        // (5) Select for decay angle, if requested
        if (m_collAnglePhi>0.0 && m_collAngleTheta>0.0) {
            int index(0);
            std::vector<int> listToDelete;
            std::vector<int>::reverse_iterator ii;
            for(jpsiItr=sortedJpsiEECandidates.begin(); jpsiItr!=sortedJpsiEECandidates.end();++jpsiItr,++index) {
                double deltatheta = fabs( (*jpsiItr).trackParticle1->theta() - (*jpsiItr).trackParticle2->theta() );
                // -3.14 < phi < +3.14 ==> correction
                double deltaphi = (*jpsiItr).trackParticle1->phi0() - (*jpsiItr).trackParticle2->phi0();
                while ( fabs(deltaphi) > M_PI ) deltaphi += ( deltaphi > 0. ) ? -2.*M_PI : 2.*M_PI;
                deltaphi = fabs(deltaphi);
                // perform the angle cuts
                if ((deltatheta > m_collAngleTheta) || (deltaphi > m_collAnglePhi)) listToDelete.push_back(index);
            }
            for (ii=listToDelete.rbegin(); ii!=listToDelete.rend(); ++ii) {
                sortedJpsiEECandidates.erase(sortedJpsiEECandidates.begin() + (*ii) );
            }
            ATH_MSG_DEBUG("Number of collimated candidates: " << sortedJpsiEECandidates.size() );
        }
        
        // (6) Select for invariant mass, if requested
        std::vector<double> trkMasses;
        trkMasses.push_back(m_trk1M);
        trkMasses.push_back(m_trk2M);
        if ( (m_invMassLower > 0.0) || (m_invMassUpper > 0.0) ) {
            int index(0);
            std::vector<int> listToDelete;
            std::vector<int>::reverse_iterator ii;
            for(jpsiItr=sortedJpsiEECandidates.begin(); jpsiItr!=sortedJpsiEECandidates.end(); ++jpsiItr,++index) {
                double invMass = getInvariantMass(*jpsiItr,trkMasses);
             //   std::cout << "inv. mass: " << invMass << std::endl;
                if ( invMass < m_invMassLower || invMass > m_invMassUpper ) {
                    listToDelete.push_back(index);
                }
            }
            for (ii=listToDelete.rbegin(); ii!=listToDelete.rend(); ++ii) {
                sortedJpsiEECandidates.erase(sortedJpsiEECandidates.begin() + (*ii) );
            }
            ATH_MSG_DEBUG("Number of candidates passing invariant mass selection: " << sortedJpsiEECandidates.size() );
        }
        
        if (sortedJpsiEECandidates.size() == 0) return StatusCode::SUCCESS;;
        
        // Fit each pair of tracks to a vertex
        int itritn = 0;
        for(jpsiItr=sortedJpsiEECandidates.begin(); jpsiItr!=sortedJpsiEECandidates.end(); ++jpsiItr) {
            ATH_MSG_DEBUG("jpsiItr: " << itritn); itritn++;
            std::vector<const xAOD::TrackParticle*> theTracks; theTracks.clear();
            theTracks.push_back((*jpsiItr).trackParticle1);
            theTracks.push_back((*jpsiItr).trackParticle2);
            ATH_MSG_DEBUG("theTracks size (should be two!) " << theTracks.size() << " being vertexed with tracks " << importedTrackCollection);
            xAOD::Vertex* myVxCandidate = fit(theTracks,importedTrackCollection); // This line actually does the fitting and object making
            if (myVxCandidate != 0) {
                // Chi2 cut if requested
                double chi2 = myVxCandidate->chiSquared();
                ATH_MSG_DEBUG("chi2 is: " << chi2);
                if (m_Chi2Cut == 0.0 || chi2 <= m_Chi2Cut) {             
                	// decorate the candidate with refitted tracks and muons via the BPhysHelper
                	xAOD::BPhysHelper jpsiHelper(myVxCandidate);
                	jpsiHelper.setRefTrks();
                	if (m_elel || m_eltrk) {
                	     std::vector<const xAOD::Electron*> theStoredElectrons;
                	     theStoredElectrons.push_back((*jpsiItr).el1);
                	     if (m_elel) theStoredElectrons.push_back((*jpsiItr).el2);
                	     jpsiHelper.setElectrons(theStoredElectrons,importedElectronCollection);
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
    
    xAOD::Vertex* JpsiFinder_ee::fit(const std::vector<const xAOD::TrackParticle*> &inputTracks,const xAOD::TrackParticleContainer* importedTrackCollection) {
        ATH_MSG_DEBUG("inside JpsiFinder_ee::fit");
        Trk::TrkV0VertexFitter* concreteVertexFitter=0;
        if (m_useV0Fitter) {
            ATH_MSG_DEBUG("using v0 fitter");
            // making a concrete fitter for the V0Fitter
            concreteVertexFitter = dynamic_cast<Trk::TrkV0VertexFitter * >(&(*m_iV0VertexFitter));
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
            ATH_MSG_DEBUG("Initial fit was a success!");
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
            ATH_MSG_DEBUG("Initial fit was a success! " << myVxCandidate);
            // Added by ASC
            if(myVxCandidate != 0){
            std::vector<ElementLink<DataVector<xAOD::TrackParticle> > > newLinkVector;
            for(unsigned int i=0; i< myVxCandidate->trackParticleLinks().size(); i++){ 
                ElementLink<DataVector<xAOD::TrackParticle> > mylink=myVxCandidate->trackParticleLinks()[i]; //makes a copy (non-const) 
                mylink.setStorableObject(*importedTrackCollection, true); 
                newLinkVector.push_back( mylink );
                ATH_MSG_DEBUG("Set a link!");
            }
            myVxCandidate->clearTracks();
            myVxCandidate->setTrackParticleLinks( newLinkVector );
            ATH_MSG_DEBUG("Set all links");
            }

            return myVxCandidate;
        }
      


        return NULL;
        
    } // End of fit method
    
    
    // *********************************************************************************
      
    // ---------------------------------------------------------------------------------
    // getPairs: forms up 2-plets of tracks
    // ---------------------------------------------------------------------------------
    
    std::vector<JpsiEECandidate> JpsiFinder_ee::getPairs(const std::vector<const xAOD::TrackParticle*> &TracksIn){
        
        std::vector<JpsiEECandidate> myPairs;
        JpsiEECandidate pair;
        std::vector<const xAOD::TrackParticle*>::const_iterator outerItr;
        std::vector<const xAOD::TrackParticle*>::const_iterator innerItr;
        
        if(TracksIn.size()>=2){
            for(outerItr=TracksIn.begin();outerItr<TracksIn.end();outerItr++){
                for(innerItr=(outerItr+1);innerItr!=TracksIn.end();innerItr++){
                    pair.trackParticle1 = *innerItr;
                    pair.trackParticle2 = *outerItr;
                    pair.pairType = TRK2;
                    myPairs.push_back(pair);
                }
            }
        }
        
        return(myPairs);
    }
    
    // *********************************************************************************
    
    // ---------------------------------------------------------------------------------
    // getPairs: forms up 2-plets of electrons
    // ---------------------------------------------------------------------------------
    
    std::vector<JpsiEECandidate> JpsiFinder_ee::getPairs(const std::vector<const xAOD::Electron*> &electronsIn){
        
        std::vector<JpsiEECandidate> myPairs;
        JpsiEECandidate pair;
        std::vector<const xAOD::Electron*>::const_iterator outerItr;
        std::vector<const xAOD::Electron*>::const_iterator innerItr;
        
        if(electronsIn.size()>=2){
            for(outerItr=electronsIn.begin();outerItr<electronsIn.end();outerItr++){
                for(innerItr=(outerItr+1);innerItr!=electronsIn.end();innerItr++){
//                    if((*outerItr)->auxdata<int>("truthOrigin") == 26 && (*innerItr)->auxdata<int>("truthOrigin")==26) ATH_MSG_DEBUG("pair is from jpsi!");
//                    else continue;
                    pair.el1 = *innerItr;
                    pair.el2 = *outerItr;
                    pair.pairType = ELEL;
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
    
    std::vector<JpsiEECandidate> JpsiFinder_ee::getPairs2Colls(const std::vector<const xAOD::TrackParticle*> &tracks, const std::vector<const xAOD::Electron*> &electrons, bool tagAndProbe){
        
        std::vector<JpsiEECandidate> myPairs;
        JpsiEECandidate pair;
        std::vector<const xAOD::TrackParticle*>::const_iterator trkItr;
        std::vector<const xAOD::Electron*>::const_iterator muItr;
        
        // Unless user is running in tag and probe mode, remove tracks which are also identified as muons
        std::vector<const xAOD::TrackParticle*> tracksToKeep;
        if (!tagAndProbe) {
            if(tracks.size()>=1 && electrons.size()>=1){
                for(trkItr=tracks.begin();trkItr<tracks.end();trkItr++){
                    bool trackIsElectron(false);
                    for(muItr=electrons.begin();muItr<electrons.end();muItr++){
                      if ( (*muItr)->trackParticleLink().cachedElement() == (*trkItr) ) {
                          trackIsElectron=true; 
                          break;
                        }
                    }
                    if (!trackIsElectron) tracksToKeep.push_back(*trkItr);
                }
            }
        } else {tracksToKeep = tracks;}
        
        if(tracksToKeep.size()>=1 && electrons.size()>=1){
            for(trkItr=tracksToKeep.begin();trkItr<tracksToKeep.end();trkItr++){
                for(muItr=electrons.begin();muItr<electrons.end();muItr++){
                    pair.el1 = *muItr;
                    // Muon track 1st
                    pair.trackParticle1 = (*muItr)->trackParticleLink().cachedElement();
                    pair.trackParticle2 = *trkItr;
                    pair.pairType = ELTRK;
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
    
    double JpsiFinder_ee::getInvariantMass(const JpsiEECandidate &jpsiIn, const std::vector<double> &massHypotheses){
      double mass1 = massHypotheses.at(0);
      double mass2 = massHypotheses.at(1);
      
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
    
    std::vector<JpsiEECandidate> JpsiFinder_ee::selectCharges(const std::vector<JpsiEECandidate> &jpsisIn, const std::string &selection) {
        
        bool opposite(false),same(false),all(false);
        if (selection=="OPPOSITE") opposite=true;
        if (selection=="SAME") same=true;
        if (selection=="ALL") all=true;
        
        JpsiEECandidate tmpJpsi;
        std::vector<JpsiEECandidate> jpsis;
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
            if (oppCh && qOverP1<0.0 && !m_doTagAndProbe) {
            	tmpJpsi.trackParticle1 = (*jpsiItr).trackParticle2;
            	tmpJpsi.trackParticle2 = (*jpsiItr).trackParticle1;
            	tmpJpsi.el1 = (*jpsiItr).el2;
            	tmpJpsi.el2 = (*jpsiItr).el1;
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
    
    bool JpsiFinder_ee::passesEgammaCuts(const xAOD::Electron* electron) {
                      
      bool passesSelection =  electron->passSelection(m_elSelection);
      ATH_MSG_DEBUG("Electron with pT, eta: " << electron->pt() << " " << electron->eta() << " passes " << m_elSelection << " " << passesSelection);
      return passesSelection;
        
    }
    
    // ---------------------------------------------------------------------------------
    // Checks whether a TPB is in the collection
    // ---------------------------------------------------------------------------------
    
    bool JpsiFinder_ee::isContainedIn(const xAOD::TrackParticle* theTrack, const xAOD::TrackParticleContainer* theCollection) {
        
        bool isContained(false);
        xAOD::TrackParticleContainer::const_iterator tpbIt;
        for (tpbIt=theCollection->begin(); tpbIt!=theCollection->end(); ++tpbIt) {
            if ( *tpbIt == theTrack ) {
                isContained=true;
                break;
            }
        }
        return(isContained);
    }
    
    // ---------------------------------------------------------------------------------
    // trackMomentum: returns refitted track momentum
    // ---------------------------------------------------------------------------------
    
    TVector3 JpsiFinder_ee::trackMomentum(const xAOD::Vertex * vxCandidate, int trkIndex) const
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
