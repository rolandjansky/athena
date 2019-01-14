/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
/////////////////////////////////////////////////////////////////
// BPhysConversionFinder.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Author: A. Chisholm <andrew.chisholm@cern.ch>
#include "DerivationFrameworkBPhys/BPhysConversionFinder.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/VertexAuxContainer.h"
#include "TrkVertexAnalysisUtils/V0Tools.h"
#include "GaudiKernel/IPartPropSvc.h"

#include "TrkVertexFitterInterfaces/IVertexFitter.h"
#include "TrkVKalVrtFitter/TrkVKalVrtFitter.h"

namespace DerivationFramework {

  BPhysConversionFinder::BPhysConversionFinder(const std::string& t,
      const std::string& n,
      const IInterface* p) :
    AthAlgTool(t,n,p),
    m_v0Tools("Trk::V0Tools"),
    m_vertexFitter("Trk::TrkVKalVrtFitter"),
    m_vertexEstimator("InDet::VertexPointEstimator"),
    m_distanceTool("Trk::SeedNewtonDistanceFinder/InDetConversionTrkDistanceFinder"),
    m_postSelector("InDet::ConversionPostSelector"),
    m_cascadeFitter("Trk::TrkVKalVrtFitter"),
    m_inputTrackParticleContainerName("InDetTrackParticles"),
    m_conversionContainerName("BPhysConversionCandidates"),
    m_maxDistBetweenTracks(10.0),
    m_maxDeltaCotTheta(0.3),
    m_requireDeltaM(true),
    m_maxDeltaM(3000.0)
  {
    declareInterface<DerivationFramework::IAugmentationTool>(this);

    // Declare user-defined properties
    declareProperty("DiMuonVertexContainer", m_diMuonCollectionToCheck);
    declareProperty("PassFlagsToCheck", m_passFlagsToCheck);
    declareProperty("V0Tools", m_v0Tools);
    declareProperty("VertexFitterTool", m_vertexFitter);
    declareProperty("VertexEstimator", m_vertexEstimator);
    declareProperty("DistanceTool", m_distanceTool);
    declareProperty("ConversionPostSelector", m_postSelector);
    declareProperty("CascadeFitter", m_cascadeFitter);
    declareProperty("InputTrackParticleContainerName", m_inputTrackParticleContainerName);
    declareProperty("ConversionContainerName", m_conversionContainerName);
    declareProperty("MaxDistBetweenTracks", m_maxDistBetweenTracks = 10.0); // Maximum allowed distance of minimum approach
    declareProperty("MaxDeltaCotTheta", m_maxDeltaCotTheta = 0.3); // Maximum allowed dCotTheta between tracks
    declareProperty("RequireDeltaM", m_requireDeltaM = true); // Only save a conversions if it's a chi_c,b candidate (must then pass "MaxDeltaM" requirement), if "False" all conversions in the event will be saved
    declareProperty("MaxDeltaM", m_maxDeltaM = 3000.0); // Maximum mass difference between di-muon+conversion and di-muon

  }

  // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

  StatusCode BPhysConversionFinder::initialize()
  {

    ATH_MSG_DEBUG("in initialize()");

    ATH_CHECK( m_v0Tools.retrieve() );
    ATH_CHECK( m_vertexFitter.retrieve() );
    ATH_CHECK( m_vertexEstimator.retrieve() );
    ATH_CHECK( m_distanceTool.retrieve() );
    ATH_CHECK( m_postSelector.retrieve() );
    ATH_CHECK( m_cascadeFitter.retrieve() );

    return StatusCode::SUCCESS;

  }

  // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

  StatusCode BPhysConversionFinder::finalize()
  {
    // everything all right
    return StatusCode::SUCCESS;
  }

  // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

