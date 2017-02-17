/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define __TrackInfo_cxx__
#include "MuonPtCalibNtupleMaker/TrackInfo.h"

TrackInfo::TrackInfo( std::string type ) {

  m_Type = type;
  Reset();

}

void TrackInfo::Reset() {

  m_Pt = -999.;
  m_Eta = -999.;
  m_Phi = -999.;
  m_Charge = -999.;
  m_QoverP = -999.;
  m_QoverPSigma = -999.;
  m_Chi2 = -999.;
  m_NDoF = 0.;

}

void TrackInfo::Register( TTree* t, std::string p ) {

  t->Branch( ( p + "_" + m_Type + "_Pt" ).c_str(), &m_Pt );
  t->Branch( ( p + "_" + m_Type + "_Eta" ).c_str(), &m_Eta );
  t->Branch( ( p + "_" + m_Type + "_Phi" ).c_str(), &m_Phi );
  t->Branch( ( p + "_" + m_Type + "_Charge" ).c_str(), &m_Charge );
  t->Branch( ( p + "_" + m_Type + "_QoverP" ).c_str(), &m_QoverP );
  t->Branch( ( p + "_" + m_Type + "_QoverPSigma" ).c_str(), &m_QoverPSigma );
  t->Branch( ( p + "_" + m_Type + "_Chi2" ).c_str(), &m_Chi2 );
  t->Branch( ( p + "_" + m_Type + "_NDoF" ).c_str(), &m_NDoF );

}

void TrackInfo::Fill( const xAOD::TrackParticle* tp ) {
   
  m_Pt = tp->pt() * CLHEP::GeV;
  m_Eta = tp->eta();
  m_Phi = tp->phi();
  m_Charge = tp->charge();
  m_QoverP = tp->qOverP() / CLHEP::GeV;
  m_QoverPSigma = sqrt( tp->definingParametersCovMatrix()( 4, 4 ) ) / CLHEP::GeV;
  m_Chi2 = tp->chiSquared();
  m_NDoF = tp->numberDoF();

}
