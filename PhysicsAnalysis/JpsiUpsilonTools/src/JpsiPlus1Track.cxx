/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ****************************************************************************
// ----------------------------------------------------------------------------
// JpsiPlus1Track
// James Catmore <James.Catmore@cern.ch>
// Results returned as a vector of xAOD::Vertex
// ----------------------------------------------------------------------------
// ****************************************************************************

#include "JpsiUpsilonTools/JpsiPlus1Track.h"
#include "xAODBPhys/BPhysHelper.h"
#include "TrkVertexFitterInterfaces/IVertexFitter.h"
#include "TrkVKalVrtFitter/TrkVKalVrtFitter.h"
#include "TrkToolInterfaces/ITrackSelectorTool.h"
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/IPartPropSvc.h"
#include "AthLinks/ElementLink.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "InDetConversionFinderTools/VertexPointEstimator.h"
#include <memory>
namespace Analysis {
    
    StatusCode JpsiPlus1Track::initialize() {
        
        // retrieving vertex Fitter
        if ( m_iVertexFitter.retrieve().isFailure() ) {
            ATH_MSG_FATAL("Failed to retrieve tool " << m_iVertexFitter);
            return StatusCode::FAILURE;
        } else {
            ATH_MSG_INFO("Retrieved tool " << m_iVertexFitter);
        }
        m_VKVFitter = dynamic_cast<Trk::TrkVKalVrtFitter*>(&(*m_iVertexFitter));
        
        // Get the track selector tool from ToolSvc
        if ( m_trkSelector.retrieve().isFailure() ) {
            ATH_MSG_FATAL("Failed to retrieve tool " << m_trkSelector);
            return StatusCode::FAILURE;
        } else {
            ATH_MSG_INFO("Retrieved tool " << m_trkSelector);
        }
        
        // Get the vertex point estimator tool from ToolSvc
        if ( m_vertexEstimator.retrieve().isFailure() ) {
            ATH_MSG_FATAL("Failed to retrieve tool " << m_vertexEstimator);
            return StatusCode::FAILURE;
        } else {
            ATH_MSG_INFO("Retrieved tool " << m_vertexEstimator);
        }
 
        // Get the Particle Properties Service
        IPartPropSvc* partPropSvc = 0;
        StatusCode sc = service("PartPropSvc", partPropSvc, true);
        if (sc.isFailure()) {
            ATH_MSG_ERROR("Could not initialize Particle Properties Service");
            return StatusCode::SUCCESS;
        } else {
            m_particleDataTable = partPropSvc->PDT();
        }
        
        ATH_MSG_INFO("Initialize successful");
        
        return StatusCode::SUCCESS;
        
    }
    
    StatusCode JpsiPlus1Track::finalize() {
        
        ATH_MSG_INFO("Finalize successful");
        return StatusCode::SUCCESS;
        
    }
    
