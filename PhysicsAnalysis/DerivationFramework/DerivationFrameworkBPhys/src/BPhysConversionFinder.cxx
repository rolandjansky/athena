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
    m_V0Tools("Trk::V0Tools"),
    m_VertexFitter("Trk::TrkVKalVrtFitter"),
    m_VertexEstimator("InDet::VertexPointEstimator"),
    m_DistanceTool("Trk::SeedNewtonDistanceFinder/InDetConversionTrkDistanceFinder"),
    m_PostSelector("InDet::ConversionPostSelector"),
    m_CascadeFitter("Trk::TrkVKalVrtFitter"),
    m_InputTrackParticleContainerName("InDetTrackParticles"),
    m_ConversionContainerName("BPhysConversionCandidates"),
    m_MaxDistBetweenTracks(10.0),
    m_MaxDeltaCotTheta(0.3),
    m_RequireDeltaM(true),
    m_MaxDeltaM(3000.0)
  {
    declareInterface<DerivationFramework::IAugmentationTool>(this);

    // Declare user-defined properties
    declareProperty("DiMuonVertexContainer", m_DiMuonCollectionToCheck);
    declareProperty("PassFlagsToCheck", m_PassFlagsToCheck);
    declareProperty("V0Tools", m_V0Tools);
    declareProperty("VertexFitterTool", m_VertexFitter);
    declareProperty("VertexEstimator", m_VertexEstimator);
    declareProperty("DistanceTool", m_DistanceTool);
    declareProperty("ConversionPostSelector", m_PostSelector);
    declareProperty("CascadeFitter", m_CascadeFitter);
    declareProperty("InputTrackParticleContainerName", m_InputTrackParticleContainerName);
    declareProperty("ConversionContainerName", m_ConversionContainerName);
    declareProperty("MaxDistBetweenTracks", m_MaxDistBetweenTracks = 10.0); // Maximum allowed distance of minimum approach
    declareProperty("MaxDeltaCotTheta", m_MaxDeltaCotTheta = 0.3); // Maximum allowed dCotTheta between tracks
    declareProperty("RequireDeltaM", m_RequireDeltaM = true); // Only save a conversions if it's a chi_c,b candidate (must then pass "MaxDeltaM" requirement), if "False" all conversions in the event will be saved
    declareProperty("MaxDeltaM", m_MaxDeltaM = 3000.0); // Maximum mass difference between di-muon+conversion and di-muon

  }

  // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

  StatusCode BPhysConversionFinder::initialize()
  {

    ATH_MSG_DEBUG("in initialize()");

    ATH_CHECK( m_V0Tools.retrieve() );
    ATH_CHECK( m_VertexFitter.retrieve() );
    ATH_CHECK( m_VertexEstimator.retrieve() );
    ATH_CHECK( m_DistanceTool.retrieve() );
    ATH_CHECK( m_PostSelector.retrieve() );
    ATH_CHECK( m_CascadeFitter.retrieve() );

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

    std::vector<const xAOD::Vertex*> OniaVertices;
    OniaVertices.clear();

    //------------------------------------
    // Look for di-muons
    //------------------------------------
    const xAOD::VertexContainer* DiMuonContainer = NULL;
    ATH_CHECK( evtStore()->retrieve(DiMuonContainer, m_DiMuonCollectionToCheck) );

    if(DiMuonContainer->size() == 0) {

        ATH_MSG_DEBUG("Vertex Container (" << m_DiMuonCollectionToCheck << ") is empty");

    } else {

        ATH_MSG_DEBUG("Vertex Container (" << m_DiMuonCollectionToCheck << ") contains " << DiMuonContainer->size() << " vertices");

        for(xAOD::VertexContainer::const_iterator vtxItr = DiMuonContainer->begin(); vtxItr != DiMuonContainer->end(); ++vtxItr) {

            const xAOD::Vertex* vertex = (*vtxItr);

            bool PassedHypothesis = false;

            for(const auto &flag : m_PassFlagsToCheck) {
                bool pass = vertex->auxdata<Char_t>(flag);
                if(pass) PassedHypothesis = true;
            }

            if(PassedHypothesis) {
                OniaVertices.push_back(vertex);
            }

        }
     }
    //------------------------------------

    // Conversion container and its auxilliary store
    xAOD::VertexContainer*    ConversionContainer(nullptr);
    xAOD::VertexAuxContainer* ConversionAuxContainer(nullptr);

    // Only call conversion finder if we've found a di-muon candidate or
    // we really want to look for conversions independently
    const bool callConvFinder = !m_RequireDeltaM || OniaVertices.size() > 0;

    if(callConvFinder) {

      ConversionContainer = new xAOD::VertexContainer;
      ConversionAuxContainer = new xAOD::VertexAuxContainer;
      ConversionContainer->setStore(ConversionAuxContainer);

      // Retrieve track particles from StoreGate
      const xAOD::TrackParticleContainer* InputTrackParticles = NULL;
      ATH_CHECK( evtStore()->retrieve(InputTrackParticles,m_InputTrackParticleContainerName) );

      ATH_MSG_DEBUG("Track particle container size " <<  InputTrackParticles->size());

      // Track Selection
      std::vector<const xAOD::TrackParticle*> posTracks; posTracks.clear();
      std::vector<const xAOD::TrackParticle*> negTracks; negTracks.clear();

      // Track Loop
      for(xAOD::TrackParticleContainer::const_iterator trkItr = InputTrackParticles->begin(); trkItr != InputTrackParticles->end(); ++trkItr) {

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
              if(m_deltaCotTheta > m_MaxDeltaCotTheta) continue;

              bool gotDistance = false;
              double m_distance = 1000000.;
              gotDistance = m_DistanceTool->CalculateMinimumDistance(trackParticle1->perigeeParameters(),trackParticle2->perigeeParameters() );
              if(gotDistance) m_distance = m_DistanceTool->GetDistance();
              if(!gotDistance || (m_distance > m_MaxDistBetweenTracks)) continue;
              //------------------------------------

              //------------------------------------
              // Estimate starting point + cuts on compatiblity of tracks
              //------------------------------------
              int sflag = 0;
              int errorcode = 0;
              Amg::Vector3D startingPoint = m_VertexEstimator->getCirclesIntersectionPoint(&trackPerigee1,&trackPerigee2,sflag,errorcode);
              if(errorcode != 0) continue;
              //------------------------------------

              nTrackPairs_Selected++;

              std::vector<const xAOD::TrackParticle*> TrackPair;
              TrackPair.clear();
              TrackPair.push_back(trackParticle1);
              TrackPair.push_back(trackParticle2);

              // Do the vertex fit
              std::unique_ptr<xAOD::Vertex> VxCandidate = std::move(std::unique_ptr<xAOD::Vertex>(  m_VertexFitter->fit(TrackPair, startingPoint) ));

              if(VxCandidate) {

                  ATH_MSG_DEBUG("Vertex Fit Succeeded");

                  VxCandidate->clearTracks();
                  ElementLink<xAOD::TrackParticleContainer> newLink1;
                  newLink1.setElement(*tpIt1);
                  newLink1.setStorableObject(*InputTrackParticles);
                  ElementLink<xAOD::TrackParticleContainer> newLink2;
                  newLink2.setElement(*tpIt2);
                  newLink2.setStorableObject(*InputTrackParticles);
                  VxCandidate->addTrackAtVertex(newLink1);
                  VxCandidate->addTrackAtVertex(newLink2);

                  nConv_VertexFit++;

                  //------------------------------------
                  // Post-vertexing cuts
                  //------------------------------------

                  // This is empty and only present for compatiblity.
                  // The cut this informtion pertains to is not used for Si-Si conversions so this is OK
                  std::vector<Amg::Vector3D> positionList;

                  // Apply Si-Si converion post-selection
                  if( !m_PostSelector->selectConversionCandidate(VxCandidate.get(),0,positionList) ) {
                      VxCandidate.reset();
                      continue;
                  }
                  //------------------------------------

                  nConv_Selected++;

                  // Get photon momentum 3-vector
                  const xAOD::Vertex * ConstConvVertex = VxCandidate.get();
                  Amg::Vector3D momentum = m_V0Tools->V0Momentum(ConstConvVertex);

                  TLorentzVector Photon;
                  Photon.SetXYZM(momentum.x(),momentum.y(),momentum.z(),0.0);

                  //------------------------------------
                  // Check if conversion is consistent with a chi_c,b candidate
                  // by requiring a small mass difference w.r.t. any di-muon in event
                  //------------------------------------
                  bool PassDeltaM = false;

                  // Use to keep track of which dimuon(s) gave a chi_c/b candidate
                  std::vector<const xAOD::Vertex*> CandidateOniaVertices;
                  CandidateOniaVertices.clear();

                  for ( std::vector<const xAOD::Vertex*>::const_iterator vtxItr = OniaVertices.begin(); vtxItr != OniaVertices.end(); ++vtxItr ) {

                      const xAOD::Vertex* OniaVertex = (*vtxItr);

                      std::vector<float> DiMuon_Px = OniaVertex->auxdata< std::vector<float> >("RefTrackPx");
                      std::vector<float> DiMuon_Py = OniaVertex->auxdata< std::vector<float> >("RefTrackPy");
                      std::vector<float> DiMuon_Pz = OniaVertex->auxdata< std::vector<float> >("RefTrackPz");

                      TLorentzVector Muon1, Muon2;
                      Muon1.SetXYZM(DiMuon_Px.at(0),DiMuon_Py.at(0),DiMuon_Pz.at(0),105.658);
                      Muon2.SetXYZM(DiMuon_Px.at(1),DiMuon_Py.at(1),DiMuon_Pz.at(1),105.658);

                      TLorentzVector DiMuon = Muon1 + Muon2;

                      const double DeltaM = (DiMuon+Photon).M() - DiMuon.M();

                      ATH_MSG_DEBUG("Candidate DeltaM = " << DeltaM << " MeV DiMuon " << OniaVertex->index() << " ( Mass = " << DiMuon.M() << " MeV )");

                      // Did we find a one di-muon + photon candidate with a mass diff. consistent with chi_c/b?
                      if(DeltaM < m_MaxDeltaM) {
                          PassDeltaM = true;
                          CandidateOniaVertices.push_back(OniaVertex);
                      }

                  }

                  // Only keep the conversion candidate if it's consistent with a chi_c,b decay
                  if(m_RequireDeltaM && !PassDeltaM) {
                      VxCandidate.reset();
                      continue;
                  }
                  //------------------------------------

                  //------------------------------------
                  // Final conversion candidates
                  //------------------------------------
                  nConv_Selected_DeltaM++;

                  //------------------------------------
                  // Decorate selected conversions
                  //------------------------------------
                  ATH_MSG_DEBUG("Decorating conversion vertices");

                  VxCandidate->auxdata<float>("px") = momentum.x();
                  VxCandidate->auxdata<float>("py") = momentum.y();
                  VxCandidate->auxdata<float>("pz") = momentum.z();

                  VxCandidate->auxdata<float>("deltaCotThetaTrk") = m_deltaCotTheta;
                  VxCandidate->auxdata<float>("minimumDistanceTrk") = m_distance;

                  VxCandidate->auxdata<float>("deltaPhiTracks") = m_VertexEstimator->getLastValues()["deltaPhiTracks"];
                  VxCandidate->auxdata<float>("DR1R2") = m_VertexEstimator->getLastValues()["DR1R2"];

                  VxCandidate->auxdata<Char_t>("passed") = true; // Used in event skimming

                  // Keep track of which dimuon(s) gave a chi_c/b candidate
                  std::vector< ElementLink<xAOD::VertexContainer> > DiMuonLinks;

                  // Output of cascade fits with various di-muon mass hypotheses
                  std::vector<float> Fit_Psi1S_Px, Fit_Psi1S_Py, Fit_Psi1S_Pz, Fit_Psi1S_M, Fit_Psi1S_ChiSq;
                  std::vector<float> Fit_Psi2S_Px, Fit_Psi2S_Py, Fit_Psi2S_Pz, Fit_Psi2S_M, Fit_Psi2S_ChiSq;
                  std::vector<float> Fit_Upsi1S_Px, Fit_Upsi1S_Py, Fit_Upsi1S_Pz, Fit_Upsi1S_M, Fit_Upsi1S_ChiSq;
                  std::vector<float> Fit_Upsi2S_Px, Fit_Upsi2S_Py, Fit_Upsi2S_Pz, Fit_Upsi2S_M, Fit_Upsi2S_ChiSq;
                  std::vector<float> Fit_Upsi3S_Px, Fit_Upsi3S_Py, Fit_Upsi3S_Pz, Fit_Upsi3S_M, Fit_Upsi3S_ChiSq;

                  for(std::vector<const xAOD::Vertex*>::const_iterator vtxItr = CandidateOniaVertices.begin(); vtxItr != CandidateOniaVertices.end(); ++vtxItr ) {

                      //------------------------------------
                      // Add an element link to each dimuon which formed a
                      // candidate, leading to the decision to save this conversion
                      //------------------------------------
                      ElementLink<xAOD::VertexContainer> myLink;
                      myLink.setElement(*vtxItr);
                      myLink.setStorableObject(*DiMuonContainer);

                       if(!myLink.isValid()) {
                           ATH_MSG_WARNING("Invalid DiMuon ElementLink!");
                       }

                      DiMuonLinks.push_back(myLink);
                      //------------------------------------

                      // Check which mass window this di-muon passed
                      bool passed_Psi = (*vtxItr)->auxdata<Char_t>("passed_Psi");
                      bool passed_Upsi = (*vtxItr)->auxdata<Char_t>("passed_Upsi");


                      //------------------------------------
                      // Cascade fit with J/psi mass hypothesis
                      //------------------------------------
                      float FitChiSq_Psi1S = 99999;
                      TLorentzVector FitResult_Psi1S;

                      // Only bother with the fit if di-muon mass is within the relveant range,
                      // but still fill an dummy 4-vector to preserve one to one correspondance with "DiMuonLinks"
                      if(passed_Psi) {
                          ATH_CHECK( DoCascadeFit(*vtxItr,ConstConvVertex,3096.916,FitResult_Psi1S,FitChiSq_Psi1S) );
                      }

                      Fit_Psi1S_Px.push_back(FitResult_Psi1S.Px());
                      Fit_Psi1S_Py.push_back(FitResult_Psi1S.Py());
                      Fit_Psi1S_Pz.push_back(FitResult_Psi1S.Pz());
                      Fit_Psi1S_M.push_back(FitResult_Psi1S.M());
                      Fit_Psi1S_ChiSq.push_back(FitChiSq_Psi1S);

                      //------------------------------------
                      // Cascade fit with psi(2S) mass hypothesis
                      //------------------------------------
                      float FitChiSq_Psi2S = 99999;
                      TLorentzVector FitResult_Psi2S;

                      // Only bother with the fit if di-muon mass is within the relveant range,
                      // but still fill an dummy 4-vector to preserve one to one correspondance with "DiMuonLinks"
                      if(passed_Psi) {
                          ATH_CHECK( DoCascadeFit(*vtxItr,ConstConvVertex,3686.097,FitResult_Psi2S,FitChiSq_Psi2S) );
                      }

                      Fit_Psi2S_Px.push_back(FitResult_Psi2S.Px());
                      Fit_Psi2S_Py.push_back(FitResult_Psi2S.Py());
                      Fit_Psi2S_Pz.push_back(FitResult_Psi2S.Pz());
                      Fit_Psi2S_M.push_back(FitResult_Psi2S.M());
                      Fit_Psi2S_ChiSq.push_back(FitChiSq_Psi2S);




                      //------------------------------------
                      // Cascade fit with Upsi(1S) mass hypothesis
                      //------------------------------------
                      float FitChiSq_Upsi1S = 99999;
                      TLorentzVector FitResult_Upsi1S;

                      // Only bother with the fit if di-muon mass is within the relveant range,
                      // but still fill an dummy 4-vector to preserve one to one correspondance with "DiMuonLinks"
                      if(passed_Upsi) {
                          ATH_CHECK( DoCascadeFit(*vtxItr,ConstConvVertex,9460.30,FitResult_Upsi1S,FitChiSq_Upsi1S) );
                      }

                      Fit_Upsi1S_Px.push_back(FitResult_Upsi1S.Px());
                      Fit_Upsi1S_Py.push_back(FitResult_Upsi1S.Py());
                      Fit_Upsi1S_Pz.push_back(FitResult_Upsi1S.Pz());
                      Fit_Upsi1S_M.push_back(FitResult_Upsi1S.M());
                      Fit_Upsi1S_ChiSq.push_back(FitChiSq_Upsi1S);

                      //------------------------------------
                      // Cascade fit with Upsi(2S) mass hypothesis
                      //------------------------------------
                      float FitChiSq_Upsi2S = 99999;
                      TLorentzVector FitResult_Upsi2S;

                      // Only bother with the fit if di-muon mass is within the relveant range,
                      // but still fill an dummy 4-vector to preserve one to one correspondance with "DiMuonLinks"
                      if(passed_Upsi) {
                          ATH_CHECK( DoCascadeFit(*vtxItr,ConstConvVertex,10023.26,FitResult_Upsi2S,FitChiSq_Upsi2S) );
                      }

                      Fit_Upsi2S_Px.push_back(FitResult_Upsi2S.Px());
                      Fit_Upsi2S_Py.push_back(FitResult_Upsi2S.Py());
                      Fit_Upsi2S_Pz.push_back(FitResult_Upsi2S.Pz());
                      Fit_Upsi2S_M.push_back(FitResult_Upsi2S.M());
                      Fit_Upsi2S_ChiSq.push_back(FitChiSq_Upsi2S);

                      //------------------------------------
                      // Cascade fit with Upsi(3S) mass hypothesis
                      //------------------------------------
                      float FitChiSq_Upsi3S = 99999;
                      TLorentzVector FitResult_Upsi3S;

                      // Only bother with the fit if di-muon mass is within the relveant range,
                      // but still fill an dummy 4-vector to preserve one to one correspondance with "DiMuonLinks"
                      if(passed_Upsi) {
                          ATH_CHECK( DoCascadeFit(*vtxItr,ConstConvVertex,10355.2,FitResult_Upsi3S,FitChiSq_Upsi3S) );
                      }
                      
                      Fit_Upsi3S_Px.push_back(FitResult_Upsi3S.Px());
                      Fit_Upsi3S_Py.push_back(FitResult_Upsi3S.Py());
                      Fit_Upsi3S_Pz.push_back(FitResult_Upsi3S.Pz());
                      Fit_Upsi3S_M.push_back(FitResult_Upsi3S.M());
                      Fit_Upsi3S_ChiSq.push_back(FitChiSq_Upsi3S);



                  }

                  VxCandidate->auxdata< std::vector< ElementLink<xAOD::VertexContainer> > >("DiMuonLinks") = DiMuonLinks;

                  VxCandidate->auxdata< std::vector<float> >("CascadeFit_Psi1S_Px") = Fit_Psi1S_Px;
                  VxCandidate->auxdata< std::vector<float> >("CascadeFit_Psi1S_Py") = Fit_Psi1S_Py;
                  VxCandidate->auxdata< std::vector<float> >("CascadeFit_Psi1S_Pz") = Fit_Psi1S_Pz;
                  VxCandidate->auxdata< std::vector<float> >("CascadeFit_Psi1S_M") = Fit_Psi1S_M;
                  VxCandidate->auxdata< std::vector<float> >("CascadeFit_Psi1S_ChiSq") = Fit_Psi1S_ChiSq;

                  VxCandidate->auxdata< std::vector<float> >("CascadeFit_Psi2S_Px") = Fit_Psi2S_Px;
                  VxCandidate->auxdata< std::vector<float> >("CascadeFit_Psi2S_Py") = Fit_Psi2S_Py;
                  VxCandidate->auxdata< std::vector<float> >("CascadeFit_Psi2S_Pz") = Fit_Psi2S_Pz;
                  VxCandidate->auxdata< std::vector<float> >("CascadeFit_Psi2S_M") = Fit_Psi2S_M;
                  VxCandidate->auxdata< std::vector<float> >("CascadeFit_Psi2S_ChiSq") = Fit_Psi2S_ChiSq;

                  VxCandidate->auxdata< std::vector<float> >("CascadeFit_Upsi1S_Px") = Fit_Upsi1S_Px;
                  VxCandidate->auxdata< std::vector<float> >("CascadeFit_Upsi1S_Py") = Fit_Upsi1S_Py;
                  VxCandidate->auxdata< std::vector<float> >("CascadeFit_Upsi1S_Pz") = Fit_Upsi1S_Pz;
                  VxCandidate->auxdata< std::vector<float> >("CascadeFit_Upsi1S_M") = Fit_Upsi1S_M;
                  VxCandidate->auxdata< std::vector<float> >("CascadeFit_Upsi1S_ChiSq") = Fit_Upsi1S_ChiSq;

                  VxCandidate->auxdata< std::vector<float> >("CascadeFit_Upsi2S_Px") = Fit_Upsi2S_Px;
                  VxCandidate->auxdata< std::vector<float> >("CascadeFit_Upsi2S_Py") = Fit_Upsi2S_Py;
                  VxCandidate->auxdata< std::vector<float> >("CascadeFit_Upsi2S_Pz") = Fit_Upsi2S_Pz;
                  VxCandidate->auxdata< std::vector<float> >("CascadeFit_Upsi2S_M") = Fit_Upsi2S_M;
                  VxCandidate->auxdata< std::vector<float> >("CascadeFit_Upsi2S_ChiSq") = Fit_Upsi2S_ChiSq;

                  VxCandidate->auxdata< std::vector<float> >("CascadeFit_Upsi3S_Px") = Fit_Upsi3S_Px;
                  VxCandidate->auxdata< std::vector<float> >("CascadeFit_Upsi3S_Py") = Fit_Upsi3S_Py;
                  VxCandidate->auxdata< std::vector<float> >("CascadeFit_Upsi3S_Pz") = Fit_Upsi3S_Pz;
                  VxCandidate->auxdata< std::vector<float> >("CascadeFit_Upsi3S_M") = Fit_Upsi3S_M;
                  VxCandidate->auxdata< std::vector<float> >("CascadeFit_Upsi3S_ChiSq") = Fit_Upsi3S_ChiSq;
                  //------------------------------------

                  ConversionContainer->push_back(VxCandidate.release());

              } else {
                  ATH_MSG_DEBUG("Vertex Fit Failed");
              }

          } // Neg Track Loop

      } // Pos Track Loop

    } else {

      //Fill with empty containers
      ConversionContainer = new xAOD::VertexContainer;
      ConversionAuxContainer = new xAOD::VertexAuxContainer;
      ConversionContainer->setStore(ConversionAuxContainer);

    }

    //---- Recording section: write the results to StoreGate ---//
    CHECK(evtStore()->record(ConversionContainer, m_ConversionContainerName));
    CHECK(evtStore()->record(ConversionAuxContainer, m_ConversionContainerName+"Aux."));

    ATH_MSG_DEBUG("-------------------------");
    ATH_MSG_DEBUG("Number of track pairs: " << nTrackPairs_Init);
    ATH_MSG_DEBUG("Number of track pairs selected: " << nTrackPairs_Selected);
    ATH_MSG_DEBUG("Number of successful vertex fits: " << nConv_VertexFit);
    ATH_MSG_DEBUG("Number of selected vertices: " << nConv_Selected);
    ATH_MSG_DEBUG("Number of selected vertices (after DeltaM req.): " << nConv_Selected_DeltaM);

    return StatusCode::SUCCESS;
  }

  StatusCode BPhysConversionFinder::DoCascadeFit(const xAOD::Vertex* DiMuonVertex, const xAOD::Vertex* ConvVertex, const double DiMuonMassConstraint, TLorentzVector & FitMom, float & ChiSq) const {

      std::vector<const xAOD::TrackParticle*> DiMuonTracks;
      DiMuonTracks.push_back(DiMuonVertex->trackParticle(0));
      DiMuonTracks.push_back(DiMuonVertex->trackParticle(1));

      std::vector<double> DiMuonTrackMasses;
      DiMuonTrackMasses.push_back(105.658);
      DiMuonTrackMasses.push_back(105.658);

      std::vector<const xAOD::TrackParticle*> ConvTracks;
      ConvTracks.push_back(ConvVertex->trackParticle(0));
      ConvTracks.push_back(ConvVertex->trackParticle(1));

      std::vector<double> ConvTrackMasses;
      ConvTrackMasses.push_back(0.511);
      ConvTrackMasses.push_back(0.511);

      // Reset
      m_CascadeFitter->setDefault();

     // Set Robustness
      int robustness = 0;
      m_CascadeFitter->setRobustness(robustness);

      // Build up the topology
      // Vertex list
      std::vector<Trk::VertexID> vrtList;
      // V0 vertex
      Trk::VertexID vID;
      vID = m_CascadeFitter->startVertex(ConvTracks,ConvTrackMasses,0.0); // Constrain converision mass to zero

      vrtList.push_back(vID);

      // chi_c/b vertex
      Trk::VertexID vID2 = m_CascadeFitter->nextVertex(DiMuonTracks,DiMuonTrackMasses,vrtList);

      std::vector<Trk::VertexID> cnstV;
      cnstV.clear();
      if ( !m_CascadeFitter->addMassConstraint(vID2,DiMuonTracks,cnstV,DiMuonMassConstraint).isSuccess() ) {
        ATH_MSG_WARNING("addMassConstraint failed");
      }

      // Do the fit
      std::unique_ptr<Trk::VxCascadeInfo> result(m_CascadeFitter->fitCascade());

      const std::vector< std::vector<TLorentzVector> > &moms = result->getParticleMoms();

      // Check for a successful fit
      if(result != NULL) {

          if(moms.size() > 2) ATH_MSG_WARNING("DoCascadeFit - More than two output momentum!?");

          TLorentzVector Conv_Fit = moms.at(0).at(0) + moms.at(0).at(1);
          TLorentzVector DiMuon_Fit = moms.at(1).at(0) + moms.at(1).at(1);

          // Momentum of DiMuon + photon system
          FitMom = DiMuon_Fit + Conv_Fit;

          ChiSq = result->fitChi2()/result->nDoF();

          // Done with the fit result
          result.reset();

      }

      return StatusCode::SUCCESS;

  }


}
