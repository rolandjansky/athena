/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


#include "TestCascadeAlg.h"


#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/VertexAuxContainer.h"
#include "TLorentzVector.h"


TestCascadeAlg::TestCascadeAlg( const std::string& name, ISvcLocator* svcLoc ) :
AthAlgorithm(name, svcLoc), m_iVertexFitter("Trk::TrkVKalVrtFitter"),
m_jpsiFinder("Analysis::JpsiFinder")
{
    declareProperty("JpsiFinder"    , m_jpsiFinder);
    declareProperty("ConstrainV0", m_constrV0);
    declareProperty("ConstrainJpsi", m_constrJpsi);
    declareProperty("TrkVertexFitterTool", m_iVertexFitter);
    declareProperty("PerEventLimit", m_perELimit);
}

StatusCode TestCascadeAlg::initialize() {
   ATH_CHECK(m_iVertexFitter.retrieve());
   ATH_CHECK(m_jpsiFinder.retrieve());
   return StatusCode::SUCCESS;
}

StatusCode TestCascadeAlg::execute() {
    std::unique_ptr<xAOD::VertexContainer>    jpsiContainer = std::make_unique<xAOD::VertexContainer>();
    std::unique_ptr<xAOD::VertexAuxContainer> jpsiauxContainer = std::make_unique<xAOD::VertexAuxContainer>();
    jpsiContainer->setStore(jpsiauxContainer.get());
    
    // call Jpsi+2track tool
    if( !m_jpsiFinder->performSearch(Gaudi::Hive::currentContext(), *jpsiContainer).isSuccess() ) {
      ATH_MSG_FATAL("Jpsi+2 track tool (" << m_jpsiFinder << ") failed.");
      return StatusCode::FAILURE;
    }
    if(jpsiContainer->size() ==0) { return StatusCode::SUCCESS; }
    std::vector<const xAOD::TrackParticle*> tracksJpsi;
    std::vector<std::vector<const xAOD::TrackParticle*>> tracksV0;
    constexpr double mass_muon = 105.658;
    constexpr double mass_pion = 139.570;
    constexpr double mass_ks   = 497.61;
    std::vector<double> massesJpsi(2, mass_muon);
    std::vector<double> massesV0;
    std::vector<double> Masses(2, mass_muon);

    massesV0.push_back(mass_pion);
    massesV0.push_back(mass_pion);
    Masses.push_back(mass_ks);

    const xAOD::TrackParticleContainer  *trackContainer(nullptr);
    CHECK(evtStore()->retrieve(trackContainer   , "InDetTrackParticles"      ));
    for(auto track1itr = trackContainer->begin(); track1itr != trackContainer->end(); ++track1itr){
       for(auto track2itr = trackContainer->begin() + 1; track2itr != trackContainer->end(); ++track2itr){
            if((*track2itr)->qOverP() * (*track1itr)->qOverP() >= 0.) continue;
            std::vector<const xAOD::TrackParticle*> l = {*track1itr, *track2itr};
            tracksV0.push_back(std::move(l));
       }
    }

    double mass_v0 = mass_ks;
    constexpr double mass_jpsi = 3.0969 * 1000;
    int pereventcount = 0;
    for(auto jpsi : *jpsiContainer) { //Iterate over Jpsi vertices

       size_t jpsiTrkNum = jpsi->nTrackParticles();
       tracksJpsi.clear();
       for( unsigned int it=0; it<jpsiTrkNum; it++) tracksJpsi.push_back(jpsi->trackParticle(it));

       for(auto &v0 : tracksV0) {
 
          std::unique_ptr<Trk::IVKalState> state (m_iVertexFitter->makeState());
          // Robustness
          int robustness = 0;
          m_iVertexFitter->setRobustness(robustness, *state);
          // Build up the topology
          // Vertex list
          std::vector<Trk::VertexID> vrtList;
          // V0 vertex
          Trk::VertexID vID;
          if (m_constrV0) {
            vID = m_iVertexFitter->startVertex(v0,massesV0,*state,mass_v0);
          } else {
            vID = m_iVertexFitter->startVertex(v0,massesV0,*state);
          }
          vrtList.push_back(vID);
          // B vertex including Jpsi
          Trk::VertexID vID2 = m_iVertexFitter->nextVertex(tracksJpsi,massesJpsi,vrtList,*state);
          if (m_constrJpsi) {
            std::vector<Trk::VertexID> cnstV;
            cnstV.clear();
            if ( !m_iVertexFitter->addMassConstraint(vID2,tracksJpsi,cnstV,*state,mass_jpsi).isSuccess() ) {
              ATH_MSG_WARNING("addMassConstraint failed");
            }
          }
          std::unique_ptr<Trk::VxCascadeInfo> result(m_iVertexFitter->fitCascade(*state));
          if(result) {
          	result->setSVOwnership(true);
          	const std::vector< std::vector<TLorentzVector> > &moms = result->getParticleMoms();
          	for(auto& p :moms) { ATH_MSG_DEBUG("new part"); for(auto &l : p) ATH_MSG_DEBUG("vect " << l.M()); }
           pereventcount++;
           if(pereventcount>=m_perELimit) return StatusCode::SUCCESS;
          }
       }
    }

    return StatusCode::SUCCESS;
}