    JpsiPlus1Track::JpsiPlus1Track(const std::string& t, const std::string& n, const IInterface* p)  : AthAlgTool(t,n,p),
    m_particleDataTable(0),
    m_piMassHyp(false),
    m_kMassHyp(true),
    m_trkThresholdPt(0.0),
    m_trkMaxEta(102.5),
    m_BThresholdPt(0.0),
    m_BMassUpper(0.0),
    m_BMassLower(0.0),
    m_jpsiCollectionKey("JpsiCandidates"),
    m_jpsiMassUpper(0.0),
    m_jpsiMassLower(0.0),
    m_TrkParticleCollection("TrackParticleCandidate"),
    m_MuonsUsedInJpsi("NONE"),
    m_excludeCrossJpsiTracks(true),
    m_iVertexFitter("Trk::TrkVKalVrtFitter"),
    m_trkSelector("InDet::TrackSelectorTool"),
    m_vertexEstimator("InDet::VertexPointEstimator"),
    m_useMassConst(true),
    m_altMassConst(-1.0),
    m_chi2cut(-1.0),
    m_trkTrippletMassUpper(-1.0),
    m_trkTrippletMassLower(-1.0),
    m_trkTrippletPt(-1.0),
    m_trkDeltaZ(-1.0)
    {
        declareInterface<JpsiPlus1Track>(this);
        declareProperty("pionHypothesis",m_piMassHyp);
        declareProperty("kaonHypothesis",m_kMassHyp);
        declareProperty("trkThresholdPt",m_trkThresholdPt);
        declareProperty("trkMaxEta",m_trkMaxEta);
        declareProperty("BThresholdPt",m_BThresholdPt);
        declareProperty("BMassUpper",m_BMassUpper);
        declareProperty("BMassLower",m_BMassLower);
        declareProperty("JpsiContainerKey",m_jpsiCollectionKey);
        declareProperty("JpsiMassUpper",m_jpsiMassUpper);
        declareProperty("JpsiMassLower",m_jpsiMassLower);
        declareProperty("TrackParticleCollection",m_TrkParticleCollection);
        declareProperty("MuonsUsedInJpsi",m_MuonsUsedInJpsi);
        declareProperty("ExcludeCrossJpsiTracks",m_excludeCrossJpsiTracks); //Essential when trying to make vertices out of multiple muons (set to false)
        declareProperty("TrkVertexFitterTool",m_iVertexFitter);
        declareProperty("TrackSelectorTool", m_trkSelector);
        declareProperty("UseMassConstraint", m_useMassConst);
        declareProperty("AlternativeMassConstraint",m_altMassConst);

        // additional cuts by Daniel Scheirich copied from 2Tracks by Adam Barton
        declareProperty("Chi2Cut",m_chi2cut);
        declareProperty("TrkTrippletMassUpper"  ,m_trkTrippletMassUpper);
        declareProperty("TrkTrippletMassLower"  ,m_trkTrippletMassLower);
        declareProperty("TrkQuadrupletPt"       ,m_trkTrippletPt       );
        declareProperty("TrkDeltaZ"             ,m_trkDeltaZ           );

    }
    
    JpsiPlus1Track::~JpsiPlus1Track() {}
    
