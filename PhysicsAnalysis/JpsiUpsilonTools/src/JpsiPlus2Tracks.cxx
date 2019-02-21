/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
#include "TrkVertexFitterInterfaces/IVertexFitter.h"
#include "TrkVKalVrtFitter/TrkVKalVrtFitter.h"
#include "TrkToolInterfaces/ITrackSelectorTool.h"
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/IPartPropSvc.h"
#include "DataModel/ElementLink.h"
#include "InDetConversionFinderTools/VertexPointEstimator.h"
#include <memory>
#include "JpsiUpsilonTools/JpsiUpsilonCommon.h"

namespace Analysis {

    // Set masses
    constexpr double muMass = 105.658;
    constexpr double kMass = 493.677;
    constexpr double piMass = 139.57;
    constexpr double pMass = 938.272;
    
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

        if(!m_manualMassHypo.empty() && m_manualMassHypo.size() !=4){
            ATH_MSG_FATAL("Invalid number of elements given for manualMass hypothesis - needs 4");
            return StatusCode::FAILURE;
        }
        if(!m_manualMassHypo.empty()){
            ATH_MSG_DEBUG("manual mass hypo " << m_manualMassHypo[0] <<
                 ',' << m_manualMassHypo[1] << ',' << m_manualMassHypo[2] << ',' << m_manualMassHypo[3]);
        }
        if(m_requiredNMuons > 0 && !m_excludeJpsiMuonsOnly) {
            ATH_MSG_FATAL("Invalid configuration");
            return StatusCode::FAILURE;
        }
        if(m_altMassMuonTracks.empty()){
            m_altMassMuonTracks.assign(2, muMass); //Default to muon mass
        }
        
        m_mumukkMasses = {m_altMassMuonTracks[0], m_altMassMuonTracks[1], kMass, kMass};
        m_mumupipiMasses = {m_altMassMuonTracks[0], m_altMassMuonTracks[1], piMass, piMass};
        m_mumukpiMasses = {m_altMassMuonTracks[0], m_altMassMuonTracks[1], kMass, piMass};
        m_mumupikMasses = {m_altMassMuonTracks[0], m_altMassMuonTracks[1], piMass, kMass};
        m_mumukpMasses = {m_altMassMuonTracks[0], m_altMassMuonTracks[1], kMass, pMass};
        m_mumupkMasses = {m_altMassMuonTracks[0], m_altMassMuonTracks[1], pMass, kMass};
        m_useGSFTrack.reset();
        for(int i : m_useGSFTrackIndices) m_useGSFTrack.set(i, true);

        ATH_MSG_INFO("Initialize successful");
        
