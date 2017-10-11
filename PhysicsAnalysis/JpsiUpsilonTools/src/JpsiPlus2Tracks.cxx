/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ****************************************************************************
// ----------------------------------------------------------------------------
// JpsiPlus2Tracks
// James Catmore <James.Catmore@cern.ch>
// Results returned as a vector of xAOD::Vertex
// ----------------------------------------------------------------------------
// ****************************************************************************

#include "JpsiUpsilonTools/JpsiPlus2Tracks.h"
#include "xAODBPhys/BPhysHelper.h"
#include "TrkVertexAnalysisUtils/V0Tools.h"
#include "TrkVertexFitterInterfaces/IVertexFitter.h"
#include "TrkVKalVrtFitter/TrkVKalVrtFitter.h"
#include "TrkToolInterfaces/ITrackSelectorTool.h"
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/IPartPropSvc.h"
#include "DataModel/ElementLink.h"
#include "InDetConversionFinderTools/VertexPointEstimator.h"
#include "EventPrimitives/EventPrimitives.h"
#include <memory>
#include "JpsiUpsilonTools/JpsiUpsilonCommon.h"

namespace Analysis {
    
    StatusCode JpsiPlus2Tracks::initialize() {
        
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
        
        ATH_MSG_INFO("Initialize successful");
        
        return StatusCode::SUCCESS;
        
    }
    
    StatusCode JpsiPlus2Tracks::finalize() {
        
        ATH_MSG_INFO("Finalize successful");
        return StatusCode::SUCCESS;
        
    }
    
    JpsiPlus2Tracks::JpsiPlus2Tracks(const std::string& t, const std::string& n, const IInterface* p)  : AthAlgTool(t,n,p),
    //    m_particleDataTable(0),
    m_pipiMassHyp(true),
    m_kkMassHyp(true),
    m_kpiMassHyp(true),
    m_trkThresholdPt(0.0),
    m_trkMaxEta(102.5),
    m_BThresholdPt(0.0),
    m_BMassUpper(0.0),
    m_BMassLower(0.0),
    m_jpsiCollectionKey("JpsiCandidates"),
    m_jpsiMassUpper(0.0),
    m_jpsiMassLower(0.0),
    m_TrkParticleCollection("InDetTrackParticles"),
    m_MuonsUsedInJpsi("NONE"),
    m_excludeJpsiMuonsOnly(false),
    m_excludeCrossJpsiTracks(true),
    m_iVertexFitter("Trk::TrkVKalVrtFitter"),
    m_trkSelector("InDet::TrackSelectorTool"),
    m_useMassConst(true),
    m_altMassConst(-1.0),
    m_diTrackMassUpper(-1.0),
    m_diTrackMassLower(-1.0),
    m_chi2cut(-1.0),
    m_diTrackPt(-1.0),
    m_trkQuadrupletMassUpper(-1.0),
    m_trkQuadrupletMassLower(-1.0),
    m_trkQuadrupletPt(-1.0),
    m_finalDiTrackMassUpper(-1.0),
    m_finalDiTrackMassLower(-1.0),
    m_finalDiTrackPt(-1.0),
    m_trkDeltaZ(-1.0)
    {
        declareInterface<JpsiPlus2Tracks>(this);
        declareProperty("pionpionHypothesis",m_pipiMassHyp);
        declareProperty("kaonkaonHypothesis",m_kkMassHyp);
        declareProperty("kaonpionHypothesis",m_kpiMassHyp);
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
        declareProperty("TrackSelectorTool",m_trkSelector);
        declareProperty("UseMassConstraint", m_useMassConst);
        declareProperty("AlternativeMassConstraint",m_altMassConst);
        declareProperty("DiTrackMassUpper",m_diTrackMassUpper);
        declareProperty("DiTrackMassLower",m_diTrackMassLower);
        
        // additional cuts by Daniel Scheirich
        declareProperty("Chi2Cut",m_chi2cut);
        declareProperty("DiTrackPt",m_diTrackPt);
        declareProperty("TrkQuadrupletMassUpper",m_trkQuadrupletMassUpper);
        declareProperty("TrkQuadrupletMassLower",m_trkQuadrupletMassLower);
        declareProperty("TrkQuadrupletPt"       ,m_trkQuadrupletPt       );
        declareProperty("FinalDiTrackMassUpper" ,m_finalDiTrackMassUpper );
        declareProperty("FinalDiTrackMassLower" ,m_finalDiTrackMassLower );
        declareProperty("FinalDiTrackPt"        ,m_finalDiTrackPt        );
        declareProperty("TrkDeltaZ"             ,m_trkDeltaZ             );
    }
    