    //-------------------------------------------------------------------------------------
    // Find the candidates
    //-------------------------------------------------------------------------------------
    StatusCode JpsiPlus1Track::performSearch(xAOD::VertexContainer*& bContainer, xAOD::VertexAuxContainer*& bAuxContainer)
    {
        ATH_MSG_DEBUG( "JpsiPlus1Track::performSearch" );
        bContainer = new xAOD::VertexContainer;
        bAuxContainer = new xAOD::VertexAuxContainer;
        bContainer->setStore(bAuxContainer);
        
        // Get the ToolSvc
        IToolSvc* toolsvc;
        StatusCode sc1=service("ToolSvc",toolsvc);
        if (sc1.isFailure() ) {
            ATH_MSG_ERROR("Problem loading tool service. BContainer will be EMPTY!");
            return StatusCode::FAILURE;
        };
        
        // Set masses
        double muMass = 105.658;
        double kMass = 493.677;
        double piMass = 139.57;
        
        // Get the J/psis from StoreGate
        const xAOD::VertexContainer* importedJpsiCollection(0);
        StatusCode sc = evtStore()->retrieve(importedJpsiCollection,m_jpsiCollectionKey);
        if(sc.isFailure()){
            ATH_MSG_ERROR("No VertexContainer with key " << m_jpsiCollectionKey << " found in StoreGate. BCandidates will be EMPTY!");
            return StatusCode::FAILURE;
        }else{
            ATH_MSG_DEBUG("Found VxCandidate container with key "<<m_jpsiCollectionKey);
        }
        ATH_MSG_DEBUG("VxCandidate container size " << importedJpsiCollection->size());
        
        // Get tracks
        const xAOD::TrackParticleContainer* importedTrackCollection(0);
        sc = evtStore()->retrieve(importedTrackCollection,m_TrkParticleCollection);
        if(sc.isFailure()){
            ATH_MSG_ERROR("No track particle collection with name " << m_TrkParticleCollection << " found in StoreGate!");
            return StatusCode::FAILURE;
        } else {
            ATH_MSG_DEBUG("Found track particle collection " << m_TrkParticleCollection << " in StoreGate!");
        }
        ATH_MSG_DEBUG("Track container size "<< importedTrackCollection->size());
        
        // Get the muon collection used to build the J/psis
        const xAOD::MuonContainer* importedMuonCollection(0);
        if (m_MuonsUsedInJpsi!="NONE") {
            sc = evtStore()->retrieve(importedMuonCollection,m_MuonsUsedInJpsi);
            if (sc.isFailure()){
                ATH_MSG_ERROR("No muon collection with name " << m_MuonsUsedInJpsi << " found in StoreGate!");
                return StatusCode::FAILURE;
            } else {
                ATH_MSG_DEBUG("Found muon collection " << m_MuonsUsedInJpsi << " in StoreGate!");
            }
            ATH_MSG_DEBUG("Muon container size "<< importedMuonCollection->size());
        }
        
        // Typedef for vectors of tracks and VxCandidates
        typedef std::vector<const xAOD::TrackParticle*> TrackBag;
        
        // Select the inner detector tracks
        const xAOD::Vertex* vx = 0;
        TrackBag theIDTracksAfterSelection;
        xAOD::TrackParticleContainer::const_iterator trkPBItr;
        for (trkPBItr=importedTrackCollection->begin(); trkPBItr!=importedTrackCollection->end(); ++trkPBItr) {
            const xAOD::TrackParticle* tp (*trkPBItr);
            if ( tp->pt()<m_trkThresholdPt ) continue;
            if ( fabs(tp->eta())>m_trkMaxEta ) continue;
            if (importedMuonCollection!=NULL) {
                if (isContainedIn(tp,importedMuonCollection)) continue;
            }
            if ( m_trkSelector->decision(*tp, vx) ) theIDTracksAfterSelection.push_back(tp);
        }
        if (theIDTracksAfterSelection.empty()) return StatusCode::SUCCESS;
        ATH_MSG_DEBUG("Number of tracks after ID trkSelector: " << theIDTracksAfterSelection.size());
        
        // Set vector of muon masses
        std::vector<double> muonMasses = {muMass , muMass};

        
        // Loop over J/psi candidates, select, collect up tracks used to build a J/psi
        std::vector<const xAOD::Vertex*> selectedJpsiCandidates;
        std::vector<const xAOD::TrackParticle*> jpsiTracks;
        xAOD::VertexContainer::const_iterator vxcItr;
        for(vxcItr=importedJpsiCollection->begin(); vxcItr!=importedJpsiCollection->end(); ++vxcItr) {
            // Check J/psi candidate invariant mass and skip if need be
            if (m_jpsiMassUpper>0.0) {
                xAOD::BPhysHelper jpsiCandidate(*vxcItr);
                jpsiCandidate.setRefTrks();
                double jpsiMass = jpsiCandidate.totalP(muonMasses).M();
                if (jpsiMass<m_jpsiMassLower || jpsiMass>m_jpsiMassUpper) continue;
            }
            selectedJpsiCandidates.push_back(*vxcItr);
            // Extract tracks from J/psi
            const xAOD::TrackParticle* jpsiTP1 = (*vxcItr)->trackParticle(0);
            const xAOD::TrackParticle* jpsiTP2 = (*vxcItr)->trackParticle(1);
            // Collect up tracks
	    if(m_excludeCrossJpsiTracks){
            	jpsiTracks.push_back(jpsiTP1);
            	jpsiTracks.push_back(jpsiTP2);
	    }
        }


        std::vector<double>  massHypotheses;
        if (m_kMassHyp) massHypotheses.push_back(kMass);
        if (m_piMassHyp) massHypotheses.push_back(piMass);
        if (!m_kMassHyp && !m_piMassHyp && m_BMassUpper>0.0) {
            massHypotheses.push_back(kMass); massHypotheses.push_back(piMass);
        }        
        std::vector<double> tripletMasses;
        tripletMasses.push_back(muMass); tripletMasses.push_back(muMass);
        // Attempt to fit each track with the two tracks from the J/psi candidates
        // Loop over J/psis
        std::vector<const xAOD::Vertex*>::iterator jpsiItr;
        for(jpsiItr=selectedJpsiCandidates.begin(); jpsiItr!=selectedJpsiCandidates.end(); ++jpsiItr) {

            if (m_jpsiMassUpper>0.0) {
               xAOD::BPhysHelper jpsiCandidate(*jpsiItr);
               jpsiCandidate.setRefTrks();
            // Check J/psi candidate invariant mass and skip if need be
               double jpsiMass = jpsiCandidate.totalP(muonMasses).M();
               if (jpsiMass<m_jpsiMassLower || jpsiMass>m_jpsiMassUpper) continue;
            }
            // Extract tracks from J/psi
            const xAOD::TrackParticle* jpsiTP1 = (*jpsiItr)->trackParticle(0);
            const xAOD::TrackParticle* jpsiTP2 = (*jpsiItr)->trackParticle(1);

	    if(!m_excludeCrossJpsiTracks){
	       jpsiTracks.clear();
	       jpsiTracks.push_back(jpsiTP1);
               jpsiTracks.push_back(jpsiTP2);
	    }

            // Loop over ID tracks, call vertexing
            std::vector<const xAOD::TrackParticle*>::iterator trkItr;
            for (trkItr=theIDTracksAfterSelection.begin(); trkItr!=theIDTracksAfterSelection.end(); ++trkItr) {
                if (isContainedIn(*trkItr,jpsiTracks)) continue; // remove tracks which were used to build J/psi
                // Convert to TrackParticleBase
                const xAOD::TrackParticle* theThirdTP = *trkItr;

                if (m_trkTrippletPt>0 && getPt(jpsiTP1, jpsiTP2, theThirdTP) < m_trkTrippletPt ) continue; // track tripplet pT cut (daniel Scheirich)
                if(m_trkDeltaZ>0 &&
                   fabs(theThirdTP->z0() - (*jpsiItr)->z()) > m_trkDeltaZ )
                    continue;
                    // apply mass cut on track tripplet if requested
                bool passes3TrackMassUpper(true);
                bool passes3TrackMassLower(true);
                if (m_trkTrippletMassUpper>0.0 || m_trkTrippletMassLower>0.0) {
                     double jpsik   = getInvariantMass(jpsiTP1, muMass, jpsiTP2, muMass, theThirdTP,kMass );
                     double jpsipi = getInvariantMass(jpsiTP1, muMass, jpsiTP2, muMass, theThirdTP,piMass);
                     if (m_trkTrippletMassUpper>0.0) {
                         if (m_kMassHyp && jpsik>m_trkTrippletMassUpper) passes3TrackMassUpper=false;
                         if (m_piMassHyp && jpsipi>m_trkTrippletMassUpper) passes3TrackMassUpper=false;
                     }
                     if (m_trkTrippletMassLower>0.0) {
                         if (m_kMassHyp && jpsik<m_trkTrippletMassLower) passes3TrackMassLower=false;
                         if (m_piMassHyp && jpsipi<m_trkTrippletMassLower) passes3TrackMassLower=false;
                     }
                 }
                 if (!passes3TrackMassUpper||!passes3TrackMassLower) continue;


                //Managed pointer, "release" if you don't want it deleted. Automatically "deleted" otherwise
                std::unique_ptr<xAOD::Vertex> bVertex( fit(jpsiTP1,jpsiTP2,theThirdTP,m_useMassConst,m_altMassConst,importedTrackCollection));
                if (bVertex) {

                        // Chi2/DOF cut
                    double bChi2DOF = bVertex->chiSquared()/bVertex->numberDoF();
                    ATH_MSG_DEBUG("Candidate chi2/DOF is " << bChi2DOF);
                        
                    bool chi2CutPassed = (m_chi2cut <= 0.0 || bChi2DOF < m_chi2cut);
                    if(!chi2CutPassed) { ATH_MSG_DEBUG("Chi Cut failed!"); continue; }

                    // create the helper class
                    xAOD::BPhysHelper bHelper(bVertex.get());
                    // setRefTrks needs to be called after BPhysHelper is created if you want to access refitted track parameters
                    bHelper.setRefTrks();
                    // Decide whether to keep the candidate
                    bool massCutPassed(true);
                    bool PtPassed(m_BThresholdPt <= 0.0);
                    if (m_BMassUpper>0.0 || m_BThresholdPt >0.0) {
                        bool lowerPassed(false); bool upperPassed(false);
                        
                        for (double masshypo3rd : massHypotheses) {
                            tripletMasses.push_back(masshypo3rd); //Add third mass
                            TLorentzVector bMomentum = bHelper.totalP(tripletMasses);//Calulcate result
                            tripletMasses.pop_back(); //Remove 3rd mass - now same as beginning
                            double bpt = bMomentum.Pt();
                            PtPassed |=  (bpt >= m_BThresholdPt);// |= OR operator
                            double bMass = bMomentum.M();
                            ATH_MSG_DEBUG("candidate pt/mass under track mass hypothesis of " << masshypo3rd << " is " << bpt << " / " << bMass);
                            if ( bMass > m_BMassLower ) lowerPassed=true;
                            if ( bMass < m_BMassUpper ) upperPassed=true;
                            
                        }
                        massCutPassed= (massCutPassed<= 0.0) ||  (lowerPassed && upperPassed);
                    }
                    bool passesCuts = ( massCutPassed && PtPassed );
                    if ((*jpsiItr)!=NULL && passesCuts) {
                        // Set links to J/psi
                        std::vector<const xAOD::Vertex*> theJpsiPreceding;
                        theJpsiPreceding.push_back(*jpsiItr);
                        bHelper.setPrecedingVertices(theJpsiPreceding, importedJpsiCollection);
                        bContainer->push_back(bVertex.release());
                    } else {
                       ATH_MSG_DEBUG("Cuts failed!");
                    }
                } else {ATH_MSG_DEBUG("Fitter failed!");}
                
            } // End of loop over tracks                        
            
        } // End of loop over J/psis
        ATH_MSG_DEBUG("bContainer size " << bContainer->size());
        return StatusCode::SUCCESS;
        
    }
    
