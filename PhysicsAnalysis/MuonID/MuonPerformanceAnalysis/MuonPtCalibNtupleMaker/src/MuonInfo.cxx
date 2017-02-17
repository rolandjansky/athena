/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define __MuonInfo_cxx__
#include "MuonPtCalibNtupleMaker/MuonInfo.h"

MuonInfo::MuonInfo() {

  Reset();

}

void MuonInfo::Reset() {

  m_d0Significance = 0.;
  m_z0 = 0.;
  m_PtVarCone20 = 0.;
  m_PtVarCone30 = 0.;
  m_PtVarCone40 = 0.;
  m_PtCone20 = 0.;
  m_PtCone30 = 0.;
  m_PtCone40 = 0.;
  m_TopoEtCone20 = 0.; 
  m_EnergyLossType = 0;
  m_EnergyLoss = 0.;
  m_EnergyLossSigma = 0.;
  m_Quality = 0;
  m_Author = 0;
  m_PrimarySector = 0;
  m_SecondarySector = 0;
  m_Type = -1 ;
  m_passHighPt = false;
  m_passIDHits = false;

  m_TruthPt=0; 
  m_TruthEta=0; 
  m_TruthPhi=0; 
}

void MuonInfo::Register( TTree* t, std::string p ) {
  
  t->Branch( ( p + "_d0Significance" ).c_str(), &m_d0Significance );
  t->Branch( ( p + "_z0" ).c_str(), &m_z0 );
  t->Branch( ( p + "_PtVarCone30" ).c_str(), &m_PtVarCone30 );
  t->Branch( ( p + "_PtVarCone20" ).c_str(), &m_PtVarCone20 );
  t->Branch( ( p + "_PtVarCone40" ).c_str(), &m_PtVarCone40 );
  t->Branch( ( p + "_PtCone30" ).c_str(), &m_PtCone30 );
  t->Branch( ( p + "_PtCone20" ).c_str(), &m_PtCone20 );
  t->Branch( ( p + "_PtCone40" ).c_str(), &m_PtCone40 );
  t->Branch( ( p + "_TopoEtCone20" ).c_str(), &m_TopoEtCone20 );
  t->Branch( ( p + "_EnergyLossType" ).c_str(), &m_EnergyLossType );
  t->Branch( ( p + "_EnergyLoss" ).c_str(), &m_EnergyLoss );
  t->Branch( ( p + "_EnergyLossSigma" ).c_str(), &m_EnergyLossSigma );
  t->Branch( ( p + "_Quality" ).c_str(), &m_Quality );
  t->Branch( ( p + "_PassHighPt" ).c_str(), &m_passHighPt );
  t->Branch( ( p + "_PassIDHits" ).c_str(), &m_passIDHits );
  t->Branch( ( p + "_Author" ).c_str(), &m_Author );
  t->Branch( ( p + "_PrimarySector" ).c_str(), &m_PrimarySector );
  t->Branch( ( p + "_SecondarySector" ).c_str(), &m_SecondarySector );
  t->Branch( ( p + "_TruthPt" ).c_str(), &m_TruthPt );
  t->Branch( ( p + "_TruthEta" ).c_str(), &m_TruthEta );
  t->Branch( ( p + "_TruthPhi" ).c_str(), &m_TruthPhi );
  //t->Branch( ( p + "_Eta" ).c_str(), &m_TruthEta );
  //t->Branch( ( p + "_Phi" ).c_str(), &m_TruthPhi );
  t->Branch( ( p + "_Type" ).c_str(), &m_Type );
}

void MuonInfo::Fill( const xAOD::Muon* muon, const xAOD::Vertex* pv, const xAOD::EventInfo* ei ) {

  const xAOD::TrackParticle* trk = muon->trackParticle( xAOD::Muon::TrackParticleType::CombinedTrackParticle );
  m_d0Significance = xAOD::TrackingHelpers::d0significance( trk, ei->beamPosSigmaX(), ei->beamPosSigmaY(), ei->beamPosSigmaXY() );
  m_z0 = TMath::Abs( trk->z0() + trk->vz() - pv->z() ) * TMath::Sin( trk->theta() );  
  m_PtVarCone30 = muon->isolation( xAOD::Iso::ptvarcone30 ) * CLHEP::GeV;
  m_PtVarCone20 = muon->isolation( xAOD::Iso::ptvarcone20 ) * CLHEP::GeV;
  m_PtVarCone40 = muon->isolation( xAOD::Iso::ptvarcone40 ) * CLHEP::GeV;

  m_PtCone30 = muon->isolation( xAOD::Iso::ptcone30 ) * CLHEP::GeV;
  m_PtCone20 = muon->isolation( xAOD::Iso::ptcone20 ) * CLHEP::GeV;
  m_PtCone40 = muon->isolation( xAOD::Iso::ptcone40 ) * CLHEP::GeV;

  m_Type=muon->muonType();

  m_TopoEtCone20 = muon->isolation( xAOD::Iso::topoetcone20 ) * CLHEP::GeV; 
  m_EnergyLossType = muon->auxdata< uint8_t >( "energyLossType" );
  m_EnergyLoss = muon->floatParameter( xAOD::Muon::EnergyLoss ) * CLHEP::GeV;
  m_EnergyLossSigma = muon->floatParameter( xAOD::Muon::EnergyLossSigma ) * CLHEP::GeV;
  m_Quality = muon->auxdata< int >( "MST_Quality" );

  m_passHighPt = muon->auxdata< bool >( "MST_PassHighPt" );
  m_passIDHits = muon->auxdata< bool >( "MST_PassIDHits" );
  m_Author = muon->auxdata< uint16_t >( "author" );
  m_PrimarySector = muon->auxdata< uint8_t >( "primarySector" );
  m_SecondarySector = muon->auxdata< uint8_t >( "secondarySector" );
  
    if( muon->isAvailable< ElementLink< xAOD::TruthParticleContainer > >( "truthParticleLink" ) ) {
      ElementLink< xAOD::TruthParticleContainer> link = 
	muon->auxdata< ElementLink< xAOD::TruthParticleContainer > >( "truthParticleLink" );
      if( link.isValid() ) {
	const xAOD::TruthParticle* matched_truth_muon = *link;
	m_TruthPt=matched_truth_muon->pt()* CLHEP::GeV;
	m_TruthEta=matched_truth_muon->eta();
	m_TruthPhi=matched_truth_muon->phi();
      }
    }
}