  StatusCode BPhysConversionFinder::addBranches() const
  {

    int nTrackPairs_Init = 0;
    int nTrackPairs_Selected = 0;
    int nConv_VertexFit = 0;
    int nConv_Selected = 0;
    int nConv_Selected_DeltaM = 0;

    std::vector<const xAOD::Vertex*> oniaVertices;
    oniaVertices.clear();

    //------------------------------------
    // Look for di-muons
    //------------------------------------
    const xAOD::VertexContainer* diMuonContainer = NULL;
    ATH_CHECK( evtStore()->retrieve(diMuonContainer, m_diMuonCollectionToCheck) );

    if(diMuonContainer->size() == 0) {

        ATH_MSG_DEBUG("Vertex Container (" << m_diMuonCollectionToCheck << ") is empty");

    } else {

        ATH_MSG_DEBUG("Vertex Container (" << m_diMuonCollectionToCheck << ") contains " << diMuonContainer->size() << " vertices");

        for(xAOD::VertexContainer::const_iterator vtxItr = diMuonContainer->begin(); vtxItr != diMuonContainer->end(); ++vtxItr) {

            const xAOD::Vertex* vertex = (*vtxItr);

            bool passedHypothesis = false;

            for(const auto &flag : m_passFlagsToCheck) {
                bool pass = vertex->auxdata<Char_t>(flag);
                if(pass) passedHypothesis = true;
            }

            if(passedHypothesis) {
                oniaVertices.push_back(vertex);
            }

        }
     }
    //------------------------------------

    // Output conversion container
    std::unique_ptr<xAOD::VertexContainer> conversionContainer( new xAOD::VertexContainer() );
    std::unique_ptr<xAOD::VertexAuxContainer> conversionAuxContainer( new xAOD::VertexAuxContainer() );
    conversionContainer->setStore(conversionAuxContainer.get());

    // Only call conversion finder if we've found a di-muon candidate or
    // we really want to look for conversions independently
    const bool callConvFinder = !m_requireDeltaM || oniaVertices.size() > 0;

    if(callConvFinder) {

      // Retrieve track particles from StoreGate
      const xAOD::TrackParticleContainer* inputTrackParticles = NULL;
      ATH_CHECK( evtStore()->retrieve(inputTrackParticles,m_inputTrackParticleContainerName) );

      ATH_MSG_DEBUG("Track particle container size " <<  inputTrackParticles->size());

      // Track Selection
      std::vector<const xAOD::TrackParticle*> posTracks; posTracks.clear();
      std::vector<const xAOD::TrackParticle*> negTracks; negTracks.clear();

      // Track Loop
      for(xAOD::TrackParticleContainer::const_iterator trkItr = inputTrackParticles->begin(); trkItr != inputTrackParticles->end(); ++trkItr) {

          const xAOD::TrackParticle* track = (*trkItr);

          uint8_t nSCT(0);
          uint8_t nPIX(0);

          track->summaryValue(nPIX,xAOD::numberOfPixelHits);
          track->summaryValue(nSCT,xAOD::numberOfSCTHits);

          // Don't want TRT-only tracks
          // Require Si hits on all tracks
          if( nSCT + nPIX < 1 ) continue;

          if( track->charge() > 0.0) {
              posTracks.push_back(track);
          } else {
              negTracks.push_back(track);
          }

      } // Track Loop

      ATH_MSG_DEBUG(posTracks.size() + negTracks.size() << " tracks pass pre-selection");

      std::vector<const xAOD::TrackParticle*>::const_iterator tpIt1;
      std::vector<const xAOD::TrackParticle*>::const_iterator tpIt2;

      // Pos Track Loop
      for(tpIt1 = posTracks.begin(); tpIt1 != posTracks.end(); ++tpIt1) {

          const xAOD::TrackParticle* trackParticle1 = (*tpIt1);

          const Trk::Perigee& trackPerigee1 = trackParticle1->perigeeParameters();

          // Neg Track Loop
          for(tpIt2 = negTracks.begin(); tpIt2 != negTracks.end(); ++tpIt2) {

              if (*tpIt1 == *tpIt2) continue;

              const xAOD::TrackParticle* trackParticle2 = (*tpIt2);

              const Trk::Perigee& trackPerigee2 = trackParticle2->perigeeParameters();

              nTrackPairs_Init++;

              //------------------------------------
              // Track pair selection
              //------------------------------------
              const double m_deltaCotTheta = fabs(1./tan(trackPerigee1.parameters()[Trk::theta]) - 1./tan(trackPerigee2.parameters()[Trk::theta]));
              if(m_deltaCotTheta > m_maxDeltaCotTheta) continue;

              bool gotDistance = false;
              double m_distance = 1000000.;
              gotDistance = m_distanceTool->CalculateMinimumDistance(trackParticle1->perigeeParameters(),trackParticle2->perigeeParameters() );
              if(gotDistance) m_distance = m_distanceTool->GetDistance();
              if(!gotDistance || (m_distance > m_maxDistBetweenTracks)) continue;
              //------------------------------------

              //------------------------------------
              // Estimate starting point + cuts on compatiblity of tracks
              //------------------------------------
              int sflag = 0;
              int errorcode = 0;
              Amg::Vector3D startingPoint = m_vertexEstimator->getCirclesIntersectionPoint(&trackPerigee1,&trackPerigee2,sflag,errorcode);
              if(errorcode != 0) continue;
              //------------------------------------

              nTrackPairs_Selected++;

              std::vector<const xAOD::TrackParticle*> trackPair;
              trackPair.clear();
              trackPair.push_back(trackParticle1);
              trackPair.push_back(trackParticle2);

              // Do the vertex fit
              std::unique_ptr<xAOD::Vertex> convVertexCandidate( m_vertexFitter->fit(trackPair, startingPoint) );

              // Check for successful fit
              if(convVertexCandidate != NULL) {

                  ATH_MSG_DEBUG("Vertex Fit Succeeded");

                  convVertexCandidate->clearTracks();
                  ElementLink<xAOD::TrackParticleContainer> newLink1;
                  newLink1.setElement(*tpIt1);
                  newLink1.setStorableObject(*inputTrackParticles);
                  ElementLink<xAOD::TrackParticleContainer> newLink2;
                  newLink2.setElement(*tpIt2);
                  newLink2.setStorableObject(*inputTrackParticles);
                  convVertexCandidate->addTrackAtVertex(newLink1);
                  convVertexCandidate->addTrackAtVertex(newLink2);

                  nConv_VertexFit++;

                  //------------------------------------
                  // Post-vertexing cuts
                  //------------------------------------

                  // This is empty and only present for compatiblity.
                  // The cut this informtion pertains to is not used for Si-Si conversions so this is OK
                  std::vector<Amg::Vector3D> positionList;

                  // Apply Si-Si converion post-selection
                  if( !m_postSelector->selectConversionCandidate(convVertexCandidate.get(),0,positionList) ) {
                      convVertexCandidate.reset();
                      continue;
                  }
                  //------------------------------------

                  nConv_Selected++;

                  // Get photon momentum 3-vector
                  const xAOD::Vertex * constConvVertex = convVertexCandidate.get();
                  Amg::Vector3D momentum = m_v0Tools->V0Momentum(constConvVertex);

                  TLorentzVector photon;
                  photon.SetXYZM(momentum.x(),momentum.y(),momentum.z(),0.0);

                  //------------------------------------
                  // Check if conversion is consistent with a chi_c,b candidate
                  // by requiring a small mass difference w.r.t. any di-muon in event
                  //------------------------------------
                  bool passDeltaM = false;

                  // Use to keep track of which dimuon(s) gave a chi_c/b candidate
                  std::vector<const xAOD::Vertex*> candidateOniaVertices;
                  candidateOniaVertices.clear();

                  for ( std::vector<const xAOD::Vertex*>::const_iterator vtxItr = oniaVertices.begin(); vtxItr != oniaVertices.end(); ++vtxItr ) {

                      const xAOD::Vertex* oniaVertex = (*vtxItr);

                      std::vector<float> diMuon_Px = oniaVertex->auxdata< std::vector<float> >("RefTrackPx");
                      std::vector<float> diMuon_Py = oniaVertex->auxdata< std::vector<float> >("RefTrackPy");
                      std::vector<float> diMuon_Pz = oniaVertex->auxdata< std::vector<float> >("RefTrackPz");

                      TLorentzVector muon1, muon2;
                      muon1.SetXYZM(diMuon_Px.at(0),diMuon_Py.at(0),diMuon_Pz.at(0),105.658);
                      muon2.SetXYZM(diMuon_Px.at(1),diMuon_Py.at(1),diMuon_Pz.at(1),105.658);

                      TLorentzVector diMuon = muon1 + muon2;

                      const double deltaM = (diMuon+photon).M() - diMuon.M();

                      ATH_MSG_DEBUG("Candidate DeltaM = " << deltaM << " MeV DiMuon " << oniaVertex->index() << " ( Mass = " << diMuon.M() << " MeV )");

                      // Did we find a one di-muon + photon candidate with a mass diff. consistent with chi_c/b?
                      if(deltaM < m_maxDeltaM) {
                          passDeltaM = true;
                          candidateOniaVertices.push_back(oniaVertex);
                      }

                  }

                  // Only keep the conversion candidate if it's consistent with a chi_c,b decay
                  if(m_requireDeltaM && !passDeltaM) {
                      convVertexCandidate.reset();
                      continue;
                  }
                  //------------------------------------

                  //------------------------------------
                  // Final conversion candidates
                  //------------------------------------
                  nConv_Selected_DeltaM++;

                  // Keep track of which dimuon(s) gave a chi_c/b candidate
                  std::vector< ElementLink<xAOD::VertexContainer> > diMuonLinks;
                  diMuonLinks.clear();

                  // Output of cascade fits with various di-muon mass hypotheses
                  std::vector<float> fit_Psi1S_Px, fit_Psi1S_Py, fit_Psi1S_Pz, fit_Psi1S_M, fit_Psi1S_ChiSq;
                  std::vector<float> fit_Psi2S_Px, fit_Psi2S_Py, fit_Psi2S_Pz, fit_Psi2S_M, fit_Psi2S_ChiSq;
                  std::vector<float> fit_Upsi1S_Px, fit_Upsi1S_Py, fit_Upsi1S_Pz, fit_Upsi1S_M, fit_Upsi1S_ChiSq;
                  std::vector<float> fit_Upsi2S_Px, fit_Upsi2S_Py, fit_Upsi2S_Pz, fit_Upsi2S_M, fit_Upsi2S_ChiSq;
                  std::vector<float> fit_Upsi3S_Px, fit_Upsi3S_Py, fit_Upsi3S_Pz, fit_Upsi3S_M, fit_Upsi3S_ChiSq;

                  // Loop over di-muon vertices associated with a candidate
                  for(std::vector<const xAOD::Vertex*>::const_iterator vtxItr = candidateOniaVertices.begin(); vtxItr != candidateOniaVertices.end(); ++vtxItr ) {

                      //------------------------------------
                      // Add an element link to each dimuon which formed a
                      // candidate, leading to the decision to save this conversion
                      //------------------------------------
                      ElementLink<xAOD::VertexContainer> myLink;
                      myLink.setElement(*vtxItr);
                      myLink.setStorableObject(*diMuonContainer);

                       if(!myLink.isValid()) {
                           ATH_MSG_WARNING("Invalid DiMuon ElementLink!");
                       }

                      diMuonLinks.push_back(myLink);
                      //------------------------------------

                      // Check which mass window this di-muon passed
                      bool passed_Psi = (*vtxItr)->auxdata<Char_t>("passed_Psi");
                      bool passed_Upsi = (*vtxItr)->auxdata<Char_t>("passed_Upsi");

                      //------------------------------------
                      // Cascade fit with J/psi mass hypothesis
                      //------------------------------------
                      float fitChiSq_Psi1S = 99999;
                      TLorentzVector fitResult_Psi1S;

                      // Only bother with the fit if di-muon mass is within the relveant range,
                      // but still fill an dummy 4-vector to preserve one to one correspondance with "DiMuonLinks"
                      if(passed_Psi) {
                          ATH_CHECK( doCascadeFit(*vtxItr,constConvVertex,3096.916,fitResult_Psi1S,fitChiSq_Psi1S) );
                      }

                      fit_Psi1S_Px.push_back(fitResult_Psi1S.Px());
                      fit_Psi1S_Py.push_back(fitResult_Psi1S.Py());
                      fit_Psi1S_Pz.push_back(fitResult_Psi1S.Pz());
                      fit_Psi1S_M.push_back(fitResult_Psi1S.M());
                      fit_Psi1S_ChiSq.push_back(fitChiSq_Psi1S);

                      //------------------------------------
                      // Cascade fit with psi(2S) mass hypothesis
                      //------------------------------------
                      float fitChiSq_Psi2S = 99999;
                      TLorentzVector fitResult_Psi2S;

                      // Only bother with the fit if di-muon mass is within the relveant range,
                      // but still fill an dummy 4-vector to preserve one to one correspondance with "DiMuonLinks"
                      if(passed_Psi) {
                          ATH_CHECK( doCascadeFit(*vtxItr,constConvVertex,3686.097,fitResult_Psi2S,fitChiSq_Psi2S) );
                      }

                      fit_Psi2S_Px.push_back(fitResult_Psi2S.Px());
                      fit_Psi2S_Py.push_back(fitResult_Psi2S.Py());
                      fit_Psi2S_Pz.push_back(fitResult_Psi2S.Pz());
                      fit_Psi2S_M.push_back(fitResult_Psi2S.M());
                      fit_Psi2S_ChiSq.push_back(fitChiSq_Psi2S);

                      //------------------------------------
                      // Cascade fit with Upsi(1S) mass hypothesis
                      //------------------------------------
                      float fitChiSq_Upsi1S = 99999;
                      TLorentzVector fitResult_Upsi1S;

                      // Only bother with the fit if di-muon mass is within the relveant range,
                      // but still fill an dummy 4-vector to preserve one to one correspondance with "DiMuonLinks"
                      if(passed_Upsi) {
                          ATH_CHECK( doCascadeFit(*vtxItr,constConvVertex,9460.30,fitResult_Upsi1S,fitChiSq_Upsi1S) );
                      }

                      fit_Upsi1S_Px.push_back(fitResult_Upsi1S.Px());
                      fit_Upsi1S_Py.push_back(fitResult_Upsi1S.Py());
                      fit_Upsi1S_Pz.push_back(fitResult_Upsi1S.Pz());
                      fit_Upsi1S_M.push_back(fitResult_Upsi1S.M());
                      fit_Upsi1S_ChiSq.push_back(fitChiSq_Upsi1S);

                      //------------------------------------
                      // Cascade fit with Upsi(2S) mass hypothesis
                      //------------------------------------
                      float fitChiSq_Upsi2S = 99999;
                      TLorentzVector fitResult_Upsi2S;

                      // Only bother with the fit if di-muon mass is within the relveant range,
                      // but still fill an dummy 4-vector to preserve one to one correspondance with "DiMuonLinks"
                      if(passed_Upsi) {
                          ATH_CHECK( doCascadeFit(*vtxItr,constConvVertex,10023.26,fitResult_Upsi2S,fitChiSq_Upsi2S) );
                      }

                      fit_Upsi2S_Px.push_back(fitResult_Upsi2S.Px());
                      fit_Upsi2S_Py.push_back(fitResult_Upsi2S.Py());
                      fit_Upsi2S_Pz.push_back(fitResult_Upsi2S.Pz());
                      fit_Upsi2S_M.push_back(fitResult_Upsi2S.M());
                      fit_Upsi2S_ChiSq.push_back(fitChiSq_Upsi2S);

                      //------------------------------------
                      // Cascade fit with Upsi(3S) mass hypothesis
                      //------------------------------------
                      float fitChiSq_Upsi3S = 99999;
                      TLorentzVector fitResult_Upsi3S;

                      // Only bother with the fit if di-muon mass is within the relveant range,
                      // but still fill an dummy 4-vector to preserve one to one correspondance with "DiMuonLinks"
                      if(passed_Upsi) {
                          ATH_CHECK( doCascadeFit(*vtxItr,constConvVertex,10355.2,fitResult_Upsi3S,fitChiSq_Upsi3S) );
                      }

                      fit_Upsi3S_Px.push_back(fitResult_Upsi3S.Px());
                      fit_Upsi3S_Py.push_back(fitResult_Upsi3S.Py());
                      fit_Upsi3S_Pz.push_back(fitResult_Upsi3S.Pz());
                      fit_Upsi3S_M.push_back(fitResult_Upsi3S.M());
                      fit_Upsi3S_ChiSq.push_back(fitChiSq_Upsi3S);

                  }

                  //------------------------------------
                  // Decorate selected conversions
                  //------------------------------------
                  ATH_MSG_DEBUG("Decorating conversion vertices");

                  convVertexCandidate->auxdata< std::vector< ElementLink<xAOD::VertexContainer> > >("DiMuonLinks") = diMuonLinks;

                  convVertexCandidate->auxdata< std::vector<float> >("CascadeFit_Psi1S_Px") = fit_Psi1S_Px;
                  convVertexCandidate->auxdata< std::vector<float> >("CascadeFit_Psi1S_Py") = fit_Psi1S_Py;
                  convVertexCandidate->auxdata< std::vector<float> >("CascadeFit_Psi1S_Pz") = fit_Psi1S_Pz;
                  convVertexCandidate->auxdata< std::vector<float> >("CascadeFit_Psi1S_M") = fit_Psi1S_M;
                  convVertexCandidate->auxdata< std::vector<float> >("CascadeFit_Psi1S_ChiSq") = fit_Psi1S_ChiSq;

                  convVertexCandidate->auxdata< std::vector<float> >("CascadeFit_Psi2S_Px") = fit_Psi2S_Px;
                  convVertexCandidate->auxdata< std::vector<float> >("CascadeFit_Psi2S_Py") = fit_Psi2S_Py;
                  convVertexCandidate->auxdata< std::vector<float> >("CascadeFit_Psi2S_Pz") = fit_Psi2S_Pz;
                  convVertexCandidate->auxdata< std::vector<float> >("CascadeFit_Psi2S_M") = fit_Psi2S_M;
                  convVertexCandidate->auxdata< std::vector<float> >("CascadeFit_Psi2S_ChiSq") = fit_Psi2S_ChiSq;

                  convVertexCandidate->auxdata< std::vector<float> >("CascadeFit_Upsi1S_Px") = fit_Upsi1S_Px;
                  convVertexCandidate->auxdata< std::vector<float> >("CascadeFit_Upsi1S_Py") = fit_Upsi1S_Py;
                  convVertexCandidate->auxdata< std::vector<float> >("CascadeFit_Upsi1S_Pz") = fit_Upsi1S_Pz;
                  convVertexCandidate->auxdata< std::vector<float> >("CascadeFit_Upsi1S_M") = fit_Upsi1S_M;
                  convVertexCandidate->auxdata< std::vector<float> >("CascadeFit_Upsi1S_ChiSq") = fit_Upsi1S_ChiSq;

                  convVertexCandidate->auxdata< std::vector<float> >("CascadeFit_Upsi2S_Px") = fit_Upsi2S_Px;
                  convVertexCandidate->auxdata< std::vector<float> >("CascadeFit_Upsi2S_Py") = fit_Upsi2S_Py;
                  convVertexCandidate->auxdata< std::vector<float> >("CascadeFit_Upsi2S_Pz") = fit_Upsi2S_Pz;
                  convVertexCandidate->auxdata< std::vector<float> >("CascadeFit_Upsi2S_M") = fit_Upsi2S_M;
                  convVertexCandidate->auxdata< std::vector<float> >("CascadeFit_Upsi2S_ChiSq") = fit_Upsi2S_ChiSq;

                  convVertexCandidate->auxdata< std::vector<float> >("CascadeFit_Upsi3S_Px") = fit_Upsi3S_Px;
                  convVertexCandidate->auxdata< std::vector<float> >("CascadeFit_Upsi3S_Py") = fit_Upsi3S_Py;
                  convVertexCandidate->auxdata< std::vector<float> >("CascadeFit_Upsi3S_Pz") = fit_Upsi3S_Pz;
                  convVertexCandidate->auxdata< std::vector<float> >("CascadeFit_Upsi3S_M") = fit_Upsi3S_M;
                  convVertexCandidate->auxdata< std::vector<float> >("CascadeFit_Upsi3S_ChiSq") = fit_Upsi3S_ChiSq;

                  convVertexCandidate->auxdata<float>("px") = momentum.x();
                  convVertexCandidate->auxdata<float>("py") = momentum.y();
                  convVertexCandidate->auxdata<float>("pz") = momentum.z();

                  convVertexCandidate->auxdata<float>("deltaCotThetaTrk") = m_deltaCotTheta;
                  convVertexCandidate->auxdata<float>("minimumDistanceTrk") = m_distance;

                  convVertexCandidate->auxdata<float>("deltaPhiTracks") = m_vertexEstimator->getLastValues()["deltaPhiTracks"];
                  convVertexCandidate->auxdata<float>("DR1R2") = m_vertexEstimator->getLastValues()["DR1R2"];

                  convVertexCandidate->auxdata<Char_t>("passed") = true; // Used in event skimming

                  conversionContainer->push_back(convVertexCandidate.release());

              } else {
                  ATH_MSG_DEBUG("Vertex Fit Failed");
              }

          } // Neg Track Loop

      } // Pos Track Loop

    } // callConvFinder

    // Write the results to StoreGate
    CHECK(evtStore()->record(conversionContainer.release(), m_conversionContainerName));
    CHECK(evtStore()->record(conversionAuxContainer.release(), m_conversionContainerName+"Aux."));

    ATH_MSG_DEBUG("-------------------------");
    ATH_MSG_DEBUG("Number of track pairs: " << nTrackPairs_Init);
    ATH_MSG_DEBUG("Number of track pairs selected: " << nTrackPairs_Selected);
    ATH_MSG_DEBUG("Number of successful vertex fits: " << nConv_VertexFit);
    ATH_MSG_DEBUG("Number of selected vertices: " << nConv_Selected);
    ATH_MSG_DEBUG("Number of selected vertices (after DeltaM req.): " << nConv_Selected_DeltaM);

    return StatusCode::SUCCESS;
  }