    // *********************************************************************************
    
    // ---------------------------------------------------------------------------------
    // fit - does the fit
    // ---------------------------------------------------------------------------------
    
    xAOD::Vertex* JpsiPlus1Track::fit(const xAOD::TrackParticle* mu1,const xAOD::TrackParticle* mu2,const xAOD::TrackParticle* trk, bool doMassConst, double massConst, const xAOD::TrackParticleContainer* importedTrackCollection) {
        
        m_VKVFitter->setDefault();
        
        // Assemble input tracks
        std::vector<const xAOD::TrackParticle*> inputTracks;
        inputTracks.push_back(mu1);
        inputTracks.push_back(mu2);
        inputTracks.push_back(trk);
        
        // Set the mass constraint if requested by user (default=true)
        // Can be set by user (m_altMassConstraint) - default is -1.0.
        // If < 0.0, uses J/psi (default)
        // If > 0.0, uses the value provided
        double jpsiTableMass = 3096.916;
        double muTableMass = 105.658;

        if (doMassConst) {
            std::vector<double> muMasses;
            muMasses.push_back(muTableMass); muMasses.push_back(muTableMass);
            m_VKVFitter->setMassInputParticles(muMasses);
            std::vector<int> indices; indices.push_back(1); indices.push_back(2);
            if (massConst<0.0) m_VKVFitter->setMassForConstraint(jpsiTableMass,indices);
            if (massConst>0.0) m_VKVFitter->setMassForConstraint(massConst,indices);
        }
        
        // Do the fit itself.......
        // Starting point (use the J/psi position)
        const Trk::Perigee& aPerigee1 = inputTracks[0]->perigeeParameters();
        const Trk::Perigee& aPerigee2 = inputTracks[1]->perigeeParameters();
        int sflag = 0;
        int errorcode = 0;
        Amg::Vector3D startingPoint = m_vertexEstimator->getCirclesIntersectionPoint(&aPerigee1,&aPerigee2,sflag,errorcode);
        if (errorcode != 0) {startingPoint(0) = 0.0; startingPoint(1) = 0.0; startingPoint(2) = 0.0;}
        xAOD::Vertex* theResult = m_VKVFitter->fit(inputTracks, startingPoint);

        // Added by ASC
        if(theResult != 0){
        std::vector<ElementLink<DataVector<xAOD::TrackParticle> > > newLinkVector;
        for(unsigned int i=0; i< theResult->trackParticleLinks().size(); i++)
        { ElementLink<DataVector<xAOD::TrackParticle> > mylink=theResult->trackParticleLinks()[i]; //makes a copy (non-const) 
        mylink.setStorableObject(*importedTrackCollection, true); 
        newLinkVector.push_back( mylink ); }
        
        theResult->clearTracks();
        theResult->setTrackParticleLinks( newLinkVector );
        }

    
        return theResult;
        
    }
    
    