        return StatusCode::SUCCESS;
        
    }
    
    StatusCode JpsiPlus2Tracks::finalize() {
        
        ATH_MSG_INFO("Finalize successful");
        return StatusCode::SUCCESS;
        
    }
    
    JpsiPlus2Tracks::JpsiPlus2Tracks(const std::string& t, const std::string& n, const IInterface* p)  : AthAlgTool(t,n,p),
    m_pipiMassHyp(true),
    m_kkMassHyp(true),
    m_kpiMassHyp(true),
    m_kpMassHyp(false),
    m_oppChargesOnly(true),
    m_trkThresholdPt(0.0),
    m_trkMaxEta(102.5),
    m_BThresholdPt(0.0),
    m_BMassUpper(0.0),
    m_BMassLower(0.0),
    m_jpsiCollectionKey("JpsiCandidates"),
    m_jpsiMassUpper(0.0),
    m_jpsiMassLower(0.0),
    m_TrkParticleCollection("InDetTrackParticles"),
    m_TrkParticleGSFCollection("GSFTrackParticles"),
    m_MuonsUsedInJpsi("NONE"),
    m_excludeJpsiMuonsOnly(true),
    m_excludeCrossJpsiTracks(false),
    m_iVertexFitter("Trk::TrkVKalVrtFitter"),
    m_trkSelector("InDet::TrackSelectorTool"),
    m_vertexEstimator("InDet::VertexPointEstimator"),
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
    m_trkDeltaZ(-1.0),
    m_requiredNMuons(0),
    m_vertexFittingWithPV(false),
    m_PVerticesCollection("PrimaryVertices")
    {
        declareInterface<JpsiPlus2Tracks>(this);
        declareProperty("pionpionHypothesis",m_pipiMassHyp);
        declareProperty("kaonkaonHypothesis",m_kkMassHyp);
        declareProperty("kaonpionHypothesis",m_kpiMassHyp);
        declareProperty("kaonprotonHypothesis",m_kpMassHyp);
	declareProperty("oppChargesOnly",m_oppChargesOnly);
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
        declareProperty("VertexPointEstimator", m_vertexEstimator);
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
        declareProperty("ManualMassHypo",        m_manualMassHypo);
        declareProperty("RequireNMuonTracks",   m_requiredNMuons);
        declareProperty("UseVertexFittingWithPV", m_vertexFittingWithPV);
        declareProperty("VertexContainer", m_PVerticesCollection);
        declareProperty("AlternativeMassConstraintTrack", m_altMassMuonTracks);
        declareProperty("UseGSFTrackIndices", m_useGSFTrackIndices);
        declareProperty("GSFCollection", m_TrkParticleGSFCollection);
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

        // Get primary vertices
        const xAOD::VertexContainer* importedPVerticesCollection(0);
        if(m_vertexFittingWithPV){
           ATH_CHECK(evtStore()->retrieve(importedPVerticesCollection, m_PVerticesCollection));
           ATH_MSG_DEBUG("PVertices container size "<< importedPVerticesCollection->size());
        }
        // Typedef for vectors of tracks and VxCandidates
        typedef std::vector<const xAOD::TrackParticle*> TrackBag;
        
        // Select the inner detector tracks
        TrackBag theIDTracksAfterSelection;
        for (auto trkPBItr=importedTrackCollection->cbegin(); trkPBItr!=importedTrackCollection->cend(); ++trkPBItr) {
            const xAOD::TrackParticle* tp (*trkPBItr);
            if ( tp->pt()<m_trkThresholdPt ) continue;
            if ( std::abs(tp->eta())>m_trkMaxEta ) continue;
            if (importedMuonCollection!=NULL && !m_excludeJpsiMuonsOnly) {
                if (JpsiUpsilonCommon::isContainedIn(tp,importedMuonCollection)) continue;
            }
            if ( m_trkSelector->decision(*tp, NULL) ) theIDTracksAfterSelection.push_back(tp);
        }
        if (theIDTracksAfterSelection.size() == 0) return StatusCode::SUCCESS;
        ATH_MSG_DEBUG("Number of tracks after ID trkSelector: " << theIDTracksAfterSelection.size());
        
        // Loop over J/psi candidates, select, collect up tracks used to build a J/psi
        std::vector<const xAOD::Vertex*> selectedJpsiCandidates;
        std::vector<const xAOD::TrackParticle*> jpsiTracks;
        for(auto vxcItr=importedJpsiCollection->cbegin(); vxcItr!=importedJpsiCollection->cend(); ++vxcItr) {
            // Check J/psi candidate invariant mass and skip if need be
            
            if (m_jpsiMassUpper>0.0 || m_jpsiMassLower > 0.0) {
                xAOD::BPhysHelper jpsiCandidate(*vxcItr);
                jpsiCandidate.setRefTrks();
                double jpsiMass = jpsiCandidate.totalP(m_altMassMuonTracks).M();
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
        

        
        
        // Attempt to fit each track with the two tracks from the J/psi candidates
        // Loop over J/psis
        std::vector<const xAOD::TrackParticle*> QuadletTracks(4, nullptr);//Initialise as 4 nulls
        
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

            // Loop over ID tracks, call vertexing
            for (TrackBag::iterator trkItr1=theIDTracksAfterSelection.begin(); trkItr1<theIDTracksAfterSelection.end(); ++trkItr1) { // outer loop
                if (!m_excludeJpsiMuonsOnly && JpsiUpsilonCommon::isContainedIn(*trkItr1,jpsiTracks)) continue; // remove tracks which were used to build J/psi
                int linkedMuonTrk1 = 0;
                if (m_excludeJpsiMuonsOnly) {
                  linkedMuonTrk1 = JpsiUpsilonCommon::isContainedIn(*trkItr1, muonTracks);
                  if (linkedMuonTrk1) ATH_MSG_DEBUG("This id track 1 is muon track!");
   
                  if (JpsiUpsilonCommon::isContainedIn(*trkItr1,jpsiTracks)) {
                    if (linkedMuonTrk1) ATH_MSG_DEBUG("ID track 1 removed: id track is selected to build Jpsi!");
                    continue; // remove tracks which were used to build J/psi
                  }
                }
        std::cout<<"trkDeltaZ trkItr1_before "<< abs((*trkItr1)->z0() + (*trkItr1)->vz() - (*jpsiItr)->z())<<std::endl;
        std::cout<<"trkDeltaZ trkItr2_before "<< abs((*trkItr1)->z0() + (*trkItr1)->vz() - (*jpsiItr)->z())<<std::endl;        
                // Daniel Scheirich: remove track too far from the Jpsi vertex (DeltaZ cut)
                if(m_trkDeltaZ>0 &&
                   std::abs((*trkItr1)->z0() + (*trkItr1)->vz() - (*jpsiItr)->z()) > m_trkDeltaZ )
                    continue;
                
                for (TrackBag::iterator trkItr2=trkItr1+1; trkItr2!=theIDTracksAfterSelection.end(); ++trkItr2) { // inner loop
                    if (!m_excludeJpsiMuonsOnly && JpsiUpsilonCommon::isContainedIn(*trkItr2,jpsiTracks)) continue; // remove tracks which were used to build J/psi

                    if (m_excludeJpsiMuonsOnly) {
                      int linkedMuonTrk2 = JpsiUpsilonCommon::isContainedIn(*trkItr2, muonTracks);
                      if (linkedMuonTrk2) ATH_MSG_DEBUG("This id track 2 is muon track!"); 
                      if (JpsiUpsilonCommon::isContainedIn(*trkItr2,jpsiTracks)) {
                        if (linkedMuonTrk2) ATH_MSG_DEBUG("ID track 2 removed: id track is selected to build Jpsi Vtx!"); 
                        continue; // remove tracks which were used to build J/psi
                      }
                      if( (linkedMuonTrk1+ linkedMuonTrk2) < m_requiredNMuons) {
                        ATH_MSG_DEBUG("Skipping Tracks with Muons " << linkedMuonTrk1 + linkedMuonTrk2 << " Limited to " << m_requiredNMuons);
                        continue;
                      }
                    }
                    
                    // Daniel Scheirich: remove track too far from the Jpsi vertex (DeltaZ cut)
                    if(m_trkDeltaZ>0 &&
                       std::abs((*trkItr2)->z0() + (*trkItr2)->vz() - (*jpsiItr)->z()) > m_trkDeltaZ )
                        continue;
                    
		    if (m_oppChargesOnly && !oppositeCharges(*trkItr1,*trkItr2)) continue; //enforce opposite charges
                    
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
                        if(m_kpMassHyp){
			   massCuts.push_back(getInvariantMass(*trkItr1,kMass,*trkItr2,piMass));
			   massCuts.push_back(getInvariantMass(*trkItr1,piMass,*trkItr2,kMass));
                        }
                        if(!m_manualMassHypo.empty()) massCuts.push_back(getInvariantMass(*trkItr1, m_manualMassHypo[2], *trkItr2, m_manualMassHypo[3]));
                        passesDiTrack = JpsiUpsilonCommon::cutRangeOR(massCuts, m_diTrackMassLower, m_diTrackMassUpper);

                    }
                    if (!passesDiTrack) continue;
                    
                    // apply mass cut on track quadruplet if requested
                    bool passes4TrackMass(true);
                    if (m_trkQuadrupletMassUpper>0.0 || m_trkQuadrupletMassLower>0.0) {
                        massCuts.clear();
                        
                        if(m_kkMassHyp) massCuts.push_back(  getInvariantMass(QuadletTracks, m_mumukkMasses) );
                        if(m_pipiMassHyp) massCuts.push_back(getInvariantMass(QuadletTracks, m_mumupipiMasses));
                        if(m_kpiMassHyp){
                           massCuts.push_back(getInvariantMass(QuadletTracks, m_mumukpiMasses));
                           massCuts.push_back(getInvariantMass(QuadletTracks, m_mumupikMasses));
                        }
                        if(m_kpMassHyp){
                           massCuts.push_back(getInvariantMass(QuadletTracks, m_mumukpMasses));
                           massCuts.push_back(getInvariantMass(QuadletTracks, m_mumupkMasses));
                        }
                        if(!m_manualMassHypo.empty()) massCuts.push_back(getInvariantMass(QuadletTracks, m_manualMassHypo));

                        passes4TrackMass = JpsiUpsilonCommon::cutRangeOR(massCuts, m_trkQuadrupletMassLower, m_trkQuadrupletMassUpper);
                    }
                    if (!passes4TrackMass) continue;

                    //Managed pointer, "release" if you don't want it deleted. Automatically deleted otherwise
                    std::unique_ptr<xAOD::Vertex> bVertex (fit(QuadletTracks, importedTrackCollection, nullptr, importedGSFTrackCollection)); // do vertexing
                    if(!bVertex) continue;
                    double bChi2DOF = bVertex->chiSquared()/bVertex->numberDoF();
                    ATH_MSG_DEBUG("Candidate chi2/DOF is " << bChi2DOF);
                    bool chi2CutPassed = (m_chi2cut <= 0.0 || bChi2DOF < m_chi2cut);
                    if(!chi2CutPassed) { ATH_MSG_DEBUG("Chi Cut failed!");  continue; }
                    xAOD::BPhysHelper bHelper(bVertex.get());//"get" does not "release" still automatically deleted
                    bHelper.setRefTrks();
                    bool passesCuts = vertexCuts(bHelper);
                    if(!passesCuts) continue;
                    // Saving successful candidates
                    // Set links to J/psi
                    std::vector<const xAOD::Vertex*> theJpsiPreceding;
                    theJpsiPreceding.push_back(*jpsiItr);
                    if(m_vertexFittingWithPV){
                        const xAOD::Vertex* closestPV = JpsiUpsilonCommon::ClosestPV(bHelper, importedPVerticesCollection);
                        std::unique_ptr<xAOD::Vertex> bVertexPV (fit(QuadletTracks, importedTrackCollection, closestPV, importedGSFTrackCollection));
                        if(!bVertexPV) continue;
                        double bChi2DOFPV = bVertexPV->chiSquared()/bVertexPV->numberDoF();
                        bool chi2CutPassed = (m_chi2cut <= 0.0 || bChi2DOFPV < m_chi2cut);
                        if(!chi2CutPassed) continue;
                        xAOD::BPhysHelper bHelperPV(bVertexPV.get());
                        bHelperPV.setRefTrks();
                        bool passesCutsPV = vertexCuts(bHelperPV);
                        if(!passesCutsPV) continue;
                        bHelperPV.setPrecedingVertices(theJpsiPreceding, importedJpsiCollection);
                        bContainer->push_back(bVertexPV.release());//ptr is released preventing deletion
                        continue; //Don't store other vertex
                    }
                    bHelper.setPrecedingVertices(theJpsiPreceding, importedJpsiCollection);
                    bContainer->push_back(bVertex.release());//ptr is released preventing deletion
 std::cout<<"trkDeltaZ trkItr1_after_innerloop "<< abs((*trkItr1)->z0() + (*trkItr1)->vz() - (*jpsiItr)->z())<<std::endl;
        std::cout<<"trkDeltaZ trkItr2_after_innerloop "<< abs((*trkItr1)->z0() + (*trkItr1)->vz() - (*jpsiItr)->z())<<std::endl;

                } // End of inner loop over tracks
 std::cout<<"trkDeltaZ trkItr1_after_outer "<< abs((*trkItr1)->z0() + (*trkItr1)->vz() - (*jpsiItr)->z())<<std::endl;
        std::cout<<"trkDeltaZ trkItr2_after_outer "<< abs((*trkItr1)->z0() + (*trkItr1)->vz() - (*jpsiItr)->z())<<std::endl;

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
                                       const xAOD::TrackParticleContainer* importedTrackCollection, const xAOD::Vertex* pv, 
                                       const xAOD::TrackParticleContainer* gsfCollection) const {
        
        m_VKVFitter->setDefault();
        


        
        // Set the mass constraint if requested by user (default=true)
        // Can be set by user (m_altMassConstraint) - default is -1.0.
        // If < 0.0, uses J/psi (default)
        // If > 0.0, uses the value provided


        if (m_useMassConst) {
            constexpr double jpsiTableMass = 3096.916;
            m_VKVFitter->setMassInputParticles(m_altMassMuonTracks);
            std::vector<int> indices= {1, 2};
            if (m_altMassConst<0.0) m_VKVFitter->setMassForConstraint(jpsiTableMass,indices);
            if (m_altMassConst>0.0) m_VKVFitter->setMassForConstraint(m_altMassConst,indices);
        }
        if (pv) {
	   m_VKVFitter->setCnstType(8);
	   m_VKVFitter->setVertexForConstraint(pv->position().x(),
					       pv->position().y(),
					       pv->position().z());
	   m_VKVFitter->setCovVrtForConstraint(pv->covariancePosition()(Trk::x,Trk::x),
					       pv->covariancePosition()(Trk::y,Trk::x),
					       pv->covariancePosition()(Trk::y,Trk::y),
					       pv->covariancePosition()(Trk::z,Trk::x),
					       pv->covariancePosition()(Trk::z,Trk::y),
					       pv->covariancePosition()(Trk::z,Trk::z) );
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

    double JpsiPlus2Tracks::getInvariantMass(const std::vector<const xAOD::TrackParticle*> &trk,
                                             const std::vector<double> &masses)
    {
        assert(trk.size() == masses.size() && trk.size()==4);
        const auto trk1V = trk[0]->p4();
        double px1 = trk1V.Px();
        double py1 = trk1V.Py();
        double pz1 = trk1V.Pz();
        double e1 = sqrt(px1*px1+py1*py1+pz1*pz1+masses[0]*masses[0]);

        const auto trk2V = trk[1]->p4();
        double px2 = trk2V.Px();
        double py2 = trk2V.Py();
        double pz2 = trk2V.Pz();
        double e2 = sqrt(px2*px2+py2*py2+pz2*pz2+masses[1]*masses[1]);
        
        const auto trk3V = trk[2]->p4();
        double px3 = trk3V.Px();
        double py3 = trk3V.Py();
        double pz3 = trk3V.Pz();
        double e3 = sqrt(px3*px3+py3*py3+pz3*pz3+masses[2]*masses[2]);
        
        const auto trk4V = trk[3]->p4();
        double px4 = trk4V.Px();
        double py4 = trk4V.Py();
        double pz4 = trk4V.Pz();
        double e4 = sqrt(px4*px4+py4*py4+pz4*pz4+masses[3]*masses[3]);
        
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
    
    bool  JpsiPlus2Tracks::vertexCuts(xAOD::BPhysHelper &bHelper) const {
        // Invariant mass calculation under the various hypotheses
        // create the helper class

       bool passesCuts = (m_kkMassHyp && passCuts(bHelper, m_mumukkMasses, "KK")) ||
                   (m_pipiMassHyp && passCuts(bHelper, m_mumupipiMasses, "pi pi")) ||
                   (m_kpiMassHyp && (passCuts(bHelper, m_mumukpiMasses, "K pi") ||
                   passCuts(bHelper, m_mumupikMasses, "pi K"))) ||
                   (m_kpMassHyp && (passCuts(bHelper, m_mumukpMasses, "K p") ||
                   passCuts(bHelper, m_mumupkMasses, "p K"))) ||
                   (!m_manualMassHypo.empty()  && passCuts(bHelper, m_manualMassHypo, "manual"));
       return passesCuts;
  }


} // End of namespace