  StatusCode BPhysConversionFinder::doCascadeFit(const xAOD::Vertex * diMuonVertex, const xAOD::Vertex * convVertex, const double diMuonMassConstraint, TLorentzVector & fitMom, float & chiSq) const {

      std::vector<const xAOD::TrackParticle*> diMuonTracks;
      diMuonTracks.push_back(diMuonVertex->trackParticle(0));
      diMuonTracks.push_back(diMuonVertex->trackParticle(1));

      std::vector<double> diMuonTrackMasses;
      diMuonTrackMasses.push_back(105.658);
      diMuonTrackMasses.push_back(105.658);

      std::vector<const xAOD::TrackParticle*> convTracks;
      convTracks.push_back(convVertex->trackParticle(0));
      convTracks.push_back(convVertex->trackParticle(1));

      std::vector<double> convTrackMasses;
      convTrackMasses.push_back(0.511);
      convTrackMasses.push_back(0.511);

      // Reset
      m_cascadeFitter->setDefault();

      // Set Robustness
      m_cascadeFitter->setRobustness(0);

      // Build up the topology
      
      // Vertex list
      std::vector<Trk::VertexID> vrtList;
      // V0 vertex
      Trk::VertexID vID;
      vID = m_cascadeFitter->startVertex(convTracks,convTrackMasses,0.0); // Constrain converision mass to zero

      vrtList.push_back(vID);

      // chi_c/b vertex
      Trk::VertexID vID2 = m_cascadeFitter->nextVertex(diMuonTracks,diMuonTrackMasses,vrtList);

      std::vector<Trk::VertexID> cnstV;
      cnstV.clear();
      if ( !m_cascadeFitter->addMassConstraint(vID2,diMuonTracks,cnstV,diMuonMassConstraint).isSuccess() ) {
        ATH_MSG_WARNING("addMassConstraint failed");
      }

      // Do the fit
      std::unique_ptr<Trk::VxCascadeInfo> result(m_cascadeFitter->fitCascade());

      const std::vector< std::vector<TLorentzVector> > &moms = result->getParticleMoms();

      // Check for a successful fit
      if(result != NULL) {

          if(moms.size() > 2) ATH_MSG_WARNING("DoCascadeFit - More than two output momentum!?");

          TLorentzVector conv_Fit = moms.at(0).at(0) + moms.at(0).at(1);
          TLorentzVector diMuon_Fit = moms.at(1).at(0) + moms.at(1).at(1);

          // Momentum of DiMuon + photon system
          fitMom = diMuon_Fit + conv_Fit;

          chiSq = result->fitChi2()/result->nDoF();

          // Done with the fit result
          result.reset();

      }

      return StatusCode::SUCCESS;

  }


}