    double JpsiPlus1Track::getInvariantMass(const xAOD::TrackParticle* trk1, double mass1,
                                             const xAOD::TrackParticle* trk2, double mass2,
                                             const xAOD::TrackParticle* trk3, double mass3)
    {
        const auto trk1V = trk1->p4();
        double px1 = trk1V.Px();
        double py1 = trk1V.Py();
        double pz1 = trk1V.Pz();
        double e1 = sqrt(px1*px1+py1*py1+pz1*pz1+mass1*mass1);

        const auto trk2V = trk2->p4();
        double px2 = trk2V.Px();
        double py2 = trk2V.Py();
        double pz2 = trk2V.Pz();
        double e2 = sqrt(px2*px2+py2*py2+pz2*pz2+mass2*mass2);
        
        const auto trk3V = trk3->p4();
        double px3 = trk3V.Px();
        double py3 = trk3V.Py();
        double pz3 = trk3V.Pz();
        double e3 = sqrt(px3*px3+py3*py3+pz3*pz3+mass3*mass3);
        
        
        double pxSum=px1+px2+px3;
        double pySum=py1+py2+py3;
        double pzSum=pz1+pz2+pz3;
        double eSum=e1+e2+e3;
        
        double M=sqrt((eSum*eSum)-(pxSum*pxSum)-(pySum*pySum)-(pzSum*pzSum));
        
        return M;
        
    }


