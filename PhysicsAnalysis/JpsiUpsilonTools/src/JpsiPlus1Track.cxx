/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
#include "DataModel/ElementLink.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "InDetConversionFinderTools/VertexPointEstimator.h"
#include <memory>
#include "JpsiUpsilonTools/JpsiUpsilonCommon.h"

namespace Analysis {


    // Set masses
    constexpr double muMass = 105.658;
    constexpr double kMass = 493.677;
    constexpr double piMass = 139.57;

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


        if (m_requiredNMuons > 0 && !m_excludeJpsiMuonsOnly) {
          ATH_MSG_FATAL("Invalid configuration");
          return StatusCode::FAILURE;
        }

        if(m_muonMasses.empty()){
          m_muonMasses.assign(2, muMass);
        }

        m_useGSFTrack.reset();
        for(int i : m_useGSFTrackIndices) m_useGSFTrack.set(i, true);

        ATH_MSG_INFO("Initialize successful");
        
        return StatusCode::SUCCESS;
        
    }
    
    StatusCode JpsiPlus1Track::finalize() {
        
        ATH_MSG_INFO("Finalize successful");
        return StatusCode::SUCCESS;
        
    }
    
    JpsiPlus1Track::JpsiPlus1Track(const std::string& t, const std::string& n, const IInterface* p)  : AthAlgTool(t,n,p),
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
    m_excludeJpsiMuonsOnly(true), 
    m_excludeCrossJpsiTracks(false),
    m_iVertexFitter("Trk::TrkVKalVrtFitter"),
    m_trkSelector("InDet::TrackSelectorTool"),
    m_vertexEstimator("InDet::VertexPointEstimator"),
    m_useMassConst(true),
    m_altMassConst(-1.0),
    m_chi2cut(-1.0),
    m_trkTrippletMassUpper(-1.0),
    m_trkTrippletMassLower(-1.0),
    m_trkTrippletPt(-1.0),
    m_trkDeltaZ(-1.0),
    m_requiredNMuons(0),
    m_TrkParticleGSFCollection("GSFTrackParticles")
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
        declareProperty("ExcludeJpsiMuonsOnly",m_excludeJpsiMuonsOnly);
        declareProperty("ExcludeCrossJpsiTracks",m_excludeCrossJpsiTracks); //Essential when trying to make vertices out of multiple muons (set to false)
        declareProperty("TrkVertexFitterTool",m_iVertexFitter);
        declareProperty("TrackSelectorTool", m_trkSelector);
        declareProperty("UseMassConstraint", m_useMassConst);
        declareProperty("AlternativeMassConstraint",m_altMassConst);
        declareProperty("VertexPointEstimator", m_vertexEstimator);

        // additional cuts by Daniel Scheirich copied from 2Tracks by Adam Barton
        declareProperty("Chi2Cut",m_chi2cut);
        declareProperty("TrkTrippletMassUpper"  ,m_trkTrippletMassUpper);
        declareProperty("TrkTrippletMassLower"  ,m_trkTrippletMassLower);
        declareProperty("TrkQuadrupletPt"       ,m_trkTrippletPt       );
        declareProperty("TrkDeltaZ"             ,m_trkDeltaZ           );
        declareProperty("RequireNMuonTracks"    ,m_requiredNMuons      );
        declareProperty("AlternativeMassConstraintTrack", m_muonMasses );
        declareProperty("UseGSFTrackIndices",    m_useGSFTrackIndices  );
        declareProperty("GSFCollection",         m_TrkParticleGSFCollection);

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

        const xAOD::TrackParticleContainer* importedGSFTrackCollection(nullptr);
        if(m_useGSFTrack.any()){
           ATH_CHECK(evtStore()->retrieve(importedGSFTrackCollection, m_TrkParticleGSFCollection));
        }


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
        for (auto trkPBItr=importedTrackCollection->cbegin(); trkPBItr!=importedTrackCollection->cend(); ++trkPBItr) {
            const xAOD::TrackParticle* tp (*trkPBItr);
            if ( tp->pt()<m_trkThresholdPt ) continue;
            if ( fabs(tp->eta())>m_trkMaxEta ) continue;
            if (importedMuonCollection!=NULL && !m_excludeJpsiMuonsOnly) {
                if (JpsiUpsilonCommon::isContainedIn(tp,importedMuonCollection)) continue;
            }
            if ( m_trkSelector->decision(*tp, vx) ) theIDTracksAfterSelection.push_back(tp);
        }
        if (theIDTracksAfterSelection.empty()) return StatusCode::SUCCESS;
        ATH_MSG_DEBUG("Number of tracks after ID trkSelector: " << theIDTracksAfterSelection.size());
        

        
        // Loop over J/psi candidates, select, collect up tracks used to build a J/psi
        std::vector<const xAOD::Vertex*> selectedJpsiCandidates;
        std::vector<const xAOD::TrackParticle*> jpsiTracks;
        for(auto vxcItr=importedJpsiCollection->cbegin(); vxcItr!=importedJpsiCollection->cend(); ++vxcItr) {
            // Check J/psi candidate invariant mass and skip if need be
            if (m_jpsiMassUpper>0.0 || m_jpsiMassLower >0.0) {
                xAOD::BPhysHelper jpsiCandidate(*vxcItr);
                jpsiCandidate.setRefTrks();
                double jpsiMass = jpsiCandidate.totalP(m_muonMasses).M();
                bool pass = JpsiUpsilonCommon::cutRange(jpsiMass, m_jpsiMassLower, m_jpsiMassUpper);
                if (!pass) continue;
            }
            selectedJpsiCandidates.push_back(*vxcItr);

            // Collect up tracks
	    if(m_excludeCrossJpsiTracks){
                // Extract tracks from J/psi
                const xAOD::TrackParticle* jpsiTP1 = (*vxcItr)->trackParticle(0);
                const xAOD::TrackParticle* jpsiTP2 = (*vxcItr)->trackParticle(1);
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
        std::vector<double> tripletMasses(m_muonMasses);
        // Attempt to fit each track with the two tracks from the J/psi candidates
        // Loop over J/psis

        std::vector<double> massCuts;

        TrackBag muonTracks;
        if (importedMuonCollection != NULL && m_excludeJpsiMuonsOnly) {
          for(auto muon : *importedMuonCollection){
            if(!muon->inDetTrackParticleLink().isValid()) continue;
            auto track = muon->trackParticle( xAOD::Muon::InnerDetectorTrackParticle );
            if(track==nullptr) continue;
            if(!JpsiUpsilonCommon::isContainedIn(track, theIDTracksAfterSelection)) continue;
            muonTracks.push_back(track);
          }
        }
        std::vector<const xAOD::TrackParticle*> tracks(3, nullptr);

        for(auto jpsiItr=selectedJpsiCandidates.cbegin(); jpsiItr!=selectedJpsiCandidates.cend(); ++jpsiItr) {

            // Extract tracks from J/psi
            const xAOD::TrackParticle* jpsiTP1 = tracks[0] =  (*jpsiItr)->trackParticle(0);
            const xAOD::TrackParticle* jpsiTP2 = tracks[1] =  (*jpsiItr)->trackParticle(1);

	    //If requested, only exclude duplicates in the same tripplet
            if(!m_excludeCrossJpsiTracks){
                jpsiTracks.resize(2);
                jpsiTracks[0] = jpsiTP1;
                jpsiTracks[1] = jpsiTP2;
            }

            // Loop over ID tracks, call vertexing
            for (auto trkItr=theIDTracksAfterSelection.cbegin(); trkItr!=theIDTracksAfterSelection.cend(); ++trkItr) {
                if (!m_excludeJpsiMuonsOnly && JpsiUpsilonCommon::isContainedIn(*trkItr,jpsiTracks)) continue; // remove tracks which were used to build J/psi
                int linkedMuonTrk = 0;
                if (m_excludeJpsiMuonsOnly) {
                  linkedMuonTrk = JpsiUpsilonCommon::isContainedIn(*trkItr, muonTracks);
                  if (linkedMuonTrk) ATH_MSG_DEBUG("This id track is a muon track!");
                  if (JpsiUpsilonCommon::isContainedIn(*trkItr,jpsiTracks)) {
                    if (linkedMuonTrk) ATH_MSG_DEBUG("ID track removed: id track is slected to build Jpsi!");
                    continue;
                    
                  }
                }
                // Convert to TrackParticleBase
                const xAOD::TrackParticle* theThirdTP = tracks[2] = *trkItr;

                if (m_trkTrippletPt>0 && JpsiUpsilonCommon::getPt(jpsiTP1, jpsiTP2, theThirdTP) < m_trkTrippletPt ) continue; // track tripplet pT cut (daniel Scheirich)
                if(m_trkDeltaZ>0 &&
                   fabs(theThirdTP->z0() + theThirdTP->vz() - (*jpsiItr)->z()) > m_trkDeltaZ )
                    continue;
                if (linkedMuonTrk < m_requiredNMuons) {
                  ATH_MSG_DEBUG("Skipping Tracks with Muons " << linkedMuonTrk << " Limited to " << m_requiredNMuons);
                    continue;
                }
                // apply mass cut on track tripplet if requested
                bool passRoughMassCuts(true);

                if (m_trkTrippletMassUpper>0.0 || m_trkTrippletMassLower>0.0) {
                     massCuts.clear();
                     if(m_kMassHyp)  massCuts.push_back(getInvariantMass(tracks, m_muonMasses[0], m_muonMasses[1], kMass));
                     if(m_piMassHyp) massCuts.push_back(getInvariantMass(tracks, m_muonMasses[0], m_muonMasses[1], piMass));
                     passRoughMassCuts = JpsiUpsilonCommon::cutRangeOR(massCuts, m_trkTrippletMassLower, m_trkTrippletMassUpper);
                 }
                 if (!passRoughMassCuts) continue;


                //Managed pointer, "release" if you don't want it deleted. Automatically "deleted" otherwise
                std::unique_ptr<xAOD::Vertex> bVertex( fit(tracks, importedTrackCollection, importedGSFTrackCollection));
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
                    bool masspTpassed = true;
                    if (m_BMassUpper>0.0 || m_BThresholdPt >0.0 || m_BMassLower > 0.0) {
                        masspTpassed = false;
                        for (double masshypo3rd : massHypotheses) {
                            tripletMasses.push_back(masshypo3rd); //Add third mass
                            TLorentzVector bMomentum = bHelper.totalP(tripletMasses);//Calulcate result
                            tripletMasses.pop_back(); //Remove 3rd mass - now same as beginning
                            double bpt = bMomentum.Pt();
                            bool PtPassed =  m_BThresholdPt <= 0.0 || (bpt >= m_BThresholdPt);
                            double bMass = bMomentum.M();
                            ATH_MSG_DEBUG("candidate pt/mass under track mass hypothesis of " << masshypo3rd << " is " << bpt << " / " << bMass);
                            bool masscut =  JpsiUpsilonCommon::cutRange(bMass, m_BMassLower, m_BMassUpper);  //( bMass >= m_BMassLower && bMass <= m_BMassUpper );
                            if(masscut && PtPassed) { masspTpassed = true; break; } 
                        }
                    }
                    if ((*jpsiItr)!=NULL && masspTpassed) {
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
    
    xAOD::Vertex* JpsiPlus1Track::fit(const std::vector<const xAOD::TrackParticle*> &inputTracks, const xAOD::TrackParticleContainer* importedTrackCollection, const xAOD::TrackParticleContainer* gsfCollection) const {
        
        m_VKVFitter->setDefault();
        
        // Set the mass constraint if requested by user (default=true)
        // Can be set by user (m_altMassConstraint) - default is -1.0.
        // If < 0.0, uses J/psi (default)
        // If > 0.0, uses the value provided
        constexpr double jpsiTableMass = 3096.916;

        if (m_useMassConst) {
            m_VKVFitter->setMassInputParticles(m_muonMasses);
            std::vector<int> indices = {1, 2};
            if (m_altMassConst<0.0) m_VKVFitter->setMassForConstraint(jpsiTableMass,indices);
            if (m_altMassConst>0.0) m_VKVFitter->setMassForConstraint(m_altMassConst,indices);
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
           {
              ElementLink<DataVector<xAOD::TrackParticle> > mylink=theResult->trackParticleLinks()[i]; //makes a copy (non-const)
              mylink.setStorableObject( m_useGSFTrack[i] ?  *gsfCollection : *importedTrackCollection, true);
              newLinkVector.push_back( mylink );
           }
           theResult->clearTracks();
           theResult->setTrackParticleLinks( newLinkVector );
        }

    
        return theResult;
        
    }
    
    

    double JpsiPlus1Track::getInvariantMass(const std::vector<const xAOD::TrackParticle*> &trk, double mass1,
                                            double mass2, double mass3)
    {
        const auto trk1V = trk[0]->p4();
        double px1 = trk1V.Px();
        double py1 = trk1V.Py();
        double pz1 = trk1V.Pz();
        double e1 = sqrt(px1*px1+py1*py1+pz1*pz1+mass1*mass1);

        const auto trk2V = trk[1]->p4();
        double px2 = trk2V.Px();
        double py2 = trk2V.Py();
        double pz2 = trk2V.Pz();
        double e2 = sqrt(px2*px2+py2*py2+pz2*pz2+mass2*mass2);
        
        const auto trk3V = trk[2]->p4();
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

    
    // *********************************************************************************
    
} // End of namespace