    JpsiPlus2Tracks::~JpsiPlus2Tracks() {}
    

    
    //-------------------------------------------------------------------------------------
    // Find the candidates
    //-------------------------------------------------------------------------------------
    StatusCode JpsiPlus2Tracks::performSearch(xAOD::VertexContainer*& bContainer, xAOD::VertexAuxContainer*& bAuxContainer)
    {
        ATH_MSG_DEBUG( "JpsiPlus2Tracks::performSearch" );
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
        constexpr double muMass = 105.658;
        constexpr double kMass = 493.677;
        constexpr double piMass = 139.57;
        
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
        TrackBag theIDTracksAfterSelection;
        for (auto trkPBItr=importedTrackCollection->cbegin(); trkPBItr!=importedTrackCollection->cend(); ++trkPBItr) {
            const xAOD::TrackParticle* tp (*trkPBItr);
            if ( tp->pt()<m_trkThresholdPt ) continue;
            if ( fabs(tp->eta())>m_trkMaxEta ) continue;
            if (importedMuonCollection!=NULL && !m_excludeJpsiMuonsOnly) {
                if (JpsiUpsilonCommon::isContainedIn(tp,importedMuonCollection)) continue;
            }
            if ( m_trkSelector->decision(*tp, NULL) ) theIDTracksAfterSelection.push_back(tp);
        }
        if (theIDTracksAfterSelection.size() == 0) return StatusCode::SUCCESS;
        ATH_MSG_DEBUG("Number of tracks after ID trkSelector: " << theIDTracksAfterSelection.size());
        
        // Set vector of muon masses
        const std::vector<double> muonMasses(2, muMass);
        
        // Loop over J/psi candidates, select, collect up tracks used to build a J/psi
        std::vector<const xAOD::Vertex*> selectedJpsiCandidates;
        std::vector<const xAOD::TrackParticle*> jpsiTracks;
        for(auto vxcItr=importedJpsiCollection->cbegin(); vxcItr!=importedJpsiCollection->cend(); ++vxcItr) {
            // Check J/psi candidate invariant mass and skip if need be
            
            if (m_jpsiMassUpper>0.0 || m_jpsiMassLower > 0.0) {
                xAOD::BPhysHelper jpsiCandidate(*vxcItr);
                jpsiCandidate.setRefTrks();
                double jpsiMass = jpsiCandidate.totalP(muonMasses).M();
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
        ATH_MSG_DEBUG("selectedJpsiCandidates: " << selectedJpsiCandidates.size());
        
        // Invariant mass calculation under the various hypotheses
        const std::vector<double> mumukkMasses = {muMass, muMass, kMass, kMass};
        const std::vector<double> mumupipiMasses = {muMass, muMass, piMass, piMass};
        const std::vector<double> mumukpiMasses = {muMass, muMass, kMass, piMass};
        const std::vector<double> mumupikMasses = {muMass, muMass, piMass, kMass};
        
        
        // Attempt to fit each track with the two tracks from the J/psi candidates
        // Loop over J/psis
        std::vector<const xAOD::TrackParticle*> QuadletTracks(4, nullptr);//Initialise as 4 nulls
        
        std::vector<double> massCuts;

        for(auto jpsiItr=selectedJpsiCandidates.begin(); jpsiItr!=selectedJpsiCandidates.end(); ++jpsiItr) {

            // Extract tracks from J/psi
            const xAOD::TrackParticle* jpsiTP1 = (*jpsiItr)->trackParticle(0);
            const xAOD::TrackParticle* jpsiTP2 = (*jpsiItr)->trackParticle(1);
            QuadletTracks[0] = jpsiTP1;
            QuadletTracks[1] = jpsiTP2;

	    //If requested, only exclude duplicates in the same tripplet
            if(!m_excludeCrossJpsiTracks){
                jpsiTracks.resize(2);
                jpsiTracks[0] = jpsiTP1;
                jpsiTracks[1] = jpsiTP2;
            }

            std::vector<const xAOD::TrackParticle*> ThisjpsiTracks;
            ThisjpsiTracks.push_back(jpsiTP1);
            ThisjpsiTracks.push_back(jpsiTP2);
            ATH_MSG_DEBUG("Number of tracks associated with jpsi: " << ThisjpsiTracks.size());

            // Loop over ID tracks, call vertexing
            for (TrackBag::iterator trkItr1=theIDTracksAfterSelection.begin(); trkItr1<theIDTracksAfterSelection.end(); ++trkItr1) { // outer loop
                if (JpsiUpsilonCommon::isContainedIn(*trkItr1,jpsiTracks)) continue; // remove tracks which were used to build J/psi

                if (m_excludeJpsiMuonsOnly) {
                  bool linkedMuonTrk1 = false;
                  for(auto muon : *importedMuonCollection){
                    if(muon->inDetTrackParticleLink().cachedElement() == *trkItr1) linkedMuonTrk1 = true;
                  }
                  if (linkedMuonTrk1) ATH_MSG_DEBUG("This id track 1 is muon track!");
   
                  if (JpsiUpsilonCommon::isContainedIn(*trkItr1,ThisjpsiTracks)) {
                    if (linkedMuonTrk1) ATH_MSG_DEBUG("ID track 1 removed: id track is selected to build Jpsi!");
                    continue; // remove tracks which were used to build J/psi
                  }
                }
                
                // Daniel Scheirich: remove track too far from the Jpsi vertex (DeltaZ cut)
                if(m_trkDeltaZ>0 &&
                   fabs((*trkItr1)->z0() - (*jpsiItr)->z()) > m_trkDeltaZ )
                    continue;
                
                for (TrackBag::iterator trkItr2=trkItr1+1; trkItr2!=theIDTracksAfterSelection.end(); ++trkItr2) { // inner loop
                    if (JpsiUpsilonCommon::isContainedIn(*trkItr2,jpsiTracks)) continue; // remove tracks which were used to build J/psi

                    if (m_excludeJpsiMuonsOnly) {
                      bool linkedMuonTrk2 = false;
                      for(auto muon : *importedMuonCollection){
                        if(muon->inDetTrackParticleLink().cachedElement() == *trkItr2) linkedMuonTrk2 = true;
                      }
                      if (linkedMuonTrk2) ATH_MSG_DEBUG("This id track 2 is muon track!"); 
                      if (JpsiUpsilonCommon::isContainedIn(*trkItr2,ThisjpsiTracks)) {
                        if (linkedMuonTrk2) ATH_MSG_DEBUG("ID track 2 removed: id track is selected to build Jpsi Vtx!"); 
                        continue; // remove tracks which were used to build J/psi
                      }
                    }
                    
                    // Daniel Scheirich: remove track too far from the Jpsi vertex (DeltaZ cut)
                    if(m_trkDeltaZ>0 &&
                       fabs((*trkItr2)->z0() - (*jpsiItr)->z()) > m_trkDeltaZ )
                        continue;
                    
                    if (!oppositeCharges(*trkItr1,*trkItr2)) continue; //enforce opposite charges
                    
                    if (m_diTrackPt>0 && JpsiUpsilonCommon::getPt(*trkItr1,*trkItr2) < m_diTrackPt ) continue; // track pair pT cut (daniel Scheirich)
                     
                    // sort the track by charge, putting the positive track first         
                    if((*trkItr2)->qOverP()>0) {
                	    QuadletTracks[2] = *trkItr2;
         	            QuadletTracks[3] = *trkItr1;
         	    }else{
                	    QuadletTracks[2] = *trkItr1;
         	            QuadletTracks[3] = *trkItr2;
         	    }            
                    
                    if (m_trkQuadrupletPt>0 && JpsiUpsilonCommon::getPt(QuadletTracks[0], QuadletTracks[1], *trkItr1,*trkItr2) < m_trkQuadrupletPt ) continue; // track quadruplet pT cut (daniel Scheirich)

                    // apply mass cut on track pair (non muon) if requested
                    bool passesDiTrack(true);
                    if (m_diTrackMassUpper>0.0 || m_diTrackMassLower>0.0) {
                        massCuts.clear();
                        if(m_kkMassHyp)   massCuts.push_back(getInvariantMass(*trkItr1,kMass,*trkItr2,kMass));
                        if(m_pipiMassHyp) massCuts.push_back(getInvariantMass(*trkItr1,piMass,*trkItr2,piMass));
                        if(m_kpiMassHyp){
			   massCuts.push_back(getInvariantMass(*trkItr1,kMass,*trkItr2,piMass));
			   massCuts.push_back(getInvariantMass(*trkItr1,piMass,*trkItr2,kMass));
                        }
                        passesDiTrack = JpsiUpsilonCommon::cutRangeOR(massCuts, m_diTrackMassLower, m_diTrackMassUpper);

                    }
                    if (!passesDiTrack) continue;
                    
                    // apply mass cut on track quadruplet if requested
                    bool passes4TrackMass(true);
                    if (m_trkQuadrupletMassUpper>0.0 || m_trkQuadrupletMassLower>0.0) {
                        massCuts.clear();
                        if(m_kkMassHyp) massCuts.push_back( getInvariantMass(jpsiTP1, muMass, jpsiTP2, muMass, *trkItr1,kMass , *trkItr2, kMass) );
                        if(m_pipiMassHyp) massCuts.push_back(getInvariantMass(jpsiTP1, muMass, jpsiTP2, muMass, *trkItr1,piMass, *trkItr2, piMass));
                        if(m_kpiMassHyp){
                           massCuts.push_back(getInvariantMass(jpsiTP1, muMass, jpsiTP2, muMass, *trkItr1,kMass , *trkItr2, piMass));
                           massCuts.push_back(getInvariantMass(jpsiTP1, muMass, jpsiTP2, muMass, *trkItr1,piMass, *trkItr2, kMass));
                        }
                        passes4TrackMass = JpsiUpsilonCommon::cutRangeOR(massCuts, m_trkQuadrupletMassLower, m_trkQuadrupletMassUpper);
                    }
                    if (!passes4TrackMass) continue;
                    
                    //Managed pointer, "release" if you don't want it deleted. Automatically deleted otherwise
                    std::unique_ptr<xAOD::Vertex> bVertex (fit(QuadletTracks,m_useMassConst,m_altMassConst, importedTrackCollection)); // do vertexing
                    if (bVertex) {

                        double bChi2DOF = bVertex->chiSquared()/bVertex->numberDoF();
                        ATH_MSG_DEBUG("Candidate chi2/DOF is " << bChi2DOF);
                        bool chi2CutPassed = (m_chi2cut <= 0.0 || bChi2DOF < m_chi2cut);
                        if(!chi2CutPassed) { ATH_MSG_DEBUG("Chi Cut failed!");   continue; }

                        // create the helper class
                        xAOD::BPhysHelper bHelper(bVertex.get());//"get" does not "release" still automatically deleted
                        bHelper.setRefTrks();


                        bool passesCuts = (m_kkMassHyp && passCuts(bHelper, mumukkMasses, "KK")) ||
                                      (m_pipiMassHyp && passCuts(bHelper, mumupipiMasses, "pi pi")) ||
                                      (m_kpiMassHyp && (passCuts(bHelper, mumukpiMasses, "K pi") ||
                                                        passCuts(bHelper, mumupikMasses, "pi K")));
                     
                        // Saving successful candidates
                        if (passesCuts) {
                            // Set refitted tracks (done above)
//                            bHelper.setRefTrks();
                            // Set links to J/psi
                            std::vector<const xAOD::Vertex*> theJpsiPreceding;
                            theJpsiPreceding.push_back(*jpsiItr);
                            bHelper.setPrecedingVertices(theJpsiPreceding, importedJpsiCollection);
                            bContainer->push_back(bVertex.release());//ptr is released preventing deletion
                        }
                    
                    } // End of "if (bVertex != 0)"
                } // End of inner loop over tracks
            } // End of outer loop over tracks
        } // End of loop over J/spi
        ATH_MSG_DEBUG("bContainer size " << bContainer->size());
        return StatusCode::SUCCESS;
        
    }
    
    // *********************************************************************************
    
    // ---------------------------------------------------------------------------------
    // fit - does the fit
    // ---------------------------------------------------------------------------------
    
    xAOD::Vertex* JpsiPlus2Tracks::fit(const std::vector<const xAOD::TrackParticle*> &inputTracks,
                                           bool doMassConst, double massConst, const xAOD::TrackParticleContainer* importedTrackCollection) {
        
        m_VKVFitter->setDefault();
        


        
        // Set the mass constraint if requested by user (default=true)
        // Can be set by user (m_altMassConstraint) - default is -1.0.
        // If < 0.0, uses J/psi (default)
        // If > 0.0, uses the value provided


        if (doMassConst) {
            constexpr double jpsiTableMass = 3096.916;
            constexpr double muTableMass = 105.658;
            std::vector<double> muMasses(2, muTableMass);
            m_VKVFitter->setMassInputParticles(muMasses);
            std::vector<int> indices= {1, 2};
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
    
   
    
    // *********************************************************************************
    
    // oppositeCharges: true if the two tracks are oppositely charged
    bool JpsiPlus2Tracks::oppositeCharges(const xAOD::TrackParticle* trk1, const xAOD::TrackParticle* trk2) {
        double qOverP1=trk1->qOverP();
        double qOverP2=trk2->qOverP();
        bool opposite = (qOverP1*qOverP2<0.0);
        return opposite;
    }

   
  
    // ---------------------------------------------------------------------------------
    // getInvariantMass: returns invariant mass given a pair of tracks and their mass
    // hypothesis. Each track must have a separate mass hypothesis in
    // the vector, and they must be in the same order as the tracks in the track vector.
    // Otherwise it will go horribly wrong.
    // ---------------------------------------------------------------------------------
    
    double JpsiPlus2Tracks::getInvariantMass(const xAOD::TrackParticle* trk1, double mass1, const xAOD::TrackParticle* trk2, double mass2){
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
        double pxSum=px1+px2;
        double pySum=py1+py2;
        double pzSum=pz1+pz2;
        double eSum=e1+e2;
        double M=sqrt((eSum*eSum)-(pxSum*pxSum)-(pySum*pySum)-(pzSum*pzSum)); 
        
        return M;
        
    }

    double JpsiPlus2Tracks::getInvariantMass(const xAOD::TrackParticle* trk1, double mass1,
                                             const xAOD::TrackParticle* trk2, double mass2,
                                             const xAOD::TrackParticle* trk3, double mass3,
                                             const xAOD::TrackParticle* trk4, double mass4)
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
        
        const auto trk4V = trk4->p4();
        double px4 = trk4V.Px();
        double py4 = trk4V.Py();
        double pz4 = trk4V.Pz();
        double e4 = sqrt(px4*px4+py4*py4+pz4*pz4+mass4*mass4);
        
        double pxSum=px1+px2+px3+px4;
        double pySum=py1+py2+py3+py4;
        double pzSum=pz1+pz2+pz3+pz4;
        double eSum=e1+e2+e3+e4;
        
        double M=sqrt((eSum*eSum)-(pxSum*pxSum)-(pySum*pySum)-(pzSum*pzSum));
        
        return M;
        
    }
    
    bool  JpsiPlus2Tracks::passCuts(xAOD::BPhysHelper &bHelper, const std::vector<double> &masses, const std::string &str) const{
       
        TLorentzVector bMomentum = bHelper.totalP(masses);
//      ATH_MSG_DEBUG(bMomentum.X() << " " << bMomentum.Y()<< " " << bMomentum.Z() << " " << bMomentum.E());
        double bPt = bMomentum.Pt();

        double bMass = bMomentum.M();
        ATH_MSG_DEBUG("Candidate pt/mass under " << str << " track mass hypothesis is " << bPt << " / " << bMass);
        if( !JpsiUpsilonCommon::cutAcceptGreater(bPt, m_BThresholdPt)) return false;
        if( !JpsiUpsilonCommon::cutRange(bMass, m_BMassLower, m_BMassUpper)) return false;
	TLorentzVector tr1 = bHelper.refTrk(2,masses.at(2));
	TLorentzVector tr2 = bHelper.refTrk(3,masses.at(3));
	double bDiTrkPt   = (tr1+tr2).Pt();
        double bDiTrkMass = (tr1+tr2).M();
        if( !JpsiUpsilonCommon::cutAcceptGreater(bDiTrkPt, m_finalDiTrackPt)) return false;
        if( !JpsiUpsilonCommon::cutRange(bDiTrkMass, m_finalDiTrackMassLower, m_finalDiTrackMassUpper)) return false;

        return true;
    }
    
} // End of namespace