    double JpsiPlus1Track::getPt(const xAOD::TrackParticle* trk1,
                                  const xAOD::TrackParticle* trk2,
                                  const xAOD::TrackParticle* trk3)
    {
        TLorentzVector momentum( trk1->p4() );
        momentum+= trk2->p4();
        momentum+= trk3->p4();
        return momentum.Perp();
    }

    // *********************************************************************************
    
    // -------------------------------------------------------------------------------------------------
    // isContainedIn: boolean function which checks if a track (1st argument) is also contained in a
    // vector (second argument)
    // -------------------------------------------------------------------------------------------------
    
    bool JpsiPlus1Track::isContainedIn(const xAOD::TrackParticle* theTrack, std::vector<const xAOD::TrackParticle*> theColl) {
        bool isContained(false);
        std::vector<const xAOD::TrackParticle*>::iterator trkItr;
        for (trkItr=theColl.begin(); trkItr!=theColl.end(); ++trkItr) {
            if ( (*trkItr) == theTrack ) {isContained=true; break;}
        }
        return isContained;
    }
    
    bool JpsiPlus1Track::isContainedIn(const xAOD::TrackParticle* theTrack, const xAOD::MuonContainer* theColl) {
        bool isContained(false);
        xAOD::MuonContainer::const_iterator muItr;
        for (muItr=theColl->begin(); muItr!=theColl->end(); ++muItr) {
            if ( (*muItr)->trackParticle(xAOD::Muon::InnerDetectorTrackParticle) == theTrack ) {isContained=true; break;}
        }
        return isContained;
    }
    
    
    // *********************************************************************************
    
} // End of namespace



