/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
/////////////////////////////////////////////////////////////////
// HDBSa0Finder.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Author: M. Marinescu <mihaela.marinescu@cern.ch>
#include "DerivationFrameworkHDBS/HDBSa0Finder.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/VertexAuxContainer.h"
#include "xAODBPhys/BPhysHelper.h"
#include "InDetTrackSelectionTool/IInDetTrackSelectionTool.h" 
#include "TrkToolInterfaces/ITrackSelectorTool.h" 
#include "TrkVertexAnalysisUtils/V0Tools.h"
#include "GaudiKernel/IPartPropSvc.h"

#include "TrkVertexFitterInterfaces/IVertexFitter.h"
#include "TrkVKalVrtFitter/TrkVKalVrtFitter.h"

#include <tuple>

namespace DerivationFramework {

  HDBSa0Finder::HDBSa0Finder(const std::string& t,
      const std::string& n,
      const IInterface* p) :
    AthAlgTool(t,n,p),
    m_v0Tools("Trk::V0Tools"),
    m_iVertexFitter("Trk::TrkVKalVrtFitter", this),
    m_vertexEstimator("InDet::VertexPointEstimator"),
    m_TrackSelectionToolTightP("InDet::InDetTrackSelectionTool/TrackTightP"), 
    m_TrackSelectionToolLooseP("InDet::InDetTrackSelectionTool/TrackLooseP"),
    m_TrackSelectionToolLoose("InDet::InDetTrackSelectionTool/TrackLoose"),
    m_inputTrackParticleContainerName("InDetTrackParticles"),
    m_TrkParticleGSFCollection("GSFTrackParticles"),
    m_HCandidateContainerName("PHYSHCandidates"),
    m_trackPtMin(400.0),
    m_deltaz0PVsinthetaMax(99999.0),
    m_deltaz0PVsignificanceMax(99999.0),
    m_trkZDeltaZ(-1.0),
    m_ditrackMassMax(1600.0),
    m_ZisMuons(true),
    m_ZisElectrons(false),
    m_d0significanceMax(99999.0),
    m_chi2cut(20.0),
    m_nHitPix(1),
    m_nHitSct(6),
    m_onlyTightPTrk(false),
    m_onlyLoosePTrk(false),
    m_onlyLooseTrk(true),
    m_trkSelector("InDet::TrackSelectorTool"),
    m_electronCollectionKey("Electrons"),
    m_eleThresholdPt(6000.0),
    m_leptonTrkThresholdPt(1000.0),
    m_muThresholdPt(4000.0),
    m_ZMassMin(70000.0),
    m_ZMassMax(112000.0),
    m_ZeeChi2Cut(200.0),
    m_ZmumuChi2Cut(200.0),
    m_muonCollectionKey("Muons"),
    m_ditrackPtMin(2000.0), 
    m_leadingTrackPt(1000.0),
    m_deltaPhiTracks(99999.0),
    m_deltaRTracks(99999.0),
    m_chiSqProbMin(-1.0)
  {
    declareInterface<DerivationFramework::IAugmentationTool>(this);

    // Declare user-defined properties
    declareProperty("DiLeptonVertexContainer", m_diLeptonCollectionToCheck);
    declareProperty("PassFlagsToCheck", m_passFlagsToCheck);
    declareProperty("V0Tools", m_v0Tools);
    declareProperty("TrkVertexFitterTool",m_iVertexFitter);
    declareProperty("VertexEstimator", m_vertexEstimator);
    declareProperty("InputTrackParticleContainerName", m_inputTrackParticleContainerName);
    declareProperty("GSFCollection", m_TrkParticleGSFCollection);
    declareProperty("HCandidateContainerName", m_HCandidateContainerName);
    declareProperty("TrackPtMin", m_trackPtMin = 400.0); 
    declareProperty("deltaz0PVsinthetaMax", m_deltaz0PVsinthetaMax = 99999.0); 
    declareProperty("deltaz0PVsignificanceMax", m_deltaz0PVsignificanceMax = 99999.0); 
    declareProperty("trkZDeltaZ", m_trkZDeltaZ = -1.0);
    declareProperty("DitrackMassMax", m_ditrackMassMax = 1600.0);
    declareProperty("ZisMuons", m_ZisMuons = true); 
    declareProperty("ZisElectrons", m_ZisElectrons = false); 
    declareProperty("d0significanceMax", m_d0significanceMax = 99999.0); 
    declareProperty("HcandidateMassMin", m_HcandidateMassMin = 98000.0);  
    declareProperty("Chi2cut", m_chi2cut = 50.0);  
    declareProperty("nHitPix", m_nHitPix = 1); 
    declareProperty("nHitSct", m_nHitSct = 0);
    declareProperty("onlyTightPTrk", m_onlyTightPTrk = true);
    declareProperty("onlyLoosePTrk", m_onlyLoosePTrk = false);
    declareProperty("onlyLooseTrk", m_onlyLooseTrk = false);
    declareProperty("UseGSFTrackIndices", m_useGSFTrackIndices); 
    declareProperty("electronCollectionKey", m_electronCollectionKey);
    declareProperty("TrackSelectorTool",m_trkSelector); 
    declareProperty("ElectronThresholdPt",m_eleThresholdPt); 
    declareProperty("LeptonTrackThresholdPt",m_leptonTrkThresholdPt); 
    declareProperty("MuonThresholdPt",m_muThresholdPt); 
    declareProperty("ZMassMin", m_ZMassMin);
    declareProperty("ZMassMax", m_ZMassMax); 
    declareProperty("ZeeChi2Cut", m_ZeeChi2Cut); 
    declareProperty("ZmumuChi2Cut", m_ZmumuChi2Cut);
    declareProperty("muonCollectionKey", m_muonCollectionKey); 
    declareProperty("DitrackPtMin", m_ditrackPtMin);
    declareProperty("LeadingTrackPt", m_leadingTrackPt); 
    declareProperty("DeltaPhiTracks", m_deltaPhiTracks);
    declareProperty("DeltaRTracks", m_deltaRTracks); 
    declareProperty("ChiSqProbMin", m_chiSqProbMin);
  }

  // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

  StatusCode HDBSa0Finder::initialize()
  {

    ATH_MSG_DEBUG("in initialize()");

    // Get the track selector tool from ToolSvc
    if ( m_trkSelector.retrieve().isFailure() ) {
        ATH_MSG_FATAL("Failed to retrieve tool " << m_trkSelector);
        return StatusCode::FAILURE;
    } else {
        ATH_MSG_DEBUG("Retrieved tool " << m_trkSelector);
    }

    ATH_CHECK( m_v0Tools.retrieve() );
    ATH_CHECK( m_vertexEstimator.retrieve() );

    ATH_CHECK(m_TrackSelectionToolTightP.retrieve());
    ATH_CHECK(m_TrackSelectionToolLooseP.retrieve());
    ATH_CHECK(m_TrackSelectionToolLoose.retrieve());

    // retrieving vertex Fitter
    if ( m_iVertexFitter.retrieve().isFailure() ) {
        ATH_MSG_FATAL("Failed to retrieve tool " << m_iVertexFitter);
        return StatusCode::FAILURE;
    } else {
        ATH_MSG_INFO("Retrieved tool " << m_iVertexFitter);
    }
    m_VKVFitter = dynamic_cast<Trk::TrkVKalVrtFitter*>(&(*m_iVertexFitter));

    m_useGSFTrack.reset();
    for(int i : m_useGSFTrackIndices) m_useGSFTrack.set(i, true);

    return StatusCode::SUCCESS;

  }

  // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

  StatusCode HDBSa0Finder::finalize()
  {
    // everything all right
    return StatusCode::SUCCESS;
  }

  // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

  StatusCode HDBSa0Finder::addBranches() const
  {

    int nTrackPairs_Init = 0;
    int nTrackPairs_Selected = 0;
    int nConv_VertexFit = 0;
    int nConv_Selected = 0;
    int nConv_Selected_DeltaM = 0;

    double mass_lepton = 0;
    if(m_ZisMuons) mass_lepton = mass_muon;
    if(!m_ZisMuons) mass_lepton = mass_electron;
    
    // Retrieve track particles from StoreGate   
    const xAOD::TrackParticleContainer* inputTrackParticles(0);
    StatusCode sc = evtStore()->retrieve(inputTrackParticles,m_inputTrackParticleContainerName);
    if(sc.isFailure()){
        ATH_MSG_WARNING("No TrackParticle collection with name " << m_inputTrackParticleContainerName << " found in StoreGate!");
        return StatusCode::SUCCESS;;
    } else {
        ATH_MSG_DEBUG("Found TrackParticle collection " << m_inputTrackParticleContainerName << " in StoreGate!");
    }
    ATH_MSG_DEBUG("ID TrackParticle container size "<< inputTrackParticles->size());

    std::vector<const xAOD::Vertex*> dileptonVertices;
    dileptonVertices.clear();
    std::vector<std::pair<const xAOD::Electron*, const xAOD::Electron*>> elePairs_vertex;
    elePairs_vertex.clear(); 
    std::vector<std::pair<const xAOD::Muon*, const xAOD::Muon*>> muPairs_vertex;
    muPairs_vertex.clear();

    // Get the electrons from StoreGate
    const xAOD::ElectronContainer* importedElectronCollection;
    const xAOD::TrackParticleContainer* importedGSFTrackCollection(nullptr);

    if(m_useGSFTrack.any()){
      ATH_CHECK(evtStore()->retrieve(importedGSFTrackCollection, m_TrkParticleGSFCollection));
      StatusCode sc = evtStore()->retrieve(importedElectronCollection,m_electronCollectionKey);
      if(sc.isFailure()){
        ATH_MSG_WARNING("a0Finder: No electron collection with key " << m_electronCollectionKey << " found in StoreGate. EECandidates will be EMPTY!");
        return StatusCode::SUCCESS;
      }else{
        ATH_MSG_DEBUG("a0Finder: Found electron collections with key "<<m_electronCollectionKey);
      }
      ATH_MSG_DEBUG("a0Finder: Electron container size "<<importedElectronCollection->size());   

      xAOD::ElectronContainer::const_iterator elItr; 
      std::vector<const xAOD::Electron*> elePos; elePos.clear();
      std::vector<const xAOD::Electron*> eleNeg; eleNeg.clear();
      for (elItr=importedElectronCollection->begin(); elItr!=importedElectronCollection->end(); ++elItr) {
        if ( *elItr == NULL ) continue;
        if (!(*elItr)->trackParticleLink().isValid()) continue; // No electrons without ID tracks
          const xAOD::TrackParticle* elTrk(0);
          elTrk = (*elItr)->trackParticleLink().cachedElement();
          if ( elTrk==NULL) continue;
          const xAOD::Vertex* vx = 0;
          if ( !m_trkSelector->decision(*elTrk, vx) ) continue; // all ID tracks must pass basic tracking cuts
          if ( fabs(elTrk->pt())<m_leptonTrkThresholdPt ) continue; 
          if ( fabs((*elItr)->pt())<m_eleThresholdPt ) continue;
          if( (*elItr)->charge() > 0.0) {
              elePos.push_back(*elItr);
            } else {
              eleNeg.push_back(*elItr);
            }
        }
      ATH_MSG_DEBUG("a0Finder: " << elePos.size() + eleNeg.size() << " electrons pass pre-selection");
      
      std::vector<const xAOD::Electron*>::const_iterator eleIt1;
      std::vector<const xAOD::Electron*>::const_iterator eleIt2;  

      std::vector<std::pair<const xAOD::Electron*, const xAOD::Electron*>> elePairs;

      // Pos Loop
      for(eleIt1 = elePos.begin(); eleIt1 != elePos.end(); ++eleIt1) {

        const xAOD::Electron* electron1 = (*eleIt1);

        // Neg Loop
        for(eleIt2 = eleNeg.begin(); eleIt2 != eleNeg.end(); ++eleIt2) {

          if (*eleIt1 == *eleIt2) continue;

          const xAOD::Electron* electron2 = (*eleIt2);

          TLorentzVector ele_vec1, ele_vec2;
          ele_vec1.SetPtEtaPhiM(electron1->pt(), electron1->eta(), electron1->phi(), mass_electron);
          ele_vec2.SetPtEtaPhiM(electron2->pt(), electron2->eta(), electron2->phi(), mass_electron);

          if((ele_vec1+ele_vec2).M() > m_ZMassMax or (ele_vec1+ele_vec2).M() < m_ZMassMin) continue;

          const xAOD::TrackParticle* elTrk1(0);
          elTrk1 = electron1->trackParticleLink().cachedElement();
          const xAOD::TrackParticle* elTrk2(0);
          elTrk2 = electron2->trackParticleLink().cachedElement();
          std::vector<const xAOD::TrackParticle*> theTracks(2, nullptr);//Initialise as 2 nulls
          theTracks[0] = elTrk1;
          theTracks[1] = elTrk2;
          const Trk::Perigee& aPerigee1 = theTracks[0]->perigeeParameters();
          const Trk::Perigee& aPerigee2 = theTracks[1]->perigeeParameters();
          int sflag = 0;
          int errorcode = 0;
          Amg::Vector3D startingPoint = m_vertexEstimator->getCirclesIntersectionPoint(&aPerigee1,&aPerigee2,sflag,errorcode);
          if (errorcode != 0) {startingPoint(0) = 0.0; startingPoint(1) = 0.0; startingPoint(2) = 0.0;}
          xAOD::Vertex* myVxCandidate = m_iVertexFitter->fit(theTracks, startingPoint);
          ATH_MSG_DEBUG("a0Finder: Initial fit was a success! " << myVxCandidate);
          if(myVxCandidate != 0){
            std::vector<ElementLink<DataVector<xAOD::TrackParticle> > > newLinkVector;
            for(unsigned int j=0; j< myVxCandidate->trackParticleLinks().size(); j++){ 
                ElementLink<DataVector<xAOD::TrackParticle> > mylink=myVxCandidate->trackParticleLinks()[j]; //makes a copy (non-const) 
                mylink.setStorableObject(*importedGSFTrackCollection, true); 
                newLinkVector.push_back( mylink );
              }
            myVxCandidate->clearTracks();
            myVxCandidate->setTrackParticleLinks( newLinkVector );
            ATH_MSG_DEBUG("a0Finder: Set all links");
            double chi2 = myVxCandidate->chiSquared();
            if (m_ZeeChi2Cut == 0.0 || chi2 <= m_ZeeChi2Cut) { 
              ATH_MSG_DEBUG("a0Finder: vertex passed chi2 cut, chi2 is: " << chi2);
              xAOD::BPhysHelper ZeeHelper(myVxCandidate);
              ZeeHelper.setRefTrks();
              std::vector<const xAOD::Electron*> theStoredElectrons;
              theStoredElectrons.push_back(electron1);
              theStoredElectrons.push_back(electron2);
              ZeeHelper.setElectrons(theStoredElectrons,importedElectronCollection);
              // Retain the vertex
              dileptonVertices.push_back(myVxCandidate);
              elePairs_vertex.push_back(std::make_pair(electron1, electron2));
            }
            else delete myVxCandidate; //chi2 failed
          }
          else ATH_MSG_DEBUG("a0Finder: Zee Fitter failed!");

        } // loop over negative electrons
      } // loop over positive electrons

      ATH_MSG_DEBUG("a0Finder: " << dileptonVertices.size() << " Zee vertices passed");
    }

    // Get the muons from StoreGate
    const xAOD::MuonContainer* importedMuonCollection;
    if(m_ZisMuons){
      StatusCode sc = evtStore()->retrieve(importedMuonCollection,m_muonCollectionKey);
      if(sc.isFailure()){
          ATH_MSG_WARNING("No muon collection with key " << m_muonCollectionKey << " found in StoreGate. JpsiCandidates will be EMPTY!");
          return StatusCode::SUCCESS;
      }else{
          ATH_MSG_DEBUG("Found muon collections with key "<<m_muonCollectionKey);
      }
      ATH_MSG_DEBUG("Muon container size "<<importedMuonCollection->size());

      xAOD::MuonContainer::const_iterator muItr; 
      std::vector<const xAOD::Muon*> muPos; muPos.clear();
      std::vector<const xAOD::Muon*> muNeg; muNeg.clear();
      for (muItr=importedMuonCollection->begin(); muItr!=importedMuonCollection->end(); ++muItr) {
        if ( *muItr == NULL ) continue;
        if (!(*muItr)->inDetTrackParticleLink().isValid()) continue; // No muons without ID tracks
          const xAOD::TrackParticle* muTrk(0);
          muTrk = (*muItr)->inDetTrackParticleLink().cachedElement();
          if ( muTrk==NULL) continue;
          const xAOD::Vertex* vx = 0;
          if ( !m_trkSelector->decision(*muTrk, vx) ) continue; // all ID tracks must pass basic tracking cuts
          if ( fabs(muTrk->pt())<m_leptonTrkThresholdPt ) continue; 
          if ( (*muItr)->pt()<m_muThresholdPt ) continue;
          if ((*muItr)->muonType() == xAOD::Muon::SiliconAssociatedForwardMuon) continue;
          if( (*muItr)->charge() > 0.0) {
            muPos.push_back(*muItr);
            } else {
            muNeg.push_back(*muItr);
            }
        }
        
      ATH_MSG_DEBUG("a0Finder: " << muPos.size() + muNeg.size() << " muons pass pre-selection");
      
      std::vector<const xAOD::Muon*>::const_iterator muIt1;
      std::vector<const xAOD::Muon*>::const_iterator muIt2; 

      // Pos Loop
      for(muIt1 = muPos.begin(); muIt1 != muPos.end(); ++muIt1) {

        const xAOD::Muon* muon1 = (*muIt1);

        // Neg Loop
        for(muIt2 = muNeg.begin(); muIt2 != muNeg.end(); ++muIt2) {

          if (*muIt1 == *muIt2) continue;
          if ((*muIt1)->muonType() != xAOD::Muon::Combined and (*muIt2)->muonType() != xAOD::Muon::Combined) continue; // at least one combined muon

          const xAOD::Muon* muon2 = (*muIt2);

          TLorentzVector mu_vec1, mu_vec2;
          mu_vec1.SetPtEtaPhiM(muon1->pt(), muon1->eta(), muon1->phi(), mass_muon);
          mu_vec2.SetPtEtaPhiM(muon2->pt(), muon2->eta(), muon2->phi(), mass_muon);

          if((mu_vec1+mu_vec2).M() > m_ZMassMax or (mu_vec1+mu_vec2).M() < m_ZMassMin) continue;
          ATH_MSG_DEBUG("a0Finder: muon pair passed Z mass requirement");
          const xAOD::TrackParticle* muTrk1 = muon1->trackParticle( xAOD::Muon::InnerDetectorTrackParticle );
          const xAOD::TrackParticle* muTrk2 = muon2->trackParticle( xAOD::Muon::InnerDetectorTrackParticle );
          std::vector<const xAOD::TrackParticle*> theTracks(2, nullptr);//Initialise as 2 nulls
          theTracks[0] = muTrk1;
          theTracks[1] = muTrk2;
          const Trk::Perigee& aPerigee1 = theTracks[0]->perigeeParameters();
          const Trk::Perigee& aPerigee2 = theTracks[1]->perigeeParameters();
          int sflag = 0;
          int errorcode = 0;
          Amg::Vector3D startingPoint = m_vertexEstimator->getCirclesIntersectionPoint(&aPerigee1,&aPerigee2,sflag,errorcode);
          if (errorcode != 0) {startingPoint(0) = 0.0; startingPoint(1) = 0.0; startingPoint(2) = 0.0;}
          xAOD::Vertex* myVxCandidate = m_iVertexFitter->fit(theTracks, startingPoint);
          ATH_MSG_DEBUG("a0Finder: Initial fit was a success! " << myVxCandidate);
          if(myVxCandidate != 0){
            std::vector<ElementLink<DataVector<xAOD::TrackParticle> > > newLinkVector;
            for(unsigned int j=0; j< myVxCandidate->trackParticleLinks().size(); j++){ 
                ElementLink<DataVector<xAOD::TrackParticle> > mylink=myVxCandidate->trackParticleLinks()[j]; //makes a copy (non-const) 
                mylink.setStorableObject(*inputTrackParticles, true); 
                newLinkVector.push_back( mylink );
              }
            myVxCandidate->clearTracks();
            myVxCandidate->setTrackParticleLinks( newLinkVector );
            ATH_MSG_DEBUG("a0Finder: Set all links");
            double chi2 = myVxCandidate->chiSquared();
            if (m_ZmumuChi2Cut == 0.0 || chi2 <= m_ZmumuChi2Cut) { 
              ATH_MSG_DEBUG("a0Finder: vertex passed chi2 cut, chi2 is: " << chi2);
              xAOD::BPhysHelper ZmumuHelper(myVxCandidate);
              ZmumuHelper.setRefTrks();
              std::vector<const xAOD::Muon*> theStoredMuons;
              theStoredMuons.push_back(muon1);
              theStoredMuons.push_back(muon2);
              ZmumuHelper.setMuons(theStoredMuons,importedMuonCollection);
              // Retain the vertex
              dileptonVertices.push_back(myVxCandidate);
              muPairs_vertex.push_back(std::make_pair(muon1, muon2));
            }
            else delete myVxCandidate; //chi2 failed
          }
          else ATH_MSG_DEBUG("a0Finder: Zmumu Fitter failed!");
    
        } // loop over negative muons
      } // loop over positive muons


      ATH_MSG_DEBUG("a0Finder: " << dileptonVertices.size() << " Zmumu vertices passed");

    }

    // Output HCandidate container
    std::unique_ptr<xAOD::VertexContainer> HCandidateContainer( new xAOD::VertexContainer() );
    std::unique_ptr<xAOD::VertexAuxContainer> HCandidateAuxContainer( new xAOD::VertexAuxContainer() );
    HCandidateContainer->setStore(HCandidateAuxContainer.get());

    // Get primary vertex
    const xAOD::VertexContainer* primaryVertices { nullptr };
    ATH_CHECK( evtStore()->retrieve( primaryVertices, "PrimaryVertices" ) );

    const xAOD::Vertex* priVtx { nullptr };
    for( const auto* vtx : *primaryVertices ) {
      if( vtx->vertexType() == xAOD::VxType::PriVtx ) {
        priVtx = vtx;
        break;
      }
    }

    ATH_MSG_DEBUG("a0Finder: Track particle container size " <<  inputTrackParticles->size());

    // Track Selection
    std::vector<const xAOD::TrackParticle*> posTracks; posTracks.clear();
    std::vector<const xAOD::TrackParticle*> negTracks; negTracks.clear();

    if(dileptonVertices.size() != 0){
      // Track Loop
      for(xAOD::TrackParticleContainer::const_iterator trkItr = inputTrackParticles->begin(); trkItr != inputTrackParticles->end(); ++trkItr) {

          const xAOD::TrackParticle* track = (*trkItr);

        // skip tracks from Z vertex
        for ( std::vector<const xAOD::Vertex*>::const_iterator vtxItr = dileptonVertices.begin(); vtxItr != dileptonVertices.end(); ++vtxItr ) {
          const xAOD::TrackParticle* lepton_trk1 = (*vtxItr)->trackParticle(0);
          const xAOD::TrackParticle* lepton_trk2 = (*vtxItr)->trackParticle(1);
          if(m_trkZDeltaZ>0 && fabs(track->z0() + track->vz() - (*vtxItr)->z()) > m_trkZDeltaZ ) continue;
          if(track == lepton_trk1 or track == lepton_trk2) continue; 
        }
          uint8_t nSCT(0);
          uint8_t nPIX(0);

          track->summaryValue(nPIX,xAOD::numberOfPixelHits);
          track->summaryValue(nSCT,xAOD::numberOfSCTHits);
          // Don't want TRT-only tracks
          // Require Si hits on all tracks
          if( nSCT + nPIX < m_nHitPix ) continue;
          if (nSCT < m_nHitSct ) continue;
          if (track->pt() < m_trackPtMin) continue;
          
          double d0significance = fabs(track->d0()) / sqrt(track->definingParametersCovMatrix()(0,0));
          if (d0significance > m_d0significanceMax) continue;
          if(!priVtx) {
            bool trk_TightP = m_TrackSelectionToolTightP->accept(*track);
            bool trk_LooseP = m_TrackSelectionToolLooseP->accept(*track);
            bool trk_Loose = m_TrackSelectionToolLoose->accept(*track);
            if(m_onlyTightPTrk and !trk_TightP) continue;
            if(m_onlyLoosePTrk and !trk_LooseP) continue;
            if(m_onlyLooseTrk and !trk_Loose) continue;
            ATH_MSG_DEBUG("a0Finder: no primary vertex found!");
          }
          if(priVtx){
            double z0value  = track->z0() + track->vz() - priVtx->z();
            if(fabs(z0value*sin(track->theta())) > m_deltaz0PVsinthetaMax) continue;
            double sigma_z0 = sqrt(track->definingParametersCovMatrix()(1,1));
            if (sigma_z0<=0.) continue;
            double z0significance = 999;
            if (sigma_z0>0.) z0significance = fabs(z0value/sigma_z0);
            if(z0significance > m_deltaz0PVsignificanceMax) continue;
            bool trk_PV_TightP = m_TrackSelectionToolTightP->accept(*track, priVtx); 
            bool trk_PV_LooseP = m_TrackSelectionToolLooseP->accept(*track, priVtx); 
            bool trk_PV_Loose = m_TrackSelectionToolLoose->accept(*track, priVtx); 
            if(m_onlyTightPTrk and !trk_PV_TightP) continue;
            if(m_onlyLoosePTrk and !trk_PV_LooseP) continue;
            if(m_onlyLooseTrk and !trk_PV_Loose) continue;
          }

          if( track->charge() > 0.0) {
              posTracks.push_back(track);
          } else {
              negTracks.push_back(track);
          }

      } // Track Loop
    }


    ATH_MSG_DEBUG("a0Finder: " << posTracks.size() + negTracks.size() << " tracks pass pre-selection");
    
    std::vector<const xAOD::TrackParticle*>::const_iterator tpIt1;
    std::vector<const xAOD::TrackParticle*>::const_iterator tpIt2;  

    std::vector<std::tuple<const xAOD::TrackParticle*, const xAOD::TrackParticle*, double>> trackPairs;

    // Pos Track Loop
    for(tpIt1 = posTracks.begin(); tpIt1 != posTracks.end(); ++tpIt1) {

      const xAOD::TrackParticle* trackParticle1 = (*tpIt1);

      // Neg Track Loop
      for(tpIt2 = negTracks.begin(); tpIt2 != negTracks.end(); ++tpIt2) {

        if (*tpIt1 == *tpIt2) continue;

        const xAOD::TrackParticle* trackParticle2 = (*tpIt2);

        nTrackPairs_Init++;

        TLorentzVector track1, track2;
        track1.SetPtEtaPhiM(trackParticle1->pt(), trackParticle1->eta(), trackParticle1->phi(), mass_pion);
        track2.SetPtEtaPhiM(trackParticle2->pt(), trackParticle2->eta(), trackParticle2->phi(), mass_pion);

        double ditrackPt = (track1+track2).Pt();
        if((track1+track2).M() > m_ditrackMassMax) continue;
        if(ditrackPt < m_ditrackPtMin) continue;
        if(trackParticle1->pt() < m_leadingTrackPt and trackParticle2->pt() < m_leadingTrackPt) continue;
        double deltaR_tracks = track1.DeltaR(track2);
        if (deltaR_tracks > m_deltaRTracks) continue;
        double deltaPhi_tracks = track1.DeltaPhi(track2);
        if (deltaPhi_tracks > m_deltaPhiTracks) continue;
        

        trackPairs.push_back(std::make_tuple(trackParticle1, trackParticle2, ditrackPt));

      } // loop over negative tracks
    } // loop over positive tracks

    std::sort(trackPairs.begin(), trackPairs.end(),sortDitrackPt); // sort track pair vector from highest ditrack pt to lowest
    ATH_MSG_DEBUG("a0Finder: " << trackPairs.size() << " track pairs to test");
    
    int itr = 0;
    for ( std::vector<const xAOD::Vertex*>::const_iterator vtxItr = dileptonVertices.begin(); vtxItr != dileptonVertices.end(); ++vtxItr ) {

      const xAOD::TrackParticle* lepton_trk1 = (*vtxItr)->trackParticle(0);
      const xAOD::TrackParticle* lepton_trk2 = (*vtxItr)->trackParticle(1);
      TLorentzVector lepton1, lepton2;
      if(!m_ZisMuons){
        const xAOD::Electron* ele1 = std::get<0>(elePairs_vertex[itr]);
        const xAOD::Electron* ele2 = std::get<1>(elePairs_vertex[itr]);
        lepton1.SetPtEtaPhiM(ele1->pt(), ele1->eta(), ele1->phi(), mass_lepton);
        lepton2.SetPtEtaPhiM(ele2->pt(), ele2->eta(), ele2->phi(), mass_lepton);
      }
      if(m_ZisMuons){
        const xAOD::Muon* mu1 = std::get<0>(muPairs_vertex[itr]);
        const xAOD::Muon* mu2 = std::get<1>(muPairs_vertex[itr]);
        lepton1.SetPtEtaPhiM(mu1->pt(), mu1->eta(), mu1->phi(), mass_lepton);
        lepton2.SetPtEtaPhiM(mu2->pt(), mu2->eta(), mu2->phi(), mass_lepton);
      }    
      itr++;

      int vertexPassed = 0;
      for(unsigned int i=0;i<trackPairs.size();i++){
        ATH_MSG_DEBUG("a0Finder: sanity check, ditrack pt: " << std::get<2>(trackPairs[i]));
        const xAOD::TrackParticle* trackParticle1 = std::get<0>(trackPairs[i]);
        const xAOD::TrackParticle* trackParticle2 = std::get<1>(trackPairs[i]);
        ATH_MSG_DEBUG("a0Finder: got tracks from vector of tuples");

        TLorentzVector track1, track2;
        track1.SetPtEtaPhiM(trackParticle1->pt(), trackParticle1->eta(), trackParticle1->phi(), mass_pion);
        track2.SetPtEtaPhiM(trackParticle2->pt(), trackParticle2->eta(), trackParticle2->phi(), mass_pion);

        TLorentzVector Hcandidate = lepton1 + lepton2 + track1 + track2;
        if(Hcandidate.M() < m_HcandidateMassMin) continue;

        std::vector<const xAOD::TrackParticle*> QuadletTracks(4, nullptr);//Initialise as 4 nulls
        QuadletTracks[0] = lepton_trk1;
        QuadletTracks[1] = lepton_trk2;
        QuadletTracks[2] = trackParticle1;
        QuadletTracks[3] = trackParticle2;

        m_VKVFitter->setDefault();
        ATH_MSG_DEBUG("a0Finder: vkvfitter set default");
        // Do the fit itself
        // Starting point (use the Z position)
        const Trk::Perigee& aPerigee1 = QuadletTracks[0]->perigeeParameters();
        const Trk::Perigee& aPerigee2 = QuadletTracks[1]->perigeeParameters();
        ATH_MSG_DEBUG("a0Finder: perigee parameters called succesfully");
        int sflag = 0;
        int errorcode = 0;
        Amg::Vector3D startingPoint = m_vertexEstimator->getCirclesIntersectionPoint(&aPerigee1,&aPerigee2,sflag,errorcode);
        ATH_MSG_DEBUG("a0Finder: vertex estimator success");
        if (errorcode != 0) {
          ATH_MSG_DEBUG("a0Finder: vertex estimator error");
          startingPoint(0) = 0.0; startingPoint(1) = 0.0; startingPoint(2) = 0.0;}
        xAOD::Vertex* HVertex = m_VKVFitter->fit(QuadletTracks, startingPoint);
        ATH_MSG_DEBUG("a0Finder: vertex fitter success");

        if(HVertex != 0){
            std::vector<ElementLink<DataVector<xAOD::TrackParticle> > > newLinkVector;
            ATH_MSG_DEBUG("a0Finder: setting new track particle links");
            for(unsigned int j=0; j< HVertex->trackParticleLinks().size(); j++){
              ElementLink<DataVector<xAOD::TrackParticle> > mylink=HVertex->trackParticleLinks()[j]; //makes a copy (non-const)
              ATH_MSG_DEBUG("a0Finder: got link "<< j);
              mylink.setStorableObject( m_useGSFTrack[j] ?  *importedGSFTrackCollection : *inputTrackParticles, true);
              newLinkVector.push_back( mylink );
            }
            HVertex->clearTracks();
            HVertex->setTrackParticleLinks( newLinkVector);
        }

        if(HVertex == 0){
          delete HVertex;
          continue;
        } 

        double HChi2DOF = HVertex->chiSquared()/HVertex->numberDoF();
        ATH_MSG_DEBUG("a0Finder: Candidate chi2/DOF is " << HChi2DOF);
        if(m_chi2cut > 0.0 and HChi2DOF > m_chi2cut) {
          delete HVertex;
          ATH_MSG_DEBUG("a0Finder: vertex after fit failed chi2/DOF: "<< HChi2DOF);
          continue;
        }

        xAOD::BPhysHelper bHelper(HVertex);
        bHelper.setRefTrks();

        TLorentzVector tr1 = bHelper.refTrk(2,mass_pion);
        TLorentzVector tr2 = bHelper.refTrk(3,mass_pion);
        double DiTrkMass = (tr1+tr2).M();
        if(DiTrkMass > m_ditrackMassMax) {
          delete HVertex;
          ATH_MSG_DEBUG("a0Finder: vertex after fit failed DiTrackMassMin: "<< DiTrkMass);
          continue;
        }
        double chiSq_prob = m_v0Tools->vertexProbability(HVertex);
        if(chiSq_prob < m_chiSqProbMin){
          delete HVertex;
          ATH_MSG_DEBUG("a0Finder: vertex after fit failed chiSqProbMin: "<< m_chiSqProbMin);
          continue;
        }
        double Zvtx_z = (*vtxItr)->z();
        HVertex->auxdata<double>("HVertex_chiSq_prob") = chiSq_prob;
        HVertex->auxdata<double>("Zvertex_z") = Zvtx_z;
        HCandidateContainer->push_back(HVertex);
        vertexPassed++;
      } // loop over track pairs

    } // loop over diLepton vertices
    
    ATH_MSG_DEBUG("a0Finder: number of vertices in container after fit: " << HCandidateContainer->size());
    // Write the results to StoreGate
    CHECK(evtStore()->record(HCandidateContainer.release(), m_HCandidateContainerName));
    CHECK(evtStore()->record(HCandidateAuxContainer.release(), m_HCandidateContainerName+"Aux."));

    ATH_MSG_DEBUG("-------------------------");
    ATH_MSG_DEBUG("Number of track pairs: " << nTrackPairs_Init);
    ATH_MSG_DEBUG("Number of track pairs selected: " << nTrackPairs_Selected);
    ATH_MSG_DEBUG("Number of successful vertex fits: " << nConv_VertexFit);
    ATH_MSG_DEBUG("Number of selected vertices: " << nConv_Selected);
    ATH_MSG_DEBUG("Number of selected vertices (after DeltaM req.): " << nConv_Selected_DeltaM);

    return StatusCode::SUCCESS;
  }

}
